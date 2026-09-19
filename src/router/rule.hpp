#pragma once
#include "common/address.hpp"
#include "router/ipnet.hpp"
#include <regex>
#include <string>
#include <vector>

class DomainRule {
public:
  explicit DomainRule(std::vector<std::string> domain,
                      std::vector<std::string> domainSuffix,
                      std::vector<std::string> domainKeyword,
                      std::vector<std::regex> domainRegex);
  bool matchDomain(std::string_view domain) const;

private:
  std::vector<std::string> domain_;
  std::vector<std::string> domainSuffix_;
  std::vector<std::string> domainKeyword_;
  std::vector<std::regex> domainRegex_;
};

struct Rule {
  DomainRule domainRule;
  std::vector<IpNetwork> cidr;
  bool match(Address address) const;
};
struct RouteRule {
  std::vector<Rule> rules;
  std::string outbound;
};