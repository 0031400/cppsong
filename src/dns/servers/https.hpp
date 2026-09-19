#pragma once
#include "dns/servers/server.hpp"
#include "utils/headers.hpp"
#include <boost/asio/io_context.hpp>

class HttpsDnsServer : public DnsServer {
public:
  explicit HttpsDnsServer(asio::io_context &io, tcp::endpoint endpoint,
                          std::string host, std::string path,
                          std::string serverName, bool insecure);
  async<bytes> relay(bytes data);

private:
  asio::io_context &io_;
  tcp::endpoint endpoint_;
  std::string host_;
  std::string path_;
  std::string serverName_;
  bool insecure_;
};