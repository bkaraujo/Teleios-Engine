#include "teleios/logger/console.h"
#include "teleios/engine/Lifecycle.h"
#include "teleios/platform/Lifecycle.h"
#include "teleios/memory/lifecycle.h"
#include "teleios/event/lifecycle.h"

TLEXPORT b8 tl_engine_initialize(void) {
    if (!tl_platform_initialize()) return false;
    if (!tl_memory_initialize()) return false;
    if (!tl_event_initialize()) return false;

    return true;
}

TLEXPORT b8 tl_engine_run(void) {

    return true;
}

TLEXPORT b8 tl_engine_terminate(void) {

    if (!tl_event_terminate()) return false;
    if (!tl_memory_terminate()) return false;
    if (!tl_platform_terminate()) return false;
    return true;
}