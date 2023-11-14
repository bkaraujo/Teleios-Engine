#include "teleios/platform/manager.h"
#include "teleios/platform/window.h"
#include "teleios/memory/manager.h"
#include "teleios/event/manager.h"
#include "teleios/event/subcriber.h"
#include "teleios/event/codes.h"
#include "teleios/engine.h"
#include "teleios/logger.h"
#include "teleios/timer.h"

static b8 running = true;

b8 tl_engine_pre_initialize(TLSpecification* spec) {
  if (!tl_platform_initialize(spec)) {
    TLERROR("tl_engine_pre_initialize: Failed to initialize the underlying platform");
    return false;
  }

  if (!tl_memory_initialize()) {
    TLERROR("tl_engine_pre_initialize: Failed to initialize the memory manager");
    return false;
  }

  return true;
}

b8 tl_engine_eventhandler(u8 code, TLEvent* event) {
  if (code == TL_EVENT_CODE_APPLICATION_QUIT) {
    running = false;
  }

  return TL_EVENT_CONTINUE;
}

b8 tl_engine_initialize(TLSpecification* spec) {
  if (!tl_event_initialize()) {
    TLERROR("tl_engine_initialize: Failed to initialize the event manager");
    return false;
  }

  if (!tl_event_subscribe(TL_EVENT_CODE_MAXIMUM, tl_engine_eventhandler)) {
    TLERROR("tl_engine_initialize: Failed to register tl_engine_eventhandler");
    return false;
  }

  if (!tl_platform_window_create(spec)) {
    TLERROR("tl_engine_initialize: Failed create window");
    return false;
  }

  return true;
}

b8 tl_engine_run(void) {
  TLTimer timer; tl_timer_begin(&timer);
  
  u64 fps = 0;
  u64 ups = 0;

  tl_platform_window_show();
  while (running) {
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
      tl_platform_window_set_title("FPS %llu", fps);
      tl_timer_reset(&timer);
    
      fps = 0;
    }
  }

  tl_platform_window_hide();
  return true;
}

b8 tl_engine_terminate(void) {
  if (!tl_event_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate the event manager");
    return false;
  }

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
