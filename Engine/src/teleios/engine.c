#include "teleios/container.h"
#include "teleios/ecs/manager.h"
#include "teleios/ecs/system.h"
#include "teleios/engine.h"
#include "teleios/event/codes.h"
#include "teleios/event/manager.h"
#include "teleios/event/subcriber.h"
#include "teleios/graphics/manager.h"
#include "teleios/identity/manager.h"
#include "teleios/input/manager.h"
#include "teleios/layer/manager.h"
#include "teleios/layer/stack.h"
#include "teleios/logger.h"
#include "teleios/memory/manager.h"
#include "teleios/platform/manager.h"
#include "teleios/platform/window.h"
#include "teleios/scene/manager.h"
#include "teleios/time/epoch.h"
#include "teleios/time/timer.h"

static b8 running = true;
static TLList* layers;

static TLEventStatus tl_engine_eventhandler(const u8 code, const TLEvent* event);

TLAPI b8 tl_engine_pre_initialize(void) {
    if (!tl_platform_initialize()) {
        TLERROR("tl_engine_pre_initialize: Failed to initialize the underlying platform");
        return false;
    }

    if (!tl_memory_initialize()) {
        TLERROR("tl_engine_pre_initialize: Failed to initialize the memory manager");
        return false;
    }

    layers = tl_list_create();
    if (layers == NULL) {
        TLERROR("tl_engine_pre_initialize: Failed to allocate layer stack");
        return false;
    }
    tl_layer_stack_set(layers);

    if (!tl_identity_initialize()) {
        TLERROR("tl_engine_pre_initialize: Failed to initialize the idedntity manager");
        return false;
    }

    if (!tl_event_initialize()) {
        TLERROR("tl_engine_initialize: Failed to initialize the event manager");
        return false;
    }

    if (!tl_event_subscribe(TL_EVENT_MAXIMUM, tl_engine_eventhandler)) {
        TLERROR("tl_engine_initialize: Failed to register tl_engine_eventhandler");
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

TLAPI b8 tl_engine_initialize(const TLSpecification* spec) {
    if (!tl_platform_window_create(spec)) {
        TLERROR("tl_engine_initialize: Failed create window");
        return false;
    }

    if (!tl_graphics_initialize(spec)) {
        TLERROR("tl_engine_initialize: Failed to initialize graphics manager");
        return false;
    }

    return true;
}

TLAPI b8 tl_engine_run(void) {
    u64 fps = 0;
    u64 ups = 0;

    TLTimer timer; tl_timer_begin(&timer);
    u64 time_last = tl_time_epoch_micros();

    u64 accumulator = 0;
    const u64 STEP = MICROSECOND / 165;

    tl_platform_window_show();
    while (running) {
        const u64 time_start = tl_time_epoch_micros();
        const u64 time_delta = time_start - time_last;
        time_last = time_start;

        // ============================================
        // Fixed time step processing
        // ============================================
        {
            // Compute needed fixed time steps
            u8 step = 0;
            accumulator += time_delta;
            while (accumulator > STEP) {
                ups++;
                step++;
                accumulator -= STEP;
            }

            // Perform the fixed time steps
            for (unsigned i = 0; i < step; ++i) {
                TLNode* current = layers->head;
                while (current != NULL) {
                    const TLLayer* layer = current->payload;
                    if (!layer->update_fixed(STEP)) {
                        TLERROR("tl_engine_run: Failed to layer->update_fixed");
                        return false;
                    }
                    current = current->next;
                }
            }
        }

        // ============================================
        // Variable time step processing
        // ============================================
        {
            fps++;
            TLNode* current = layers->head;
            while (current != NULL) {
                const TLLayer* layer = current->payload;
                if (!layer->update_variable(time_delta)) {
                    TLERROR("tl_engine_run: Failed to layer->update_variable");
                    return false;
                }

                if (!layer->update_after()) {
                    TLERROR("tl_engine_run: Failed to layer->update_after");
                    return false;
                }
                current = current->next;
            }
        }

        // ============================================
        // Frame finalization rotines
        // ============================================
        tl_input_update();
        tl_platform_update();
        tl_timer_update(&timer);

        // ============================================
        // Once Per-Second rotines
        // ============================================
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
    if (!tl_graphics_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate graphic manager");
        return false;
    }

    while (layers->head != NULL) {
        const TLLayer* layer = layers->head->payload;
        TLWARN("tl_engine_terminate: Destroying layer \"%s\". %s", layer->name, MSG_PLEASE_DO_IT_YOURSELF);
        tl_layer_stack_destroy(layer->identity);
    }

    if (!tl_list_destroy(layers)) {
        TLERROR("tl_engine_terminate: Failed to destroy layers list");
        return false;
    }

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

    if (!tl_identity_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate the identity manager");
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

static TLEventStatus tl_engine_eventhandler(const u8 code, const TLEvent* event) {
    if (code == TL_EVENT_APPLICATION_QUIT) {
        running = false;
    }

    return TL_EVENT_STATUS_CONTUNE;
}