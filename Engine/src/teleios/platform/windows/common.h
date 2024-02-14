#ifndef TELEIOS_PLATFORM_WINDOW_COMMON_H
#define TELEIOS_PLATFORM_WINDOW_COMMON_H

#include <Windows.h>
#include <windowsx.h>
#include "teleios/types.h"

extern HINSTANCE e_hinstance;
extern HANDLE e_heap;
extern HWND e_window;
extern LARGE_INTEGER e_frequency;
extern HANDLE e_hstdout;
extern CONSOLE_SCREEN_BUFFER_INFO e_csbi;

LRESULT CALLBACK tl_platform_window_procedure(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam);

#endif // TELEIOS_PLATFORM_WINDOW_COMMON_H
