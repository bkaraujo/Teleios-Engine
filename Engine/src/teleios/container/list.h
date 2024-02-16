#ifndef TELEIOS_CONTAINER_LIST_H
#define TELEIOS_CONTAINER_LIST_H

#include "teleios/types.h"

TLEXPORT TLList* tl_list_create(void);
TLEXPORT void tl_list_destroy(TLList* list);

TLEXPORT b8 tl_list_append(TLList* list, void* payload);
TLEXPORT b8 tl_list_prepend(TLList* list, void* payload);
TLEXPORT b8 tl_list_remove(TLList* list, void* payload);
TLEXPORT b8 tl_list_contains(TLList* list, void* payload);

TLEXPORT TLListIterator* tl_list_create_iterator(TLList* list);
TLEXPORT void tl_list_destroy_iterator(TLListIterator* iterator);
TLEXPORT b8 tl_list_has_next(TLListIterator* iterator);
TLEXPORT void* tl_list_next(TLListIterator* iterator);
TLEXPORT b8 tl_list_has_previous(TLListIterator* iterator);
TLEXPORT void* tl_list_previous(TLListIterator* iterator);

#endif // TELEIOS_CONTAINER_LIST_H