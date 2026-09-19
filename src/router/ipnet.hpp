#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address.hpp>
#include <boost/system/detail/error_code.hpp>
#include <stdexcept>
#include <string>
struct IpNetwork {
  ip::address network;
  u8 prefix;
  bool contains(const ip::address &addr) const;
};
IpNetwork parseCidr(std::string cidr);