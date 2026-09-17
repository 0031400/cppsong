#pragma once
#include "utils/headers.hpp"
#include <cstddef>
u8 read_u8(const bytes &buffer, std::size_t pos);
u16 read_u16(const bytes &buffer, std::size_t pos);
u32 read_u32(const bytes &buffer, std::size_t pos);
u64 read_u64(const bytes &buffer, std::size_t pos);
void write_u8(bytes &buffer, u8 value, int pos = -1);
void write_u16(bytes &buffer, u16 value, int pos = -1);
void write_u32(bytes &buffer, u32 value, int pos = -1);
void write_u64(bytes &buffer, u64 value, int pos = -1);