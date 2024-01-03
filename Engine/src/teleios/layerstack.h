#ifndef TELEIOS_LAYER_STACK_H
#define TELEIOS_LAYER_STACK_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI b8 tl_layerstack_append(const TLLayer* layer);
TLAPI b8 tl_layerstack_remove(const TLLayer* layer);

#endif // TELEIOS_LAYERSTACK_H