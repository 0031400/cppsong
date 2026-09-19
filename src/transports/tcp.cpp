#include "transports/tcp.hpp"
#include "connections/tcp.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include <memory>
#include <utility>
TcpTransport::TcpTransport(asio::io_context &io) : io_(io) {}
async<std::unique_ptr<Connection>> TcpTransport::connect(Address address) {
  auto socket = co_await connect_address(io_, address);
  co_return std::make_unique<TcpConnection>(std::move(socket));
}