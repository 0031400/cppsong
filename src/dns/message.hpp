#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address.hpp>
#include <string>
#include <vector>
bytes build_dns_query(const std::string &domain, bool is_v6);
std::vector<ip::address> parse_dns_response(const bytes &packet);