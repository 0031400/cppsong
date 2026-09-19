#include "router/router.hpp"
Router::Router(std::vector<RouteRule> rules, std::string final)
    : rules_(rules), final_(final) {}
std::string Router::match(Address address) {
  for (const auto &item : rules_) {
    for (const auto &i : item.rules) {
      if (i.match(address)) {
        return item.outbound;
      }
    }
  }
  return final_;
}