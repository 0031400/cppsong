#include "dns/dns.hpp"
#include "dns/message.hpp"
#include "udp/udpclient.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <format>
#include <stdexcept>
#include <utility>

async<std::vector<ip::address>> resolve(std::string_view domain) {
  co_return std::vector<ip::address>{
      ip::make_address("220.181.111.232"), ip::make_address("220.181.111.1"),
      ip::make_address("240e:83:205:381:0:ff:b00f:96a2"),
      ip::make_address("240e:83:205:1cd:0:ff:b0b8:dee9")};
}
DnsCenter::DnsCenter(
    asio::io_context &io, std::optional<udp::endpoint> endpoint,
    std::vector<DnsRule> rules,
    std::unordered_map<std::string, std::unique_ptr<DnsServer>> servers,
    std::string final)
    : io_(io), endpoint_(endpoint), rules_(rules), servers_(std::move(servers)),
      final_(final) {}
async<bytes> DnsCenter::relay(bytes data) {
  auto domain = parse_dns_query(data);
  log("dns", std::format("<- {}", domain));
  auto serverTag = match_(domain);
  auto it = servers_.find(serverTag);
  if (it == servers_.end()) {
    throw std::runtime_error("fail to find dns server");
  }
  data = co_await it->second->relay(data);
  auto ips = parse_dns_response(data);
  std::string result;
  for (std::size_t i = 0; i < ips.size(); i++) {
    result.append(ips[i].to_string());
    if (i != ips.size() - 1) {
      result.append(",");
    }
  }
  log("dns", std::format("{} -> {}", domain, result));
  co_return data;
}
std::string DnsCenter::match_(std::string_view domain) {
  for (auto item : rules_) {
    if (item.rule.matchDomain(domain)) {
      return item.server;
    }
  }
  return final_;
}
async<void> DnsCenter::start() {
  try {
    if (!endpoint_.has_value()) {
      co_return;
    }
    client_.emplace(io_, endpoint_);
    co_await client_->start();
    while (true) {
      auto session = co_await client_->session();
      asio::co_spawn(io_, handleClient_(session), asio::detached);
    }
  } catch (const std::exception &e) {
    log_error("dns center start", e);
  }
}
async<void> DnsCenter::handleClient_(UdpSession session) {
  try {
    session.data = co_await relay(session.data);
    co_await client_->send(session);
  } catch (const std::exception &e) {
    log_error("dns handle", e);
  }
}
void DnsCenter::stop() {
  if (client_.has_value()) {
    client_->close();
  }
}