#include "teleios/platform/manager.h"
#include "teleios/engine.h"
#include "teleios/logger.h"
#include "teleios/platform/time.h"

TLAPI b8 tl_engine_pre_initialize(void) {
  if (!tl_platform_initialize()) {
    return false;
  }

  return true;
}

TLAPI b8 tl_engine_initialize(void) {
  return true;
}

TLAPI b8 tl_engine_run(void) {
  while (true) {
    TLINFO("tl_platform_time_epoch: %u", tl_platform_time_epoch());

    tl_platform_update();
  }
  return true;
}

TLAPI b8 tl_engine_terminate(void) {
  if (!tl_platform_terminate()) {
    return false;
  }

  return true;
}
