#include "dns/servers/https.hpp"
HttpsDnsServer::HttpsDnsServer(asio::io_context &io, tcp::endpoint endpoint,
                               std::string host, std::string path,
                               std::string serverName, bool insecure)
    : io_(io), endpoint_(endpoint), host_(host), path_(path),
      serverName_(serverName), insecure_(insecure) {}

async<bytes> HttpsDnsServer::relay(bytes data) { co_return bytes{}; }