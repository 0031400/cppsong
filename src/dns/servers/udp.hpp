#pragma once
#include "dns/servers/server.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>

class UdpDnsServer : public DnsServer {
public:
  explicit UdpDnsServer(asio::io_context &io, udp::endpoint endpoint);
  async<bytes> relay(bytes data);

private:
  asio::io_context &io_;
  udp::endpoint endpoint_;
};