#include "teleios/platform/memory.h"
#include "teleios/memory/operator.h"

TLEXPORT void tl_memory_set(void* pointer, i32 value, u64 size) {
    tl_platform_memory_set(pointer, value, size);
}

TLEXPORT void tl_memory_zero(void* pointer, u64 size) {
    tl_platform_memory_zero(pointer, size);
}

TLEXPORT void tl_memory_copy(void* target, void* source, u64 size) {
    tl_platform_memory_copy(target, source, size);
}