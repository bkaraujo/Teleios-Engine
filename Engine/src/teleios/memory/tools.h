#ifndef TELEIOS_MEMORY_TOOLS_H
#define TELEIOS_MEMORY_TOOLS_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void tl_memory_zero(const void* target, u64 size);
TLAPI void tl_memory_set(const void* target, i32 value, u64 size);
TLAPI void tl_memory_copy(const void* source, const void* target, u64 size);

#endif // TELEIOS_MEMORY_TOOLS_H
