#include "connections/ws.hpp"
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/websocket/rfc6455.hpp>
#include <stdexcept>
WsConnection::WsConnection(websocket::stream<beast::tcp_stream> stream)
    : stream_(std::move(stream)) {}
async<bytes> WsConnection::read(int n) {
  if (!buffer_.empty()) {
    bytes res(buffer_);
    buffer_.clear();
    co_return res;
  }
  beast::flat_buffer buffer;
  co_await stream_.async_read(buffer, asio::use_awaitable);
  if (!stream_.got_binary()) {
    throw std::runtime_error("ws only accept binary");
  }
  co_return bytes(asio::buffers_begin(buffer.data()),
                  asio::buffers_end(buffer.data()));
}
async<bytes> WsConnection::read_exactly(int n) {
  while (buffer_.size() < n) {
    beast::flat_buffer buffer;
    co_await stream_.async_read(buffer, asio::use_awaitable);
    if (!stream_.got_binary()) {
      throw std::runtime_error("ws only accept binary");
    }
    buffer_.insert(buffer_.end(), asio::buffers_begin(buffer.data()),
                   asio::buffers_end(buffer.data()));
  }
  bytes res{buffer_.begin(), buffer_.begin() + n};
  buffer_.erase(buffer_.begin(), buffer_.begin() + n);
  co_return res;
}
async<void> WsConnection::write(bytes data) {
  co_await stream_.async_write(asio::buffer(data), asio::use_awaitable);
}
async<void> WsConnection::close() {
  co_await stream_.async_close(websocket::close_code::normal,
                               asio::use_awaitable);
}