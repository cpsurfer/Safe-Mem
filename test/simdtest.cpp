#include <immintrin.h>
#include <iostream>
#include <cstdint>
#include "safemem.h"

// A simple replacement for DoNotOptimize using a compiler barrier
template <typename T>
void keep_alive(T&& val) {
    asm volatile("" : "+r,m"(val) : : "memory");
}

void test_simd_alignment() {
    std::cout << "Running SIMD Alignment Torture Test...\n";

    for (int i = 0; i < 1000; ++i) {
        // Allocate 16 bytes. Header + Data must be 16-byte aligned.
        void* p = safe_malloc(16);
        
        // This intrinsic REQUIRES 16-byte alignment.
        // If your alignment logic is off, this triggers a Segfault immediately.
        __m128i v = _mm_load_si128((const __m128i*)p);
        
        // Perform dummy work to verify the memory is usable
        v = _mm_add_epi32(v, v);
        keep_alive(v);

        safe_free(p);
    }
    std::cout << "✅ SIMD Alignment Verified: No crashes detected.\n";
}

int main() {
    test_simd_alignment();
    return 0;
}
