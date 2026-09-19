#include "connections/tls.hpp"
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <utility>
TlsConnection::TlsConnection(beast::ssl_stream<beast::tcp_stream> stream)
    : stream_(std::move(stream)) {}
async<bytes> TlsConnection::read(int n) {
  auto buffer = bytes(n);
  auto readN = co_await stream_.async_read_some(asio::buffer(buffer),
                                                asio::use_awaitable);
  buffer.resize(readN);
  co_return buffer;
}
async<bytes> TlsConnection::read_exactly(int n) {
  auto buffer = bytes(n);
  co_await asio::async_read(stream_, asio::buffer(buffer), asio::use_awaitable);
  co_return buffer;
}

async<void> TlsConnection::write(bytes data) {
  co_await asio::async_write(stream_, asio::buffer(data), asio::use_awaitable);
  co_return;
}
async<void> TlsConnection::close() {
  stream_.shutdown();
  beast::get_lowest_layer(stream_).socket().close();
  co_return;
}