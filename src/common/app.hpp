#pragma once
#include "utils/headers.hpp"
#include <string>
#include <string_view>
class App {
public:
  explicit App(std::string_view configPath);
  void run();

private:
  std::string configPath_;
  asio::io_context io_;
};