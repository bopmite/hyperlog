# Hyperlog

High-performance write-ahead log with sub-microsecond latency

## Performance

```
100K ops | 1.3M ops/s | p50: 0.58µs | p99: 2.32µs
```

## Build

```
cmake -B build && cmake --build build
./hyperlog
```

## Benchmark

```
./scripts/bench.sh
```

## API

```
POST /append
{
  "key": "foo",
  "data": "bar"
}
```

Returns LSN, deduplicates by key+CRC32C
