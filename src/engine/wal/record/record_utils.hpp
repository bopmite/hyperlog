#pragma once
#include "record_header.hpp"
#include "record_footer.hpp"
#include "util/crc32c.hpp"
#include "record_types.hpp"

namespace hyperlog::wal
{

  inline bool validate_header(const RecordHeader &h, uint32_t crc)
  {
    return util::CRC32C::compute(&h, sizeof(RecordHeader) - sizeof(uint16_t)) == crc;
  }

  inline bool validate_footer(const RecordFooter &f, uint32_t total_crc)
  {
    return f.crc_footer == total_crc;
  }

  inline size_t record_size(uint32_t payload_len)
  {
    return RecordOffsets::HEADER_SIZE + payload_len + RecordOffsets::FOOTER_SIZE;
  }

}