#include "teleios/platform/winapi/externals.h"
#include "teleios/platform/time.h"

void tl_platform_time_now(TLDateTime* dt) {
  SYSTEMTIME st; GetLocalTime(&st);
  
  dt->year = st.wYear;
  dt->month = st.wMonth;
  dt->day = st.wDay;
  dt->hour = st.wHour;
  dt->minute = st.wMinute;
  dt->second = st.wSecond;
  dt->milliseconds = st.wMilliseconds;
}

u64 tl_platform_time_epoch(void) {
  FILETIME ft = { 0 }; 
  GetSystemTimePreciseAsFileTime(&ft);

  // takes the last modified date
  LARGE_INTEGER date, adjust;
  date.HighPart = ft.dwHighDateTime;
  date.LowPart = ft.dwLowDateTime;

  // 100-nanoseconds = milliseconds * 10000
  adjust.QuadPart = 11644473600000 * 10000;

  // removes the diff between 1970 and 1601
  date.QuadPart -= adjust.QuadPart;

  // converts back from 100-nanoseconds to seconds
  return date.QuadPart / 10000000;
}