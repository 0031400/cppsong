#include "inbounds/inbound.hpp"
Inbound::Inbound(asio::io_context &io) : sessions_(io) {}
async<InTcpSession> Inbound::session() { co_return co_await sessions_.get(); }