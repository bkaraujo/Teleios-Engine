#include "teleios/container.h"
#include "teleios/identity/generator.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

static TLList* layers;
// ##############################################################################################
//
//                                        STACK
//
// ##############################################################################################
#include "teleios/layer/stack.h"
TLAPI TLIdentity* tl_layer_stack_create(
    const char* name,
    b8(*on_attach)(void),
    b8(*on_detach)(void),
    b8(*update_variable)(const u64 delta),
    b8(*update_fixed)(const u64 delta),
    b8(*update_after)(void)
) {
    TLIdentity* layerid = tl_memory_alloc(TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
    if (layerid == NULL) {
        TLERROR("tl_layer_stack_create: Failed to allocate TLIdentity");
        return NULL;
    }

    tl_identity_generate(layerid);
    TLLayer* layer = tl_memory_alloc(TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
    if (layer == NULL) {
        TLERROR("tl_layer_stack_create: Failed to allocate TLLayer");
        return NULL;
    }

    TLDEBUG("tl_layer_stack_create: TLLayer(\"%s\")->on_attach()", name);
    if (!on_attach()) {
        tl_memory_free(layerid, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
        tl_memory_free(layer, TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
        TLERROR("tl_layer_stack_create: Failed to TLLayer(\"%s\")->on_attach()", name);
        return NULL;
    }

    layer->identity = layerid;
    layer->name = name == NULL ? "Layer" : name;
    if (!tl_list_append(layers, layer)) {
        tl_memory_free(layerid, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
        tl_memory_free(layer, TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
        TLERROR("tl_layer_stack_create: Failed to append TLLayer");
        return NULL;
    }

    layer->on_attach = on_attach;
    layer->on_detach = on_detach;
    layer->update_after = update_after;
    layer->update_fixed = update_fixed;
    layer->update_variable = update_variable;

    return layerid;
}

TLAPI b8 tl_layer_stack_destroy(TLIdentity* layerid) {
    if (layerid == NULL) {
        TLWARN("tl_layer_stack_destroy: layerid is null");
        return false;
    }

    TLNode* current = layers->head;
    while (current != NULL) {
        const TLLayer* layer = current->payload;
        if (tl_identity_equals(layerid, layer->identity)) {
            TLDEBUG("tl_layer_stack_destroy: TLLayer(\"%s\")->on_detach()", layer->name);
            if (!layer->on_detach()) {
                TLERROR("tl_layer_stack_destroy: Failed to TLLayer(\"%s\")->on_detach()", layer->name);
                return false;
            }

            tl_memory_free(layer, TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
            tl_memory_free(layerid, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
            if (!tl_list_remove_node(layers, current)) {
                TLERROR("tl_layer_stack_destroy: Failed to remove TLLayer from list");
                return false;
            }
            return true;
        }

        current = current->next;
    }

    return false;
}

// ##############################################################################################
//
//                                        STACK
//
// ##############################################################################################
#include "teleios/layer/manager.h"

void tl_layer_stack_set(TLList* stack) {
    layers = stack;
}
