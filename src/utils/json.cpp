#include "utils/json.hpp"
bool getBool(const json::object &obj, std::string_view key, bool def) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return def;
  }
  return it->value().as_bool();
}
std::string getString(const json::object &obj, std::string_view key,
                      const std::string &def) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return def;
  }
  return std::string(it->value().as_string());
}
u16 getu16(const json::object &obj, std::string_view key, u16 def) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return def;
  }
  auto value = it->value().as_int64();
  if (value > 0xffff) {
    throw std::runtime_error("parse json u16 fail");
  }
  return static_cast<u16>(value);
}
std::vector<std::string> getStringList(const json::object &obj,
                                       std::string_view key) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return {};
  }
  auto value = it->value();
  if (value.is_string()) {
    return {std::string(value.as_string())};
  } else if (value.is_array()) {
    std::vector<std::string> res;
    for (const auto &item : value.as_array()) {
      res.emplace_back(item.as_string());
    }
    return res;
  }
  return {};
}
json::object getObj(const json::object &obj, std::string_view key) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return {};
  }
  return it->value().as_object();
}
json::array getArray(const json::object &obj, std::string_view key) {
  auto it = obj.find(key);
  if (it == obj.end()) {
    return {};
  }
  return it->value().as_array();
}