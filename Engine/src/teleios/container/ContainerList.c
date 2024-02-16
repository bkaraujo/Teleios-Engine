#include "teleios/container/list.h"
#include "teleios/logger/console.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"

typedef struct TLNode {
    void* payload;
    struct TLNode* next;
    struct TLNode* previous;
} TLNode;

typedef struct TLList {
    u16 size;
    TLNode* head;
    TLNode* tail;
} TLList;

TLEXPORT TLList* tl_list_create(void) {
    TLList* container = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_LIST, sizeof(TLList));
    if (container == NULL) { TLERROR("tl_list_create: Failed to allocate memory"); return NULL; }
    return container;
}

TLEXPORT void tl_list_destroy(TLList* list) {
    if (list == NULL) { TLWARN("tl_list_destroy: List is null"); return; }
    if (list->size != 0) { TLWARN("tl_list_destroy: List is not empty"); return; }
    tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_LIST, list, sizeof(TLList));
}

TLEXPORT b8 tl_list_append(TLList* list, void* payload) {
    if (list == NULL) { TLWARN("tl_list_append: List is null"); return false; }
    if (payload == NULL) { TLWARN("tl_list_append: payload is null"); return false; }

    TLNode* node = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_LIST_NODE, sizeof(TLNode));
    node->payload = payload;
    node->next = NULL;

    switch (list->size) {
    case 0: {
        list->head = node;
    } break;

    case 1: {
        node->previous = list->head;
        list->head->next = node;
    } break;

    default: {
        node->previous = list->tail;
        list->tail->next = node;
    }
    }

    list->size++;
    list->tail = node;

    return true;
}

TLEXPORT b8 tl_list_prepend(TLList* list, void* payload) {
    if (list == NULL) { TLWARN("tl_list_prepend: List is null"); return false; }
    if (payload == NULL) { TLWARN("tl_list_prepend: payload is null"); return false; }

    TLNode* node = tl_memory_halloc(TL_MEMORY_TYPE_CONTAINER_LIST_NODE, sizeof(TLNode));
    node->payload = payload;
    node->next = list->head;

    list->head->previous = node;

    list->head = node;
    list->size++;

    return true;
}

TLEXPORT b8 tl_list_remove(TLList* list, void* payload) {
    if (list == NULL) { TLWARN("tl_list_remove: List is null"); return false; }
    if (payload == NULL) { TLWARN("tl_list_remove: payload is null"); return false; }

    switch (list->size) {
    case 0: {
        return false;
    }
    case 1: {
        if (list->head->payload != payload) {
            return false;
        }

        tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_LIST_NODE, list->head, sizeof(TLNode));
        list->head = NULL;
        list->tail = NULL;
    } break;
    default: {
        TLNode* current = list->head;
        while (current != NULL) {
            if (current->payload == payload) {
                if (current == list->head) list->head = current->next;
                if (current == list->tail) list->tail = current->previous;
                if (current->previous != NULL) current->previous->next = current->next;
                if (current->next != NULL) current->next->previous = current->previous;

                tl_memory_hfree(TL_MEMORY_TYPE_CONTAINER_LIST_NODE, current, sizeof(TLNode));
                break;
            }

            current = current->next;
        }
    }
    }

    list->size--;
    return true;
}

TLEXPORT b8 tl_list_contains(TLList* list, void* payload) {
    if (list == NULL) { TLWARN("tl_list_contains: List is null"); return false; }
    if (payload == NULL) { TLWARN("tl_list_contains: payload is null"); return false; }

    TLNode* current = list->head;
    while (current != NULL) {
        if (current->payload == payload) {
            return true;
        }

        current = current->next;
    }

    return false;
}

typedef struct TLListIterator {
    TLList* list;
    u16 size;
    TLNode* current;
}TLListIterator;

TLEXPORT TLListIterator* tl_list_create_iterator(TLList* list) {
    if (list == NULL) { TLWARN("tl_list_create_iterator: TLList is null"); return NULL; }
    TLListIterator* iterator = tl_memory_salloc(TL_MEMORY_TYPE_CONTAINER_LIST_ITERATOR, sizeof(TLListIterator));

    iterator->list = list;
    iterator->size = list->size;
    iterator->current = list->head;

    return iterator;
}

TLEXPORT void tl_list_destroy_iterator(TLListIterator* iterator) {
    if (iterator == NULL) {
        TLWARN("tl_list_destroy_iterator: TLListIterator is null");
        return;
    }

    tl_memory_sfree(TL_MEMORY_TYPE_CONTAINER_LIST_ITERATOR, iterator, sizeof(TLListIterator));
}

TLEXPORT b8 tl_list_has_next(TLListIterator* iterator) {
    if (iterator->list->size != iterator->size) {
        TLWARN("tl_list_has_next: TLList was modified");
        return false;
    }

    return iterator->current->next != NULL;
}

TLEXPORT void* tl_list_next(TLListIterator* iterator) {
    if (iterator->list->size != iterator->size) {
        TLWARN("tl_list_next: TLList was modified");
        return NULL;
    }

    iterator->current = iterator->current->next;
    return iterator->current;
}

TLEXPORT b8 tl_list_has_previous(TLListIterator* iterator) {
    if (iterator->list->size != iterator->size) {
        TLWARN("tl_list_has_previous: TLList was modified");
        return false;
    }
    return iterator->current->previous != NULL;
}

TLEXPORT void* tl_list_previous(TLListIterator* iterator) {
    if (iterator->list->size != iterator->size) {
        TLWARN("tl_list_previous: TLList was modified");
        return NULL;
    }

    iterator->current = iterator->current->previous;
    return iterator->current;
}
