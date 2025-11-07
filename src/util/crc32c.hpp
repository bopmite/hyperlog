#pragma once
#include <cstddef>
#include <cstdint>

namespace util
{
class CRC32C
{
 public:
  static uint32_t compute(const void* data, size_t length);
};
}