# Hyperlog

Hyperlog is a minimal, high‑performance write‑ahead log server for production use

### Current State

Only one endpoint exists right now:

````md
POST /append
{
"key": "foo",
"data": "bar"
}```

What it does:

- Computes a CRC32C checksum of `data`
- Checks if `key` already exists
  - Same checksum → `201 Created` (“already stored”)
  - Different checksum → `409 Conflict`
  - New key → stores checksum in memory and returns `201 Created`

### Run

```
cmake -B build
cmake --build build
./build/hyperlog
```

### TODO

- [ ] Implement write‑ahead log engine (append‑only file, sync + recovery)
- [ ] Add `/retrieve` endpoint for range and RID lookups
- [ ] Replace in‑memory cache with persistent storage
- [ ] Integrate io_uring for async, microbatched writes
- [ ] Introduce segment rotation and LSN fences
- [ ] Add unit tests for append, CRC, recovery
- [ ] Build lightweight metrics and latency histograms
- [ ] Implement structured logging and error codes
- [ ] Provide configuration file support (YAML/TOML)
- [ ] Write benchmarks for append latency and throughput
