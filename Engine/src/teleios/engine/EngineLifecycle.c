#include "teleios/logger/console.h"
#include "teleios/engine/Lifecycle.h"
#include "teleios/platform/Lifecycle.h"
#include "teleios/memory/lifecycle.h"
#include "teleios/event/lifecycle.h"
#include "teleios/input/lifecycle.h"
#include "teleios/window/window.h"

TLEXPORT b8 tl_engine_initialize(void) {
    if (!tl_platform_initialize()) return false;
    if (!tl_memory_initialize()) return false;
    if (!tl_event_initialize()) return false;
    if (!tl_input_initialize()) return false;
    if (!tl_window_create(1024, 768, "Teleios Engine")) return false;

    return true;
}

TLEXPORT b8 tl_engine_run(void) {
    tl_window_show();
    while (true) {

        tl_input_update();
        tl_platform_update();
    }

    tl_window_hide();
    return true;
}

TLEXPORT b8 tl_engine_terminate(void) {
    tl_window_destroy();
    if (!tl_input_terminate()) return false;
    if (!tl_event_terminate()) return false;
    if (!tl_memory_terminate()) return false;
    if (!tl_platform_terminate()) return false;
    return true;
}