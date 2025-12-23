#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "safemem.h"

// Define how many allocations to perform
const int NUM_ALLOCS = 1000000;

int main() {
    std::cout << "--- Benchmarking Safemem vs System Malloc ---\n";
    std::cout << "Allocating " << NUM_ALLOCS << " blocks of 16 bytes...\n\n";

    // --- TEST 1: System Malloc ---
    std::vector<void*> std_ptrs;
    std_ptrs.reserve(NUM_ALLOCS);

    auto start_std = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ALLOCS; i++) {
        std_ptrs.push_back(std::malloc(16));
    }
    auto end_std = std::chrono::high_resolution_clock::now();
    
    // Cleanup standard malloc
    for (void* ptr : std_ptrs) std::free(ptr);

    std::chrono::duration<double, std::milli> std_duration = end_std - start_std;
    std::cout << "System malloc time: " << std_duration.count() << " ms\n";


    // --- TEST 2: SafeMalloc ---
    std::vector<void*> safe_ptrs;
    safe_ptrs.reserve(NUM_ALLOCS);

    auto start_safe = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_ALLOCS; i++) {
        safe_ptrs.push_back(safe_malloc(16));
    }
    auto end_safe = std::chrono::high_resolution_clock::now();

    // Cleanup safe malloc
    for (void* ptr : safe_ptrs) safe_free(ptr);

    std::chrono::duration<double, std::milli> safe_duration = end_safe - start_safe;
    std::cout << "SafeMalloc time:    " << safe_duration.count() << " ms\n";

    // --- RESULT ---
    std::cout << "\n---------------------------------------------\n";
    if (safe_duration < std_duration) {
        std::cout << "WINNER: SafeMalloc is " << (std_duration.count() / safe_duration.count()) << "x faster!\n";
    } else {
        std::cout << "WINNER: System malloc (Optimization needed)\n";
    }

    return 0;
}
