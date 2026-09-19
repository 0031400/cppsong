#include "outbounds/direct.hpp"
#include "transports/tcp.hpp"
#include <memory>
DirectOutbound::DirectOutbound(asio::io_context &io) : io_(io) {}
async<std::unique_ptr<Connection>> DirectOutbound::connect(OutTcpSession session) {
  TcpTransport transport(io_);
  auto conn = co_await transport.connect(session.address);
  co_await conn->write(session.firstData);
  co_return conn;
}