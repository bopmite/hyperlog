#include "segment_writer.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;
namespace hyperlog::wal
{

SegmentWriter::SegmentWriter(const std::string& dir, uint64_t max_bytes)
    : dir_(dir), max_bytes_(max_bytes), seg_id_(0), offset_(0), fd_(-1)
{
  fs::create_directories(dir_);
  open_seg();
}

SegmentWriter::~SegmentWriter()
{
  if (fd_ >= 0)
  {
    sync();
    close(fd_);
  }
}

std::string SegmentWriter::seg_path(uint64_t id) const
{
  std::ostringstream oss;
  oss << dir_ << "/segment_" << std::setw(6) << std::setfill('0') << id << ".log";
  return oss.str();
}

bool SegmentWriter::open_seg()
{
  std::string path = seg_path(seg_id_);
  fd_ = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
  return fd_ >= 0;
}

bool SegmentWriter::rotate()
{
  if (fd_ >= 0)
  {
    sync();
    close(fd_);
  }
  seg_id_++;
  offset_ = 0;
  return open_seg();
}

bool SegmentWriter::write(const void* data, size_t len)
{
  if (fd_ < 0 && !open_seg())
    return false;

  ssize_t written = ::write(fd_, data, len);
  if (written != static_cast<ssize_t>(len))
    return false;

  offset_ += len;

  if (offset_ >= max_bytes_)
    rotate();

  return true;
}

void SegmentWriter::flush()
{
}

void SegmentWriter::sync()
{
  if (fd_ >= 0)
    fdatasync(fd_);
}

}
