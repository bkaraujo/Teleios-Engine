#include "teleios/input/common.h"
#include "teleios/input/keyboard/pool.h"

TLEXPORT b8 tl_input_key_is_active(TLInputKeyboard key) { return current.keyboard[key]; }
TLEXPORT b8 tl_input_key_is_pressed(TLInputKeyboard key) { return current.keyboard[key] && !previous.keyboard[key]; }
TLEXPORT b8 tl_input_key_is_released(TLInputKeyboard key) { return !current.keyboard[key] && previous.keyboard[key]; }

