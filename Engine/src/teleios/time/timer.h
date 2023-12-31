#ifndef TELEIOS_TIME_TIMER_H
#define TELEIOS_TIME_TIMER_H

#include "teleios/types.h"

void tl_timer_begin(TLTimer* timer);
void tl_timer_reset(TLTimer* timer);
void tl_timer_update(TLTimer* timer);

f64 tl_timer_seconds(const TLTimer* timer);
f64 tl_timer_millis(const TLTimer* timer);
f64 tl_timer_micros(const TLTimer* timer);

#endif // TELEIOS_TIME_TIMER_H
