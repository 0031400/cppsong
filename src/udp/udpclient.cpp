#include "udp/udpclient.hpp"
#include "udp/udpsession.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <array>
#include <boost/asio/use_awaitable.hpp>
#include <stdexcept>

UdpClient::UdpClient(asio::io_context &io, std::optional<udp::endpoint> local)
    : io_(io), local_(local), socket_(io_), sessions_(io_) {}
void UdpClient::start(std::optional<udp::endpoint> remote) {
  if (!local_.has_value() && !remote.has_value()) {
    throw std::runtime_error("udp local or remote addr should be set one");
  }
  if (local_.has_value()) {
    socket_.open(local_.value().protocol());
    socket_.bind(local_.value());
  } else {
    socket_.open(remote.value().protocol());
    socket_.connect(remote.value());
  }
  asio::co_spawn(io_, receive_work_(), asio::detached);
}
async<void> UdpClient::receive_work_() {
  try {
    std::array<u8, 4096> buffer;
    udp::endpoint sender;
    while (true) {
      auto n = co_await socket_.async_receive_from(asio::buffer(buffer), sender,
                                                   asio::use_awaitable);
      UdpSession session{sender, bytes(buffer.begin(), buffer.begin() + n)};
      co_await sessions_.put(session);
    }
  } catch (const std::exception &e) {
    log_error("udp receive work", e);
  }
}
async<void> UdpClient::send(UdpSession session) {
  co_await socket_.async_send_to(asio::buffer(session.data), session.endpoint);
}
async<UdpSession> UdpClient::session() { co_return co_await sessions_.get(); }
void UdpClient::close() { socket_.close(); }