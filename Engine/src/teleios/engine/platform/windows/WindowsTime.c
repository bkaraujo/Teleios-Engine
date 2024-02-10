#include "teleios/engine/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/engine/platform/windows/common.h"
#include "teleios/engine/chrono/time.h"

void tl_chrono_time_now(TLTime* time) {
    SYSTEMTIME lt = { 0 }; GetLocalTime(&lt);

    time->year = (u16)lt.wYear;
    time->month = (u8)lt.wMonth;
    time->day = (u8)lt.wDay;
    time->hour = (u8)lt.wHour;
    time->minute = (u8)lt.wMinute;
    time->second = (u8)lt.wSecond;
    time->millis = (u16)lt.wMilliseconds;
}

// Seconds from 1 Jan. 1601 00:00 to 1 Jan 1970 00:00 UTC
#define EPOCH_DIFFERENCE 11644473600LL

static f64 tl_platform_time_millis(void) {
    FILETIME ft = { 0 }; GetSystemTimeAsFileTime(&ft);
    LARGE_INTEGER li = { 0 };
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    return (li.QuadPart / (e_frequency.QuadPart * TLSECONDS)) - EPOCH_DIFFERENCE;
}

f64 tl_chrono_time_millis(void) {
    return tl_platform_time_millis();
}

f64 tl_chrono_time_micros(void) {
    return tl_platform_time_millis() * TLMILLIS;
}

#endif // TL_PLATFORM_WINDOWS