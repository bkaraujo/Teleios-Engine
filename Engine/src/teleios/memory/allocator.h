#ifndef TELEIOS_MEMORY_ALLOCATOR_H
#define TELEIOS_MEMORY_ALLOCATOR_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void* tl_memory_alloc(TLEMemoryType type, u64 size);
TLAPI void tl_memory_free(const void* target, TLEMemoryType type, u64 size);

#endif // TELEIOS_MEMORY_ALLOCATOR_H
