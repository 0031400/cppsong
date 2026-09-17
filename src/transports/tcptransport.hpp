#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "transports/transport.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

class TcpTransport : public Transport {
public:
  explicit TcpTransport(asio::io_context &io);
  async<std::shared_ptr<Connection>> connect(Address address);

private:
  asio::io_context &io_;
};