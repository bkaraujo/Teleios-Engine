#include "teleios/memory/common.h"
#include "teleios/memory/lifecycle.h"
#include "teleios/platform/memory.h"
#include "teleios/logger/console.h"

TLMemoryRegistry registry;

b8 tl_memory_initialize(void) {
    TLDEBUG("tl_memory_initialize: Initializing the Memory Subsystem");
    tl_platform_memory_set(&registry, 0, sizeof(TLMemoryRegistry));
    return true;
}

static const char* tl_memory_name(TLMemoryType type) {
    switch (type)
    {
    case TL_MEMORY_TYPE_FILE: return "TL_MEMORY_TYPE_FILE";
    case TL_MEMORY_TYPE_CONTAINER_ARRAY: return "TL_MEMORY_TYPE_CONTAINER_ARRAY";
    case TL_MEMORY_TYPE_CONTAINER_LIST: return "TL_MEMORY_TYPE_CONTAINER_LIST";
    case TL_MEMORY_TYPE_CONTAINER_LIST_NODE: return "TL_MEMORY_TYPE_CONTAINER_LIST_NODE";
    case TL_MEMORY_TYPE_CONTAINER_LIST_ITERATOR: return "TL_MEMORY_TYPE_CONTAINER_LIST_ITERATOR";
    case TL_MEMORY_TYPE_GRAPHICS: return "TL_MEMORY_TYPE_GRAPHICS";
    case TL_MEMORY_TYPE_MAXIMUM: return "TL_MEMORY_TYPE_MAXIMUM";
    }
    return "???";
}

b8 tl_memory_terminate(void) {
    TLDEBUG("tl_memory_terminate: Terminating the Memory Subsystem");
    if (registry.total != 0) {
        TLERROR("tl_memory_terminate: Memory leaked %llu bytes", registry.total);
        for (unsigned i = 0; i < TL_MEMORY_TYPE_MAXIMUM; ++i) {
            if (registry.named[i] == 0) continue;
            TLERROR("tl_memory_terminate: %s leaked %llu bytes", tl_memory_name(i), registry.named[i]);
        }

        return false;
    }

    return true;
}
