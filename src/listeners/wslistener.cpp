#include "listeners/wslistener.hpp"
#include "connections/ws.hpp"
#include "listener.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <format>
#include <stdexcept>
#include <string>
WsListener::WsListener(asio::io_context &io, tcp::endpoint endpoint,
                       std::string_view path)
    : Listener(io), io_(io), endpoint_(endpoint), path_(path) {}
void WsListener::start() { asio::co_spawn(io_, mainWork_(), asio::detached); }
async<void> WsListener::mainWork_() {
  try {
    acceptor_ = tcp::acceptor(io_, endpoint_);
    while (true) {
      auto socket = co_await acceptor_->async_accept(asio::use_awaitable);
      asio::co_spawn(io_, handle_(std::move(socket)), asio::detached);
    }
  } catch (const std::exception &e) {
    log_error("dns center start", e);
  }
}
async<void> WsListener::handle_(tcp::socket socket) {
  try {
    auto ws = websocket::stream<beast::tcp_stream>(std::move(socket));
    beast::flat_buffer buffer;
    http::request<http::string_body> req;
    co_await http::async_read(ws.next_layer(), buffer, req,
                              asio::use_awaitable);
    if (req.target() != path_) {
      throw std::runtime_error(std::format("error path {}", req.target()));
    }
    co_await ws.async_accept(asio::use_awaitable);
    co_await sessions_.put(std::make_unique<WsConnection>(std::move(ws)));
  } catch (const std::exception &e) {
    log_error("ws handle", e);
  }
}
void WsListener::close() {
  if (acceptor_.has_value()) {
    acceptor_->close();
  }
}