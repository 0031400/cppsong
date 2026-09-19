#include "transports/wss.hpp"
#include "connections/wss.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/ssl/verify_mode.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <memory>
#include <utility>
WssTransport::WssTransport(asio::io_context &io, std::string host,
                           std::string path)
    : io_(io), host_(host), path_(path) {}
async<std::unique_ptr<Connection>> WssTransport::connect(Address address) {
  auto socket = co_await connect_address(io_, address);
  ssl::context ctx(ssl::context::tls_client);
  ctx.set_verify_mode(ssl::verify_none);
  ctx.set_default_verify_paths();
  websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws(std::move(socket),
                                                             ctx);
  co_await ws.next_layer().async_handshake(ssl::stream_base::client,
                                           asio::use_awaitable);
  co_await ws.async_handshake(host_, path_, asio::use_awaitable);
  co_return std::make_unique<WssConnection>(std::move(ws));
}