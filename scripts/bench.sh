#!/bin/bash
cd "$(dirname "$0")/.."
cmake --build build --target bench > /dev/null 2>&1
./bench
rm -f bench
