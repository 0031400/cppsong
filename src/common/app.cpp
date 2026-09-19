#include "common/app.hpp"
#include "builder.hpp"
#include "config/config.hpp"
#include "connections/relay.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json/parse.hpp>
#include <fstream>
#include <memory>
#include <sstream>
#include <utility>

namespace {
std::string readFile(std::string filePath) {
  std::ifstream file(filePath);
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}
} // namespace
App::App(std::string_view configPath) : configPath_(configPath) {}
void App::run() {
  auto appConfig =
      paresAppConfig(json::parse(readFile(configPath_)).as_object());
  auto builder = Builder(io_, appConfig);
  auto dnsCenter = std::make_shared<DnsCenter>(builder.buildDnsCenter());
  dnsCenter->start();
  DnsCenter::instance = dnsCenter;
  for (auto item : appConfig.outbounds) {
    outbounds[item.tag] = builder.buildOutbound(item);
  }
  for (auto item : appConfig.inbounds) {
    auto inbound = builder.buildInbound(item);
    inbound->start();
    asio::co_spawn(io_, inboundWork_(std::move(inbound)), asio::detached);
  }
  io_.run();
}
async<void> App::inboundWork_(std::unique_ptr<Inbound> inbound) {
  try {
    while (true) {
      auto session = co_await inbound->session();
      asio::co_spawn(io_, handleSession_(std::move(session)), asio::detached);
    }
  } catch (const std::exception &e) {
    log_error("app inbound work", e);
  }
}
async<void> App::handleSession_(InTcpSession session) {
  try {
    auto outbound = outbounds["direct"];
    auto conn1 = std::move(session.conn);
    auto conn2 =
        co_await outbound->connect({session.address, session.firstData});
    co_await relay(io_, std::move(conn1), std::move(conn2));
  } catch (const std::exception &e) {
    log_error("app handle session", e);
  }
}