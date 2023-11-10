#ifndef TELEIOS_PLATFORM_MEMORY_H
#define TELEIOS_PLATFORM_MEMORY_H

#include "teleios/types.h"

void* tl_platform_memory_alloc(u64 size);
void tl_platform_memory_free(const void* block);

void tl_platform_memory_copy(const void* source, const void* target, u64 size);
void tl_platform_memory_set(const void* target, i32 value, u64 size);

#endif // TELEIOS_PLATFORM_MEMORY_H
