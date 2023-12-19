#ifndef TELEIOS_LOGGER_H
#define TELEIOS_LOGGER_H
#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_logger_initialize(void);
b8 tl_logger_terminate(void);

TLAPI void tl_logger_write(const u8 level, const char* message, ...);

#define TLFATAL(msg, ...) tl_logger_write(0, msg, ##__VA_ARGS__)
#define TLERROR(msg, ...) tl_logger_write(1, msg, ##__VA_ARGS__)
#define TLWARN(msg, ...) tl_logger_write(2, msg, ##__VA_ARGS__)
#define TLINFO(msg, ...) tl_logger_write(3, msg, ##__VA_ARGS__)
#define TLDEBUG(msg, ...) tl_logger_write(4, msg, ##__VA_ARGS__)
#define TLTRACE(msg, ...) tl_logger_write(5, msg, ##__VA_ARGS__)

#endif // TELEIOS_LOGGER_H
