#include "teleios/platform/manager.h"
#include "teleios/memory/manager.h"
#include "teleios/engine.h"
#include "teleios/logger.h"
#include "teleios/timer.h"

TLAPI b8 tl_engine_pre_initialize(void) {
  if (!tl_platform_initialize()) {
    TLERROR("tl_engine_pre_initialize: Failed to initialize the underlying platform");
    return false;
  }

  if (!tl_memory_initialize()) {
    TLERROR("tl_engine_pre_initialize: Failed to initialize the memory manager");
    return false;
  }

  return true;
}
TLAPI b8 tl_engine_initialize(void) {
  return true;
}

TLAPI b8 tl_engine_run(void) {
  TLTimer timer; tl_timer_begin(&timer);
  
  u64 fps = 0;
  u64 ups = 0;

  while (true) {
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Frame initialization
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // CPU-bounded Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    ups++;

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // GPU-bounded Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    fps++;
    
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Frame finalization
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    tl_platform_update();
    tl_timer_update(&timer);

    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // One-Per-Second Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    if (timer.current > 10000000) {
      TLDEBUG("FPS %llu", fps);
      tl_timer_reset(&timer);
    
      fps = 0;
    }
  }

  return true;
}

TLAPI b8 tl_engine_terminate(void) {
  if (!tl_memory_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate the memory manager");
    return false;
  }

  if (!tl_platform_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate the underlying platform");
    return false;
  }

  return true;
}
