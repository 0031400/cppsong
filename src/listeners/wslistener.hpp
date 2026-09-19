#pragma once
#include "listeners/listener.hpp"
#include "utils/headers.hpp"
#include <optional>
class WsListener : public Listener {
public:
  explicit WsListener(asio::io_context &io, tcp::endpoint endpoint,
                      std::string_view path);
  void start();
  void close();

private:
  async<void> handle_(tcp::socket socket);
  async<void> mainWork_();
  asio::io_context &io_;
  tcp::endpoint endpoint_;
  std::optional<tcp::acceptor> acceptor_;
  std::string path_;
};