#include "engine/wal/wal.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>

using namespace std::chrono;
using namespace hyperlog::wal;

int main()
{
  system("rm -rf ./data/wal && mkdir -p ./data/wal");

  WAL wal(SyncMode::NONE);
  std::vector<char> payload(100, 'x');
  std::vector<double> lat;
  size_t ops = 100000;

  auto t0 = high_resolution_clock::now();
  for (size_t i = 0; i < ops; ++i)
  {
    auto s = high_resolution_clock::now();
    wal.append("key", payload.data(), 100, i, 0xDEADBEEF, 1);
    lat.push_back(duration_cast<nanoseconds>(high_resolution_clock::now() - s).count() / 1000.0);
  }
  double sec = duration_cast<nanoseconds>(high_resolution_clock::now() - t0).count() / 1e9;

  std::sort(lat.begin(), lat.end());
  double p50 = lat[lat.size() / 2];
  double p99 = lat[static_cast<size_t>(lat.size() * 0.99)];
  double mean = std::accumulate(lat.begin(), lat.end(), 0.0) / lat.size();

  printf("100K ops | %.1fM ops/s | p50: %.2fµs | p99: %.2fµs | mean: %.2fµs\n",
         ops / sec / 1e6, p50, p99, mean);

  return 0;
}
