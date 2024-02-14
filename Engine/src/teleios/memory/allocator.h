#ifndef TELEIOS_MEMORY_ALLOCATOR_H
#define TELEIOS_MEMORY_ALLOCATOR_H

#include "Teleios/Types.h"

void* tl_memory_salloc(TLMemoryType type, u64 size);
void tl_memory_sfree(TLMemoryType type, void* pointer, u64 size);

void* tl_memory_halloc(TLMemoryType type, u64 size);
void tl_memory_hfree(TLMemoryType type, void* pointer, u64 size);

#endif // TELEIOS_MEMORY_ALLOCATOR_H
