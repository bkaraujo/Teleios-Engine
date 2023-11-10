#include "teleios/platform/detector.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>

#include "teleios/logger.h"

static HINSTANCE hinstance;
// ##############################################################################################
//
//                                        MEMORY
//
// ##############################################################################################
#include "teleios/platform/memory.h"
static HANDLE heap;

void* tl_platform_memory_alloc(u64 size) {
  return HeapAlloc(heap, HEAP_ZERO_MEMORY, size);
}

void tl_platform_memory_free(void* block) {
  HeapFree(heap, HEAP_NO_SERIALIZE, block);
}

void tl_platform_memory_copy(const void* source, const void* target, u64 size) {
#pragma warning( push )
#pragma warning( disable : 4090)
  i32 error = memcpy_s(target, size, source, size);
#pragma warning( pop )
  if (error != 0) {
    TLFATAL("tl_platform_memory_copy: Failed with platform code: %d", error);
  }
}

void tl_platform_memory_set(const void* target, i32 value, u64 size) {
#pragma warning( push )
#pragma warning( disable : 4090)
  memset(target, value, size);
#pragma warning( pop )
}
// ##############################################################################################
//
//                                        CONSOLE
//
// ##############################################################################################
#include "teleios/platform/console.h"
#include <malloc.h>

static HANDLE hconsole;

#define TL_CONSOLE_BLACK        ( 0 )
#define TL_CONSOLE_DARKBLUE     ( FOREGROUND_BLUE )
#define TL_CONSOLE_DARKGREEN    ( FOREGROUND_GREEN )
#define TL_CONSOLE_DARKCYAN     ( FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_DARKRED      ( FOREGROUND_RED )
#define TL_CONSOLE_DARKMAGENTA  ( FOREGROUND_RED | FOREGROUND_BLUE )
#define TL_CONSOLE_DARKYELLOW   ( FOREGROUND_RED | FOREGROUND_GREEN )
#define TL_CONSOLE_DARKGRAY     ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_GRAY         ( FOREGROUND_INTENSITY )
#define TL_CONSOLE_BLUE         ( FOREGROUND_INTENSITY | FOREGROUND_BLUE )
#define TL_CONSOLE_GREEN        ( FOREGROUND_INTENSITY | FOREGROUND_GREEN )
#define TL_CONSOLE_CYAN         ( FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_RED          ( FOREGROUND_INTENSITY | FOREGROUND_RED )
#define TL_CONSOLE_MAGENTA      ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE )
#define TL_CONSOLE_YELLOW       ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN )
#define TL_CONSOLE_WHITE        ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )

static const u8 levels[6] = {
  BACKGROUND_RED | TL_CONSOLE_WHITE,
  TL_CONSOLE_DARKRED,
  TL_CONSOLE_DARKYELLOW,
  TL_CONSOLE_DARKGRAY,
  TL_CONSOLE_DARKCYAN,
  TL_CONSOLE_DARKMAGENTA
};

void tl_platform_stdout(u8 level, const char* message) {

  // Backup console screen buffer info
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo(hconsole, &csbi);

  // transforme the ansi to utf8
  i32 needed = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message, -1, NULL, 0);
  u16* utf8 = _malloca(sizeof(u16) * needed);
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message, -1, utf8, needed);

  // Apply the console color and print the message
  SetConsoleTextAttribute(hconsole, levels[level]);
  WriteConsole(hconsole, utf8, needed, NULL, 0);

  // Free and Restore resources
  _freea(utf8);
  SetConsoleTextAttribute(hconsole, csbi.wAttributes);
}

// ##############################################################################################
//
//                                        TIME
//
// ##############################################################################################
#include "teleios/platform/time.h"

u64 tl_platform_time_epoch(void) {
  // Read the system-time and set it to local-time
  FILETIME ft; GetSystemTimePreciseAsFileTime(&ft);
  FILETIME lft; FileTimeToLocalFileTime(&ft, &lft);

  // takes the last modified date
  LARGE_INTEGER date;
  date.HighPart = lft.dwHighDateTime;
  date.LowPart = lft.dwLowDateTime;

  // 100-nanoseconds = milliseconds * 10000
  LARGE_INTEGER adjust;
  adjust.QuadPart = 11644473600000 * 10000;

  // removes the diff between 1970 and 1601
  date.QuadPart -= adjust.QuadPart;
  return date.QuadPart;
}

u64 tl_platform_time_epoch_seconds(void) {
  return tl_platform_time_epoch() / 10000000;
}

u64 tl_platform_time_epoch_millis(void) {
  return tl_platform_time_epoch() / 10000;
}

u64 tl_platform_time_epoch_micros(void) {
  return tl_platform_time_epoch() / 10;
}

u64 tl_platform_time_epoch_nanos(void) {
  return tl_platform_time_epoch() * 100;
}

void tl_platform_time_now(TLDateTime* dt) {
  SYSTEMTIME st; GetLocalTime(&st);

#pragma warning( push )
#pragma warning( disable : 4244)
  dt->year = st.wYear;
  dt->month = st.wMonth;
  dt->day = st.wDay;
  dt->hour = st.wHour;
  dt->minute = st.wMinute;
  dt->second = st.wSecond;
  dt->milliseconds = st.wMilliseconds;
#pragma warning( pop ) 
}
// ##############################################################################################
//
//                                        TIMER
//
// ##############################################################################################
#include "teleios/timer.h"

static LARGE_INTEGER frequency;

void tl_timer_begin(TLTimer * timer) {
  LARGE_INTEGER start; QueryPerformanceCounter(&start);
  timer->start = start.QuadPart;
  timer->current = 0;
}

void tl_timer_reset(TLTimer* timer) {
  tl_timer_begin(timer);
}

void tl_timer_update(TLTimer* timer) {
  LARGE_INTEGER end; QueryPerformanceCounter(&end);
  timer->current = end.QuadPart - timer->start;
}

u64 tl_timer_micros(TLTimer* timer) {
  return (timer->current * 1000000) / frequency.QuadPart;
}
// ##############################################################################################
//
//                                        WINDOW
//
// ##############################################################################################
#include "teleios/platform/window.h"

static HWND hwnd;

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/platform/manager.h"

b8 tl_platform_initialize(void) {
  QueryPerformanceFrequency(&frequency);

  hinstance = GetModuleHandle(NULL);
  hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
  heap = HeapCreate(HEAP_NO_SERIALIZE, GiB(1), 0);

  return true;
}

b8 tl_platform_update(void) {
  MSG message;

  while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  return true;
}

b8 tl_platform_terminate(void) {


  if (heap != NULL) {
    if (!HeapDestroy(heap)) {
      TLERROR("tl_platform_terminate: Failed to destroy heap 0x%x", GetLastError());
      return false;
    }
  }
  return true;
}

#endif // TELEIOS_PLATFORM_WINDOWS
