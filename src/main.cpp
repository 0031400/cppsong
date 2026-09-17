#include "dns/message.hpp"
#include "udp/udpclient.hpp"
#include "udp/udpsession.hpp"
#include "utils/asyncqueue.hpp"
#include "utils/headers.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>

class App {
public:
  explicit App(asio::io_context &io) : io_(io), queue_(io_) {}
  // asio::awaitable<void> one() {
  //   asio::steady_timer timer(io_);
  //   timer.expires_after(std::chrono::seconds(1));
  //   co_await timer.async_wait(asio::use_awaitable);
  //   co_await queue_.put({0});
  // }
  asio::awaitable<void> run() {
    UdpClient client(io_);
    auto remote = udp::endpoint(ip::make_address_v4("119.29.29.29"), 53);
    co_await client.start(remote);
    co_await client.send(
        UdpSession{remote, build_dns_query("www.baidu.com", true)});
    auto session = co_await client.session();
    client.close();
    std::cout << "data: ";
    for (u8 b : session.data) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(b) << ' ';
    }
    std::cout << std::endl;
    auto result = parse_dns_response(session.data);
    std::cout << "result: " << std::endl;
    for (auto ip : result) {
      std::cout << ip.to_string() << ' ';
    }
    std::cout << std::endl;

    // asio::co_spawn(io_, one(), asio::detached);
    // auto data = co_await queue_.get();
    // std::cout << "data: " << data.size() << std::endl;
    // co_return;
    // auto acceptor = tcp::acceptor(io_, tcp::endpoint(tcp::v4(), 3000));
    // while (true) {
    //   auto socket = co_await acceptor.async_accept(asio::use_awaitable);
    //   std::cout << "client connected\n";
    //   asio::co_spawn(io_, handle(std::move(socket)), asio::detached);
    // }
  }
  // asio::awaitable<void> handle(tcp::socket socket) {
  //   try {
  //     char data[1024];
  //     while (true) {
  //       auto n = co_await socket.async_read_some(asio::buffer(data),
  //                                                asio::use_awaitable);
  //       co_await asio::async_write(socket, asio::buffer(data, n),
  //                                  asio::use_awaitable);
  //     }
  //   } catch (const boost::system::system_error &e) {
  //     auto ec = e.code();
  //     std::cout << "client disconnected: " << ec.message() << std::endl;
  //   } catch (const std::exception &e) {
  //     std::cout << "client disconnected: " << e.what() << std::endl;
  //   }
  // }

private:
  asio::io_context &io_;
  AsyncQueue<bytes> queue_;
};
int main() {
  asio::io_context io;
  App app(io);
  asio::co_spawn(io, app.run(), asio::detached);
  io.run();
}
