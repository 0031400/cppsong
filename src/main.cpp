#include "common/app.hpp"
#include <format>
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  if (argc == 4 && std::string(argv[1]) == "run" &&
      std::string(argv[2]) == "-c") {
    App app(argv[3]);
    app.run();
  }
  std::cout << std::format("usage: {} run -c <config file>\n", argv[0]);
}
