#include "teleios/core/types.h"
#include "teleios/logger/console.h"
#include "teleios/container/array.h"
#include "teleios/event/lifecycle.h"
#include "teleios/event/publisher.h"
#include "teleios/event/registry.h"
#include "teleios/memory/allocator.h"

static TLArray* registry[U16MAX];

b8 tl_event_initialize(void) {
    TLDEBUG("tl_event_initialize: Initializing the Event System");
    for (unsigned i = 0; i < U16MAX; ++i) {
        registry[i] = tl_array_create();
        if (registry[i] == NULL) {
            TLERROR("tl_event_initialize: Failed to create registry");
            return false;
        }
    }

    return true;
}

b8 tl_event_register(u16 code, PFN_handler handler) {
    if (handler == NULL) {
        TLWARN("tl_event_register: Handler function is null");
        return false;
    }

    if (tl_array_contains(registry[code], handler)) {
        TLWARN("tl_event_register: Handler function already registered for event %llu", code);
        return false;
    }

    TLDEBUG("tl_event_register: %llu, %p)", code, handler);
    return tl_array_insert(registry[code], handler);
}

TLEXPORT void tl_event_fire(u16 code, const TLEvent* event) {
    for (unsigned i = 0; i < registry[code]->lenth; ++i) {
        PFN_handler handler = tl_array_get(registry[code], i);
        if (handler(code, event)) {
            break;
        }
    }
}

b8 tl_event_terminate(void) {
    TLDEBUG("tl_event_terminate: Terminating the Event System");
    for (unsigned i = 0; i < U16MAX; ++i) {
        tl_array_destroy(registry[i]);
    }

    return true;
}