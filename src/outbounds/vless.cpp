#include "outbounds/vless.hpp"
#include "utils/bytes.hpp"
#include "utils/log.hpp"
#include <boost/asio/ip/address.hpp>
#include <memory>
#include <stdexcept>
#include <utility>
#include <variant>

VlessOutbound::VlessOutbound(asio::io_context &io,
                             std::unique_ptr<Transport> transport,
                             Address server, bytes uuid)
    : io_(io), transport_(std::move(transport)), server_(server), uuid_(uuid) {}
async<std::unique_ptr<Connection>>
VlessOutbound::connect(OutTcpSession session) {
  auto conn = co_await transport_->connect(server_);
  auto requestData = bytes{0};
  requestData.append_range(uuid_);
  requestData.append_range(bytes{0, 1});
  write_u16(requestData, session.address.port);
  if (std::holds_alternative<std::string>(session.address.address)) {
    auto value = std::get<std::string>(session.address.address);
    auto domainData = bytes(value.begin(), value.end());
    write_u8(requestData, 2);
    write_u8(requestData, static_cast<u8>(value.size()));
    requestData.append_range(domainData);
  } else {
    auto value = std::get<ip::address>(session.address.address);
    if (value.is_v4()) {
      write_u8(requestData, 1);
      requestData.append_range(value.to_v4().to_bytes());
    } else {
      write_u8(requestData, 3);
      requestData.append_range(value.to_v6().to_bytes());
    }
  }
  requestData.append_range(session.firstData);
  co_await conn->write(requestData);
  auto data = co_await conn->read_exactly(2);
  if (data[0] != 0 || data[1] != 0) {
    throw std::runtime_error("vless fail to handshake");
  }
  co_return conn;
}