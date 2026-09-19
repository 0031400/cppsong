#pragma once
#include "inbounds/inbound.hpp"
#include "outbounds/outbound.hpp"
#include "tcp/session.hpp"
#include "utils/headers.hpp"
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
class App {
public:
  explicit App(std::string_view configPath);
  void run();

private:
  async<void> inboundWork_(std::unique_ptr<Inbound> inbound);
  async<void> handleSession_(InTcpSession session);
  std::string configPath_;
  asio::io_context io_;
  std::unordered_map<std::string, std::shared_ptr<Outbound>> outbounds;
};