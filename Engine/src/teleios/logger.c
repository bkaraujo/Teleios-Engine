#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "teleios/logger.h"
#include "teleios/platform/console.h"
#include "teleios/platform/memory.h"
#include "teleios/platform/time.h"

static const char* LITERALS[6] = { "FATAL", "ERROR", "WARN ", "INFO ", "DEBUG", "TRACE" };
static const char* LAYOUT = "%d-%d-%d %d:%d:%d,%d %s - %s\n";

static char intermediate[1030];
static char formated[1030];

TLAPI void tl_logger_write(const u8 level, const char* message, ...) {
  tl_platform_memory_set(&intermediate, 0, 1030);

  va_list parameters; va_start(parameters, message);
  vsnprintf(intermediate, 1030, message, parameters);
  va_end(parameters);

  TLDateTime dt; tl_platform_time_now(&dt);

  tl_platform_memory_set(&formated, 0, 1030);
  sprintf_s(formated, 1030, LAYOUT, 
    dt.year, dt.month, dt.day,
    dt.hour, dt.minute, dt.second, dt.milliseconds,
    LITERALS[level], intermediate
  );

  tl_platform_stdout(level, formated);
  if (level == 0) exit(99);
}