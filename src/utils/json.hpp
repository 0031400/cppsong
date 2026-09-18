#pragma once
#include <boost/json.hpp>
#include "utils/headers.hpp"
namespace json = boost::json;
bool getBool(const json::object &obj, std::string_view key, bool def = false);
std::string getString(const json::object &obj, std::string_view key,
                      const std::string &def = "");
u16 getu16(const json::object &obj, std::string_view key, u16 def = 0);
std::vector<std::string> getStringList(const json::object &obj,
                                       std::string_view key);
json::object getObj(const json::object &obj, std::string_view key);
json::array getArray(const json::object &obj, std::string_view key);