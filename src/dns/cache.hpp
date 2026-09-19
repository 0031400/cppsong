#pragma once
#include "utils/headers.hpp"
#include <boost/asio/ip/address.hpp>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
struct DnsCacheItem {
  std::chrono::steady_clock::time_point lastAccess;
  std::vector<ip::address> ips;
};
class DnsCache {
public:
  explicit DnsCache(asio::io_context &io);
  std::vector<ip::address> lookup(std::string domain);
  void store(std::string domain, std::vector<ip::address> ips);
  void start();

private:
  async<void> mainWork_();
  asio::io_context &io_;
  std::unordered_map<std::string, DnsCacheItem> data_;
  std::chrono::hours expTime_{1};
};