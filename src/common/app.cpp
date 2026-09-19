#include "common/app.hpp"
#include "builder.hpp"
#include "config/config.hpp"
#include "connections/relay.hpp"
#include "sniff/sniff.hpp"
#include "utils/file.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json/parse.hpp>
#include <format>
#include <memory>
#include <utility>

App::App(std::string_view configPath) : configPath_(configPath) {}
void App::run() {
  auto appConfig =
      paresAppConfig(json::parse(readFile(configPath_)).as_object());
  auto builder = Builder(io_, appConfig);
  auto dnsCenter = std::make_shared<DnsCenter>(builder.buildDnsCenter());
  dnsCenter->start();
  DnsCenter::instance = dnsCenter;
  router_ = builder.buildRouter();
  for (auto item : appConfig.outbounds) {
    outbounds[item.tag] = builder.buildOutbound(item);
  }
  for (auto item : appConfig.inbounds) {
    auto inbound = builder.buildInbound(item);
    inbound->start();
    log("inbound", std::format("{} start", item.tag));
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
    auto sniffed = sniffDomain(session.firstData);
    if (!sniffed.empty()) {
      auto oldAddress = session.address;
      session.address.address = sniffed;
      log("sniff", std::format("{} -> {}", oldAddress.toString(),
                               session.address.toString()));
    }
    auto outboundTag = router_->match(session.address);
    log("route",
        std::format("{} -> {}", session.address.toString(), outboundTag));
    auto outbound = outbounds[outboundTag];
    auto conn1 = std::move(session.conn);
    auto conn2 =
        co_await outbound->connect({session.address, session.firstData});
    co_await relay(io_, std::move(conn1), std::move(conn2));
  } catch (const std::exception &e) {
    log_error("app handle session", e);
  }
}