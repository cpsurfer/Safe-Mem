#ifndef INTERNAL_H
#define INTERNAL_H

#include <cstddef> //for size_t
#include <cstdint> //for uint32_t

// A magic number helps us detect if the user corrupted memory.
// It acts like a security seal.
const uint32_t MAGIC_NUM = 0xDEADBEEF;

// This structure sits secretly *behind* the pointer we give the user.
// It tracks the size so we know how much to free later.
struct BlockHeader {
	size_t size;
	uint32_t magic;
	char padding[4];
};

// A structure to represent a free block in our memory pool.
struct FreeBlock {
    FreeBlock* next;
};

#endif
