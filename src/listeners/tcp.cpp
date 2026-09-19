#include "listeners/tcp.hpp"
#include "connections/tcp.hpp"
#include "utils/log.hpp"
#include "listener.hpp"
#include "utils/headers.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <memory>
TcpListener::TcpListener(asio::io_context &io, tcp::endpoint endpoint)
    : Listener(io), io_(io), endpoint_(endpoint) {}
void TcpListener::start() { asio::co_spawn(io_, mainWork_(), asio::detached); }
async<void> TcpListener::mainWork_() {
  try {
    acceptor_ = tcp::acceptor(io_, endpoint_);
    while (true) {
      auto socket = co_await acceptor_->async_accept(asio::use_awaitable);
      co_await sessions_.put(
          std::make_unique<TcpConnection>(std::move(socket)));
    }
  } catch (const std::exception &e) {
    log_error("tcp listen main work", e);
  }
}
void TcpListener::close() {
  if (acceptor_.has_value()) {
    acceptor_->close();
  }
}