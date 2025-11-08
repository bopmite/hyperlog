#include "wal.hpp"
#include "record/record_header.hpp"
#include "record/record_footer.hpp"
#include "util/hash.hpp"
#include "util/crc32c.hpp"
#include "core/version.hpp"
#include <cstring>
#include <thread>

namespace hyperlog::wal
{

WAL::WAL(SyncMode mode, std::chrono::microseconds interval)
    : lsn_(0),
      sync_mode_(mode),
      group_interval_(interval),
      writer_("./data/wal"),
      running_(true)
{
  writer_thread_ = std::thread(&WAL::writer_loop, this);
}

WAL::~WAL()
{
  running_.store(false, std::memory_order_release);
  if (writer_thread_.joinable())
    writer_thread_.join();
}

void WAL::writer_loop()
{
  uint8_t buf[4096];
  size_t len;
  size_t batch_count = 0;

  auto last_sync = std::chrono::steady_clock::now();

  while (running_.load(std::memory_order_acquire))
  {
    bool wrote = false;

    while (ring_.read(buf, len))
    {
      writer_.write(buf, len);
      batch_count++;
      wrote = true;

      if (batch_count >= 1024)
        break;
    }

    if (wrote)
    {
      auto now = std::chrono::steady_clock::now();
      bool should_sync = false;

      if (sync_mode_ == SyncMode::EVERY)
        should_sync = true;
      else if (sync_mode_ == SyncMode::GROUP)
      {
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - last_sync);
        if (elapsed >= group_interval_)
          should_sync = true;
      }

      if (should_sync)
      {
        writer_.sync();
        last_sync = now;
        batch_count = 0;
      }
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
  }

  while (ring_.read(buf, len))
    writer_.write(buf, len);
  writer_.sync();
}

LSN WAL::append(const std::string& key,
                const void* data,
                uint32_t len,
                uint64_t rid,
                uint32_t crc,
                uint32_t stream_id)
{
  LSN lsn = next_lsn();

  static thread_local uint8_t buf[4096];
  size_t total = RecordOffsets::HEADER_SIZE + len + RecordOffsets::FOOTER_SIZE;

  RecordHeader hdr{};
  hdr.magic = RECORD_MAGIC;
  hdr.version = hyperlog::version::RECORD_VERSION;
  hdr.flags = FLAG_DEFAULT;
  hdr.stream_id = stream_id;
  hdr.rid_hash = rid;
  hdr.key_hash = hyperlog::hash::hash64(key);
  hdr.payload_len = len;
  hdr.reserved[0] = 0;
  hdr.reserved[1] = 0;
  hdr.header_crc = hyperlog::util::CRC32C::compute(&hdr, sizeof(hdr));

  RecordFooter ftr{};
  ftr.lsn = lsn;
  ftr.epoch_ns = static_cast<uint64_t>(
      std::chrono::duration_cast<std::chrono::nanoseconds>(
          std::chrono::steady_clock::now().time_since_epoch())
          .count());
  ftr.crc_header = hdr.header_crc;
  ftr.crc_payload = crc;
  ftr.flags = FLAG_NONE;
  ftr.pad = 0;
  ftr.crc_footer = hyperlog::util::CRC32C::compute(&ftr, sizeof(ftr));

  std::memcpy(buf, &hdr, sizeof(hdr));
  std::memcpy(buf + sizeof(hdr), data, len);
  std::memcpy(buf + sizeof(hdr) + len, &ftr, sizeof(ftr));

  while (!ring_.write(buf, total))
    std::this_thread::yield();

  return lsn;
}

}
