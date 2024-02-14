#include "teleios/container/array.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"
#include "teleios/logger/console.h"

typedef struct TLArray {
    u32 lenth;
    u32 size;
    TLALIGN(PSIZE) void** payload;
} TLArray;

TLEXPORT TLArray* tl_array_create(void) {
    TLArray* array = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_ARRAY, sizeof(TLArray));
    if (array == NULL) {
        TLERROR("tl_array_create: Failed to allocate TLArray");
        return NULL;
    }

    array->size = 1;
    array->lenth = 0;
    array->payload = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_ARRAY, PSIZE);
    if (array->payload == NULL) {
        tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array, sizeof(TLArray));
        TLERROR("tl_array_create: Failed to allocate TLArray->payload");
        return NULL;
    }

    return array;
}

TLEXPORT void tl_array_destroy(TLArray* array) {
    if (array == NULL) { TLWARN("tl_array_destroy: TLArray is null"); return; }
    if (array->payload == NULL) { TLWARN("tl_array_destroy: TLArray->payload is null"); return; }

    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array->payload, array->size * PSIZE);
    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array, sizeof(TLArray));
}

TLEXPORT u32 tl_array_lenth(TLArray* array) {
    if (array == NULL) { TLWARN("tl_array_lenth: TLArray is null"); return 0; }
    return array->lenth;
}

static b8 tl_array_resize(TLArray* array) {
    u32 newsize = (u32)((array->size * 1.5) + 1);
    void* resized = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_ARRAY, newsize * PSIZE);
    if (resized == NULL) { TLERROR("tl_array_resize: Failed to allocate memory"); return false; }

    tl_memory_copy(array->payload, resized, array->lenth * PSIZE);
    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array->payload, array->size * PSIZE);
    array->payload = resized;
    array->size = newsize;

    return true;
}

TLEXPORT b8 tl_array_append(TLArray* array, void* payload) {
    if (array == NULL) { TLWARN("tl_array_append: TLArray is null"); return false; }
    if (payload == NULL) { TLWARN("tl_array_append: Payload is null"); return false; }

    if (array->lenth >= array->size) {
        if (!tl_array_resize(array)) {
            TLERROR("tl_array_append: Failed to resize array");
            return false;
        }
    }

    array->payload[array->lenth] = payload;
    array->lenth++;

    return true;
}

TLEXPORT b8 tl_array_prepend(TLArray* array, void* payload) {
    if (array == NULL) { TLWARN("tl_array_prepend: TLArray is null"); return false; }
    if (payload == NULL) { TLWARN("tl_array_prepend: Payload is null"); return false; }

    if (array->lenth == 0) {
        array->payload[0] = payload;
        array->lenth++;
        return true;
    }

    if (array->lenth >= array->size) {
        if (!tl_array_resize(array)) {
            TLERROR("tl_array_prepend: Failed to resize array");
            return false;
        }
    }

    tl_memory_copy(&array->payload[0], &array->payload[1], array->lenth * PSIZE);
    array->payload[0] = payload;
    array->lenth++;

    return true;
}

TLEXPORT b8 tl_array_contains(TLArray* array, void* payload) {
    if (array == NULL) { TLWARN("tl_array_contains: TLArray is null"); return false; }
    if (payload == NULL) { TLWARN("tl_array_contains: Payload is null"); return false; }

    for (unsigned i = 0; i < array->lenth; ++i) {
        if (array->payload[i] == payload) {
            return true;
        }
    }

    return false;
}

TLEXPORT b8 tl_array_remove(TLArray* array, void* payload) {
    if (array == NULL) { TLWARN("tl_array_remove: TLArray is null"); return false; }
    if (payload == NULL) { TLWARN("tl_array_remove: Payload is null"); return false; }

    for (unsigned i = 0; i < array->lenth; ++i) {
        if (array->payload[i] == payload) {
            tl_memory_copy(&array->payload[i + 1], &array->payload[i], (array->lenth - i - 1) * PSIZE);
            array->lenth--;
#ifdef TELEIOS_DEBUG
            // Zero the rest of the array to facilitate visual debugging
            u32 avaliable = array->size - array->lenth;
            tl_memory_zero(&array->payload[array->lenth], avaliable * PSIZE);
#endif
            return true;
        }
    }

    TLWARN("tl_array_get: Payload not found");
    return false;
}

TLEXPORT void* tl_array_get(TLArray* array, u32 index) {
    if (array == NULL) { TLWARN("tl_array_get: TLArray is null"); return false; }
    if (index >= array->lenth) { TLWARN("tl_array_get: TLArray is null"); return NULL; }

    return array->payload[index];
}

TLEXPORT b8 tl_array_insert(TLArray* array, u32 index, void* payload) {
    return false;
}

TLEXPORT b8 tl_array_remove_at(TLArray* array, u32 index) {
    return false;
}


TLEXPORT b8 tl_array_merge(TLArray* array, TLArray* from) {
    return false;
}

TLEXPORT b8 tl_array_purge(TLArray* array, TLArray* unwanted) {
    return false;
}
