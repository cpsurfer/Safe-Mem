# ⚡ SafeMem: Low-Latency Memory Allocator

SafeMem is an ultra-high-performance, slab-based C++ memory allocator engineered for the extreme requirements of **High-Frequency Trading (HFT)** and low-level systems programming.

By aligning software logic with the physical architecture of modern CPUs and the Linux kernel, SafeMem achieves **sub-nanosecond allocation latencies**, outperforming the standard `glibc malloc` by up to **27×** in single-threaded environments.

---

## 🛠️ Engineering & Optimizations

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
## Performance Benchmarks 📊

![Safe-Mem Benchmark Results](./assets/benchmark_results.png)

> **Test Environment:** Benchmarks were run on an Ubuntu 24.04 environment, pinned to a single core using `taskset` to minimize jitter.

---

## 🚀 Build & Run Guide


```bash
# System tools (Ensure these are present)
sudo apt-get update
sudo apt-get install build-essential cmake -y

# Python setup for charts
python3 -m venv .venv
source .venv/bin/activate
pip install pandas matplotlib seaborn

chmod +x scripts/run.sh

# In the Root directory (~/Safemem) :
sudo ./scripts/run.sh
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
