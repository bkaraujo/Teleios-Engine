#ifndef TELEIOS_LAYER_H
#define TELEIOS_LAYER_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLLayer* tl_layer_create(const char* name);
TLAPI b8 tl_layer_destroy(TLLayer* layer);

#endif // TELEIOS_LAYERSTACK_H