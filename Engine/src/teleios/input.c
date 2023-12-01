#include "teleios/logger.h"
#include "teleios/memory/tools.h"

struct TLRegistry {
    b8 keyboard[TL_KEYS_MAXIMUM];
    b8 button[TL_MOUSE_BUTTON_MAXIMUM];
};

static i8 scroll;
static i32 mouse_x, mouse_y;
static struct TLRegistry previous;
static struct TLRegistry current;

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/input/pool.h"
TLAPI b8 tl_input_key_active(TLKeyboardKey key) { return current.keyboard[key]; }
TLAPI b8 tl_input_key_pressed(TLKeyboardKey key) { return !previous.keyboard[key] && current.keyboard[key]; }
TLAPI b8 tl_input_key_released(TLKeyboardKey key) { return previous.keyboard[key] && !current.keyboard[key]; }

TLAPI i32 tl_input_mouse_x(void) { return mouse_x; }
TLAPI i32 tl_input_mouse_y(void) { return mouse_y; }
TLAPI i8 tl_input_mouse_scroll(void) { return scroll; }

TLAPI b8 tl_input_mouse_active(TLMouseButton button) { return current.button[button]; }
TLAPI b8 tl_input_mouse_pressed(TLMouseButton button) { return !previous.button[button] && current.button[button]; }
TLAPI b8 tl_input_mouse_released(TLMouseButton button) { return previous.button[button] && !current.button[button]; }
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/event/subcriber.h"
#include "teleios/event/codes.h"

static TLEventStatus tl_input_event(const u8 code, const TLEvent* event) {
    switch (code) {
    case TL_EVENT_INPUT_KEY_PRESSED: current.keyboard[event->data.u16[0]] = true; break;
    case TL_EVENT_INPUT_KEY_RELEASED: current.keyboard[event->data.u16[0]] = false; break;
    case TL_EVENT_INPUT_MOUSE_PRESSED: current.button[event->data.u8[0]] = true; break;
    case TL_EVENT_INPUT_MOUSE_RELEASED: current.button[event->data.u8[0]] = true; break;
    case TL_EVENT_INPUT_MOUSE_WHELL: scroll = event->data.i8[0]; break;
    case TL_EVENT_INPUT_MOUSE_MOVE: mouse_x = event->data.i32[0]; mouse_y = event->data.i32[1]; break;
    }

    return TL_EVENT_STATUS_CONTUNE;
}

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/input/manager.h"
#include "teleios/memory/allocator.h"

b8 tl_input_initialize(void) {
    mouse_y = mouse_x = 0;
    tl_memory_set(&previous, 0, sizeof(struct TLRegistry));
    tl_memory_set(&current, 0, sizeof(struct TLRegistry));

    if (!tl_event_subscribe(TL_EVENT_INPUT_KEY_PRESSED, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_KEY_PRESSED");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_INPUT_KEY_RELEASED, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_KEY_RELEASED");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_INPUT_MOUSE_MOVE, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_MOUSE_MOVE");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_INPUT_MOUSE_WHELL, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_MOUSE_WHELL");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_INPUT_MOUSE_PRESSED, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_MOUSE_PRESSED");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_INPUT_MOUSE_RELEASED, tl_input_event)) {
        TLERROR("tl_input_initialize: Failed to subscribe for TL_EVENT_INPUT_MOUSE_RELEASED");
        return false;
    }

    return true;
}

void tl_input_update(void) {
    tl_memory_copy(&current, &previous, sizeof(struct TLRegistry));
}

b8 tl_input_terminate(void) {
    return true;
}
