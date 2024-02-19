#include "teleios/logger/console.h"
#include "teleios/engine/Lifecycle.h"
#include "teleios/platform/Lifecycle.h"
#include "teleios/memory/lifecycle.h"
#include "teleios/event/lifecycle.h"
#include "teleios/event/registry.h"
#include "teleios/input/lifecycle.h"
#include "teleios/window/window.h"
#include "teleios/window/event.h"
#include "teleios/application/event.h"
#include "teleios/chrono/time.h"
#include "teleios/chrono/timer.h"
#include "teleios/graphics/lifecycle.h"
#include "teleios/ui/lifecycle.h"

static b8 m_running = true;
static b8 m_paused = false;
static u64 fps = 0;
static u64 ups = 0;

static b8 tl_engine_eventlistener(u16 code, const TLEvent* event) {
    switch (code) {
    case TL_EVENT_APPLICATION_QUIT: m_running = false; break;

    case TL_EVENT_WINDOW_RESTORED:
    case TL_EVENT_WINDOW_RESIZED: m_paused = false; break;
    case TL_EVENT_WINDOW_MINIMIZED: {
        m_paused = true;
        fps = 0;
        ups = 0;
    } break;

    }

    return false;
}

TLEXPORT b8 tl_engine_initialize(const TLSpecification* spec) {
    if (!tl_platform_initialize(spec)) return false;
    if (!tl_memory_initialize()) return false;
    if (!tl_event_initialize()) return false;
    if (!tl_event_register(U16MAX, tl_engine_eventlistener)) return false;
    if (!tl_input_initialize()) return false;

    if (!tl_window_create(spec)) return false;
    if (!tl_graphics_initialize(spec)) return false;
    if (!tl_ui_initialize()) return false;

    return true;
}

static void tl_engine_loop();

TLEXPORT b8 tl_engine_run(void) {
    TLTRACE("tl_engine_run: Will now run");
    tl_window_show();
    tl_engine_loop();
    tl_window_hide();
    return true;
}

static void tl_engine_loop() {
    TLTRACE("tl_engine_loop: Starting game loop");
    TLTimer timer = { 0 };
    tl_chrono_timer_start(&timer);

    f64 time_last = tl_chrono_time_micros();
    fps = ups = 0;

    while (m_running) {
        if (!m_paused) {
            const f64 time_delta = tl_chrono_time_micros() - time_last;
            time_last += time_delta;
            tl_graphics_clear();

            // tl_ecs_update();
            tl_ui_update();
            tl_graphics_update();

            tl_input_update();

            fps++;
        }

        tl_platform_update();
        tl_chrono_timer_update(&timer);
        if (tl_chrono_timer_seconds(&timer) > 1) {
            tl_chrono_timer_start(&timer);

            if (!m_paused) TLDEBUG("FPS: %u UPS: %u", fps, ups);
            fps = ups = 0;
        }
    }

    TLTRACE("tl_engine_loop: Finished game loop");
}

TLEXPORT b8 tl_engine_terminate(void) {
    TLTRACE("tl_engine_terminate: Terminating the engine");

    if (!tl_ui_terminate()) return false;
    if (!tl_graphics_terminate()) return false;
    tl_window_destroy();

    if (!tl_input_terminate()) return false;
    if (!tl_event_terminate()) return false;
    if (!tl_memory_terminate()) return false;
    if (!tl_platform_terminate()) return false;

    return true;
}