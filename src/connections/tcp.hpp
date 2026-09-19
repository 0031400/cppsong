#pragma once
#include "connections/connection.hpp"
#include "utils/headers.hpp"

class TcpConnection : public Connection {
public:
  explicit TcpConnection(beast::tcp_stream stream);
  async<bytes> read(int n);
  async<bytes> read_exactly(int n);
  async<void> write(bytes data);
  async<void> close();

private:
  beast::tcp_stream stream_;
};