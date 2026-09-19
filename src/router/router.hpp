#pragma once
#include "common/address.hpp"
#include "router/rule.hpp"
#include <vector>
class Router {
public:
  explicit Router(std::vector<RouteRule> rules, std::string final);
  std::string match(Address address);

private:
  std::vector<RouteRule> rules_;
  std::string final_;
};