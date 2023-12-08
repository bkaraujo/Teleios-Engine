#ifndef TELEIOS_EDITOR_LAYER_DEBUG_H
#define TELEIOS_EDITOR_LAYER_DEBUG_H

#include <teleios/types.h>

b8 editor_layer_debug_initialize(void);
b8 editor_layer_debug_terminate(void);

const TLLayer* editor_layer_debug_get(void);

#endif // TELEIOS_EDITOR_LAYER_DEBUG_H
