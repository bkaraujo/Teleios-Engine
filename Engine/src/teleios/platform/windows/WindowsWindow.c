#include "teleios/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"

#include "teleios/application/event.h"
#include "teleios/event/publisher.h"
#include "teleios/input/keyboard/event.h"
#include "teleios/input/mouse/event.h"
#include "teleios/input/mouse/keycode.h"
#include "teleios/logger/console.h"
#include "teleios/window/event.h"
#include "teleios/window/window.h"

static b8 m_window_maximized;
static b8 m_window_minimized;

void tl_window_show(void) {
    if (e_window == NULL) return;
    ShowWindow(e_window, SW_SHOWNORMAL);
    tl_event_fire(TL_EVENT_WINDOW_SHOWED, NULL);
}

void tl_window_hide(void) {
    if (e_window == NULL) return;
    ShowWindow(e_window, SW_HIDE);
    tl_event_fire(TL_EVENT_WINDOW_HIDDEN, NULL);
}

b8 tl_window_is_minimized(void) {
    return m_window_minimized;
}

b8 tl_window_is_maximized(void) {
    return m_window_maximized;
}

void tl_window_destroy(void) {
    if (e_window == NULL) return;
    DestroyWindow(e_window);
    e_window = NULL;
    tl_event_fire(TL_EVENT_WINDOW_CLOSED, NULL);
}

b8 tl_window_create(const TLSpecification* spec) {
    // ===================================================
    // Define the window styles
    // ===================================================
    u32 window_style = WS_OVERLAPPEDWINDOW;
    u32 window_ex_style = WS_EX_APPWINDOW;
    // ===================================================
    // Adjust the window size to acomodate the borders
    // ===================================================
    u32 window_width = spec->window.width;
    u32 window_height = spec->window.height;

    RECT border_rect = { 0, 0, 0, 0 };
    if (!AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style)) {
        TLERROR("tl_window_create: Failed to AdjustWindowRectEx: 0x%x", GetLastError());
        return false;
    }

    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;
    // ===================================================
    // Centralize the window position
    // ===================================================
    u32 window_x = (GetSystemMetrics(SM_CXSCREEN) - window_width) / 2;
    u32 window_y = (GetSystemMetrics(SM_CYSCREEN) - window_height) / 2;
    // ===================================================
    // Create the window
    // ===================================================
    e_window = CreateWindowEx(
        window_ex_style,
        "teleios_window_class",
        spec->window.title,
        window_style,
        window_x, window_y,
        window_width, window_height,
        0,
        0,
        e_hinstance,
        0
    );
    // ===================================================
    // Validate and return
    // ===================================================
    if (e_window == 0) {
        TLERROR("tl_window_create: Failed to CreateWindowEx: 0x%x", GetLastError());
        e_window = NULL;
        return false;
    }

    tl_event_fire(TL_EVENT_WINDOW_CREATED, NULL);
    return true;
}

LRESULT CALLBACK tl_platform_window_procedure(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND: {
        return 1;
    }

    case WM_CLOSE: {
        tl_event_fire(TL_EVENT_APPLICATION_QUIT, NULL);
        return 0;
    }

    case WM_SETFOCUS: {
        tl_event_fire(TL_EVENT_WINDOW_FOCUS_GAINED, NULL);
    } break;

    case WM_KILLFOCUS: {
        tl_event_fire(TL_EVENT_WINDOW_FOCUS_LOST, NULL);
    } break;

    case WM_MOVE: {
        TLEvent event = { 0 };
        event.payload.i32[0] = (i32)(i16)LOWORD(lParam);   // horizontal position 
        event.payload.i32[1] = (i32)(i16)HIWORD(lParam);   // vertical position 
        tl_event_fire(TL_EVENT_WINDOW_MOVED, &event);
    } break;

    case WM_SIZE: {
        u8 code = 0;
        TLEvent event = { 0 };
        event.payload.u32[0] = LOWORD(lParam);
        event.payload.u32[1] = HIWORD(lParam);

        switch (wParam) {
        case SIZE_MINIMIZED: {
            m_window_minimized = true;
            m_window_maximized = false;
            code = TL_EVENT_WINDOW_MINIMIZED;
        } break;
        case SIZE_MAXIMIZED: {
            m_window_maximized = true;
            m_window_minimized = false;
            code = TL_EVENT_WINDOW_MAXIMIZED;
        } break;
        case SIZE_RESTORED: {
            if (m_window_maximized || m_window_minimized) {
                m_window_maximized = false;
                m_window_minimized = false;
                code = TL_EVENT_WINDOW_RESTORED;
            }
            else {
                m_window_maximized = false;
                m_window_minimized = false;
                code = TL_EVENT_WINDOW_RESIZED;
            }
        } break;
        }

        tl_event_fire(code, &event);
    } break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN: {
        TLEvent event = { 0 };
        switch (msg) {
        case WM_LBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_LEFT;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_PRESSED, &event);
        } break;
        case WM_MBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_MIDDLE;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_PRESSED, &event);
        } break;
        case WM_RBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_RIGHT;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_PRESSED, &event);
        } break;
        }
    } break;

    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP: {
        TLEvent event = { 0 };

        switch (msg) {
        case WM_LBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_LEFT;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_RELEASED, &event);
        } break;
        case WM_MBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_MIDDLE;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_RELEASED, &event);
        } break;
        case WM_RBUTTONDOWN: {
            event.payload.u8[0] = TL_MOUSE_RIGHT;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_RELEASED, &event);
        } break;
        }
    } break;

    case WM_MOUSEMOVE: {
        TLEvent event = { 0 };
        event.payload.i32[0] = GET_X_LPARAM(lParam);
        event.payload.i32[1] = GET_Y_LPARAM(lParam);
        tl_event_fire(TL_EVENT_INPUT_MOUSE_MOVED, &event);
    } break;

    case WM_MOUSEWHEEL: {
        i32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta != 0) {
            TLEvent event = { 0 };
            event.payload.i8[0] = (delta < 0) ? -1 : 1;
            tl_event_fire(TL_EVENT_INPUT_MOUSE_SCROLLED, &event);
        }
    } break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        TLEvent event = { 0 };
        event.payload.u16[0] = (u16)wParam;
        tl_event_fire(TL_EVENT_INPUT_KEY_PRESSED, &event);
    } break;

    case WM_KEYUP:
    case WM_SYSKEYUP: {
        TLEvent event = { 0 };
        event.payload.u16[0] = (u16)wParam;
        tl_event_fire(TL_EVENT_INPUT_KEY_RELEASED, &event);
    } break;

    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // TL_PLATFORM_WINDOWS
