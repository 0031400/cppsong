#pragma once
#include "connection.hpp"
#include "utils/headers.hpp"
async<void> relay(asio::io_context &io, std::shared_ptr<Connection> conn1,
                  std::shared_ptr<Connection> conn2);