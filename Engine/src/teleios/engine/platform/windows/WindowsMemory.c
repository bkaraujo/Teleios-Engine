#include "teleios/engine/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include <malloc.h>
#include "teleios/engine/platform/windows/common.h"
#include "teleios/engine/platform/memory.h"
#include "teleios/engine/logger/console.h"

void* tl_platform_memory_halloc(u64 size) {
    void* block = HeapAlloc(e_heap, HEAP_ZERO_MEMORY, size);
    if (block == NULL) TLFATAL("tl_platform_memory_halloc: Failed to allocate memory");

    return block;
}

void tl_platform_memory_hfree(void* pointer) {
    if (!HeapFree(e_heap, HEAP_NO_SERIALIZE, pointer)) {
        TLFATAL("tl_platform_memory_hfree: Failed to dealocate %p", pointer);
    }
}

void* tl_platform_memory_salloc(u64 size) {
    void* block = _malloca(size);
    if (block == NULL) TLFATAL("tl_platform_memory_salloc: Failed to allocate memory");

    return block;
}

void tl_platform_memory_sfree(void* pointer) {
    _freea(pointer);
}

void tl_platform_memory_set(void* pointer, u8 value, u64 size) {
    FillMemory(pointer, size, value);
}

void tl_platform_memory_zero(void* pointer, u64 size) {
    SecureZeroMemory(pointer, size);
}

void tl_platform_memory_copy(void* target, void* source, u64 size) {
    CopyMemory(target, source, size);
}

#endif // TL_PLATFORM_WINDOWS