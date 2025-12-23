#include <cassert>
#include <iostream>
#include "safemem.h"

int main() {
    std::cout << "--- Testing Small Allocations ---" << std::endl;

    // Request three 16-byte blocks
    void* ptr1 = safe_malloc(10); 
    void* ptr2 = safe_malloc(15); 
    void* ptr3 = safe_malloc(16); 

    // Check for NULL before writing!
    // If your allocator fails (returns 0), writing to it causes a Segfault.

    assert(ptr1 != nullptr);
    assert(ptr2 != nullptr);
    assert(ptr3 != nullptr);

    // Cast to (int*) before dereferencing
    *(int*)ptr1 = 44;    
    *(int*)ptr2 = 45;
    *(int*)ptr3 = 46;

    std::cout << "16B Block 1 Address: " << ptr1 << std::endl;
    std::cout << "16B Block 2 Address: " << ptr2 << std::endl;
    std::cout << "16B Block 3 Address: " << ptr3 << std::endl;

    // Verify values
    // We cast to (int*) again to read the value back
    assert(*(int*)ptr1 == 44);
    std::cout << "Write/Read Test 1 Passed: " << *(int*)ptr1 << "\n";

    assert(*(int*)ptr2 == 45);
    std::cout << "Write/Read Test 2 Passed: " << *(int*)ptr2 << "\n";

    assert(*(int*)ptr3 == 46);
    std::cout << "Write/Read Test 3 Passed: " << *(int*)ptr3 << "\n";

    std::cout << "\n--- Testing Large Allocation ---" << std::endl;
    void* large_ptr = safe_malloc(5000);

    assert(large_ptr != nullptr); 

    *(int*)large_ptr = 100000;
    if (large_ptr) {
        std::cout << "Large Block (>4KB): " << large_ptr << std::endl;
    } else {
        std::cout << "Large Allocation Failed!" << std::endl;
    }
    assert(*(int*)large_ptr==100000);
    std::cout << "Write/Read Test 4 Passed: " << *(int*)large_ptr << "\n";

    return 0;
}
