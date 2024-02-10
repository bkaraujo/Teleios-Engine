#ifndef TELEIOS_CHRONO_TIME_H
#define TELEIOS_CHRONO_TIME_H

#include "Teleios/Types.h"

void tl_chrono_time_now(TLTime* time);
f64 tl_chrono_time_millis(void);
f64 tl_chrono_time_micros(void);

#endif // TELEIOS_CHRONO_TIME_H