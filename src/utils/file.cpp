#include "utils/file.hpp"
#include <fstream>
#include <sstream>
std::string readFile(std::string filePath) {
  std::ifstream file(filePath);
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}