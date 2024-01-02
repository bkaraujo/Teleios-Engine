#include "teleios/container.h"
#include "teleios/engine.h"
#include "teleios/environment.h"
#include "teleios/event.h"
#include "teleios/eventcodes.h"
#include "teleios/graphics.h"
#include "teleios/identity.h"
#include "teleios/input.h"
#include "teleios/layerstack.h"
#include "teleios/logger.h"
#include "teleios/memory.h"
#include "teleios/platform.h"
#include "teleios/renderer.h"
#include "teleios/time.h"
#include "teleios/timer.h"

static b8 running = true;
static b8 paused = false;

static u64 STEP = 0;
static TLList* layers;

static TLEventStatus tl_engine_eventhandler(const u8 code, const TLEvent* event);
static b8 tl_layer_comparator(const void* first, const void* second);

// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################

TLAPI b8 tl_engine_pre_initialize(void) {
    if (!tl_platform_initialize()) return false;
    if (!tl_memory_initialize()) return false;
    if (!tl_logger_initialize()) return false;

    if (!tl_environment_initialize()) {
        TLERROR("tl_engine_pre_initialize: Failed to initialize environment manager");
        return false;
    }

    layers = tl_list_create();
    if (layers == NULL) {
        TLERROR("tl_engine_pre_initialize: Failed to initialize layer stack");
        return false;
    }

    return true;
}

TLAPI b8 tl_engine_initialize(const TLSpecification* spec) {
    if (!tl_identity_initialize()) {
        TLERROR("tl_engine_initialize: Failed to initialize the idedntity manager");
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

    if (!tl_platform_window_create(spec)) {
        TLERROR("tl_input_initialize: Failed create window");
        return false;
    }

    if (!tl_graphics_initialize(spec)) {
        TLERROR("tl_input_initialize: Failed to initialize graphics manager");
        return false;
    }

    if (!tl_renderer_initialize(spec)) {
        TLERROR("tl_input_initialize: Failed to initialize renderer");
        return false;
    }

    STEP = MICROSECOND / spec->simulation.per_second;


    return true;
}

TLAPI b8 tl_engine_run(void) {
    u64 fps = 0;
    u64 ups = 0;

    TLTimer timer; tl_timer_begin(&timer);
    u64 time_last = tl_time_epoch_micros();

    u64 accumulator = 0;

#ifdef TELEIOS_DEBUG
    tl_memory_print();
#endif
    tl_platform_window_show();
    while (running) {
        const u64 time_start = tl_time_epoch_micros();
        const u64 time_delta = time_start - time_last;
        time_last = time_start;

        if (!paused) {
            tl_renderer_frame_begin();

            // ============================================
            // Fixed time step processing
            // ============================================
            {
                // Compute needed fixed time steps
                u8 step = 0;
                accumulator += time_delta;
                while (accumulator > STEP) {
                    step++;
                    accumulator -= STEP;
                }

                // Perform the actual steps
                for (unsigned i = 0; i < step; ++i) {
                    ups++;
                    TLNode* current = layers->head;
                    while (current != NULL) {
                        const TLLayer* layer = current->payload;
                        layer->update_fixed(STEP);
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
                    layer->update(time_delta);
                    current = current->next;
                }

                current = layers->head;
                while (current != NULL) {
                    const TLLayer* layer = current->payload;
                    layer->update_late();
                    current = current->next;
                }
            }

            tl_renderer_frame_end();
        }

        if (tl_input_key_released(TL_KEY_PAUSE)) {
            paused = !paused;
            if (paused) tl_platform_window_set_title("PAUSED");
            else tl_platform_window_set_title("FPS: ?? UPS: ??");
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
            if (!paused)
                tl_platform_window_set_title("FPS: %llu UPS: %llu", fps, ups);

            fps = ups = 0;
            tl_timer_reset(&timer);
        }
    }

    tl_platform_window_hide();
#ifdef TELEIOS_DEBUG
    tl_memory_print();
#endif
    return true;
}

TLAPI b8 tl_engine_terminate(void) {
    if (!tl_renderer_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate renderer");
        return false;
    }

    if (!tl_graphics_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate graphics manager");
        return false;
    }

    if (!tl_list_destroy(layers)) {
        TLERROR("tl_engine_terminate: Failed to terminate layer stack");
        return false;
    }

    tl_platform_window_destroy();

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

    if (!tl_environment_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate environment manager");

    }

    if (!tl_memory_terminate()) {
        TLERROR("tl_engine_terminate: Failed to terminate memory manager");
        return false;
    }

    if (!tl_logger_terminate()) return false;

    if (!tl_platform_terminate()) return false;

    return true;
}

TLAPI b8 tl_engine_layer_append(const TLLayer* layer) {
    if (layer == NULL) {
        TLERROR("tl_engine_layer_remove: Layer is NULL.");
        return false;
    }

    if (!tl_identity_empty(&layer->identity)) {
        TLERROR("tl_engine_layer_append: Layer %s already registered", layer->name);
    }

    tl_identity_generate(&layer->identity);
    if (!tl_list_append(layers, layer)) {
        TLERROR("tl_engine_layer_append: Failed to append layer to stack");
        return false;
    }

    if (!layer->initialize()) {
        TLERROR("tl_engine_layer_append: Failed to TLLayer(%s)->initialize()", layer->name);
        tl_list_remove(layers, tl_layer_comparator, layer);

        return false;
    }
    return true;
}

TLAPI b8 tl_engine_layer_remove(const TLLayer* layer) {
    if (layer == NULL || tl_identity_empty(&layer->identity)) {
        TLERROR("tl_engine_layer_remove: Layer is NULL or identity is empty.");
        return false;
    }

    if (layer != tl_list_remove(layers, tl_layer_comparator, layer)) {
        TLERROR("tl_engine_layer_remove: Wrong layer removed from the stack");
        return false;
    }

    if (!layer->terminate()) {
        TLERROR("tl_engine_layer_remove: Failed to TLLayer(%s)->terminate()", layer->name);
        return false;
    }

    return true;
}

// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################

static TLEventStatus tl_engine_eventhandler(const u8 code, const TLEvent* event) {
    switch (code) {
    case TL_EVENT_APPLICATION_QUIT: running = false; break;
    case TL_EVENT_WINDOW_MINIMIZED: paused = true; break;
    case TL_EVENT_WINDOW_RESTORED: paused = false; break;
    }

    return TL_EVENT_STATUS_CONTUNE;
}

static b8 tl_layer_comparator(const void* first, const void* second) {
    return tl_identity_equals(first, second);
}