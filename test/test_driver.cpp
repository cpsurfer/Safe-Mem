#include <iostream>
#include <vector>
#include "safemem.h"

int main() {
    std::cout << "--- Testing Small Allocations ---" << std::endl;

    // Request three 16-byte blocks
    void* ptr1 = safe_malloc(10); // Should land in 16B bucket
    void* ptr2 = safe_malloc(15); // Should land in 16B bucket
    void* ptr3 = safe_malloc(16); // Should land in 16B bucket

    std::cout << "16B Block 1: " << ptr1 << std::endl;
    std::cout << "16B Block 2: " << ptr2 << std::endl;
    std::cout << "16B Block 3: " << ptr3 << std::endl;

    std::cout << "\n--- Testing Large Allocation ---" << std::endl;
    void* large_ptr = safe_malloc(5000); 
    std::cout << "Large Block (>4KB): " << large_ptr << std::endl;

    return 0;
}
