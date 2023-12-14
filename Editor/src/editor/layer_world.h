#ifndef EDITOR_LAYER_WORLD_H
#define EDITOR_LAYER_WORLD_H

#include <teleios/types.h>

b8 editor_layer_world_initialize(void);
b8 editor_layer_world_terminate(void);

const TLLayer* editor_layer_world_get(void);

#endif // EDITOR_LAYER_WORLD_H
