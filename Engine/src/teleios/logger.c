#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "teleios/concurrent.h"
#include "teleios/logger.h"
#include "teleios/platform.h"
#include "teleios/platform_detector.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>

static CRITICAL_SECTION mutex;
#endif


// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################

static const char* LITERALS[6] = { "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE" };
static const char* LAYOUT = "[TID %llu] %d-%d-%d %d:%d:%d,%d %s - %s\n";

static char intermediate[1030];
static char formated[1030];

TLAPI void tl_logger_write(const u8 level, const char* message, ...) {
    tl_mutex_lock(&mutex);

    {
        tl_platform_memory_set(&intermediate, 0, 1030);

        va_list parameters; va_start(parameters, message);
        vsnprintf(intermediate, 1030, message, parameters);
        va_end(parameters);

        TLDateTime dt; tl_platform_time_now(&dt);

        tl_platform_memory_set(&formated, 0, 1030);
        sprintf_s(formated, 1030, LAYOUT,
            tl_thread_current_id(),
            dt.year, dt.month, dt.day,
            dt.hour, dt.minute, dt.second, dt.milliseconds,
            LITERALS[level], intermediate
        );

        tl_platform_stdout(level, formated);
        if (level == 0) exit(99);
    }

    tl_mutex_unlock(&mutex);
}

// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

b8 tl_logger_initialize(void) {
#ifdef TELEIOS_PLATFORM_WINDOWS
    InitializeCriticalSection(&mutex);
    return true;
#else
    return false;
#endif
}

b8 tl_logger_terminate(void) {
#ifdef TELEIOS_PLATFORM_WINDOWS
    DeleteCriticalSection(&mutex);
    return true;
#else
    return false;
#endif
}

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################
