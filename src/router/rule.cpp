#include "router/rule.hpp"
#include <regex>
bool Rule::matchDomain(std::string_view domain) {
  for (const auto &item : domain_) {
    if (domain == item) {
      return true;
    }
  }
  for (const auto &item : domainSuffix_) {
    if (domain.size() >= item.size() &&
        domain.compare(domain.size() - item.size(), item.size(), item) == 0) {
      if (domain.size() == item.size() ||
          domain[domain.size() - item.size() - 1] == '.') {
        return true;
      }
    }
  }
  for (const auto &item : domainKeyword_) {
    if (domain.find(item) != std::string::npos) {
      return true;
    }
  }
  for (const auto &item : domainRegex_) {
    if (std::regex_match(domain.begin(), domain.end(), item)) {
      return true;
    }
  }
  return false;
}
Rule::Rule(std::vector<std::string> domain,
           std::vector<std::string> domainSuffix,
           std::vector<std::string> domainKeyword,
           std::vector<std::regex> domainRegex)
    : domain_(domain), domainSuffix_(domainSuffix),
      domainKeyword_(domainKeyword), domainRegex_(domainRegex) {}