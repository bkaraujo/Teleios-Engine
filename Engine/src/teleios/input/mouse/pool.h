#ifndef TELEIOS_INPUT_MOUSE_POOL_H
#define TELEIOS_INPUT_MOUSE_POOL_H

#include "teleios/types.h"
#include "teleios/input/mouse/keycode.h"

TLEXPORT b8 tl_input_mouse_is_active(TLInputMouse button);
TLEXPORT b8 tl_input_mouse_is_pressed(TLInputMouse button);
TLEXPORT b8 tl_input_mouse_is_released(TLInputMouse button);
TLEXPORT ivec2s tl_input_mouse_get_position(void);
TLEXPORT i8 tl_input_mouse_get_scroll(void);

#endif // TELEIOS_INPUT_MOUSE_POOL_H