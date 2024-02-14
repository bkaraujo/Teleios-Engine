#ifndef TELEIOS_CONTAINER_ARRAY_H
#define TELEIOS_CONTAINER_ARRAY_H

#include "teleios/types.h"

TLEXPORT TLArray* tl_array_create(void);
TLEXPORT void tl_array_destroy(TLArray* array);

TLEXPORT u32 tl_array_lenth(TLArray* array);

TLEXPORT b8 tl_array_append(TLArray* array, void* payload);
TLEXPORT b8 tl_array_prepend(TLArray* array, void* payload);
TLEXPORT b8 tl_array_contains(TLArray* array, void* payload);
TLEXPORT b8 tl_array_remove(TLArray* array, void* payload);

TLEXPORT void* tl_array_get(TLArray* array, u32 index);
TLEXPORT b8 tl_array_insert(TLArray* array, u32 index, void* payload);
TLEXPORT b8 tl_array_remove_at(TLArray* array, u32 index);

TLEXPORT b8 tl_array_merge(TLArray* array, TLArray* from);
TLEXPORT b8 tl_array_purge(TLArray* array, TLArray* unwanted);

#endif // TELEIOS_CONTAINER_ARRAY_H
