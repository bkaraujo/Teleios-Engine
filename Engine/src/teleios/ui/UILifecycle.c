#include "teleios/logger/console.h"
#include "teleios/ui/lifecycle.h"
#include "teleios/ui/backend.h"

b8 tl_ui_initialize(void) {
    TLDEBUG("tl_ui_initialize: Initializing the UI System");
    if (!tl_uibackend_initialize()) {
        TLERROR("tl_ui_initialize: Failed to initialize backend");
        return false;
    }

    return true;
}

b8 tl_ui_update(void) {
    return tl_uibackend_update();
}

b8 tl_ui_terminate(void) {
    TLDEBUG("tl_ui_terminate: Terminating the UI System");
    if (!tl_uibackend_terminate()) {
        TLERROR("tl_ui_terminate: Failed to shutdown backend");
        return false;
    }

    return true;
}