#include "utils/log.hpp"
#include <boost/system/system_error.hpp>
#include <cmath>
#include <format>
#include <iostream>
void log(std::string_view title, std::string_view content) {
  std::cout << std::format("[{}] {}\n", title, content);
}
void log_error(std::string_view message, const std::exception &e) {
  if (const auto *se = dynamic_cast<const boost::system::system_error *>(&e)) {
    const auto ec = se->code();
    log("error", std::format("{} {}", message, ec.message()));
  }
}