#ifndef TELEIOS_CONTAINER_H
#define TELEIOS_CONTAINER_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLList* tl_list_create(void);
TLAPI b8 tl_list_clear(TLList* list, b8(*dealocator)(const void* payload));
TLAPI b8 tl_list_destroy(TLList* list);
TLAPI b8 tl_list_append(TLList* list, const void* payload);
TLAPI b8 tl_list_remove_payload(TLList* list, const void* payload);
TLAPI b8 tl_list_remove_node(TLList* list, const TLNode* node);
#endif // TELEIOS_CONTAINER_H
