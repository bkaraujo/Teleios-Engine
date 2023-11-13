#include "teleios/container.h"
#include "teleios/memory/allocator.h"
#include "teleios/logger.h"

static const LSIZE = sizeof(TLList);
static const NSIZE = sizeof(TLNode);

TLAPI TLList* tl_list_create(void) {
  TLList* list = tl_memory_alloc(TL_MEMORY_TYPE_CONTAINER_LIST, LSIZE);
  if (list == NULL) {
    TLERROR("tl_list_create: Failed to allocate memory");
    return NULL;
  }

  return list;
}

TLAPI b8 tl_list_append(TLList* list, void* payload) {
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

TLAPI b8 tl_list_clear(TLList* list, b8 (*dealocator)(const void* payload)) {
  if (list == NULL) {
    TLERROR("tl_list_clear: list is null");
    return false;
  }

  if (list->size == 0) {
    return true;
  }

  if (dealocator == NULL) {
    TLERROR("tl_list_clear: dealocator is null");
    return false;
  }

  TLNode* current = list->head;
  for (unsigned i = 0; i < list->size; ++i) {
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
      for (unsigned i = 0; i < list->size; ++i) {
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