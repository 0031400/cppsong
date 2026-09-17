#include "dns/dns.hpp"
#include <boost/asio/ip/address.hpp>
async<std::vector<ip::address>> resolve(std::string_view domain) {
  co_return std::vector<ip::address>{
      ip::make_address("220.181.111.232"), ip::make_address("220.181.111.1"),
      ip::make_address("240e:83:205:381:0:ff:b00f:96a2"),
      ip::make_address("240e:83:205:1cd:0:ff:b0b8:dee9")};
}