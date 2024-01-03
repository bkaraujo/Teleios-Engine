#ifndef TELEIOS_MEMORY_H
#define TELEIOS_MEMORY_H

#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_memory_initialize(void);
b8 tl_memory_terminate(void);

typedef enum {
    TL_MEMORY_TYPE_LAYER,
    TL_MEMORY_TYPE_CONTAINER_MAP,
    TL_MEMORY_TYPE_CONTAINER_LIST,
    TL_MEMORY_TYPE_CONTAINER_NODE,
    TL_MEMORY_TYPE_GRAPHICS,
    TL_MEMORY_TYPE_STRING,
    TL_MEMORY_TYPE_FILE,
    TL_MEMORY_TYPE_FILE_CONTENT,
    TL_MEMORY_TYPE_ENVIRONMENT,
    TL_MEMORY_TYPE_RENDERER,
    TL_MEMORY_TYPE_THREAD,
    TL_MEMORY_TYPE_MAXIMUM
} TLEMemoryType;

#ifdef TELEIOS_DEBUG
void tl_memory_print(void);
#endif
void* tl_memory_alloc(TLEMemoryType type, u64 size);
void tl_memory_free(const void* target, TLEMemoryType type, u64 size);
void tl_memory_zero(const void* target, u64 size);
void tl_memory_set(const void* target, i32 value, u64 size);
void tl_memory_copy(const void* source, const void* target, u64 size);
#endif // TELEIOS_MEMORY_H

