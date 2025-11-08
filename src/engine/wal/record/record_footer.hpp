#pragma once
#include <cstdint>

namespace hyperlog::wal
{

  struct RecordFooter
  {
    uint64_t lsn;
    uint64_t epoch_ns;
    uint32_t crc_payload;
    uint32_t crc_header;
    uint32_t crc_footer;
    uint16_t flags;
    uint16_t pad;
  } __attribute__((packed));

  enum FooterFlags : uint16_t
  {
    FLAG_NONE = 0,
    FLAG_END_OF_SEGMENT = 1 << 0,
    FLAG_SYNC_FENCE = 1 << 1,
    FLAG_TRUNCATED = 1 << 2
  };

}