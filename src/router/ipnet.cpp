#include "router/ipnet.hpp"
bool IpNetwork::contains(const ip::address &addr) const {
  if (network.is_v4() != addr.is_v4()) {
    return false;
  }
  if (network.is_v4()) {
    const auto net = network.to_v4().to_uint();
    const auto ipAddr = network.to_v4().to_uint();
    if (prefix > 32) {
      return false;
    }
    if (prefix == 0) {
      return true;
    }
    auto mask = 0xffffffffu << (32 - prefix);
    return (net & mask) == (ipAddr & mask);
  }
  const auto net = network.to_v6().to_bytes();
  const auto ipAddr = network.to_v6().to_bytes();
  const auto fullBytes = prefix / 8;
  const auto bits = prefix % 8;
  for (int i = 0; i < fullBytes; i++) {
    if (net[i] != ipAddr[i])
      return false;
  }
  if (bits != 0) {
    const auto mask = 0xffu << (8 - bits);
    if ((net[fullBytes] & mask) != (ipAddr[fullBytes] & mask)) {
      return false;
    }
  }
  return true;
}

IpNetwork parseCidr(std::string cidr) {
  const auto pos = cidr.find('/');
  if (pos == std::string::npos) {
    auto ipAddr = ip::make_address(cidr);
    if (ipAddr.is_v4()) {
      return IpNetwork{ipAddr, 32};
    } else {
      return IpNetwork{ipAddr, 128};
    }
  }
  auto ipStr = cidr.substr(0, pos);
  auto prefixStr = cidr.substr(pos + 1);
  auto ipAddr = ip::make_address(ipStr);
  auto prefix = std::stoul(prefixStr);
  if ((ipAddr.is_v4() && (prefix > 32 || prefix < 0)) ||
      (ipAddr.is_v6() && (prefix > 128 || prefix < 0))) {
    throw std::runtime_error("cidr error prefix");
  }
  return {ipAddr, static_cast<u8>(prefix)};
}