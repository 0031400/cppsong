#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address_v4.hpp>
#include <string>
#include <variant>
struct Address {
  std::variant<std::string, ip::address> address;
  u16 port;
  std::string toString() const;
};