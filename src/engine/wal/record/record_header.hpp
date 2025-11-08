#pragma once
#include <cstdint>
#include <array>

namespace hyperlog::wal
{

  struct RecordHeader
  {
    uint32_t magic;
    uint8_t version;
    uint8_t flags;
    uint16_t header_crc;
    uint32_t stream_id;
    uint64_t rid_hash;
    uint64_t key_hash;
    uint32_t payload_len;
    std::array<uint8_t, 2> reserved;
  } __attribute__((packed));

  constexpr uint32_t RECORD_MAGIC = 0x48C0DE01;

}