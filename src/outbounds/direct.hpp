#pragma once
#include "outbounds/outbound.hpp"
#include <boost/asio/io_context.hpp>
class DirectOutbound : public Outbound {
public:
  explicit DirectOutbound(asio::io_context &io);
  async<std::unique_ptr<Connection>> connect(OutTcpSession session) override;

private:
  asio::io_context &io_;
};