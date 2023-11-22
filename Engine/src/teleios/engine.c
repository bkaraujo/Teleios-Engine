#include "teleios/ecs/manager.h"
#include "teleios/engine.h"
#include "teleios/event/codes.h"
#include "teleios/event/manager.h"
#include "teleios/event/subcriber.h"
#include "teleios/input/manager.h"
#include "teleios/logger.h"
#include "teleios/memory/manager.h"
#include "teleios/platform/manager.h"
#include "teleios/platform/window.h"
#include "teleios/scene/active.h"
#include "teleios/scene/manager.h"
#include "teleios/time.h"
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
  TLDEBUG("tl_engine_initialize");
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

  if (!tl_ecs_initialize()) {
    TLERROR("tl_engine_initialize: Failed to initialize ecs manager");
    return false;
  }

  return true;
}

TLAPI b8 tl_engine_run(void) {
  TLDEBUG("tl_engine_run");
  u64 fps = 0;
  u64 ups = 0;
    
  TLTimer timer; tl_timer_begin(&timer);
  u64 time_last = tl_time_epoch_micros();

  u64 accumulator = 0;
  const u64 STEP = MICROSECOND / 165;
  
  tl_platform_window_show();
  while (running) {
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Frame initialization
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    const u64 time_start = tl_time_epoch_micros();
    const u64 time_delta = time_start - time_last;
    time_last = time_start;
    
    if (!tl_scene_prepare()) {
      TLERROR("tl_engine_run: Failed to prepare scene");
      return false;
    }
    
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // CPU-bounded Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    accumulator += time_delta;
    while (accumulator > STEP) {
      accumulator -= STEP;
      tl_scene_update_fixed(STEP);
      ups++;
    }
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // GPU-bounded Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    fps++;
    tl_scene_update(time_delta);
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // Frame finalization
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    tl_scene_update_after();
    tl_input_update();
    tl_platform_update();
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // One-Per-Second Rotines
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    tl_timer_update(&timer);
    if (tl_timer_seconds(&timer) >= SECOND) {
      TLDEBUG("FPS: %llu UPS: %llu", fps, ups);
      
      fps = ups = 0;
      tl_timer_reset(&timer);
    }
  }

  tl_platform_window_hide();
  return true;
}

TLAPI b8 tl_engine_terminate(void) {
  TLDEBUG("tl_engine_terminate");

  if (!tl_ecs_terminate()) {
    TLERROR("tl_engine_terminate: Failed to terminate ecs manager");
    return false;
  }

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
