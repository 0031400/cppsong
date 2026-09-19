#pragma once
#include "config/config.hpp"
#include "dns/dns.hpp"
#include "inbounds/inbound.hpp"
#include "outbounds/outbound.hpp"
#include "listeners/listener.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>
class Builder {
public:
  explicit Builder(asio::io_context &io, AppConfig config);
  DnsCenter buildDnsCenter();
  std::unique_ptr<DnsServer> buildDnsServer(DnsServerConfig config);
  std::unique_ptr<Inbound> buildInbound(InboundConfig config);
  std::shared_ptr<Outbound> buildOutbound(OutboundConfig config);
  std::unique_ptr<Listener> buildListener(InboundConfig config);
  Rule buildRule(DomainRuleConfig config);

private:
  asio::io_context &io_;
  AppConfig config_;
};