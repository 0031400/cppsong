#include "common/app.hpp"
#include "builder.hpp"
#include "config/config.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/json/parse.hpp>
#include <fstream>
#include <sstream>
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
  auto dnsCenter = builder.buildDnsCenter();
  asio::co_spawn(io_, dnsCenter.start(), asio::detached);
  io_.run();
}