#pragma once
#include "utils/headers.hpp"
class DnsServer {
public:
  virtual async<bytes> relay(bytes data) = 0;
  virtual ~DnsServer() = default;
};