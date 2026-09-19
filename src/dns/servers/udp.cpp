#include "dns/servers/udp.hpp"
#include "udp/udpclient.hpp"
#include "udp/udpsession.hpp"
UdpDnsServer::UdpDnsServer(asio::io_context &io, udp::endpoint endpoint)
    : io_(io), endpoint_(endpoint) {}
async<bytes> UdpDnsServer::relay(bytes data) {
  UdpClient client(io_);
  client.start(endpoint_);
  co_await client.send(UdpSession{endpoint_, data});
  auto session = co_await client.session();
  client.close();
  co_return session.data;
}