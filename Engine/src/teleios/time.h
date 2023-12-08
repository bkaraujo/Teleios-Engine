#ifndef TELEIOS_TIME_H
#define TELEIOS_TIME_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI const u64 tl_time_epoch_seconds(void);
TLAPI const u64 tl_time_epoch_millis(void);
TLAPI const u64 tl_time_epoch_micros(void);
TLAPI void tl_time_wallclock(TLDateTime* dt);

#endif // TELEIOS_TIME_H
