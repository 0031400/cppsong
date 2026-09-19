#pragma once
#include "tcp/session.hpp"
#include "utils/asyncqueue.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>

class Inbound {
public:
  explicit Inbound(asio::io_context &io);
  virtual void start() = 0;
  virtual async<InTcpSession> session();
  virtual void close() = 0;
  virtual ~Inbound() = default;

protected:
  AsyncQueue<InTcpSession> sessions_;
};