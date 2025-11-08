#pragma once
#include "engine/wal/record/record_types.hpp"
#include "segment_writer.hpp"
#include "ring_buffer.hpp"
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

namespace hyperlog::wal
{

enum class SyncMode
{
  NONE,
  EVERY,
  GROUP
};

class WAL
{
public:
  explicit WAL(SyncMode mode = SyncMode::GROUP,
               std::chrono::microseconds group_interval = std::chrono::microseconds(100));
  ~WAL();

  LSN append(const std::string& key,
             const void* data,
             uint32_t len,
             uint64_t rid,
             uint32_t crc,
             uint32_t stream_id);

private:
  std::atomic<uint64_t> lsn_;
  SyncMode sync_mode_;
  std::chrono::microseconds group_interval_;
  SegmentWriter writer_;
  RingBuffer<1024, 4096> ring_;
  std::thread writer_thread_;
  std::atomic<bool> running_;

  void writer_loop();
  LSN next_lsn() { return lsn_.fetch_add(1, std::memory_order_relaxed) + 1; }
};

}
