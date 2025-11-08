#pragma once
#include <cstdint>
#include <cstddef>

namespace hyperlog::wal
{

  using LSN = uint64_t;
  using Epoch = uint64_t;

  enum RecordFlags : uint8_t
  {
    FLAG_DEFAULT = 0,
    FLAG_COMPRESSED = 1 << 0,
    FLAG_ENCRYPTED = 1 << 1
  };

  struct RecordOffsets
  {
    static constexpr size_t HEADER_SIZE = 32;
    static constexpr size_t FOOTER_SIZE = 64;
  };

}