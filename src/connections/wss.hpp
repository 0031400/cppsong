#pragma once
#include "connections/connection.hpp"
#include "utils/headers.hpp"
#include <boost/beast/ssl/ssl_stream.hpp>

class WssConnection : public Connection {
public:
  explicit WssConnection(websocket::stream<beast::ssl_stream<beast::tcp_stream>> stream);
  async<bytes> read(int n);
  async<bytes> read_exactly(int n);
  async<void> write(bytes data);
  async<void> close();

private:
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> stream_;
  bytes buffer_;
};