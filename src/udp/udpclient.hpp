#pragma once
#include "udp/udpsession.hpp"
#include "utils/asyncqueue.hpp"
#include "utils/headers.hpp"
#include <optional>
class UdpClient {
public:
  explicit UdpClient(asio::io_context &io,
                     std::optional<udp::endpoint> local = std::nullopt);
  async<void> start(std::optional<udp::endpoint> remote = std::nullopt);
  async<UdpSession> session();
  async<void> send(UdpSession session);
  void close();

private:
  async<void> receive_work_();
  asio::io_context &io_;
  std::optional<udp::endpoint> local_;
  udp::socket socket_;
  AsyncQueue<UdpSession> sessions_;
};