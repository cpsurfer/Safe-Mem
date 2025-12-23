#ifndef INTERNAL_H
#define INTERNAL_H

// A structure to represent a free block in our memory pool.
// Since the block is free, we can use its own memory to store
// the pointer to the next free block in the list.
struct FreeBlock {
    FreeBlock* next;
};

#endif
