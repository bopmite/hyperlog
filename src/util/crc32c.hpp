#pragma once
#include <cstddef>
#include <cstdint>

namespace hyperlog::util
{
  class CRC32C
  {
  public:
    static uint32_t compute(const void *data, size_t length);
  };
}