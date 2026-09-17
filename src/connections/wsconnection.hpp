#pragma once
#include "connections/connection.hpp"
#include "utils/headers.hpp"

class WsConnection : public Connection {
public:
  explicit WsConnection(websocket::stream<beast::tcp_stream> stream);
  async<bytes> read(int n);
  async<bytes> read_exactly(int n);
  async<void> write(bytes data);
  async<void> close();

private:
  websocket::stream<beast::tcp_stream> stream_;
  bytes buffer_;
};