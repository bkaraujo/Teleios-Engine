#ifndef TELEIOS_PLATFORM_TIME_H
#define TELEIOS_PLATFORM_TIME_H

#include "teleios/types.h"

void tl_platform_time_now(TLDateTime *dt);

const u64 tl_platform_time_epoch_micros(void);

#endif // TELEIOS_PLATFORM_TIME_H
