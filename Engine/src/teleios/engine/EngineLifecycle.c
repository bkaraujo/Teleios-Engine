#include "teleios/engine/Lifecycle.h"
#include "teleios/logger/console.h"
#include "teleios/platform/Lifecycle.h"

TLEXPORT b8 tl_engine_initialize(void) {
    if (!tl_platform_initialize()) return false;

    return true;
}

TLEXPORT b8 tl_engine_run(void) {

    tl_logger_console(5, "5");
    tl_logger_console(4, "4");
    tl_logger_console(3, "3");
    tl_logger_console(2, "2");
    tl_logger_console(1, "1");

    return true;
}

TLEXPORT b8 tl_engine_terminate(void) {

    if (!tl_platform_terminate()) return false;
    return true;
}