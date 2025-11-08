#pragma once
#include <atomic>
#include <cstring>
#include <cstdint>
#include <memory>

namespace hyperlog::wal
{

template<size_t NUM_SLOTS = 1024, size_t SLOT_SIZE = 4096>
class RingBuffer
{
public:
  RingBuffer() : write_idx_(0), read_idx_(0)
  {
    slots_ = new Slot[NUM_SLOTS];
    for (size_t i = 0; i < NUM_SLOTS; ++i)
    {
      slots_[i].ready.store(false, std::memory_order_relaxed);
      slots_[i].len = 0;
    }
  }

  ~RingBuffer()
  {
    delete[] slots_;
  }

  bool write(const void* data, size_t len)
  {
    if (len > SLOT_SIZE)
      return false;

    uint64_t idx = write_idx_.fetch_add(1, std::memory_order_acq_rel);
    uint64_t slot = idx % NUM_SLOTS;

    while (slots_[slot].ready.load(std::memory_order_acquire))
      ;

    std::memcpy(slots_[slot].data, data, len);
    slots_[slot].len = len;
    slots_[slot].ready.store(true, std::memory_order_release);

    return true;
  }

  bool read(void* data, size_t& len)
  {
    uint64_t ridx = read_idx_.load(std::memory_order_acquire);
    uint64_t widx = write_idx_.load(std::memory_order_acquire);

    if (ridx >= widx)
      return false;

    uint64_t slot = ridx % NUM_SLOTS;

    if (!slots_[slot].ready.load(std::memory_order_acquire))
      return false;

    len = slots_[slot].len;
    std::memcpy(data, slots_[slot].data, len);
    slots_[slot].ready.store(false, std::memory_order_release);

    read_idx_.fetch_add(1, std::memory_order_release);
    return true;
  }

  size_t pending() const
  {
    return write_idx_.load(std::memory_order_acquire) - read_idx_.load(std::memory_order_acquire);
  }

private:
  struct Slot
  {
    alignas(64) std::atomic<bool> ready;
    size_t len;
    uint8_t data[SLOT_SIZE];
  };

  alignas(64) std::atomic<uint64_t> write_idx_;
  alignas(64) std::atomic<uint64_t> read_idx_;
  Slot* slots_;
};

}
