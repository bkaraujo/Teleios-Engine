#include "teleios/logger/console.h"
#include "teleios/memory/pool.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"

TLMemoryPool* tl_memory_pool_create(void) {
    TLMemoryPool* pool = tl_memory_salloc(TL_MEMORY_TYPE_MEMORY_POOL, sizeof(TLMemoryPool));
    if (pool == NULL) TLERROR("tl_memory_pool_create: Failed to allocate TLMemoryPool");

    return pool;
}

void tl_memory_pool_destroy(TLMemoryPool* pool) {
    if (pool == NULL) return;

    TLMemoryPage* current = pool->head;
    while (current != NULL) {
        TLMemoryPage* next = current->next;
        tl_memory_hfree(TL_MEMORY_TYPE_MEMORY_PAGE, current, sizeof(TLMemoryPage));
        current = next;
    }

    tl_memory_sfree(TL_MEMORY_TYPE_MEMORY_POOL, pool, sizeof(TLMemoryPool));
}

void* tl_memory_pool_alloc(TLMemoryPool* pool, u32 size) {
    if (pool == NULL) { TLWARN("tl_memory_pool_alloc: TLMemoryPool is null"); return NULL; }
    if (size > TLPAGESIZE) { TLWARN("tl_memory_pool_alloc: Requested memory is bigger then TLMemoryPage size"); return NULL; }

    if (pool->head == NULL) {
        TLMemoryPage* page = tl_memory_halloc(TL_MEMORY_TYPE_MEMORY_PAGE, sizeof(TLMemoryPage));

        pool->size += TLPAGESIZE;
        pool->head = page;
        pool->tail = page;
    }

    // ####################################################################################
    // Search for a page that has enougth memory to honor the request.
    // ####################################################################################
    TLMemoryPage* page = pool->head;
    while (page != NULL) {
        u32 available = TLPAGESIZE - page->index;
        if (available < size) { page = page->next; continue; }
        break;
    }
    // ####################################################################################
    // Add a new page to the pool and honor the request with the newly added page storage
    // ####################################################################################
    if (page == NULL) {
        page = tl_memory_halloc(TL_MEMORY_TYPE_MEMORY_PAGE, sizeof(TLMemoryPage));
        pool->size += TLPAGESIZE;
        if (pool->tail != NULL) pool->tail->next = page;
        pool->tail = page;
    }

    void* block = &page->raw[page->index];
    page->index += size;
    return block;
}
