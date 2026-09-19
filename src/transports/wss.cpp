#include "transports/wss.hpp"
#include "connections/wss.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include "utils/log.hpp"
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <memory>
#include <openssl/tls1.h>
#include <stdexcept>
#include <utility>

WssTransport::WssTransport(asio::io_context &io, std::string host,
                           std::string path, std::string serverName,
                           bool insecure)
    : io_(io), host_(host), path_(path), serverName_(serverName),
      insecure_(insecure), ctx_(ssl::context::tls_client) {
  ctx_.set_verify_mode(ssl::verify_none);
  ctx_.set_default_verify_paths();
}
async<std::unique_ptr<Connection>> WssTransport::connect(Address address) {
  auto socket = co_await connect_address(io_, address);
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws(
      beast::tcp_stream(std::move(socket)), ctx_);
  if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(),
                                serverName_.c_str())) {
    throw std::runtime_error("fail to set tls hostname");
  }
  co_await ws.next_layer().async_handshake(ssl::stream_base::client,
                                           asio::use_awaitable);
  co_await ws.async_handshake(host_, path_, asio::use_awaitable);
  co_return std::make_unique<WssConnection>(std::move(ws));
}