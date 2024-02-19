#include "teleios/memory/common.h"
#include "teleios/memory/allocator.h"
#include "teleios/platform/memory.h"
#include "teleios/logger/console.h"

void* tl_memory_salloc(TLMemoryType type, u64 size) {
    void* block = tl_platform_memory_salloc(size);
    if (block == NULL) { TLERROR("tl_memory_salloc: Failed to allocate %llu bytes", size); return NULL; }

    registry.total += size;
    registry.named[type] += size;
    return block;

}
void tl_memory_sfree(TLMemoryType type, void* pointer, u64 size) {
    if (pointer == NULL) { TLERROR("tl_memory_sfree: Pointer is null"); return; }
    tl_platform_memory_sfree(pointer);

    registry.total -= size;
    registry.named[type] -= size;
}

void* tl_memory_halloc(TLMemoryType type, u64 size) {
    void* block = tl_platform_memory_halloc(size);
    if (block == NULL) { TLERROR("tl_memory_halloc: Failed to allocate %llu bytes", size); return NULL; }

    registry.total += size;
    registry.named[type] += size;
    return block;
}

void tl_memory_hfree(TLMemoryType type, void* pointer, u64 size) {
    if (pointer == NULL) { TLERROR("tl_memory_hfree: Pointer is null"); return; }
    tl_platform_memory_hfree(pointer);

    registry.total -= size;
    registry.named[type] -= size;
}