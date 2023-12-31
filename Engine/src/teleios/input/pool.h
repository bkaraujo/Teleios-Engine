#ifndef TELEIOS_INPUT_POOL_H
#define TELEIOS_INPUT_POOL_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI b8 tl_input_key_active(TLKeyboardKey key);
TLAPI b8 tl_input_key_pressed(TLKeyboardKey key);
TLAPI b8 tl_input_key_released(TLKeyboardKey key);

TLAPI i8 tl_input_mouse_scroll(void);
TLAPI i32 tl_input_mouse_x(void);
TLAPI i32 tl_input_mouse_y(void);

TLAPI b8 tl_input_mouse_active(TLMouseButton button);
TLAPI b8 tl_input_mouse_pressed(TLMouseButton button);
TLAPI b8 tl_input_mouse_released(TLMouseButton button);

#endif // TELEIOS_INPUT_POOL_H
