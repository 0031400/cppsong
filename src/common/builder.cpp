#include "common/builder.hpp"
#include "dns/servers/udp.hpp"
#include "router/rule.hpp"
#include <boost/asio/ip/address.hpp>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
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