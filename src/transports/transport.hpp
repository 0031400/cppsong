#pragma once
#include "common/address.hpp"
#include "connections/connection.hpp"
#include "utils/headers.hpp"
#include <memory>

class Transport {
public:
  virtual async<std::unique_ptr<Connection>> connect(Address address) = 0;
  virtual ~Transport() = default;
};