#include "connections/relay.hpp"
#include "connections/connection.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <memory>

namespace {
async<void> forward(std::shared_ptr<asio::steady_timer> timer,
                    std::shared_ptr<Connection> conn1,
                    std::shared_ptr<Connection> conn2) {
  try {
    while (true) {
      auto data = co_await conn1->read(4096);
      co_await conn2->write(data);
    }
  } catch (const std::exception &e) {
    log_error("tcp forward", e);
  }
  timer->cancel();
}
}; // namespace
async<void> relay(asio::io_context &io, std::shared_ptr<Connection> conn1,
                  std::shared_ptr<Connection> conn2) {
  auto timer = std::make_shared<asio::steady_timer>(io);
  timer->expires_at(asio::steady_timer::time_point::max());
  asio::co_spawn(io, forward(timer, conn1, conn2), asio::detached);
  asio::co_spawn(io, forward(timer, conn2, conn1), asio::detached);
  co_await timer->async_wait(asio::use_awaitable);
  co_return;
}