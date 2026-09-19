#pragma once
#include "config/config.hpp"
#include "dns/dns.hpp"
#include "inbounds/inbound.hpp"
#include "listeners/listener.hpp"
#include "outbounds/outbound.hpp"
#include "router/router.hpp"
#include "transports/transport.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>
#include <string>

class Builder {
public:
  explicit Builder(asio::io_context &io, AppConfig config);
  DnsCenter buildDnsCenter();
  std::unique_ptr<DnsServer> buildDnsServer(DnsServerConfig config);
  std::unique_ptr<Inbound> buildInbound(InboundConfig config);
  std::shared_ptr<Outbound> buildOutbound(OutboundConfig config);
  std::unique_ptr<Listener> buildListener(InboundConfig config);
  std::unique_ptr<Transport> buildTransport(OutboundConfig config);
  bytes buildUuid(std::string uuid);
  RouteRule buildRouteRule(RouteRuleConfig config);
  Router buildRouter();
  Address buildAddress(std::string server, u16 port);
  DomainRule buildDomainRule(DomainRuleConfig config);

private:
  asio::io_context &io_;
  AppConfig config_;
};