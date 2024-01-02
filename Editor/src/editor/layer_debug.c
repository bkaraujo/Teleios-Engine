#include "editor/layer_debug.h"
//#include <glad/glad.h>
#include <teleios/teleios.h>

static b8 editor_layer_initialize(void) {
    TLDEBUG("editor_layer_initialize: Initializing Editor Debug Layer");
    return true;
}

static b8 editor_layer_terminate(void) {
    TLDEBUG("editor_layer_terminate: Terminating Editor Debug Layer");
    return true;
}

static b8 editor_layer_update(const u64 delta) {
    return true;
}

static b8 editor_ayer_update_fixed(const u64 delta) {
    return true;
}

static b8 line = false;
static b8 editor_layer_update_late(void) {

    if (tl_input_key_released(TL_KEY_ESCAPE)) {
        tl_event_fire(TL_EVENT_APPLICATION_QUIT, NULL);
    }

    //if (tl_input_key_released(TL_KEY_F12)) {
    //    line = !line;

    //    if (line)   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    else        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //}

    return true;
}

static TLLayer* layer;
const TLLayer* editor_layer_debug_get(void) {
    return layer;
}

b8 editor_layer_debug_initialize(void) {
    layer = tl_layer_create("Editor Debug");
    if (layer == NULL) {
        TLERROR("editor_layer_debug_initialize: Failed to create layer");
        return false;
    }

    layer->initialize = editor_layer_initialize;
    layer->terminate = editor_layer_terminate;
    layer->update = editor_layer_update;
    layer->update_fixed = editor_ayer_update_fixed;
    layer->update_late = editor_layer_update_late;

    if (!tl_engine_layer_append(editor_layer_debug_get())) {
        TLERROR("editor_layer_debug_initialize: Engine refused to append layer");
        return false;
    }

    return true;
}

b8 editor_layer_debug_terminate(void) {
    return tl_layer_destroy(layer);
}
