#pragma once
#include "utils/headers.hpp"
class Connection {
public:
  virtual async<bytes> read(int n) = 0;
  virtual async<bytes> read_exactly(int n) = 0;
  virtual async<void> write(bytes data) = 0;
  virtual async<void> close() = 0;
  virtual ~Connection() = default;
};