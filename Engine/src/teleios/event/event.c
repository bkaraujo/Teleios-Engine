#include "teleios/container.h"
#include "teleios/logger.h"

static TLList* list[U8MAX];
// ##############################################################################################
//
//                                        SUBSCRIBER
//
// ##############################################################################################
#include "teleios/event/subcriber.h"
#include "teleios/event/codes.h"

b8 tl_event_subscribe(u8 code, PFN_EventHandler handler) {
  if (code != TL_EVENT_MAXIMUM) {
    if (!tl_list_append(list[code], handler)) {
      TLERROR("tl_event_subscribe: Failed to append handler to event handling list");
      return false;
    }

    return true;
  }

  for (unsigned i = 0; i < U8MAX; ++i) {
    if (!tl_list_append(list[i], handler)) {
      TLERROR("tl_event_subscribe: Failed to append handler to event handling list");
      return false;
    }
  }

  return true;
}
// ##############################################################################################
//
//                                        PUBLISHER
//
// ##############################################################################################
#include "teleios/event/publisher.h"

void tl_event_fire(u8 code, TLEvent* event) {
  TLNode* current = list[code]->head;
  while (current != NULL) {
    PFN_EventHandler handler = current->payload;
    if (!handler(code, event)) {
      break;
    }

    current = current->next;
  }
}
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/event/manager.h"

b8 tl_event_initialize(void) {
  for (unsigned i = 0; i < U8MAX; ++i) {
    list[i] = tl_list_create();
    if (list[i] == NULL) {
      TLERROR("tl_event_initialize: Failed to create list");
      return false;
    }
  }
  return true;
}

static b8 tl_dealocator(const void* payload) {
  return true;
}

b8 tl_event_terminate(void) {
  for (unsigned i = 0; i < U8MAX; ++i) {
    if (!tl_list_clear(list[i], tl_dealocator)) {
      TLERROR("tl_event_terminate: Failed to dealocat list node payload");
      return false;
    }

    if (!tl_list_destroy(list[i])) {
      TLERROR("tl_event_terminate: Failed to destroy list");
      return false;
    }

    list[i] = NULL;
  }

  return true;
}
