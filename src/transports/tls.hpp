#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "transports/transport.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

class TlsTransport : public Transport {
public:
  explicit TlsTransport(asio::io_context &io, std::string serverName,
                        bool insecure);
  async<std::unique_ptr<Connection>> connect(Address address);

private:
  asio::io_context &io_;
  std::string serverName_;
  bool insecure_;
  ssl::context ctx_;
};