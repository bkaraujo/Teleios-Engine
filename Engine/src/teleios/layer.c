#include "teleios/teleios.h"
#include "teleios/layer.h"

// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################

TLAPI TLLayer* tl_layer_create(const char* name) {
    TLLayer* layer = tl_memory_alloc(TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
    if (layer == NULL) {
        TLERROR("tl_layer_create: Failed to create layer");
        return NULL;
    }

    layer->name = name;
    return layer;
}

TLAPI b8 tl_layer_destroy(TLLayer* layer) {
    if (layer == NULL) return true;

    if (!tl_layerstack_remove(layer)) {
        TLERROR("editor_layer_debug_terminate: Engine refused to remove layer");
        return false;
    }

    tl_memory_free(layer, TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
    return true;
}

// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################
