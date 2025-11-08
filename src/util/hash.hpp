#pragma once
#include <string>

namespace hyperlog::hash
{
  inline uint64_t hash64(const std::string &s)
  {
    uint64_t h = 1469598103934665603ULL;
    for (auto c : s)
    {
      h ^= static_cast<uint8_t>(c);
      h *= 1099511628211ULL;
    }
    return h;
  }
}