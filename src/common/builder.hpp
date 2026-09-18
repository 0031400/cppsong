#pragma once
#include "config/config.hpp"
#include "dns/dns.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>
class Builder {
public:
  explicit Builder(asio::io_context &io, AppConfig config);
  DnsCenter buildDnsCenter();
  std::unique_ptr<DnsServer> buildDnsServer(DnsServerConfig config);
  Rule buildRule(DomainRuleConfig config);

private:
  asio::io_context &io_;
  AppConfig config_;
};