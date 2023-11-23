#include "teleios/container.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

static const LSIZE = sizeof(TLList);
static const NSIZE = sizeof(TLNode);

TLAPI b8 tl_container_noop_dealocator(const void* pointer) {
    return true;
}

TLAPI TLList* tl_list_create(void) {
    TLList* list = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_LIST, LSIZE);
    if (list == NULL) {
        TLERROR("tl_list_create: Failed to allocate memory");
        return NULL;
    }

    return list;
}

TLAPI b8 tl_list_append(TLList* list, const void* payload) {
    if (list == NULL) {
        TLERROR("tl_list_append: list is null");
        return false;
    }

    if (payload == NULL) {
        TLERROR("tl_list_append: payload is null");
        return false;
    }

    TLNode* node = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
    node->payload = payload;

    switch (list->size) {
    case 0:
        list->head = node;
        break;
    case 1:
        list->head->next = node;
        node->previous = list->head;
        break;
    default:
        node->previous = list->tail;

    }

    list->size++;
    list->tail = node;
    return true;
}

TLAPI b8 tl_list_remove_node(TLList* list, const TLNode* node) {
    if (list == NULL) {
        TLERROR("tl_list_remove_node: list is null");
        return false;
    }

    if (node == NULL) {
        TLERROR("tl_list_remove_node: node is null");
        return false;
    }

    if (node == list->head) {
        list->size--;
        list->head = node->next;
        tl_memory_free(node, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);

        return true;
    }

    if (node == list->tail) {
        list->size--;
        list->tail = node->previous;
        tl_memory_free(node, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);

        return true;
    }

    list->size--;
    if (node->previous != NULL) node->previous->next = node->next;
    tl_memory_free(node, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);

    return true;
}

TLAPI const void* tl_list_remove(TLList* list, b8(*comparator)(const void*, const void*), const void* payload) {
    if (list == NULL) {
        TLERROR("tl_list_remove: list is null");
        return NULL;
    }

    if (payload == NULL) {
        TLERROR("tl_list_remove: payload is null");
        return NULL;
    }

    switch (list->size) {
    case 0:
        TLWARN("tl_list_remove: Trying to remove content from a empty list");
        return NULL;

    case 1:
        const void* element = list->head->payload;
        if (!comparator(element, payload))
            return false;

        tl_memory_free(list->head, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
        list->head = NULL;
        list->tail = NULL;
        list->size--;
        return element;

    default:
        if (comparator(list->head->payload, payload)) {
            const void* element = list->head->payload;
            TLNode* next = list->head->next;

            list->size--;
            tl_memory_free(list->head, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
            list->head = next;

            return element;
        }

        if (comparator(list->tail->payload, payload)) {
            const void* element = list->tail->payload;
            TLNode* previous = list->tail->previous;
            previous->next = NULL;

            list->size--;
            tl_memory_free(list->tail, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
            list->tail = previous;

            return element;
        }

        TLNode* current = list->head;
        while (current != NULL) {
            TLNode* next = current->next;
            if (!comparator(current->payload, payload)) {
                current = next;
                continue;
            }

            list->size--;
            const void* element = current->payload;
            current->previous->next = current->next;
            tl_memory_free(current, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
            return element;
        }
    }

    return NULL;
}

TLAPI b8 tl_list_contains(TLList* list, b8(*comparator)(const void*, const void*), const void* payload) {
    if (list == NULL) {
        TLWARN("tl_list_contains: list is null");
        return false;
    }

    if (payload == NULL) {
        TLWARN("tl_list_contains: payload is null");
        return false;
    }

    switch (list->size) {
    case 0:
        TLWARN("tl_list_contains: List is empty");
        return false;

    case 1:
        return comparator(list->head->payload, payload);

    default:
        TLNode* current = list->head;
        while (current != NULL) {
            if (comparator(current->payload, payload)) {
                return true;
            }
            current = current->next;
        }
    }

    return false;
}

static b8 tl_list_address_comparator(const void* first, const void* second) {
    return first == second;
}

TLAPI b8 tl_list_remove_payload(TLList* list, const void* payload) {
    return tl_list_remove(list, tl_list_address_comparator, payload) != NULL;
}

TLAPI b8 tl_list_clear(TLList* list, b8(*dealocator)(const void* payload)) {
    if (list == NULL) {
        TLERROR("tl_list_clear: list is null");
        return false;
    }

    if (dealocator == NULL) {
        TLERROR("tl_list_clear: dealocator is null");
        return false;
    }

    TLNode* current = list->head;
    while (current != NULL) {
        if (!dealocator(current->payload)) {
            TLERROR("tl_list_clear: Failed to dealocate TLNode->payload");
            return false;
        }

        current->payload = NULL;
        current = current->next;
    }

    return true;
}

TLAPI b8 tl_list_destroy(TLList* list) {
    if (list == NULL) {
        TLERROR("tl_list_clear: list is null");
        return false;
    }

    switch (list->size) {
    case 0: {
        tl_memory_free(list, TL_MEMORY_TYPE_CONTAINER_LIST, LSIZE);
    } break;

    case 1: {
        if (list->head->payload != NULL) {
            TLERROR("tl_list_destroy: TLNode->payload must be null");
            return false;
        }

        tl_memory_free(list->head, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
        tl_memory_free(list, TL_MEMORY_TYPE_CONTAINER_LIST, LSIZE);
    } break;

    default: {
        TLNode* current = list->head;
        while (current != NULL) {
            if (current->payload != NULL) {
                TLERROR("tl_list_destroy: TLNode->payload must be null");
                return false;
            }

            TLNode* next = current->next;
            tl_memory_free(current, TL_MEMORY_TYPE_CONTAINER_NODE, NSIZE);
            current = next;

            // Adjust internal state in case of 'return false'

            list->size--;
            list->head = current;
        }

        tl_memory_free(list, TL_MEMORY_TYPE_CONTAINER_LIST, LSIZE);
    }
    }

    return true;
}