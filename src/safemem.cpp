#include "safemem.h"
#include <sys/mman.h>
#include "internal.h" // We need this to know what 'FreeBlock' is

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
// and slicing it into blocks.
static void refill_slab(size_t index) {
    // 1. Calculate the block size for this list (e.g., Index 0 -> 16 bytes)
    size_t block_size = (index + 1) * 16;

    // 2. Request a standard 4KB page from the OS
    size_t page_size = 4096;
    char* memory = (char*)map_memory(page_size);

    if (memory == nullptr) {
        return; // Out of memory!
    }

    // 3. Carve the page into linked blocks
    size_t block_count = page_size / block_size;
    FreeBlock* head = (FreeBlock*)memory;
    FreeBlock* current = head;

    // Link each block to the next one
    for (size_t i = 1; i < block_count; ++i) {
        char* next_addr = memory + (i * block_size);
        FreeBlock* next_block = (FreeBlock*)next_addr;

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
        return map_memory(size);
    }
    
    //If list empty :: Fill it first
    if(free_lists[index]==nullptr){
	    refill_slab(index);
	    // if still empty after Refill, the OS is out of memory
	    if(free_lists[index]==nullptr){
		return nullptr;
	    }
    }
    //Pop a block from the list
    FreeBlock* block=free_lists[index];
    free_lists[index]=block->next;

    return block;
}
