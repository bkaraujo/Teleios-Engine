#include "teleios/platform/manager.h"
#include "teleios/engine.h"
#include "teleios/logger.h"

TLAPI b8 tl_engine_pre_initialize(void) {
  if (!tl_platform_initialize()) {
    return false;
  }

  return true;
}

TLAPI b8 tl_engine_initialize(void) {
  TLTRACE("Trace");
  TLDEBUG("Trace");
  TLINFO("Trace");
  TLWARN("Trace");
  TLERROR("Trace");
  TLFATAL("Trace");
  return true;
}

TLAPI b8 tl_engine_run(void) {
  return true;
}

TLAPI b8 tl_engine_terminate(void) {
  if (!tl_platform_terminate()) {
    return false;
  }

  return true;
}
