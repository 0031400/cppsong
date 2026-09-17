#include "common/address.hpp"
#include "connections/tcpconnection.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <format>

class App {
public:
  explicit App(asio::io_context &io) : io_(io) {}
  asio::awaitable<void> run() {
    // auto socket =
    //     co_await connect_ip(io_, ip::make_address("220.181.111.1"), 80);
    auto socket =
        co_await connect_address(io_, DomainAddress("www.baidu.com", 80));
    auto conn = TcpConnection(std::move(socket));
    std::string req("GET / HTTP/1.1\r\nHost: www.baidu.com\r\n\r\n");
    co_await conn.write({req.begin(), req.end()});
    auto data = co_await conn.read(4096);
    log("tcp", std::format("size: {}", data.size()));
    // log("tcp", std::string(data.begin(), data.end()));
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
