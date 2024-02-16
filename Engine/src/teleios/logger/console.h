#ifndef TELEIOS_LOGGER_CONSOLE_H
#define TELEIOS_LOGGER_CONSOLE_H

#include "teleios/types.h"

TLEXPORT TLINLINE void tl_logger_console(u8 level, const i8* message, ...);

#define TLFATAL(message, ...) tl_logger_console(0, message, ##__VA_ARGS__)
#define TLERROR(message, ...) tl_logger_console(1, message, ##__VA_ARGS__)
#define TLWARN(message, ...) tl_logger_console(2, message, ##__VA_ARGS__)
#define TLINFO(message, ...) tl_logger_console(3, message, ##__VA_ARGS__)

#if defined(TELEIOS_RELEASE) || defined(TELEIOS_DIST)
#   define TLDEBUG(message, ...)
#   define TLTRACE(message, ...)
#else
#   define TLDEBUG(message, ...) tl_logger_console(4, message, ##__VA_ARGS__)
#   define TLTRACE(message, ...) tl_logger_console(5, message, ##__VA_ARGS__)
#endif

#endif // TELEIOS_LOGGER_CONSOLE_H