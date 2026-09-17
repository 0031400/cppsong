#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "utils/headers.hpp"
#include <memory>

class Transport {
public:
  virtual std::unique_ptr<Connection> connect(Address address) = 0;
};