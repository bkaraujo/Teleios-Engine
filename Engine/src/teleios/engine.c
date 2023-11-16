#include "teleios/platform/manager.h"
#include "teleios/platform/window.h"
#include "teleios/memory/manager.h"
#include "teleios/event/manager.h"
#include "teleios/event/subcriber.h"
#include "teleios/event/codes.h"
#include "teleios/input/manager.h"
#include "teleios/scene/manager.h"
#include "teleios/engine.h"
#include "teleios/logger.h"
#include "teleios/timer.h"

static b8 running = true;

TLAPI b8 tl_engine_pre_initialize(const TLSpecification* spec) {
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

static b8 tl_engine_eventhandler(const u8 code, const TLEvent* event) {
  if (code == TL_EVENT_APPLICATION_QUIT) {
    running = false;
  }

  return TL_EVENT_CONTINUE;
}

TLAPI b8 tl_engine_initialize(const TLSpecification* spec) {
  if (!tl_event_initialize()) {
    TLERROR("tl_engine_initialize: Failed to initialize the event manager");
    return false;
  }

  if (!tl_event_subscribe(TL_EVENT_MAXIMUM, tl_engine_eventhandler)) {
    TLERROR("tl_engine_initialize: Failed to register tl_engine_eventhandler");
    return false;
  }

  if (!tl_platform_window_create(spec)) {
    TLERROR("tl_engine_initialize: Failed create window");
    return false;
  }

  if (!tl_input_initialize()) {
    TLERROR("tl_engine_initialize: Failed to initialize input manager");
    return false;
  }

  if (!tl_scene_initialize()) {
    TLERROR("tl_engine_initialize: Failed to initialize scene manager");
    return false;
  }

  return true;
}

TLAPI b8 tl_engine_run(void) {
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
    tl_input_update();
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

TLAPI b8 tl_engine_terminate(void) {
  if (!tl_scene_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate scene manager");
    return false;
  }

  if (!tl_input_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate input manager");
    return false;
  }

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
