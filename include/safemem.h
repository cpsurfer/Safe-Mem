#ifndef SAFEMEM_H
#define SAFEMEM_H

#include <cstddef> // Required for size_t

// Request a raw page of memory from the OS.
void* map_memory(size_t size);

// Our custom allocator function
void* safe_malloc(size_t size);

// Frees the memory pointed to by ptr
void safe_free(void* ptr);

#endif
