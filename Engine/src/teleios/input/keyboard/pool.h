#ifndef TELEIOS_INPUT_KEYBOARD_POOL_H
#define TELEIOS_INPUT_KEYBOARD_POOL_H

#include "teleios/types.h"
#include "teleios/input/keyboard/keycode.h"

TLEXPORT b8 tl_input_key_is_active(TLInputKeyboard key);
TLEXPORT b8 tl_input_key_is_pressed(TLInputKeyboard key);
TLEXPORT b8 tl_input_key_is_released(TLInputKeyboard key);

#endif // TELEIOS_INPUT_KEYBOARD_POOL_H