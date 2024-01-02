#include "teleios/logger.h"
#include "teleios/memory.h"
#include "teleios/platform.h"

#include <stdlib.h>

struct TLRegistry {
    u64 allocated;
    u64 pool[TL_MEMORY_TYPE_MAXIMUM];
};

static struct TLRegistry registry = { 0 };

static const char* tl_memory_type(TLEMemoryType type);

// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################

void* tl_memory_alloc(TLEMemoryType type, u64 size) {
    void* block = tl_platform_memory_alloc(size);
    if (block == NULL) {
        TLERROR("tl_memory_alloc: Failed to allocate %llu bytes", size);
        return NULL;
    }

    registry.allocated += size;
    registry.pool[type] += size;

    return block;
}

void tl_memory_free(const void* target, TLEMemoryType type, u64 size) {
    tl_platform_memory_free(target);

    registry.allocated -= size;
    registry.pool[type] -= size;
}

void tl_memory_zero(const void* target, u64 size) {
    tl_platform_memory_set(target, 0, size);
}

void tl_memory_set(const void* target, i32 value, u64 size) {
    tl_platform_memory_set(target, value, size);
}

void tl_memory_copy(const void* source, const void* target, u64 size) {
    tl_platform_memory_copy(source, target, size);
}

// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

b8 tl_memory_initialize(void) {
    return true;
}
#ifdef TELEIOS_DEBUG
void tl_memory_print(void) {
    TLTRACE("tl_memory_print: Total memory utilization %llu bytes", registry.allocated);
    for (unsigned i = 0; i < TL_MEMORY_TYPE_MAXIMUM; ++i)
        if (registry.pool[i] > 0)
            TLTRACE("tl_memory_print: %s - %u bytes", tl_memory_type(i), registry.pool[i]);
}
#endif
b8 tl_memory_terminate(void) {
    if (registry.allocated == 0) return true;

    for (unsigned i = 0; i < TL_MEMORY_TYPE_MAXIMUM; ++i)
        if (registry.pool[i] > 0)
            TLERROR("tl_memory_terminate: Leak %s - %u bytes", tl_memory_type(i), registry.pool[i]);

    return false;
}

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################
static const char* tl_memory_type(TLEMemoryType type) {
    switch (type) {
    case TL_MEMORY_TYPE_LAYER:          return "TL_MEMORY_TYPE_LAYER         ";
    case TL_MEMORY_TYPE_CONTAINER_LIST: return "TL_MEMORY_TYPE_CONTAINER_LIST";
    case TL_MEMORY_TYPE_CONTAINER_NODE: return "TL_MEMORY_TYPE_CONTAINER_NODE";
    case TL_MEMORY_TYPE_GRAPHICS:       return "TL_MEMORY_TYPE_GRAPHICS      ";
    case TL_MEMORY_TYPE_STRING:         return "TL_MEMORY_TYPE_STRING        ";
    case TL_MEMORY_TYPE_FILE:           return "TL_MEMORY_TYPE_FILE          ";
    case TL_MEMORY_TYPE_FILE_CONTENT:   return "TL_MEMORY_TYPE_FILE_CONTENT  ";
    case TL_MEMORY_TYPE_ENVIRONMENT:    return "TL_MEMORY_TYPE_ENVIRONMENT   ";
    case TL_MEMORY_TYPE_RENDERER:       return "TL_MEMORY_TYPE_RENDERER      ";
    case TL_MEMORY_TYPE_THREAD:         return "TL_MEMORY_TYPE_THREAD        ";
    case TL_MEMORY_TYPE_MAXIMUM:        return "TL_MEMORY_TYPE_MAXIMUM       ";
    }

    return "TL_MEMORY_TYPE_ ???";
}