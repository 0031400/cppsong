#include "utils/bytes.hpp"
#include <stdexcept>
u8 read_u8(const bytes &buffer, std::size_t pos) {
  if (buffer.size() < pos + 1) {
    throw std::runtime_error("read out of range");
  }
  return buffer[pos];
}
u16 read_u16(const bytes &buffer, std::size_t pos) {
  auto data1 = read_u8(buffer, pos);
  auto data2 = read_u8(buffer, pos + 1);
  return (static_cast<u16>(data1) << 8) | static_cast<u16>(data2);
}
u32 read_u32(const bytes &buffer, std::size_t pos) {
  auto data1 = read_u16(buffer, pos);
  auto data2 = read_u16(buffer, pos + 2);
  return (static_cast<u32>(data1) << 16) | static_cast<u32>(data2);
}
u64 read_u64(const bytes &buffer, std::size_t pos) {
  auto data1 = read_u32(buffer, pos);
  auto data2 = read_u32(buffer, pos + 4);
  return (static_cast<u64>(data1) << 32) | static_cast<u64>(data2);
}
void write_u8(bytes &buffer, u8 value, int pos) {
  if (pos < 0) {
    buffer.push_back(value);
    return;
  }
  if (buffer.size() < pos + 1) {
    buffer.resize(pos + 1);
  }
  buffer[pos] = value;
}
void write_u16(bytes &buffer, u16 value, int pos) {
  auto data1 = static_cast<u8>((value >> 8) & 0xff);
  auto data2 = static_cast<u8>(value & 0xff);
  if (pos < 0) {
    write_u8(buffer, data1);
    write_u8(buffer, data2);
    return;
  }
  write_u8(buffer, data1, pos);
  write_u8(buffer, data2, pos + 1);
}
void write_u32(bytes &buffer, u32 value, int pos) {
  auto data1 = static_cast<u16>((value >> 16) & 0xffff);
  auto data2 = static_cast<u16>(value & 0xffff);
  if (pos < 0) {
    write_u16(buffer, data1);
    write_u16(buffer, data2);
    return;
  }
  write_u16(buffer, data1, pos);
  write_u16(buffer, data2, pos + 2);
}
void write_u64(bytes &buffer, u64 value, int pos) {
  auto data1 = static_cast<u32>((value >> 32) & 0xffffffff);
  auto data2 = static_cast<u32>(value & 0xffffffff);
  if (pos < 0) {
    write_u32(buffer, data1);
    write_u32(buffer, data2);
    return;
  }
  write_u32(buffer, data1, pos);
  write_u32(buffer, data2, pos + 4);
}