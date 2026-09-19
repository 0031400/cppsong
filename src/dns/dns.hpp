#pragma once
#include "cache.hpp"
#include "rule.hpp"
#include "servers/server.hpp"
#include "udp/udpclient.hpp"
#include "udp/udpsession.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <vector>
class DnsCenter {
public:
  explicit DnsCenter(
      asio::io_context &io, std::optional<udp::endpoint> endpoint,
      std::vector<DnsRule> rules,
      std::unordered_map<std::string, std::unique_ptr<DnsServer>> servers,
      std::string final);
  void start();
  void stop();
  async<bytes> relay(bytes data);
  inline static std::shared_ptr<DnsCenter> instance = nullptr;
  static async<std::vector<ip::address>> resolve(std::string domain);

private:
  std::string match_(std::string_view domain);
  async<void> mainWork_();
  async<void> handleClient_(UdpSession session);
  asio::io_context &io_;
  std::optional<udp::endpoint> endpoint_;
  std::vector<DnsRule> rules_;
  std::unordered_map<std::string, std::unique_ptr<DnsServer>> servers_;
  std::string final_;
  std::optional<UdpClient> client_;
  DnsCache cache_;
};