#include "teleios/container/array.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"
#include "teleios/logger/console.h"

typedef struct TLArray {
    u32 lenth;
    u32 size;
    TLALIGN(PSIZE) const void** payload;
} TLArray;

#define NullUnused                                                  \
{                                                                   \
    u64 avaliable = (u64)(array->size - array->lenth);              \
    tl_memory_zero(&array->payload[array->lenth], avaliable* PSIZE);\
}

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

    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array->payload, (u64)array->size * PSIZE);
    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array, sizeof(TLArray));
}

TLEXPORT u32 tl_array_lenth(TLArray* array) {
    if (array == NULL) { TLWARN("tl_array_lenth: TLArray is null"); return 0; }
    return array->lenth;
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
            tl_memory_copy(&array->payload[i + 1], &array->payload[i], (u64)(array->lenth - i - 1) * PSIZE);
            array->lenth--;
#ifdef TELEIOS_DEBUG
            NullUnused;
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

    return (void*)array->payload[index];
}

TLEXPORT b8 tl_array_append(TLArray* array, const void* payload) {
    if (array == NULL) { TLWARN("tl_array_append: TLArray is null"); return false; }
    return tl_array_insert(array, payload);
}

TLEXPORT b8 tl_array_prepend(TLArray* array, const void* payload) {
    if (array == NULL) { TLWARN("tl_array_prepend: TLArray is null"); return false; }
    return tl_array_insert_at(array, 0, payload);
}

TLEXPORT b8 tl_array_insert(TLArray* array, const void* payload) {
    if (array == NULL) { TLWARN("tl_array_insert: TLArray is null"); return false; }
    return tl_array_insert_at(array, array->lenth, payload);
}

TLEXPORT b8 tl_array_insert_at(TLArray* array, u32 index, const void* payload) {
    if (array == NULL) { TLWARN("tl_array_insert_at: TLArray is null"); return false; }
    if (payload == NULL) { TLWARN("tl_array_insert_at: Payload is null"); return false; }
    if (index > array->lenth) { TLWARN("tl_array_insert_at: Index beyond TLArray->length"); return false; }

    if (array->lenth >= array->size) {
        u32 newsize = (u32)((array->size * 1.5) + 1);
        void* resized = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_ARRAY, (u64)newsize * PSIZE);
        if (resized == NULL) { TLERROR("tl_array_insert_at: Failed to resize TLArray->payload"); return false; }

        tl_memory_copy(array->payload, resized, (u64)array->lenth * PSIZE);
        tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_ARRAY, array->payload, (u64)array->size * PSIZE);
        array->payload = resized;
        array->size = newsize;
    }

    // At begining
    if (index == 0) {
        if (array->lenth == 0) {
            array->payload[0] = payload;
            array->lenth++;
            return true;
        }

        tl_memory_copy(&array->payload[0], &array->payload[1], (u64)array->lenth * PSIZE);
        array->payload[0] = payload;
        array->lenth++;
    }

    // At end
    if (index == array->lenth - 1) {
        array->payload[array->lenth] = payload;
        array->lenth++;
    }

    // In middle
    if (index > 0 && index < array->lenth) {
        tl_memory_copy(&array->payload[index], &array->payload[index + 1], (u64)(array->lenth - index) * PSIZE);
        array->payload[index] = payload;
    }

    return true;
}

TLEXPORT b8 tl_array_remove_at(TLArray* array, u32 index) {
    if (array == NULL) { TLWARN("tl_array_remove_at: TLArray is null"); return false; }
    if (index > array->lenth) { TLWARN("tl_array_remove_at: Index beyond TLArray->length"); return false; }

    if (index != (array->lenth - 1) && array->lenth > 1) {
        tl_memory_copy(&array->payload[index + 1], &array->payload[index], (u64)(array->lenth - index - 1) * PSIZE);
    }

    array->lenth--;
#ifdef TELEIOS_DEBUG
    NullUnused;
#endif
    return false;
}

TLEXPORT b8 tl_array_merge(TLArray* array, const TLArray* from) {
    return false;
}

TLEXPORT b8 tl_array_purge(TLArray* array, const TLArray* unwanted) {
    return false;
}
