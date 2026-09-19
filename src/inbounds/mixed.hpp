#pragma once
#include "common/userpass.hpp"
#include "connections/connection.hpp"
#include "listeners/listener.hpp"
#include "inbounds/inbound.hpp"
#include "utils/headers.hpp"
#include <memory>

class MixedInbound : public Inbound {
public:
  explicit MixedInbound(asio::io_context &io,
                        std::unique_ptr<Listener> listener,
                        std::vector<UserPass> users, std::vector<bytes> uuids);
  void start();
  void close();

private:
  async<void> mainWork_();
  async<void> handle_(std::unique_ptr<Connection> session);
  async<void> handleSocks5_(std::unique_ptr<Connection> session);
  async<void> handleHttp_(std::unique_ptr<Connection> session);
  asio::io_context &io_;
  std::unique_ptr<Listener> listener_;
  std::vector<UserPass> users_;
  std::vector<bytes> uuids_;
};