#include "safemem.h"
#include "internal.h"
#include <cstddef>
#include <sys/mman.h>
#include <iostream>
#include <cassert>
#include <errno.h>

/**
 * 1. False Sharing Protection
 * We align the FreeBlock pointers to 64 bytes (cache line size) 
 * so that different threads don't fight over the same memory line.
 */
struct alignas(64) ThreadFreeLists {
    FreeBlock* heads[8] = {nullptr};
};

// 2. Thread-Local Storage (TLS)
// This gives every CPU core its own private slab allocator.
// No locks = No contention = Maximum HFT Speed.
static thread_local ThreadFreeLists tls_lists;

/**
 * 3. Enhanced mmap with Hugepage Fallback
 */
void* map_memory(size_t size) {
    // Attempt Hugepages first for HFT performance boost
    void* ptr = mmap(nullptr, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);

    // Fallback to standard 4KB pages if Hugepages are disabled in kernel
    if (ptr == MAP_FAILED) {
        ptr = mmap(nullptr, size,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1, 0);
    }

    return (ptr == MAP_FAILED) ? nullptr : ptr;
}

/**
 * 4. Slab Refill (The "Slow Path")
 */
static void refill_slab(size_t index) {
    size_t data_size = (index + 1) * 16; 
    size_t total_block_size = sizeof(BlockHeader) + data_size;

    size_t slab_size = 2 * 1024 * 1024; 
    char* memory = (char*)map_memory(slab_size);

    if (__builtin_expect(memory == nullptr, 0)) return;

    // --- ADD THE PRE-WARMING LOOP HERE ---
    // This "touches" every 4KB window in the 2MB slab.
    // It forces the kernel to physically map the Hugepage now.
    for (size_t i = 0; i < slab_size; i += 4096) {
        memory[i] = 0; 
    }
    // -------------------------------------

    size_t block_count = slab_size / total_block_size;
    
    // Now carve the "warm" memory into blocks...
    FreeBlock* head = (FreeBlock*)(memory + sizeof(BlockHeader));
    FreeBlock* current = head;

    for (size_t i = 1; i < block_count; ++i) {
        char* next_addr = memory + (i * total_block_size);
        FreeBlock* next_block = (FreeBlock*)(next_addr + sizeof(BlockHeader));

        current->next = next_block;
        current = next_block;
    }

    current->next = nullptr;
    tls_lists.heads[index] = head;
}

/**
 * 5. safe_malloc (The "Fast Path")
 */
void* safe_malloc(size_t size) {
    if (__builtin_expect(size == 0, 0)) return nullptr;

    size_t index = (size + 15) / 16 - 1;

    // Large Allocation Path (Direct kernel call)
    if (index > 7) {
        size_t total_size = sizeof(BlockHeader) + size;
        void* ptr = map_memory(total_size);
        if (ptr == nullptr) return nullptr;

        BlockHeader* header = (BlockHeader*)ptr;
        header->size = size;
        header->magic = MAGIC_NUM;

        return (void*)(header + 1);
    }
    
    // Check local thread cache first
    if(__builtin_expect(tls_lists.heads[index] == nullptr, 0)) {
        refill_slab(index);
        if(tls_lists.heads[index] == nullptr) return nullptr;
    }
    
    // Pop from Thread-Local Free List
    FreeBlock* node = tls_lists.heads[index];
    tls_lists.heads[index] = node->next;

    BlockHeader* header = (BlockHeader*)((char*)node - sizeof(BlockHeader));
    header->size = (index + 1) * 16;
    header->magic = MAGIC_NUM;

    return (void*)node;
}

/**
 * 6. safe_free (Cache-Friendly Return)
 */
void safe_free(void* ptr) {
    if(__builtin_expect(ptr == nullptr, 0)) return;

    BlockHeader* header = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    
    // Integrity check
    if(__builtin_expect(header->magic != MAGIC_NUM, 0)) {
        std::cerr << "[SafeMem] Critical: Corruption/Double-Free at " << ptr << "\n";
        return;
    }

    size_t index = (header->size / 16) - 1;

    // Large blocks are returned to OS
    if (index > 7) {
        size_t total_size = sizeof(BlockHeader) + header->size;
        munmap((void*)header, total_size);
        return;
    }

    // Return block to the Thread-Local LIFO list
    header->magic = 0; // Invalidate to prevent double-free
    FreeBlock* node = (FreeBlock*)ptr;
    node->next = tls_lists.heads[index];
    tls_lists.heads[index] = node;
}
