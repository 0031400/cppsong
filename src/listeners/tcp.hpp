#pragma once
#include "listeners/listener.hpp"
#include "utils/headers.hpp"
#include <optional>
class TcpListener : public Listener {
public:
  explicit TcpListener(asio::io_context &io, tcp::endpoint endpoint);
  void start();
  void close();
  
  private:
  async<void> mainWork_();
  asio::io_context &io_;
  tcp::endpoint endpoint_;
  std::optional<tcp::acceptor> acceptor_;
};