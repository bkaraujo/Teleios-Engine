#include "teleios/input/common.h"
#include "teleios/input/mouse/pool.h"

TLEXPORT b8 tl_input_mouse_is_active(TLInputMouse button) { return current.mouse[button]; }
TLEXPORT b8 tl_input_mouse_is_pressed(TLInputMouse button) { return current.mouse[button] && !previous.mouse[button]; }
TLEXPORT b8 tl_input_mouse_is_released(TLInputMouse button) { return !current.mouse[button] && previous.mouse[button]; }
TLEXPORT i8 tl_input_mouse_get_scroll(void) { return current.scroll; }
TLEXPORT ivec2s tl_input_mouse_get_position(void) { return current.position; }
