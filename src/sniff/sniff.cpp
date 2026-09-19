#include "sniff.hpp"
std::string sniffDomain(bytes data) {
  if (data.size() < 5)
    return {};

  if (data[0] != 0x16 || data[1] != 0x03)
    return {};

  const size_t recordLen = (static_cast<size_t>(data[3]) << 8) | data[4];

  if (recordLen + 5 > data.size())
    return {};

  size_t p = 5;

  if (p + 4 > data.size())
    return {};

  if (data[p] != 0x01)
    return {};

  const size_t helloLen = (static_cast<size_t>(data[p + 1]) << 16) |
                          (static_cast<size_t>(data[p + 2]) << 8) | data[p + 3];

  p += 4;

  if (p + helloLen > data.size())
    return {};

  if (p + 2 + 32 > data.size())
    return {};

  p += 2;
  p += 32;

  if (p >= data.size())
    return {};

  const uint8_t sessionIdLen = data[p++];

  if (p + sessionIdLen > data.size())
    return {};

  p += sessionIdLen;

  if (p + 2 > data.size())
    return {};

  const size_t cipherLen = (static_cast<size_t>(data[p]) << 8) | data[p + 1];

  p += 2;

  if (p + cipherLen > data.size())
    return {};

  p += cipherLen;

  if (p >= data.size())
    return {};

  const uint8_t compressionLen = data[p++];

  if (p + compressionLen > data.size())
    return {};

  p += compressionLen;

  if (p + 2 > data.size())
    return {};

  const size_t extensionsLen =
      (static_cast<size_t>(data[p]) << 8) | data[p + 1];

  p += 2;

  if (p + extensionsLen > data.size())
    return {};

  const size_t end = p + extensionsLen;

  while (p + 4 <= end) {
    const uint16_t type = (static_cast<uint16_t>(data[p]) << 8) | data[p + 1];

    const size_t len = (static_cast<size_t>(data[p + 2]) << 8) | data[p + 3];

    p += 4;

    if (p + len > end)
      return {};

    if (type == 0x0000) {
      if (len < 2)
        return {};

      size_t q = p;

      const size_t listLen = (static_cast<size_t>(data[q]) << 8) | data[q + 1];

      q += 2;

      if (q + listLen > p + len)
        return {};

      while (q + 3 <= p + len) {
        const uint8_t nameType = data[q++];

        const size_t nameLen =
            (static_cast<size_t>(data[q]) << 8) | data[q + 1];

        q += 2;

        if (q + nameLen > p + len)
          return {};

        if (nameType == 0) {
          return std::string(reinterpret_cast<const char *>(&data[q]), nameLen);
        }

        q += nameLen;
      }

      return {};
    }

    p += len;
  }

  return {};
}