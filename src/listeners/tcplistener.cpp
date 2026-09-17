#include "listeners/tcplistener.hpp"
#include "connections/tcpconnection.hpp"
#include "listener.hpp"
#include <memory>
TcpListener::TcpListener(asio::io_context &io, tcp::endpoint endpoint)
    : Listener(io), io_(io), endpoint_(endpoint) {}
async<void> TcpListener::start() {
  acceptor_ = tcp::acceptor(io_, endpoint_);
  while (true) {
    auto socket = co_await acceptor_->async_accept(asio::use_awaitable);
    co_await sessions_.put(std::make_unique<TcpConnection>(std::move(socket)));
  }
}
void TcpListener::close() {
  if (acceptor_.has_value()) {
    acceptor_->close();
  }
}