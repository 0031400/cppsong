#include "listeners/listener.hpp"
#include "utils/headers.hpp"
Listener::Listener(asio::io_context &io) : sessions_(io) {}
async<std::shared_ptr<Connection>> Listener::session() {
  co_return co_await sessions_.get();
}