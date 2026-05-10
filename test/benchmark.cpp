#define NDEBUG
#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <random>
#include "safemem.h"

/**
 * 1. BASIC ALLOCATION BENCHMARK (The 1.8x Test)
 * Measures how fast you can allocate and free sequentially.
 */
static void BM_SafeMalloc_Basic(benchmark::State& state) {
    size_t size = state.range(0);
    for (auto _ : state) {
        void* p = safe_malloc(size);
        benchmark::DoNotOptimize(p);
        safe_free(p);
    }
    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(size));
}
BENCHMARK(BM_SafeMalloc_Basic)->RangeMultiplier(8)->Range(8, 128);

/**
 * 2. MULTI-THREADED SCALING TEST
 * Verifies that thread_local storage prevents lock contention.
 * The "RealTime" metric is key here to see if threads fight each other.
 */
static void BM_ThreadScaling(benchmark::State& state) {
    for (auto _ : state) {
        void* p = safe_malloc(16);
        benchmark::DoNotOptimize(p);
        safe_free(p);
    }
}
BENCHMARK(BM_ThreadScaling)->Threads(1)->Threads(2)->Threads(4)->Threads(8)->UseRealTime();

/**
 * 3. RANDOMIZED FRAGMENTATION TEST (The "Swiss Cheese" Test)
 * Simulates real-world usage where memory isn't freed in a perfect order.
 */
static void BM_RandomFragmentation(benchmark::State& state) {
    const int N = 1000;
    std::vector<void*> ptrs(N);
    std::vector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);
    
    std::random_device rd;
    std::mt19937 g(rd());

    // Shuffle ko loop se bahar rakha hai takki CPU noise kam ho
    std::shuffle(indices.begin(), indices.end(), g);

    for (auto _ : state) {
        // Step 1: Sequential Allocation (1000 times)
        for (int i = 0; i < N; ++i) {
            ptrs[i] = safe_malloc(16);
        }

        // Memory barrier to ensure everything is written
        benchmark::ClobberMemory();

        // Step 2: Random Deallocation (1000 times)
        for (int idx : indices) {
            safe_free(ptrs[idx]);
        }
        
        benchmark::ClobberMemory();
    }

    // MANDATORY: Ye line total time ko N se divide kar degi
    // Output table ab 7us ki jagah 7ns (per-op) dikhayegi
    state.SetItemsProcessed(state.iterations() * N);
}
// Unit ko Nanosecond rakhein for better precision
BENCHMARK(BM_RandomFragmentation)->Unit(benchmark::kNanosecond);
/**
 * 4. JITTER & TAIL LATENCY TRACKING
 * We add custom statistics to see the "Max" latency and Standard Deviation.
 * This proves if your "Pre-warming" fixed the cold-start spikes.
 */
BENCHMARK(BM_SafeMalloc_Basic)
    ->Arg(16)
    ->Repetitions(10)
    ->DisplayAggregatesOnly(false)
    ->ComputeStatistics("max", [](const std::vector<double>& v) -> double {
        return *std::max_element(v.begin(), v.end());
    });

BENCHMARK_MAIN();


