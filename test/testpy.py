import ctypes
import time
import os

try:
    safemem_lib = ctypes.CDLL("libfmem.so")
    print("✅ Success: Loaded libfmem.so from Global System Path!")
except OSError:
    print("⚠️  Global load failed, checking local folder...")
    safemem_lib = ctypes.CDLL("./libfmem.so")

safemem_lib.safemem.argtypes = [ctypes.c_size_t]
safemem_lib.safemem.restype = ctypes.c_void_p
safemem_lib.safe_free.argtypes = [ctypes.c_void_p]
safemem_lib.safe_free.restype = None

def benchmark_python_interop(iterations=1000000):
    print(f"\n🚀 Running Benchmark: {iterations} allocations...")
    size = 128

    start_ns = time.perf_counter_ns()
    
    for _ in range(iterations):
        ptr = safemem_lib.safemem(size)
        if ptr:
            safemem_lib.safe_free(ptr)
            
    end_ns = time.perf_counter_ns()
    
    total_time_ms = (end_ns - start_ns) / 1e6
    avg_latency = (end_ns - start_ns) / iterations
    
    print(f"⏱️  Total Time: {total_time_ms:.2f} ms")
    print(f"📊 Avg Latency per Call: **{avg_latency:.2f} ns**")

if __name__ == "__main__":
    benchmark_python_interop()

    import time
import ctypes

def benchmark_with_baseline(iterations=1000000):
    print(f"🚀 Measuring Python Overhead...")

    start_ns = time.perf_counter_ns()
    for _ in range(iterations):
        pass
    end_ns = time.perf_counter_ns()
    baseline_ns = (end_ns - start_ns) / iterations
    print(f"🛑 Python Loop Overhead: {baseline_ns:.2f} ns")

    start_ns = time.perf_counter_ns()
    for _ in range(iterations):
        ptr = safemem_lib.safemem(128)
        safemem_lib.safe_free(ptr)
    end_ns = time.perf_counter_ns()
    total_avg = (end_ns - start_ns) / iterations

    actual_engine_time = total_avg - baseline_ns

    print(f"📊 Total Avg per Call: {total_avg:.2f} ns")
    print(f"🔥 Calculated Engine + Bridge Time: **{actual_engine_time:.2f} ns**")

benchmark_with_baseline()
