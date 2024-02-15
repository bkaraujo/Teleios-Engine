#include "teleios/input/common.h"
#include "teleios/input/keyboard/event.h"
#include "teleios/input/lifecycle.h"
#include "teleios/input/mouse/event.h"
#include "teleios/logger/console.h"
#include "teleios/memory/operator.h"
#include "teleios/event/registry.h"

static b8 tl_input_handler(u16 code, const TLEvent* event) {
    switch (code) {
    case TL_EVENT_INPUT_KEY_PRESSED: current.keyboard[event->payload.u16[0]] = true; break;
    case TL_EVENT_INPUT_KEY_RELEASED: current.keyboard[event->payload.u16[0]] = false; break;


    case TL_EVENT_INPUT_MOUSE_PRESSED: current.keyboard[event->payload.u8[0]] = true; break;
    case TL_EVENT_INPUT_MOUSE_RELEASED: current.keyboard[event->payload.u8[0]] = false; break;
    case TL_EVENT_INPUT_MOUSE_SCROLLED: current.scroll = event->payload.i8[0]; break;
    case TL_EVENT_INPUT_MOUSE_MOVED: {
        current.position.x = event->payload.i32[0];
        current.position.y = event->payload.i32[1];
    } break;

    }
    return false;
}

b8 tl_input_initialize(void) {
    TLDEBUG("tl_input_initialize: Initializing the Input Subsystem");
    tl_memory_set(&current, 0, sizeof(struct TLInput));
    tl_memory_set(&previous, 0, sizeof(struct TLInput));

    if (!tl_event_register(TL_EVENT_INPUT_KEY_PRESSED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_KEY_PRESSED"); return false; }
    if (!tl_event_register(TL_EVENT_INPUT_KEY_RELEASED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_KEY_RELEASED"); return false; }

    if (!tl_event_register(TL_EVENT_INPUT_MOUSE_PRESSED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_MOUSE_PRESSED"); return false; }
    if (!tl_event_register(TL_EVENT_INPUT_MOUSE_RELEASED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_MOUSE_RELEASED"); return false; }
    if (!tl_event_register(TL_EVENT_INPUT_MOUSE_SCROLLED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_MOUSE_SCROLLED"); return false; }
    if (!tl_event_register(TL_EVENT_INPUT_MOUSE_MOVED, tl_input_handler)) { TLERROR("tl_input_initialize: Failed to register TL_EVENT_INPUT_MOUSE_MOVED"); return false; }

    return true;
}

void tl_input_update(void) {
    tl_memory_copy(&previous, &current, sizeof(struct TLInput));
}

b8 tl_input_terminate(void) {
    TLDEBUG("tl_input_terminate: Terminating the Input Subsystem");
    tl_memory_set(&current, 0, sizeof(struct TLInput));
    tl_memory_set(&previous, 0, sizeof(struct TLInput));

    return true;
}