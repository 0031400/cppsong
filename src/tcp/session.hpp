#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "utils/headers.hpp"
#include <memory>

struct InTcpSession {
  std::unique_ptr<Connection> conn;
  Address address;
  bytes firstData;
};
struct OutTcpSession {
  Address address;
  bytes firstData;
};