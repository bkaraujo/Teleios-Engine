#ifndef TELEIOS_LAYERSTACK_H
#define TELEIOS_LAYERSTACK_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLLayer* tl_layer_create(const char* name);
TLAPI b8 tl_layer_destroy(TLLayer* layer);

TLAPI b8 tl_engine_layer_append(const TLLayer* layer);
TLAPI b8 tl_engine_layer_remove(const TLLayer* layer);

#endif // TELEIOS_LAYERSTACK_H