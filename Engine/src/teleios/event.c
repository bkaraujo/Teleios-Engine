#include "teleios/container.h"
#include "teleios/event.h"
#include "teleios/eventcodes.h"
#include "teleios/logger.h"

static TLList* list[U8MAX];

static b8 tl_dealocator(const void* payload);

// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################


// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

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

b8 tl_event_subscribe(const u8 code, PFN_EventHandler handler) {
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

void tl_event_fire(const u8 code, const TLEvent* event) {
    TLNode* current = list[code]->head;
    while (current != NULL) {
        PFN_EventHandler handler = (PFN_EventHandler)current->payload;
        if (handler(code, event) == TL_EVENT_STATUS_STOP) {
            break;
        }

        current = current->next;
    }
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

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################

static b8 tl_dealocator(const void* payload) {
    return true;
}
