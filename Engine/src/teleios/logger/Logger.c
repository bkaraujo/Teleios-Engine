#include "teleios/logger/console.h"
#include "teleios/platform/console.h"
#include "teleios/memory/operator.h"
#include "teleios/chrono/time.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define INTERMEDIATE 1024
#define FORMATED 1100
#define FORMAT "%u-%u-%u %u:%u:%u,%u %s - %s\n"

static i8 formatted[FORMATED];
static i8 intermediate[INTERMEDIATE];
static const char* NAMES[] = { "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE" };

TLEXPORT TLINLINE void tl_logger_console(u8 level, const i8* message, ...) {
    tl_memory_zero(&intermediate, INTERMEDIATE);

    va_list parameters;
    va_start(parameters, message);
    vsnprintf(intermediate, INTERMEDIATE, message, parameters);
    va_end(parameters);

    tl_memory_zero(&formatted, FORMATED);

    TLTime now = { 0 }; tl_chrono_time_now(&now);
    sprintf_s(formatted, FORMATED, FORMAT,
        now.year, now.month, now.day,                   // Date
        now.hour, now.minute, now.second, now.millis,   // Time
        NAMES[level],                                   // Level name
        intermediate                                    // Message
    );

    tl_platform_console_write(level, formatted);
    if (level == 0) {
        exit(EXIT_FAILURE);
    }
}