#ifndef TELEIOS_INPUT_COMMON_H
#define TELEIOS_INPUT_COMMON_H

#include "teleios/types.h"
#include "teleios/input/mouse/keycode.h"
#include "teleios/input/keyboard/keycode.h"

struct TLInput {
    i8 scroll;
    ivec2s position;
    b8 keyboard[TL_KEY_MAXIMUM];
    b8 mouse[TL_MOUSE_MAXIMUM];
};

static struct TLInput current;
static struct TLInput previous;

#endif // TELEIOS_INPUT_COMMON_H
