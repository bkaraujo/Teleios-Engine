#ifndef TELEIOS_ENGINE_CHRONO_TIMER_H
#define TELEIOS_ENGINE_CHRONO_TIMER_H

#include "teleios/types.h"

void tl_chrono_timer_start(TLTimer* timer);
void tl_chrono_timer_update(TLTimer* timer);

f64 tl_chrono_timer_nanos(TLTimer* timer);
f64 tl_chrono_timer_micros(TLTimer* timer);
f64 tl_chrono_timer_millis(TLTimer* timer);
f64 tl_chrono_timer_seconds(TLTimer* timer);

#endif // TELEIOS_ENGINE_CHRONO_TIMER_H