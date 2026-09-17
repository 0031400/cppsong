#include <exception>
#include <string_view>
void log(std::string_view title, std::string_view content);
void log_error(std::string_view message, const std::exception &e);