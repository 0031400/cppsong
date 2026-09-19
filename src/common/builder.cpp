#include "common/builder.hpp"
#include "address.hpp"
#include "config/config.hpp"
#include "dns/servers/https.hpp"
#include "dns/servers/udp.hpp"
#include "inbounds/mixed.hpp"
#include "listeners/tcp.hpp"
#include "outbounds/direct.hpp"
#include "outbounds/vless.hpp"
#include "router/ipnet.hpp"
#include "router/rule.hpp"
#include "transports/tcp.hpp"
#include "transports/tls.hpp"
#include "transports/wss.hpp"
#include "utils/file.hpp"
#include "utils/json.hpp"
#include <boost/asio/ip/address.hpp>
#include <boost/json/parse.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

Builder::Builder(asio::io_context &io, AppConfig config)
    : io_(io), config_(config) {
  loadRuleSets();
}
DnsCenter Builder::buildDnsCenter() {
  auto config = config_.dns;
  std::optional<udp::endpoint> addr;
  if (!config.listen.empty()) {
    addr = udp::endpoint(ip::make_address(config.listen), config.listen_port);
  }
  std::unordered_map<std::string, std::unique_ptr<DnsServer>> servers;
  for (auto item : config.servers) {
    servers.emplace(item.tag, buildDnsServer(item));
  }
  std::vector<DnsRule> rules;
  for (auto item : config.rules) {
    for (const auto &ruleSet : item.rule_set) {
      auto it = ruleSets_.find(ruleSet);
      if (it == ruleSets_.end()) {
        throw std::runtime_error("fail to find rule set");
      }
      for (const auto &v : it->second) {
        rules.emplace_back(v.domainRule, item.server);
      }
    }
    rules.emplace_back(buildDomainRule(item.rule), item.server);
  }
  return DnsCenter(io_, addr, rules, std::move(servers), config.final);
}

std::unique_ptr<DnsServer> Builder::buildDnsServer(DnsServerConfig config) {
  if (config.type == "udp") {
    return std::make_unique<UdpDnsServer>(
        io_,
        udp::endpoint(ip::make_address(config.server), config.server_port));
  } else if (config.type == "https") {
    return std::make_unique<HttpsDnsServer>(
        io_, tcp::endpoint(ip::make_address(config.server), config.server_port),
        config.host, config.path, config.tls.serverName, config.tls.insecure);
  }
  throw std::runtime_error("unsupport dns server type");
}
DomainRule Builder::buildDomainRule(DomainRuleConfig config) {
  std::vector<std::regex> regexs;
  for (auto item : config.domainRegex) {
    regexs.emplace_back(std::regex(item));
  }
  return DomainRule(config.domain, config.domainSuffix, config.domainKeyword,
                    regexs);
}
std::unique_ptr<Listener> Builder::buildListener(InboundConfig config) {
  return std::make_unique<TcpListener>(
      io_, tcp::endpoint(ip::make_address(config.listen), config.listen_port));
}
std::unique_ptr<Inbound> Builder::buildInbound(InboundConfig config) {
  if (config.type == "mixed") {
    auto listener = buildListener(config);
    return std::make_unique<MixedInbound>(io_, std::move(listener),
                                          std::vector<UserPass>{},
                                          std::vector<bytes>{});
  }
  throw std::runtime_error("unsupport inbound type");
}

std::shared_ptr<Outbound> Builder::buildOutbound(OutboundConfig config) {
  auto transport = buildTransport(config);
  if (config.type == "direct") {
    return std::make_shared<DirectOutbound>(io_);
  } else if (config.type == "vless") {
    return std::make_shared<VlessOutbound>(
        io_, std::move(transport),
        buildAddress(config.server, config.server_port),
        buildUuid(config.uuid));
  }
  throw std::runtime_error("unsupport outbound type");
}
bytes Builder::buildUuid(std::string uuid) {
  uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
  if (uuid.size() != 32) {
    throw std::runtime_error("error uuid");
  }
  bytes result(16, 0);
  for (std::size_t i = 0; i < 16; i++) {
    result[i] = static_cast<u8>(std::stoul(uuid.substr(i * 2, 2), nullptr, 16));
  }
  return result;
}
Address Builder::buildAddress(std::string server, u16 port) {
  Address address;
  boost::system::error_code ec;
  auto ipAddress = ip::make_address(server, ec);
  if (ec) {
    address.address = server;
  } else {
    address.address = ipAddress;
  }
  address.port = port;
  return address;
}
std::unique_ptr<Transport> Builder::buildTransport(OutboundConfig config) {
  if (config.transport.type.empty() || config.transport.type == "tcp") {
    if (config.tls.enabled) {
      return std::make_unique<TlsTransport>(io_, config.tls.serverName,
                                            config.tls.insecure);
    }
    return std::make_unique<TcpTransport>(io_);
  } else if (config.transport.type == "ws" && config.tls.enabled) {
    return std::make_unique<WssTransport>(
        io_, config.transport.host, config.transport.path,
        config.tls.serverName, config.tls.insecure);
  }
  throw std::runtime_error("unsupport transport type");
}

RouteRule Builder::buildRouteRule(RouteRuleConfig config) {
  std::vector<Rule> rules{buildRule(config.rule)};
  for (const auto &ruleSet : config.rule_set) {
    auto it = ruleSets_.find(ruleSet);
    if (it == ruleSets_.end()) {
      throw std::runtime_error("fail to find rule set");
    }
    rules.append_range(it->second);
  }
  return {rules, config.outbound};
}
Rule Builder::buildRule(RuleConfig config) {
  std::vector<IpNetwork> networks;
  for (const auto &item : config.cidr) {
    networks.emplace_back(parseCidr(item));
  }
  return {buildDomainRule(config.domainRule), networks};
}
Router Builder::buildRouter() {
  std::vector<RouteRule> rules;
  for (const auto &item : config_.router.rules) {
    rules.emplace_back(buildRouteRule(item));
  }
  return Router(rules, config_.router.final);
}
void Builder::loadRuleSets() {
  std::string content;
  for (const auto &item : config_.router.ruleSets) {
    std::vector<Rule> rules;
    if (item.type == "local") {
      content = readFile(item.path);
    } else {
      throw std::runtime_error("unsupport rule set type");
    }
    if (item.format == "source") {
      auto obj = json::parse(content).as_object();
      auto rulesArray = getArray(obj, "rules");
      for (const auto &i : rulesArray) {
        auto ruleConfig = parseRuleConfig(i.as_object());
        rules.emplace_back(buildRule(ruleConfig));
      }
    } else {
      throw std::runtime_error("unsupport rule set format");
    }
    ruleSets_[item.tag] = rules;
  }
}