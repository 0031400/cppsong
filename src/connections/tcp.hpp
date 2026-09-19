#pragma once
#include "connections/connection.hpp"
#include "utils/headers.hpp"

class TcpConnection : public Connection {
public:
  explicit TcpConnection(tcp::socket socket);
  async<bytes> read(int n);
  async<bytes> read_exactly(int n);
  async<void> write(bytes data);
  async<void> close();

private:
  tcp::socket socket_;
};