#pragma once

#include <cstdint>
#include <array>

namespace hyperlog::version
{
  constexpr auto MAJOR = 1;
  constexpr auto MINOR = 0;
  constexpr auto PATCH = 0;
  constexpr auto API_VERSION = "v1";

  constexpr uint8_t RECORD_VERSION =
      (MAJOR << 4) | (MINOR << 2) | PATCH;
}
