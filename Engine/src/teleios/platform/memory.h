#ifndef TELEIOS_PLATFORM_MEMORY_H
#define TELEIOS_PLATFORM_MEMORY_H

#include "teleios/types.h"

void* tl_platform_memory_alloc(u64 size);
void tl_platform_memory_free(void* block);

void tl_platform_memory_copy(void* source, void* target, u64 size);
void tl_platform_memory_set(void* target, i32 value, u64 size);

#endif // TELEIOS_PLATFORM_MEMORY_H
