#include "teleios/platform/detector.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>
#include <malloc.h>

#include "teleios/logger.h"
#include "teleios/string.h"

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

//void* tl_platform_memory_stack_alloc(const u64 size) {
//    return _malloca(size);
//}

//void tl_platform_memory_stack_free(void* block) {
//    _freea(block);
//}

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
  TL_CONSOLE_MAGENTA
};

void tl_platform_stdout(const u8 level, const char* message) {
    // Backup console screen buffer info
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hconsole, &csbi);

    // Apply the console color and print the message
    SetConsoleTextAttribute(hconsole, levels[level]);
    WriteConsole(hconsole, message, (DWORD)tl_string_length(message), NULL, NULL);

    // Free and Restore resources
    SetConsoleTextAttribute(hconsole, csbi.wAttributes);
}
// ##############################################################################################
//
//                                        FILESYSTEM
//
// ##############################################################################################
#include "teleios/filesystem/files.h"
#include "teleios/memory/allocator.h"

const u64 tl_filesyste_file_size(const char* path) {
    LARGE_INTEGER file_size = { 0 };

    WIN32_FILE_ATTRIBUTE_DATA file_attr_data;
    if (GetFileAttributesEx(path, GetFileExInfoStandard, &file_attr_data)) {
        file_size.LowPart = file_attr_data.nFileSizeLow;
        file_size.HighPart = file_attr_data.nFileSizeHigh;
        return file_size.QuadPart;
    }

    return 0;
}

const char* tl_filesystem_file_tochar(const char* path) {
    HANDLE file = CreateFile(path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (file == INVALID_HANDLE_VALUE) {
        TLERROR("tl_filesystem_file_load: Unable to open \"%s\".", path);
        return NULL;
    }

    u64 file_size = tl_filesyste_file_size(path);
    if (file_size == 0) {
        CloseHandle(file);
        TLERROR("tl_filesystem_file_load: Failed to read \"%s\" size.", path);
        return NULL;
    }
    // Using tl_memory_alloc instead of platform_alloc for better memory tracking
    char* payload = tl_memory_alloc(TL_MEMORY_TYPE_FILE, file_size + 1);

    DWORD read = 0;
    if (ReadFile(file, payload, (DWORD)(file_size), &read, NULL) == FALSE) {
        CloseHandle(file);
        tl_platform_memory_free(payload);
        TLERROR("tl_filesystem_file_load: Failed to read \"%s\".", path);
        return NULL;
    }

    if (read != file_size) {
        TLWARN("tl_filesystem_file_load: Expected %llu bytes but got %llu from \"%s\"", file_size, read, path);
    }

    CloseHandle(file);
    payload[file_size + 1] = '\0';
    return payload;
}

const u32* tl_filesystem_file_tou32(const char* path) {
    HANDLE file = CreateFile(path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (file == INVALID_HANDLE_VALUE) {
        TLERROR("tl_filesystem_file_load: Unable to open \"%s\".", path);
        return NULL;
    }

    u64 file_size = tl_filesyste_file_size(path);
    if (file_size == 0) {
        CloseHandle(file);
        TLERROR("tl_filesystem_file_load: Failed to read \"%s\" size.", path);
        return NULL;
    }
    // Using tl_memory_alloc instead of platform_alloc for better memory tracking
    u32* payload = tl_memory_alloc(TL_MEMORY_TYPE_FILE, file_size);

    DWORD read = 0;
    if (ReadFile(file, payload, (DWORD)(file_size), &read, NULL) == FALSE) {
        CloseHandle(file);
        tl_platform_memory_free(payload);
        TLERROR("tl_filesystem_file_load: Failed to read \"%s\".", path);
        return NULL;
    }

    if (read != file_size) {
        TLWARN("tl_filesystem_file_load: Expected %llu bytes but got %llu from \"%s\"", file_size, read, path);
    }

    CloseHandle(file);
    return payload;
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
    // FILETIME lft; FileTimeToLocalFileTime(&ft, &lft);

    // takes the last modified date
    LARGE_INTEGER date;
    date.HighPart = ft.dwHighDateTime;
    date.LowPart = ft.dwLowDateTime;

    // 100-nanoseconds = milliseconds * 10000
    LARGE_INTEGER adjust;
    adjust.QuadPart = 11644473600000 * 10000;

    // removes the diff between 1970 and 1601
    return date.QuadPart - adjust.QuadPart;
}

const u64 tl_platform_time_epoch_micros(void) {
    return tl_platform_time_epoch() / 10;
}

//const u64 tl_platform_time_epoch_nanos(void) {
//  return tl_platform_time_epoch() * 100;
//}

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
#include "teleios/time/timer.h"

static LARGE_INTEGER frequency;
void tl_timer_begin(TLTimer* timer) {
    LARGE_INTEGER start; QueryPerformanceCounter(&start);
    timer->start = start.QuadPart;
    timer->current = start.QuadPart;
}

void tl_timer_reset(TLTimer* timer) {
    tl_timer_begin(timer);
}

void tl_timer_update(TLTimer* timer) {
    LARGE_INTEGER end; QueryPerformanceCounter(&end);
    timer->current = end.QuadPart - timer->start;
}

f64 tl_timer_seconds(const TLTimer* timer) {
    return (timer->current / frequency.QuadPart) * 1.0;
}

f64 tl_timer_millis(const TLTimer* timer) {
    return (timer->current * 1000 / frequency.QuadPart) * 1.0;
}

f64 tl_timer_micros(const TLTimer* timer) {
    return (timer->current * 1000000 / frequency.QuadPart) * 1.0;
}
// ##############################################################################################
//
//                                        WINDOW
//
// ##############################################################################################
#include "teleios/platform/window.h"
#include "teleios/event/codes.h"
#include "teleios/event/publisher.h"
#include <windowsx.h>
#include <stdarg.h>
#include <stdio.h>

static HWND hwnd;
static b8 minimized = false;
static b8 maximized = false;
static b8 mouse_inside = false;
static const char* prefix;

b8 tl_platform_window_create(const TLSpecification* spec) {
    u32 window_width = spec->window.witdh;
    u32 window_height = spec->window.height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    // Obtain the size of the border.
    RECT border_rect = { 0, 0, 0, 0 };
    AdjustWindowRectEx(&border_rect, window_style, false, window_ex_style);

    // Ensure border doesn't overlap with desired content area
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    // Centralize the window
    u32 window_x = (GetSystemMetrics(SM_CXSCREEN) - spec->window.witdh) / 2;
    u32 window_y = (GetSystemMetrics(SM_CYSCREEN) - spec->window.height) / 2;

    hwnd = CreateWindowEx(
        window_ex_style,
        TEXT("teleios_window_class"),
        spec->name,
        window_style,
        window_x, window_y,
        window_width, window_height,
        0,
        0,
        hinstance,
        0
    );

    if (hwnd == NULL) {
        TLERROR("tl_platform_window_create: Failed to create window 0x%x", GetLastError());
        return false;
    }

    prefix = spec->name;
    return true;
}

void tl_platform_window_show(void) {
    if (hwnd == NULL) return;
    ShowWindow(hwnd, SW_SHOW);
}

void tl_platform_window_hide(void) {
    if (hwnd == NULL) return;
    ShowWindow(hwnd, SW_HIDE);
}

static char intermediate[80];
static char formated[100];
void tl_platform_window_set_title(const char* title, ...) {
    tl_platform_memory_set(intermediate, 0, 80);
    va_list parameters; va_start(parameters, title);
    vsnprintf(intermediate, 80, title, parameters);
    va_end(parameters);

    tl_platform_memory_set(formated, 0, 80);
    sprintf_s(formated, 100, "%s (%s)", prefix, intermediate);
    SetWindowText(hwnd, formated);
}

LRESULT CALLBACK tl_platform_window_procedure(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND: {
        // Background will be handled by the graphics context
        return 1;
    } break;

    case WM_CLOSE: {
        PostQuitMessage(0);
        tl_event_fire(TL_EVENT_APPLICATION_QUIT, NULL);
        return 0;
    } break;

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    } break;

    case WM_SETFOCUS: {
        tl_event_fire(TL_EVENT_WINDOW_FOCUS_GAINED, NULL);
    } break;

    case WM_KILLFOCUS: {
        tl_event_fire(TL_EVENT_WINDOW_FOCUS_LOST, NULL);
    } break;

    case WM_MOVE: {
        TLEvent event = { 0 };
        event.data.i32[0] = (i32)(i16)LOWORD(lParam);
        event.data.i32[1] = (i32)(i16)HIWORD(lParam);
        tl_event_fire(TL_EVENT_WINDOW_MOVED, &event);
    } break;

    case WM_SIZE: {
        switch (wParam) {

        case SIZE_MINIMIZED: {
            maximized = false;
            minimized = true;
            tl_event_fire(TL_EVENT_WINDOW_MINIMIZED, NULL);
        } break;

        case SIZE_MAXIMIZED: {
            maximized = true;
            minimized = false;
            tl_event_fire(TL_EVENT_WINDOW_MAXIMIZED, NULL);
        } break;

        case SIZE_RESTORED: {
            if (maximized || minimized) {
                maximized = false;
                minimized = false;
                tl_event_fire(TL_EVENT_WINDOW_RESTORED, NULL);
            }
            else {
                TLEvent event = { 0 };
                event.data.u32[0] = LOWORD(lParam);
                event.data.u32[1] = HIWORD(lParam);
                tl_event_fire(TL_EVENT_WINDOW_RESIZED, &event);
            }
        } break;
        }
    } break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        TLEvent event = { 0 };
        event.data.u16[0] = (u16)wParam;

        tl_event_fire(TL_EVENT_INPUT_KEY_PRESSED, &event);

        // Prevent default window behaviour such as context menu, etc.
        return 0;
    } break;

    case WM_KEYUP:
    case WM_SYSKEYUP: {
        TLEvent event = { 0 };
        event.data.u16[0] = (u16)wParam;

        tl_event_fire(TL_EVENT_INPUT_KEY_RELEASED, &event);
    } break;

    case WM_MOUSELEAVE: {
        mouse_inside = false;
        tl_event_fire(TL_EVENT_INPUT_MOUSE_LEAVE, NULL);
    } break;

    case WM_MOUSEMOVE: {
        TLEvent event = { 0 };
        event.data.i32[0] = GET_X_LPARAM(lParam);
        event.data.i32[1] = GET_Y_LPARAM(lParam);

        if (!mouse_inside) {
            TRACKMOUSEEVENT e = { 0 };
            e.cbSize = sizeof(TRACKMOUSEEVENT);
            e.dwFlags = TME_LEAVE;
            e.hwndTrack = hwnd;
            TrackMouseEvent(&e);

            mouse_inside = true;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_ENTER, &event);
        }
        else {
            tl_event_fire(TL_EVENT_INPUT_MOUSE_MOVE, &event);
        }
    } break;

    case WM_MOUSEWHEEL: {
        i32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta != 0) {
            TLEvent event = { 0 };
            event.data.i8[0] = (delta < 0) ? -1 : 1;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_WHELL, &event);
        }
    } break;

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP: {
        TLEvent event = { 0 };

        switch (msg) {
        case WM_LBUTTONUP: event.data.u8[0] = TL_MOUSE_BUTTON_LEFT; break;
        case WM_RBUTTONUP: event.data.u8[0] = TL_MOUSE_BUTTON_RIGHT; break;
        case WM_MBUTTONUP: event.data.u8[0] = TL_MOUSE_BUTTON_MIDDLE; break;
        }

        tl_event_fire(TL_EVENT_INPUT_MOUSE_RELEASED, &event);
    } break;

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        TLEvent event = { 0 };

        switch (msg) {
        case WM_LBUTTONDOWN: event.data.u8[0] = TL_MOUSE_BUTTON_LEFT; break;
        case WM_RBUTTONDOWN: event.data.u8[0] = TL_MOUSE_BUTTON_RIGHT; break;
        case WM_MBUTTONDOWN: event.data.u8[0] = TL_MOUSE_BUTTON_MIDDLE; break;
        }

        tl_event_fire(TL_EVENT_INPUT_MOUSE_PRESSED, &event);
    } break;
    }
    return DefWindowProcA(hwnd, msg, wParam, lParam);
}
// ##############################################################################################
//
//                                        INFO
//
// ##############################################################################################
#include "teleios/platform/info.h"

void* tl_platform_handle(void) {
    return &hinstance;
}

void* tl_platform_window_handle(void) {
    return &hwnd;
}

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
    heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);

    WNDCLASS wc = { 0 };
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = tl_platform_window_procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = TEXT("teleios_window_class");

    if (!RegisterClass(&wc)) {
        TLERROR("tl_platform_initialize: Failed to RegisterClass 0x%x", GetLastError());
        return false;
    }

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
