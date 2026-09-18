#pragma once
#include "router/rule.hpp"
#include <string>
struct DnsRule {
  Rule rule;
  std::string server;
};