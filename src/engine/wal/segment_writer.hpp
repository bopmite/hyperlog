#pragma once
#include <cstdint>
#include <string>
#include <atomic>

namespace hyperlog::wal
{
class SegmentWriter
{
public:
  explicit SegmentWriter(const std::string& dir, uint64_t max_bytes = 1ULL << 30);
  ~SegmentWriter();

  bool write(const void* data, size_t len);
  void flush();
  void sync();

private:
  std::string dir_;
  uint64_t max_bytes_;
  uint64_t seg_id_;
  uint64_t offset_;
  int fd_;

  std::string seg_path(uint64_t id) const;
  bool open_seg();
  bool rotate();
};
}
