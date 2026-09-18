#pragma once
#include <regex>
#include <string>
#include <vector>

class Rule {
public:
  explicit Rule(std::vector<std::string> domain,
                std::vector<std::string> domainSuffix,
                std::vector<std::string> domainKeyword,
                std::vector<std::regex> domainRegex);
  bool matchDomain(std::string_view domain);

private:
  std::vector<std::string> domain_;
  std::vector<std::string> domainSuffix_;
  std::vector<std::string> domainKeyword_;
  std::vector<std::regex> domainRegex_;
};