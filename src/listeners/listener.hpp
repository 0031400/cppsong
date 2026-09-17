#pragma once
#include "connections/connection.hpp"
#include "utils/asyncqueue.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>
#include <memory>
class Listener {
public:
  explicit Listener(asio::io_context &io);
  virtual async<void> start() = 0;
  async<std::unique_ptr<Connection>> session();
  virtual void close() = 0;

protected:
  AsyncQueue<std::unique_ptr<Connection>> sessions_;
};