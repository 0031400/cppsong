#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "tcp/session.hpp"
#include "utils/headers.hpp"

class Outbound {
public:
  virtual async<std::unique_ptr<Connection>> connect(OutTcpSession session) = 0;
  virtual ~Outbound() = default;
};