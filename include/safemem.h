// Copyright (c) 2026 cpsurfer[Rahul]. All rights reserved.
#ifndef SAFEMEM_H
#define SAFEMEM_H

#include <stddef.h> 

#ifdef __cplusplus
extern "C" {
#endif

void* safemem(size_t size);

void safe_free(void* ptr);

void* map_memory(size_t size);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
inline void* safe_malloc(size_t size) {
    return safemem(size);
}
#endif

#endif 
