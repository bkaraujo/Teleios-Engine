#ifndef TELEIOS_CONTAINER_H
#define TELEIOS_CONTAINER_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI b8 tl_container_noop_dealocator(const void*);

TLAPI TLList* tl_list_create(void);
TLAPI b8 tl_list_clear(TLList* list, b8(*dealocator)(const void* payload));
TLAPI b8 tl_list_destroy(TLList* list);
TLAPI b8 tl_list_contains(TLList* list, b8(*comparator)(const void*, const void*), const void* payload);
TLAPI b8 tl_list_append(TLList* list, const void* payload);
TLAPI b8 tl_list_remove_payload(TLList* list, const void* payload);
TLAPI b8 tl_list_remove_node(TLList* list, const TLNode* node);
TLAPI const void* tl_list_remove(TLList* list, b8(*comparator)(const void*, const void*), const void* payload);
#endif // TELEIOS_CONTAINER_H
