#include "teleios/logger.h"
#include "teleios/platform/manager.h"
#include "teleios/platform/winapi/externals.h"

HINSTANCE hinstance;
HWND hwnd;
HANDLE hconsole;
HANDLE heap;

b8 tl_platform_initialize(void) {
  hinstance = GetModuleHandle(NULL);
  hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
  heap = HeapCreate(HEAP_NO_SERIALIZE, 1024 * 1024 * 1024, 0);

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
      TLERROR("Failed to destroy heap 0x%x", GetLastError());
      return false;
    }
  }
  return true;
}