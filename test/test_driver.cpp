#include <iostream>
#include <cassert>
#include "safemem.h"

// Helper to print addresses cleanly
void print_addr(const char* label, void* ptr) {
    std::cout << label << ": " << ptr << "\n";
}

int main() {
    std::cout << "=== Test 1: Basic Allocation & Alignment ===\n";
    void* p1 = safe_malloc(16);
    void* p2 = safe_malloc(10); // Should round up to 16
    void* p3 = safe_malloc(32); // Different size class

    assert(p1 != nullptr);
    assert(p2 != nullptr);
    assert(p3 != nullptr);

    print_addr("16B Block 1", p1);
    print_addr("16B Block 2", p2);
    print_addr("32B Block 1", p3);

    // Verify pointers are different
    assert(p1 != p2);
    
    // Verify we can write to them
    *(int*)p1 = 12345;
    *(int*)p3 = 67890;
    std::cout << "Data Write Check: " << *(int*)p1 << " / " << *(int*)p3 << " [PASS]\n";


    std::cout << "\n=== Test 2: The 'Reuse' Test (Checking safe_free) ===\n";
    
    // 1. Remember the address of p1
    void* old_p1_address = p1;
    
    // 2. Free p1. It should go back to the top of the 16-byte list.
    std::cout << "Freeing Block 1...\n";
    safe_free(p1);

    // 3. Ask for 16 bytes again.
    // If safe_free works, we should get the EXACT SAME block back.
    std::cout << "Requesting 16B again...\n";
    void* p4 = safe_malloc(16);
    print_addr("New 16B Block", p4);

    if (p4 == old_p1_address) {
        std::cout << "SUCCESS: Memory was reused! safe_free() is working.\n";
    } else {
        std::cout << "FAILURE: Allocator returned a new block instead of the freed one.\n";
        return 1; // Fail the test
    }


    std::cout << "\n=== Test 3: Large Allocations ===\n";
    void* huge = safe_malloc(5000); // Should use mmap
    print_addr("Large Block", huge);
    assert(huge != nullptr);
    safe_free(huge); // Note: Current implementation just leaks this or crashes if not handled, 
                     // but my code handles index > 7 by returning mapped memory.
                     // Real mmap free support requires munmap, which i would add later.


    std::cout << "\n=== All Tests Passed! ===\n";
    return 0;
}
