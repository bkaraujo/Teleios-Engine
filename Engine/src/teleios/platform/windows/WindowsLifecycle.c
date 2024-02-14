#include "teleios/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"
#include "teleios/platform/memory.h"
#include "teleios/logger/console.h"

HINSTANCE e_hinstance;
HANDLE e_heap;
HWND e_window;
LARGE_INTEGER e_frequency;
HANDLE e_hstdout;
CONSOLE_SCREEN_BUFFER_INFO e_csbi;

b8 tl_platform_initialize(void) {
    e_hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (e_hstdout == INVALID_HANDLE_VALUE) { return false; }

    if (!QueryPerformanceFrequency(&e_frequency)) {
        TLERROR("tl_platform_initialize: Failed to QueryPerformanceFrequency: 0x%x", GetLastError());
        return false;
    }

    TLDEBUG("tl_platform_initialize: Initializing the Platform");
    e_hinstance = GetModuleHandle(0);
    if (e_hinstance == NULL) { TLERROR("tl_platform_initialize: Failed to GetModuleHandle: 0x%x", GetLastError()); return false; }

    e_heap = HeapCreate(HEAP_NO_SERIALIZE, 0, 0);
    if (e_heap == NULL) { TLERROR("tl_platform_initialize: Failed to HeapCreate: 0x%x", GetLastError()); return false; }

    WNDCLASS wc = { 0 };
    wc.style = CS_DBLCLKS;  // Get double-clicks
    wc.lpfnWndProc = tl_platform_window_procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = e_hinstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);  // NULL; // Manage the cursor manually
    wc.hbrBackground = NULL;                   // Transparent
    wc.lpszClassName = TEXT("teleios_window_class");

    if (!RegisterClass(&wc)) { TLFATAL("tl_platform_initialize: Failed to RegisterClass: 0x%x", GetLastError()); return false; }

    return true;
}

void tl_platform_update(void) {
    if (e_window == NULL) return;

    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
        // TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

b8 tl_platform_terminate(void) {
    TLDEBUG("tl_platform_terminate: Terminating the Platform");
    if (e_heap != NULL) {
        HeapDestroy(e_heap);
        tl_platform_memory_set(&e_heap, 0, sizeof(e_heap));
    }

    tl_platform_memory_set(&e_csbi, 0, sizeof(e_csbi));
    tl_platform_memory_set(&e_hstdout, 0, sizeof(e_hstdout));
    tl_platform_memory_set(&e_frequency, 0, sizeof(e_frequency));
    tl_platform_memory_set(&e_hinstance, 0, sizeof(e_hinstance));

    return true;
}
#endif // TL_PLATFORM_WINDOWS