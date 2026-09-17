#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address_v4.hpp>
#include <string>
#include <variant>
class DomainAddress {
public:
  std::string address;
  u16 port;
};
class Ipv4Address {
public:
  ip::address_v4 address;
  u16 port;
};
class Ipv6Address {
public:
  ip::address_v4 address;
  u16 port;
};
using Address = std::variant<DomainAddress, Ipv4Address, Ipv6Address>;