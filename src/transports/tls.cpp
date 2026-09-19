#include "transports/tls.hpp"
#include "connections/tcp.hpp"
#include "connections/tls.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/headers.hpp"
#include <memory>
#include <utility>
TlsTransport::TlsTransport(asio::io_context &io, std::string serverName,
                           bool insecure)
    : io_(io), serverName_(serverName), insecure_(insecure),
      ctx_(ssl::context::tls_client) {
  ctx_.set_verify_mode(ssl::verify_none);
  ctx_.set_default_verify_paths();
}
async<std::unique_ptr<Connection>> TlsTransport::connect(Address address) {
  auto socket = co_await connect_address(io_, address);
  beast::ssl_stream<beast::tcp_stream> stream(
      beast::tcp_stream(std::move(socket)), ctx_);
  if (!SSL_set_tlsext_host_name(stream.native_handle(), serverName_.c_str())) {
    throw std::runtime_error("fail to set tls hostname");
  }
  co_await stream.async_handshake(ssl::stream_base::client, asio::use_awaitable);
  co_return std::make_unique<TlsConnection>(std::move(stream));
}