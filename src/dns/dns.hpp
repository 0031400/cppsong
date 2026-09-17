#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address.hpp>
#include <string_view>
#include <vector>
async<std::vector<ip::address>> resolve(std::string_view domain);