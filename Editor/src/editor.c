#include "editor/layer_debug.h"
#include <teleios/application.h>
#include <teleios/logger.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Teleios Editor";
}

b8 tl_application_initialize(const TLSpecification* spec) {
    if (!editor_layer_debug_initialize()) {
        TLERROR("tl_application_initialize: Failed to create Debug Layer");
        return false;
    }

    return true;
}

b8 tl_application_terminate(void) {
    if (!editor_layer_debug_terminate()) {
        TLERROR("tl_application_terminate: Failed to terminate layer");
        return false;
    }

    return true;
}