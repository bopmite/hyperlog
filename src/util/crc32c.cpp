#include "crc32c.hpp"
#include <cstddef>
#include <cstdint>
#include <immintrin.h>

namespace util
{
uint32_t CRC32C::compute(const void* data, size_t length)
{
  const uint8_t* ptr = static_cast<const uint8_t*>(data);
  uint64_t crc = 0xFFFFFFFFu;
#if defined(__SSE4_2__)
  size_t i = 0;
  for (; i + sizeof(uint64_t) <= length; i += sizeof(uint64_t))
    crc = _mm_crc32_u64(crc, *reinterpret_cast<const uint64_t*>(ptr + i));
  for (; i < length; ++i)
    crc = _mm_crc32_u8(crc, ptr[i]);
#else
  for (size_t i = 0; i < length; ++i)
    crc = (crc >> 8) ^ ((crc ^ ptr[i]) & 0xFF);
#endif
  return static_cast<uint32_t>(crc ^ 0xFFFFFFFFu);
}
}