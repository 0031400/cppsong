#pragma once
#include "connections/connection.hpp"
#include "utils/headers.hpp"

class TlsConnection : public Connection {
public:
  explicit TlsConnection(beast::ssl_stream<beast::tcp_stream> stream);
  async<bytes> read(int n);
  async<bytes> read_exactly(int n);
  async<void> write(bytes data);
  async<void> close();

private:
  beast::ssl_stream<beast::tcp_stream> stream_;
};