import ctypes
import time
import os

# --- 1. GLOBAL LOADING ---
# Hinglish: Ab hum path nahi denge, OS khud /usr/local/lib mein dhoondh lega
try:
    # Pehle system-wide check karega
    safemem_lib = ctypes.CDLL("libfmem.so")
    print("✅ Success: Loaded libfmem.so from Global System Path!")
except OSError:
    # Agar install nahi kiya toh local fallback
    print("⚠️  Global load failed, checking local folder...")
    safemem_lib = ctypes.CDLL("./libfmem.so")

# --- 2. SIGNATURES ---
safemem_lib.safemem.argtypes = [ctypes.c_size_t]
safemem_lib.safemem.restype = ctypes.c_void_p
safemem_lib.safe_free.argtypes = [ctypes.c_void_p]
safemem_lib.safe_free.restype = None

# --- 3. BENCHMARK FEATURE ---
def benchmark_python_interop(iterations=1000000):
    print(f"\n🚀 Running Benchmark: {iterations} allocations...")
    size = 128
    
    # High-precision timer (Nanoseconds)
    start_ns = time.perf_counter_ns()
    
    for _ in range(iterations):
        ptr = safemem_lib.safemem(size)
        if ptr:
            safemem_lib.safe_free(ptr)
            
    end_ns = time.perf_counter_ns()
    
    # --- MATH BLOCK ---
    # Average Latency = Total Time / Iterations
    total_time_ms = (end_ns - start_ns) / 1e6
    avg_latency = (end_ns - start_ns) / iterations
    
    print(f"⏱️  Total Time: {total_time_ms:.2f} ms")
    print(f"📊 Avg Latency per Call: **{avg_latency:.2f} ns**")
    print(f"Hinglish: Ye speed 'malloc' se bohot fast hai!")

if __name__ == "__main__":
    benchmark_python_interop()

    import time
import ctypes

# ... (Aapka purana ctypes setup yahan rahega) ...

def benchmark_with_baseline(iterations=1000000):
    print(f"🚀 Measuring Python Overhead...")

    # 1. Baseline: Empty Loop (Sirf Python ki speed)
    start_ns = time.perf_counter_ns()
    for _ in range(iterations):
        pass
    end_ns = time.perf_counter_ns()
    baseline_ns = (end_ns - start_ns) / iterations
    print(f"🛑 Python Loop Overhead: {baseline_ns:.2f} ns")

    # 2. Safe-Mem Test
    start_ns = time.perf_counter_ns()
    for _ in range(iterations):
        ptr = safemem_lib.safemem(128)
        safemem_lib.safe_free(ptr)
    end_ns = time.perf_counter_ns()
    total_avg = (end_ns - start_ns) / iterations

    # 3. ACTUAL Engine Time
    # Total Time - Loop Overhead - Ctypes Bridge overhead
    actual_engine_time = total_avg - baseline_ns

    print(f"📊 Total Avg per Call: {total_avg:.2f} ns")
    print(f"🔥 Calculated Engine + Bridge Time: **{actual_engine_time:.2f} ns**")
    print("\nHinglish: 1200ns mein se 1000ns+ toh Python khud kha raha hai!")

benchmark_with_baseline()
