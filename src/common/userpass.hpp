#pragma once
#include <string>
struct UserPass {
  std::string username;
  std::string password;
  bool operator==(const UserPass &) const = default;
};