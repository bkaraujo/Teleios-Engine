#ifndef TELEIOS_ENGINE_PLATFORM_MEMORY_H
#define TELEIOS_ENGINE_PLATFORM_MEMORY_H

#include "Teleios/Types.h"

void* tl_platform_memory_halloc(u64 size);
void tl_platform_memory_hfree(void* pointer);

void* tl_platform_memory_salloc(u64 size);
void tl_platform_memory_sfree(void* pointer);

void tl_platform_memory_zero(void* pointer, u64 size);
void tl_platform_memory_set(void* pointer, u8 value, u64 size);
void tl_platform_memory_copy(void* target, void* source, u64 size);

#endif // TELEIOS_ENGINE_PLATFORM_MEMORY_H