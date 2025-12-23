# SafeMem: High-Performance Custom Memory Allocator

A custom C++ memory allocator designed for high-performance and low-latency applications (like HFT). It outperforms the standard system `malloc` by approximately **30% (1.3x)** in high-volume benchmarks.

## 🚀 Features
* **Pool Allocator Architecture:** Uses fixed-size buckets (16B, 32B... 128B) to ensure O(1) allocation time.
* **Cache Locality:** Minimizes cache misses by carving blocks sequentially from contiguous pages.
* **Memory Safety:** Implements "Magic Number" headers to detect heap corruption and double-free errors.
* **Smart Recycling:** `safe_free` returns blocks to the pool for immediate reuse, preventing fragmentation.
* **Large Block Support:** Automatically handles large allocations (>128B) via direct `mmap`/`munmap` calls.

## 📊 Performance Benchmark
**Test Conditions:** Single-threaded allocation/deallocation of 1,000,000 objects (16 bytes each).
**Optimization:** Compiled with `-O3`.

| Allocator | Time (ms) | Speedup |
|-----------|-----------|---------|
| System `malloc` | ~44 ms | 1.0x |
| **SafeMem** | **~33 ms** | **~1.33x** |

*(Note: SafeMem eliminates the overhead of general-purpose allocation logic, making it ideal for specialized, high-throughput systems.)*

## 🛠️ Build & Run
Prerequisites: `g++` and `make`.

```bash
# Build the library and test driver
make

# Run the correctness tests
./test_driver

# Run the performance benchmark
make benchmark
./benchmark
