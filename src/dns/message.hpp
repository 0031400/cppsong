#pragma once
#include "utils/headers.hpp"
#include <string>
#include <vector>
bytes build_dns_query(const std::string &domain, bool is_v6);
std::string parse_dns_query(const bytes &data);
std::vector<ip::address> parse_dns_response(const bytes &packet);