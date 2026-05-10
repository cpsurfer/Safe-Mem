//Copyright (c) 2026 cpsurfer[Rahul]. All rights reserved.
/*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE*/

#include "safemem.h"
#include "internal.h"
#include <sys/mman.h>
#include <iostream>
#include <cassert>
#include <errno.h>


struct alignas(64) ThreadFreeLists {
    FreeBlock* heads[8] = {nullptr};
};

static thread_local ThreadFreeLists tls_lists;

void* map_memory(size_t size) {
    void* ptr = mmap(nullptr, size,
                     PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                     -1, 0);
    if (ptr == MAP_FAILED) {
        ptr = mmap(nullptr, size,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS,
                   -1, 0);
    }

    return (ptr == MAP_FAILED) ? nullptr : ptr;
}

static void refill_slab(size_t index) {
    size_t data_size = (index + 1) * 16; 
    size_t total_block_size = sizeof(BlockHeader) + data_size;

    size_t slab_size = 2 * 1024 * 1024; 
    char* memory = (char*)map_memory(slab_size);

    if (__builtin_expect(memory == nullptr, 0)) return;


    for (size_t i = 0; i < slab_size; i += 4096) {
        memory[i] = 0; 
    }
    // -------------------------------------

    size_t block_count = slab_size / total_block_size;
    
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

void* safemem(size_t size) {
    if (__builtin_expect(size == 0, 0)) return nullptr;

    size_t index = (size + 15) / 16 - 1;

    if (index > 7) {
        size_t total_size = sizeof(BlockHeader) + size;
        void* ptr = map_memory(total_size);
        if (ptr == nullptr) return nullptr;

        BlockHeader* header = (BlockHeader*)ptr;
        header->size = size;
        header->magic = MAGIC_NUM;

        return (void*)(header + 1);
    }
    
    if(__builtin_expect(tls_lists.heads[index] == nullptr, 0)) {
        refill_slab(index);
        if(tls_lists.heads[index] == nullptr) return nullptr;
    }
    
    FreeBlock* node = tls_lists.heads[index];
    tls_lists.heads[index] = node->next;

    BlockHeader* header = (BlockHeader*)((char*)node - sizeof(BlockHeader));
    header->size = (index + 1) * 16;
    header->magic = MAGIC_NUM;

    return (void*)node;
}

void safe_free(void* ptr) {
    if(__builtin_expect(ptr == nullptr, 0)) return;

    BlockHeader* header = (BlockHeader*)((char*)ptr - sizeof(BlockHeader));

    if(__builtin_expect(header->magic != MAGIC_NUM, 0)) {
        std::cerr << "[SafeMem] Critical: Corruption/Double-Free at " << ptr << "\n";
        return;
    }

    size_t index = (header->size / 16) - 1;

    if (index > 7) {
        size_t total_size = sizeof(BlockHeader) + header->size;
        munmap((void*)header, total_size);
        return;
    }

    header->magic = 0; 
    FreeBlock* node = (FreeBlock*)ptr;
    node->next = tls_lists.heads[index];
    tls_lists.heads[index] = node;
}


