#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "transports/transport.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>

class WssTransport : public Transport {
public:
  explicit WssTransport(asio::io_context &io, std::string host_,
                        std::string path_);
  async<std::unique_ptr<Connection>> connect(Address address);

private:
  asio::io_context &io_;
  std::string host_;
  std::string path_;
};