#ifndef TELEIOS_PLATFORM_TIME_H
#define TELEIOS_PLATFORM_TIME_H

#include "teleios/types.h"

void tl_platform_time_now(TLDateTime *dt);
const u64 tl_platform_time_epoch_seconds(void);
const u64 tl_platform_time_epoch_millis(void);
const u64 tl_platform_time_epoch_micros(void);
const u64 tl_platform_time_epoch_nanos(void);

#endif // TELEIOS_PLATFORM_TIME_H
