#pragma once

#include <httplib.h>
#include "util/id_gen.hpp"
#include "engine/cache/rid_cache.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <atomic>
#include <chrono>
#include "core/constants.hpp"
#include "util/crc32c.hpp"

namespace routes
{
  void HandleSingularAppend(const httplib::Request &, httplib::Response &res);
}