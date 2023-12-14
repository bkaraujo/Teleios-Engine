#include "editor/layer_debug.h"
#include "editor/layer_world.h"
#include <teleios/application.h>
#include <teleios/teleios.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Teleios Editor";
}

b8 tl_application_initialize(const TLSpecification* spec) {
    if (!editor_layer_debug_initialize()) {
        TLERROR("tl_application_initialize: Failed to create Debug Layer");
        return false;
    }
    if (!editor_layer_world_initialize()) {
        TLERROR("tl_application_initialize: Failed to create World Layer");
        return false;
    }

    return true;
}

b8 tl_application_terminate(void) {
    if (!editor_layer_world_terminate()) {
        TLERROR("tl_application_terminate: Failed to terminate world layer");
        return false;
    }

    if (!editor_layer_debug_terminate()) {
        TLERROR("tl_application_terminate: Failed to terminate debug layer");
        return false;
    }

    return true;
}