#include "common/address.hpp"
#include <boost/asio/ip/address.hpp>
#include <format>
#include <variant>
std::string Address::toString() const {
  if (std::holds_alternative<std::string>(address)) {
    auto value = std::get<std::string>(address);
    return std::format("{}:{}", value, port);
  } else {
    auto value = std::get<ip::address>(address);
    if (value.is_v4()) {
      return std::format("{}:{}", value.to_string(), port);
    } else {
      return std::format("[{}]:{}", value.to_string(), port);
    }
  }
}