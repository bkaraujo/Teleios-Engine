#ifndef TELEIOS_MEMORY_COMMON_H
#define TELEIOS_MEMORY_COMMON_H

#include "teleios/types.h"

typedef struct {
    u64 total;
    u64 named[TL_MEMORY_TYPE_MAXIMUM];
} TLMemoryRegistry;

extern TLMemoryRegistry registry;

#endif // TELEIOS_MEMORY_COMMON_H
