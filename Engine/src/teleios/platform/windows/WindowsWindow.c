#include "teleios/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"

LRESULT CALLBACK tl_platform_window_procedure(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#endif // TL_PLATFORM_WINDOWS
