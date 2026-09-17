#pragma once
#include "common/address.hpp"
#include "utils/headers.hpp"

async<tcp::socket> connect_ip(asio::io_context &io, ip::address ipAddress,
                              u16 port);
async<tcp::socket> connect_address(asio::io_context &io, Address address);
