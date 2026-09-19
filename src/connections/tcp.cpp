#include "connections/tcp.hpp"
#include <boost/asio/read.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
TcpConnection::TcpConnection(tcp::socket socket) : socket_(std::move(socket)) {}
async<bytes> TcpConnection::read(int n) {
  auto buffer = bytes(n);
  auto readN = co_await socket_.async_read_some(asio::buffer(buffer),
                                                asio::use_awaitable);
  buffer.resize(readN);
  co_return buffer;
}
async<bytes> TcpConnection::read_exactly(int n) {
  auto buffer = bytes(n);
  co_await asio::async_read(socket_, asio::buffer(buffer), asio::use_awaitable);
  co_return buffer;
}

async<void> TcpConnection::write(bytes data) {
  co_await asio::async_write(socket_, asio::buffer(data), asio::use_awaitable);
  co_return;
}
async<void> TcpConnection::close() {
  socket_.close();
  co_return;
}