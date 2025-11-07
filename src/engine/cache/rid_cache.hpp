#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

namespace hyperlog::rid_cache
{
  class RID_Cache
  {
  public:
    struct CacheResponse
    {
      bool found;
      uint64_t value;
    };

    void set(const std::string& key, uint64_t value);
    CacheResponse get(const std::string& key);

  private:
    std::unordered_map<std::string, uint64_t> dict_;
    mutable std::shared_mutex mu_;
  };
}