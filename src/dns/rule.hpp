#pragma once
#include "router/rule.hpp"
#include <string>
struct DnsRule {
  DomainRule rule;
  std::string server;
};