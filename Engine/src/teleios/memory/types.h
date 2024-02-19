#ifndef TELEIOS_MEMORY_TYPES_H
#define TELEIOS_MEMORY_TYPES_H

#include "teleios/types.h"

#define TLPAGESIZE (32 * 1024 * 1024)
typedef struct TLMemoryPage {
    u16 index;
    const char* raw[TLPAGESIZE];
    struct TLMemoryPage* next;
} TLMemoryPage;

/**
 * A linked list of memory pools used for fast small allocations. This approach has the benefit
 * of reduing platform allocations and pointer invalidations throw realoc and also give all the
 * allocations a single lifetime since all pages are freed together.
 *
 *
 * Each TLMemoryPage inside the TLMemoryPool is composed of 32Mb of space. Once a page is full
 * a new one is allocated and added to the list.
 */
typedef struct TLMemoryPool {
    u16 size;
    struct TLMemoryPage* head;
    struct TLMemoryPage* tail;
} TLMemoryPool;

#endif // TELEIOS_MEMORY_TYPES_H
