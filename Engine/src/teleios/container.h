#ifndef TELEIOS_CONTAINER_H
#define TELEIOS_CONTAINER_H

#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_container_noop_dealocator(const void*);

TLList* tl_list_create(void);
b8 tl_list_clear(TLList* list, b8(*dealocator)(const void* payload));
b8 tl_list_destroy(TLList* list);
TLList* tl_list_clone(TLList* list);
b8 tl_list_contains(TLList* list, b8(*comparator)(const void*, const void*), const void* payload);
b8 tl_list_append(TLList* list, const void* payload);
b8 tl_list_append_all(const TLList* soource, TLList* target);
b8 tl_list_remove_payload(TLList* list, const void* payload);
b8 tl_list_remove_node(TLList* list, const TLNode* node);
b8 tl_list_remove_all(TLList* list);
const void* tl_list_remove(TLList* list, b8(*comparator)(const void*, const void*), const void* payload);
#endif // TELEIOS_CONTAINER_H
