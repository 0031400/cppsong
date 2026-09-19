#include "common/builder.hpp"
#include "address.hpp"
#include "config/config.hpp"
#include "dns/servers/udp.hpp"
#include "inbounds/mixed.hpp"
#include "listeners/tcp.hpp"
#include "outbounds/direct.hpp"
#include "outbounds/vless.hpp"
#include "router/rule.hpp"
#include "transports/tcp.hpp"
#include "transports/wss.hpp"
#include <boost/asio/ip/address.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

Builder::Builder(asio::io_context &io, AppConfig config)
    : io_(io), config_(config) {}
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
    rules.emplace_back(buildRule(item.rule), item.server);
  }
  return DnsCenter(io_, addr, rules, std::move(servers), config.final);
}

std::unique_ptr<DnsServer> Builder::buildDnsServer(DnsServerConfig config) {
  if (config.type == "udp") {
    auto v = std::make_unique<UdpDnsServer>(
        io_,
        udp::endpoint(ip::make_address(config.server), config.server_port));
    return v;
  }
  throw std::runtime_error("unsupport dns server type");
}
Rule Builder::buildRule(DomainRuleConfig config) {
  std::vector<std::regex> regexs;
  for (auto item : config.domainRegex) {
    regexs.emplace_back(std::regex(item));
  }
  return Rule(config.domain, config.domainSuffix, config.domainKeyword, regexs);
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
    return std::make_unique<TcpTransport>(io_);
  } else if (config.transport.type == "ws" && config.tls.enabled) {
    return std::make_unique<WssTransport>(
        io_, config.transport.host, config.transport.path,
        config.tls.serverName, config.tls.insecure);
  }
  throw std::runtime_error("unsupport transport type");
}
