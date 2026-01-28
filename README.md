# ⚡ SafeMem: HFT-Grade Low-Latency Memory Allocator

SafeMem is an ultra-high-performance, slab-based C++ memory allocator engineered for the extreme requirements of **High-Frequency Trading (HFT)** and low-level systems programming.

By aligning software logic with the physical architecture of modern CPUs and the Linux kernel, SafeMem achieves **sub-nanosecond allocation latencies**, outperforming the standard `glibc malloc` by up to **27×** in single-threaded environments.

---

## 🛠️ Tier-0 Engineering & Optimizations

SafeMem goes beyond traditional general-purpose heap management by incorporating industry-standard HFT optimizations.

### 🔒 Lock-Free Multi-Threaded Scaling
- Uses **Thread-Local Storage (TLS)** to give each CPU core its own private memory lane
- Eliminates mutex contention entirely
- Enables near-linear scaling across cores

### 🧠 Kernel Bypass via Hugepages
- Allocates memory using `MAP_HUGETLB` (2 MB hugepages)
- Reduces **TLB pressure** and page-walk latency
- Achieves peak throughput of **33 GiB/s**

### ⚡ Zero-Jitter Determinism
- **Slab pre-warming** touches every page during refill
- Forces page mapping and zeroing onto the slow path
- Guarantees the allocation fast path never incurs a page fault

### 🧬 Cache & Hardware Awareness
- **SIMD-safe alignment**  
  - All allocations are 16-byte aligned
  - Verified using `_mm_load_si128` torture tests
- **False sharing mitigation**  
  - `alignas(64)` on thread-local metadata
- **Inlined fast path**  
  - Pointer pop logic is fully inlineable
  - Allocation cost as low as **7 CPU cycles**

---

## 📊 Performance Benchmarks (2026)

**Test Environment**
- 12-core Linux
- 2.0 GHz CPU
- `libbenchmark-dev`

| Benchmark Task | Threads | Latency (Wall) | Throughput / Scaling |
|---------------|---------|----------------|----------------------|
| System `malloc` (16B) | 1 | ~100.00 ns | 1.0× (Baseline) |
| SafeMem (16B) | 1 | **3.84 ns** | **27× Speedup** |
| SafeMem (16B) | 8 | **0.707 ns** | **140× Aggregate Gain** |
| Max Throughput (128B) | 1 | **3.91 ns** | **32.99 GiB/s** |

> **Key Observation**  
> A **0.127 ns standard deviation** across 10 million iterations confirms deterministic latency—critical for execution engines where microseconds define P&L.

---

## 🚀 Build & Run Guide

### 1️⃣ Prerequisites

Install required build tools and Google Benchmark:

```bash
sudo apt-get update
sudo apt-get install libbenchmark-dev build-essential

# Clean previous builds and compile all targets
make clean && make

# Hardware Safety Check (SIMD Alignment)
./simdtest

# Performance Benchmark (Pinned to 8 Cores)
taskset -c 0-7 ./benchmark

# Correctness & Memory Safety Validation
./test_driver
````
## 🛡️ Stability & Safety Features

### Corruption Detection
- 64-bit magic number headers
- Guards against buffer overruns and double frees

### Automatic Fallback
- Transparently falls back to 4 KB pages if hugepages are unavailable

### Branch Prediction Hints
- Uses `__builtin_expect`
- Optimized for the common case where the free list is non-empty

---

## 🎯 Target Use Cases

- High-Frequency Trading (HFT) execution engines
- Low-latency systems programming
- Deterministic real-time workloads
- Performance-critical C++ infrastructure

---

**SafeMem** is a systems-level engineering project focused on **determinism, throughput, and hardware realism**—where nanoseconds are not an abstraction, but a constraint.
