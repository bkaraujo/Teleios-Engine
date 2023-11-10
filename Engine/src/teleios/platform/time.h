#ifndef TELEIOS_PLATFORM_TIME_H
#define TELEIOS_PLATFORM_TIME_H

#include "teleios/types.h"

void tl_platform_time_now(TLDateTime *dt);
u64 tl_platform_time_epoch_seconds(void);
u64 tl_platform_time_epoch_millis(void);
u64 tl_platform_time_epoch_micros(void);
u64 tl_platform_time_epoch_nanos(void);

#endif // TELEIOS_PLATFORM_TIME_H
