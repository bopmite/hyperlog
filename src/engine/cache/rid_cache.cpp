#include "rid_cache.hpp"

namespace hyperlog::rid_cache
{
  void RID_Cache::set(const std::string& key, uint64_t value)
  {
    std::unique_lock lock(mu_);
    dict_[key] = value;
  }

  RID_Cache::CacheResponse RID_Cache::get(const std::string& key)
  {
    std::shared_lock lock(mu_);
    auto it = dict_.find(key);
    if (it != dict_.end())
      return {true, it->second};
    return {false, 0};
  }
}