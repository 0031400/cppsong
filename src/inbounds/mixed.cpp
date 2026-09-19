#include "inbounds/mixed.hpp"
#include "common/address.hpp"
#include "common/userpass.hpp"
#include "connections/connection.hpp"
#include "inbound.hpp"
#include "utils/bytes.hpp"
#include <algorithm>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <string>
#include <utility>
MixedInbound::MixedInbound(asio::io_context &io,
                           std::unique_ptr<Listener> listener,
                           std::vector<UserPass> users,
                           std::vector<bytes> uuids)
    : Inbound(io), io_(io), listener_(std::move(listener)), users_(users),
      uuids_(uuids) {}

void MixedInbound::start() {
  listener_->start();
  asio::co_spawn(io_, mainWork_(), asio::detached);
}
async<void> MixedInbound::mainWork_() {
  while (true) {
    auto session = co_await listener_->session();
    asio::co_spawn(io_, handle_(std::move(session)), asio::detached);
  }
}
async<void> MixedInbound::handle_(std::unique_ptr<Connection> conn) {
  auto data = co_await conn->read_exactly(1);
  if (data[0] == 5) {
    co_await handleSocks5_(std::move(conn));
    co_return;
  }
  co_await handleHttp_(std::move(conn));
}

async<void> MixedInbound::handleSocks5_(std::unique_ptr<Connection> conn) {
  auto data = co_await conn->read_exactly(1);
  auto nmethods = data[0];
  data = co_await conn->read_exactly(nmethods);
  if (users_.empty()) {
    if (std::find(data.begin(), data.end(), 0) != data.end()) {
      co_await conn->write({5, 0});
    } else {
      co_await conn->write({5, 0xff});
      co_await conn->close();
      co_return;
    }
  } else {
    co_await conn->write({5, 2});
    data = co_await conn->read_exactly(2);
    if (data[0] != 1) {
      throw std::runtime_error("fail to auth");
    }
    data = co_await conn->read_exactly(data[1]);
    auto username = std::string(data.begin(), data.end());
    data = co_await conn->read_exactly(1);
    data = co_await conn->read_exactly(data[1]);
    auto passwd = std::string(data.begin(), data.end());
    if (std::find(users_.begin(), users_.end(), UserPass{username, passwd}) !=
        users_.end()) {
      co_await conn->write({1, 0});
    } else {
      co_await conn->write({1, 1});
      co_await conn->close();
      co_return;
    }
  }
  data = co_await conn->read_exactly(4);
  if (data[0] != 5) {
    throw std::runtime_error("socks5 version error");
  }
  auto cmd = data[1];
  if (cmd != 1) {
    throw std::runtime_error("cmd error");
  }
  auto atyp = data[3];
  Address address;
  if (atyp == 1) {
    data = co_await conn->read_exactly(4);
    address.address = ip::address_v4({data[0], data[1], data[2], data[3]});
  } else if (atyp == 3) {
    data = co_await conn->read_exactly(1);
    data = co_await conn->read_exactly(data[0]);
    address.address = std::string(data.begin(), data.end());
  } else if (atyp == 4) {
    data = co_await conn->read_exactly(16);
    address.address =
        ip::address_v6({data[0], data[1], data[2], data[3], data[4], data[5],
                        data[6], data[7], data[8], data[9], data[10], data[11],
                        data[12], data[13], data[14], data[15]});
  } else {
    throw std::runtime_error("atyp error");
  }
  data = co_await conn->read_exactly(2);
  address.port = read_u16(data, 0);
  co_await conn->write({5, 0, 0, 1, 0, 0, 0, 0, 0, 0});
  data = co_await conn->read(4096);
  co_await sessions_.put({std::move(conn), address, data});
  co_return;
}
async<void> MixedInbound::handleHttp_(std::unique_ptr<Connection> session) {
  co_return;
}
void MixedInbound::close() { listener_->close(); }