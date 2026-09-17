#include "tcp/tcpclient.hpp"
#include "common/address.hpp"
#include "dns/dns.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <memory>
#include <utility>
namespace {
async<void>
connect_address_work(std::shared_ptr<asio::steady_timer> timer,
                     asio::io_context &io, ip::address ipAddress, u16 port,
                     std::shared_ptr<std::optional<tcp::socket>> result) {
  try {
    tcp::socket socket(io);
    co_await socket.async_connect(tcp::endpoint(ipAddress, port),
                                  asio::use_awaitable);
    if (result->has_value()) {
      co_return;
    }
    *result = std::move(socket);
    timer->cancel();
  } catch (const std::exception &e) {
    log_error("connect address work", e);
  }
}
} // namespace
async<tcp::socket> connect_ip(asio::io_context &io, ip::address ipAddress,
                              u16 port) {
  tcp::socket socket(io);
  co_await socket.async_connect(tcp::endpoint(ipAddress, port),
                                asio::use_awaitable);
  co_return socket;
}
async<tcp::socket> connect_address(asio::io_context &io, Address address) {
  if (std::holds_alternative<Ipv4Address>(address)) {
    auto value = std::get<Ipv4Address>(address);
    co_return co_await connect_ip(io, value.address, value.port);
  } else if (std::holds_alternative<Ipv6Address>(address)) {
    auto value = std::get<Ipv6Address>(address);
    co_return co_await connect_ip(io, value.address, value.port);
  }
  auto value = std::get<DomainAddress>(address);
  std::vector<ip::address> ips = co_await resolve(value.address);
  u16 port = value.port;
  auto result = std::make_shared<std::optional<tcp::socket>>();
  auto timer = std::make_shared<asio::steady_timer>(io);
  timer->expires_at(asio::steady_timer::time_point::max());
  for (auto ipAddress : ips) {
    asio::co_spawn(io, connect_address_work(timer, io, ipAddress, port, result),
                   asio::detached);
  }
  boost::system::error_code ec;
  co_await timer->async_wait(asio::redirect_error(asio::use_awaitable, ec));
  if (!result->has_value()) {
    throw std::runtime_error("all tries to connect address fail");
  }
  co_return std::move(result->value());
}