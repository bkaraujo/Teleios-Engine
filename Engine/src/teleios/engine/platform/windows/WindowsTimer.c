#include "teleios/engine/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/engine/platform/windows/common.h"
#include "teleios/engine/chrono/timer.h"

void tl_chrono_timer_start(TLTimer* timer) {
    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);
    timer->start = time.QuadPart;
}

void tl_chrono_timer_update(TLTimer* timer) {
    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);
    timer->current = time.QuadPart;
}

f64 tl_chrono_timer_nanos(TLTimer* timer) {
    return ((timer->current - timer->start) * 10 * TLNANOS) / e_frequency.QuadPart;
}

f64 tl_chrono_timer_micros(TLTimer* timer) {
    return ((timer->current - timer->start) * 10 * TLMICROS) / e_frequency.QuadPart;
}

f64 tl_chrono_timer_millis(TLTimer* timer) {
    return ((timer->current - timer->start) * 10 * TLMILLIS) / e_frequency.QuadPart;
}

f64 tl_chrono_timer_seconds(TLTimer* timer) {
    return (timer->current - timer->start) / (e_frequency.QuadPart * TLSECONDS);
}

#endif // TL_PLATFORM_WINDOWS