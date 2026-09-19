#include "dns/cache.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <chrono>
#include <format>

DnsCache::DnsCache(asio::io_context &io) : io_(io) {}
std::vector<ip::address> DnsCache::lookup(std::string domain) {
  auto it = data_.find(domain);
  if (it == data_.end()) {
    return {};
  }

  log("dns cache", std::format("lookup {}", it->first));
  it->second.lastAccess = std::chrono::steady_clock::now();
  return it->second.ips;
}
void DnsCache::store(std::string domain, std::vector<ip::address> ips) {
  log("dns cache", std::format("store {}", domain));
  data_[domain] = {std::chrono::steady_clock::now(), ips};
}
void DnsCache::start() { asio::co_spawn(io_, mainWork_(), asio::detached); }
async<void> DnsCache::mainWork_() {
  while (true) {
    auto timer = asio::steady_timer(io_);
    timer.expires_after(expTime_);
    co_await timer.async_wait(asio::use_awaitable);
    auto now = std::chrono::steady_clock::now();
    for (auto it = data_.begin(); it != data_.end();) {
      if (now - it->second.lastAccess > expTime_) {
        log("dns cache", std::format("erase {}", it->first));
        it = data_.erase(it);
      } else {
        it++;
      }
    }
  }
}