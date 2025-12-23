#include "safemem.h"
#include <cstddef>
#include <sys/mman.h>
#include "internal.h"
#include <iostream>

// Array of free lists, one for each size class.
// Index 0: 16 bytes, Index 1: 32 bytes ... Index 7: 128 bytes
static FreeBlock* free_lists[8];

void* map_memory(size_t size) {
    void* ptr = mmap(nullptr, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS,
                     -1, 0);

    if (ptr == MAP_FAILED) {
        return nullptr;
    }
    return ptr;
}

// Helper: Refill the free list for a given index by allocating a new page
static void refill_slab(size_t index) {
    // 1. Calculate sizes
    size_t data_size = (index + 1) * 16; // The space the user gets
    size_t total_block_size = sizeof(BlockHeader) + data_size; // The space we actually take

    // 2. Request a standard 4KB page from the OS
    size_t page_size = 128*1024;
    char* memory = (char*)map_memory(page_size);

    if (memory == nullptr) {
        return; // Out of memory!
    }

    // 3. Carve the page into linked blocks    
    size_t block_count = page_size / total_block_size;
    
    // First block setup: Skip the header to find the user pointer
    FreeBlock* head = (FreeBlock*)(memory + sizeof(BlockHeader));
    FreeBlock* current = head;

    // Link each block to the next one
    for (size_t i = 1; i < block_count; ++i) {
        // Calculate start of next block (Header + Data)
        char* next_addr = memory + (i * total_block_size);
        
        // The user pointer is AFTER the header
        FreeBlock* next_block = (FreeBlock*)(next_addr + sizeof(BlockHeader));

        current->next = next_block;
        current = next_block;
    }

    // Terminate the list
    current->next = nullptr;

    // 4. Update the global array to point to our new list
    free_lists[index] = head;
}

void* safe_malloc(size_t size) {
    // 1. Calculate the list index
    size_t index = (size + 15) / 16 - 1;

    // 2. Handle Large Allocations
    if (index > 7) {
        size_t total_size = sizeof(BlockHeader) + size;
        void* ptr = map_memory(total_size);
        if (ptr == nullptr) return nullptr;

        // Write the header at the very start
        BlockHeader* header = (BlockHeader*)ptr;
        header->size = size;
        header->magic = MAGIC_NUM;

        // Return the memory just AFTER the header
        return (void*)(header + 1);
    }
    
    // If list empty :: Fill it first
    if(free_lists[index] == nullptr) {
        refill_slab(index);
        // if still empty after Refill, the OS is out of memory
        if(free_lists[index] == nullptr) {
            return nullptr;
        }
    }
    
    // Pop a block from the list
    // Renamed 'block' to 'node' to match usage below
    FreeBlock* node = free_lists[index];
    free_lists[index] = node->next;

    // Write the header info
    BlockHeader* header = (BlockHeader*)((char*)node - sizeof(BlockHeader));
    header->size = (index + 1) * 16;
    header->magic = MAGIC_NUM;

    return (void*)node;
}

void safe_free(void* ptr) {
    if(ptr == nullptr) return;

    // 1. Check Header
    BlockHeader* header = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));
    
    if(header->magic != MAGIC_NUM) {
        std::cerr << "[SafeMem Error] Double free or memory corruption detected at " << ptr << "\n";
        return;
    }

    // 2. Return to list
    size_t index = (header->size / 16) - 1;

    // --- FOR LARGE BLOCKS ---
    if (index > 7) {
        // This is a large block. We don't put it in a free list.
        // We return it to the OS using munmap.
        size_t total_size = sizeof(BlockHeader) + header->size;

        // Point to the START of the header (which is the start of the mmap region)
        munmap((void*)header, total_size);
        return;
    }
    // ----------------------------

    FreeBlock* node = (FreeBlock*)ptr;
    node->next = free_lists[index];
    free_lists[index] = node;
}
