#pragma once
#include "utils/headers.hpp"
struct UdpSession {
  udp::endpoint endpoint;
  bytes data;
};