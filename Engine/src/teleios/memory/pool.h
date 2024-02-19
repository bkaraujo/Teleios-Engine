#ifndef TELEIOS_MEMORY_POOL_H
#define TELEIOS_MEMORY_POOL_H

#include "teleios/types.h"
#include "teleios/memory/types.h"

TLMemoryPool* tl_memory_pool_create(void);
void tl_memory_pool_destroy(TLMemoryPool* pool);

void* tl_memory_pool_alloc(TLMemoryPool* pool, u64 size);

#endif // TELEIOS_MEMORY_POOL_H