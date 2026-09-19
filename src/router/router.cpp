#include "router/router.hpp"
Router::Router(std::vector<RouteRule> rules, std::string final)
    : rules_(rules), final_(final) {}
std::string Router::match(Address address) {
  for (const auto &item : rules_) {
    if (item.match(address)) {
      return item.outbound;
    }
  }
  return final_;
}