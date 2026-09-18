#include "dns/message.hpp"
#include "utils/bytes.hpp"
#include <algorithm>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <random>
#include <stdexcept>
#include <string_view>
#include <vector>
namespace {
u16 randomId() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<u16> dist(0, 0xffff);
  return dist(gen);
}
bytes encodeQname(std::string_view domain) {
  if (domain.empty()) {
    throw std::runtime_error("domain is empty");
  }
  if (domain.size() > 255) {
    throw std::runtime_error("domain is too long");
  }
  if (domain.back() == '.') {
    domain.remove_suffix(1);
  }
  if (domain.empty()) {
    throw std::runtime_error("domain is invalid");
  }
  std::size_t start = 0;
  bytes buffer;
  while (start < domain.size()) {
    auto dot = domain.find('.', start);
    std::size_t end;
    if (dot == std::string::npos) {
      end = domain.size();
    } else {
      end = dot;
    }
    auto labelLength = end - start;
    if (labelLength == 0 || labelLength > 63) {
      throw std::runtime_error("label is invalid");
    }
    write_u8(buffer, static_cast<u8>(labelLength));
    for (auto i = start; i < end; i++) {
      write_u8(buffer, static_cast<u8>(domain[i]));
    }
    if (dot == std::string::npos) {
      break;
    }
    start = dot + 1;
  }
  write_u8(buffer, 0);
  return buffer;
}
std::size_t skipName(const bytes &packet, std::size_t offset) {
  while (true) {
    if (offset >= packet.size()) {
      throw std::runtime_error("invalid dns name");
    }
    u8 len = packet[offset];
    if ((len & 0xc0) == 0xc0) {
      if (offset + 2 > packet.size()) {
        throw std::runtime_error("invalid dns pointer");
      }
      return offset + 2;
    }
    if (len == 0) {
      return offset + 1;
    }
    if (len > 63) {
      throw std::runtime_error("invalid dns label");
    }
    offset += len + 1;
  }
}
} // namespace
bytes build_dns_query(const std::string &domain, bool is_v6) {
  bytes packet;
  packet.reserve(512);
  write_u16(packet, randomId());
  write_u16(packet, 0x0100);
  write_u16(packet, 1);
  write_u16(packet, 0);
  write_u16(packet, 0);
  write_u16(packet, 0);
  auto qname = encodeQname(domain);
  packet.insert(packet.end(), qname.begin(), qname.end());
  if (is_v6) {
    write_u16(packet, 28);
  } else {
    write_u16(packet, 1);
  }
  write_u16(packet, 1);
  return packet;
}
std::vector<ip::address> parse_dns_response(const bytes &packet) {
  std::vector<ip::address> result;
  if (packet.size() < 12) {
    throw std::runtime_error("dns packet too short");
  }
  const auto qdcount = read_u16(packet, 4);
  const auto ancount = read_u16(packet, 6);
  std::size_t offset = 12;
  for (u16 i = 0; i < qdcount; i++) {
    offset = skipName(packet, offset);
    if (offset + 4 > packet.size()) {
      throw std::runtime_error("invalid dns question");
    }
    offset += 4;
  }
  for (u16 i = 0; i < ancount; i++) {
    offset = skipName(packet, offset);
    if (offset + 10 > packet.size()) {
      throw std::runtime_error("invalid dns answer");
    }
    const auto type = read_u16(packet, offset);
    offset += 2;
    const auto klass = read_u16(packet, offset);
    offset += 2;
    offset += 4;
    const auto rdlength = read_u16(packet, offset);
    offset += 2;
    if (offset + rdlength > packet.size()) {
      throw std::runtime_error("invalid dns rdata");
    }
    if (type == 1 && klass == 1 && rdlength == 4) {
      ip::address_v4::bytes_type addr{};
      std::copy_n(packet.begin() + offset, 4, addr.begin());
      result.emplace_back(ip::address_v4(addr));
    }
    if (type == 28 && klass == 1 && rdlength == 16) {
      ip::address_v6::bytes_type addr{};
      std::copy_n(packet.begin() + offset, 16, addr.begin());
      result.emplace_back(ip::address_v6(addr));
    }
    offset += rdlength;
  }
  return result;
}
std::string parse_dns_query(const bytes &data) {
  if (data.size() < 12) {
    throw std::runtime_error("invalid dns packet");
  }
  std::size_t pos = 12;
  std::string domain;
  while (true) {
    if (pos >= data.size()) {
      throw std::runtime_error("invalid dns name");
    }
    auto len = data[pos++];
    if (len == 0) {
      break;
    }
    if (pos + len > data.size()) {
      throw std::runtime_error("invalid dns label");
    }
    if (!domain.empty()) {
      domain += '.';
    }
    domain.append(reinterpret_cast<const char *>(&data[pos]), len);
    pos += len;
  }
  return domain;
}