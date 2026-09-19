#pragma once
#include "outbounds/outbound.hpp"
#include "transports/transport.hpp"
#include <memory>
class VlessOutbound : public Outbound {
public:
  explicit VlessOutbound(asio::io_context &io,
                         std::unique_ptr<Transport> transport, Address server,
                         bytes uuid);
  async<std::unique_ptr<Connection>> connect(OutTcpSession session) override;

private:
  asio::io_context &io_;
  std::unique_ptr<Transport> transport_;
  Address server_;
  bytes uuid_;
};