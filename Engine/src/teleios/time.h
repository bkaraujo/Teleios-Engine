#ifndef TELEIOS_TIME_H
#define TELEIOS_TIME_H

#include "teleios/defines.h"
#include "teleios/types.h"

const u64 tl_time_epoch_seconds(void);
const u64 tl_time_epoch_millis(void);
const u64 tl_time_epoch_micros(void);
void tl_time_wallclock(TLDateTime* dt);

#endif // TELEIOS_TIME_H
