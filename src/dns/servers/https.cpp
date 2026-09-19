#include "dns/servers/https.hpp"
#include "tcp/tcpclient.hpp"
#include "utils/bytes.hpp"
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/message_fwd.hpp>
#include <boost/beast/http/vector_body.hpp>
#include <boost/beast/http/verb.hpp>
HttpsDnsServer::HttpsDnsServer(asio::io_context &io, tcp::endpoint endpoint,
                               std::string host, std::string path,
                               std::string serverName, bool insecure)
    : io_(io), endpoint_(endpoint), host_(host), path_(path),
      serverName_(serverName), insecure_(insecure),
      ctx_(ssl::context::tls_client) {
  ctx_.set_verify_mode(ssl::verify_none);
  ctx_.set_default_verify_paths();
}

async<bytes> HttpsDnsServer::relay(bytes data) {
  auto socket = co_await connect_ip(io_, endpoint_.address(), endpoint_.port());
  beast::ssl_stream<beast::tcp_stream> stream(
      beast::tcp_stream(std::move(socket)), ctx_);
  if (!SSL_set_tlsext_host_name(stream.native_handle(), serverName_.c_str())) {
    throw std::runtime_error("fail to set tls hostname");
  }
  co_await stream.async_handshake(ssl::stream_base::client,
                                  asio::use_awaitable);
  http::request<http::vector_body<u8>> req{http::verb::post, path_, 11};
  req.set(http::field::host, host_);
  req.set(http::field::content_type, "application/dns-message");
  req.set(http::field::accept, "application/dns-message");
  req.body() = data;
  req.prepare_payload();
  co_await http::async_write(stream, req, asio::use_awaitable);
  http::response<http::vector_body<u8>> res;
  beast::flat_buffer buffer;
  co_await http::async_read(stream, buffer, res, asio::use_awaitable);
  co_return res.body();
}