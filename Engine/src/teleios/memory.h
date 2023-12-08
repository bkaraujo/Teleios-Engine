#ifndef TELEIOS_MEMORY_H
#define TELEIOS_MEMORY_H

#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_memory_initialize(void);
b8 tl_memory_terminate(void);

TLAPI void* tl_memory_alloc(TLEMemoryType type, u64 size);
TLAPI void tl_memory_free(const void* target, TLEMemoryType type, u64 size);
TLAPI void tl_memory_zero(const void* target, u64 size);
TLAPI void tl_memory_set(const void* target, i32 value, u64 size);
TLAPI void tl_memory_copy(const void* source, const void* target, u64 size);

#endif // TELEIOS_MEMORY_H

