#include "common/address.hpp"
#include "connections/relay.hpp"
#include "connections/tcpconnection.hpp"
#include "listeners/tcplistener.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <format>
#include <memory>

class App {
public:
  explicit App(asio::io_context &io) : io_(io) {}
  asio::awaitable<void> run() {
    auto listener =
        TcpListener(io_, tcp::endpoint(ip::make_address_v4("127.0.0.1"), 3000));
    asio::co_spawn(io_, listener.start(), asio::detached);
    while (true) {
      auto conn1 = co_await listener.session();
      auto socket =
          co_await connect_address(io_, DomainAddress("www.baidu.com", 80));
      auto conn2 = std::make_shared<TcpConnection>(std::move(socket));
      asio::co_spawn(io_, relay(io_, conn1, conn2), asio::detached);
    }
  }

private:
  asio::io_context &io_;
};
int main() {
  asio::io_context io;
  App app(io);
  asio::co_spawn(io, app.run(), asio::detached);
  io.run();
}
