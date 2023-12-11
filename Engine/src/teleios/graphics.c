#include "teleios/event.h"
#include "teleios/eventcodes.h"
#include "teleios/graphics.h"
#include "teleios/graphics_opengl.h"
#include "teleios/logger.h"

static TLEventStatus tl_graphics_resize(const u8 code, const TLEvent* event) {
    ivec2s size = { 0 };

    if (code == TL_EVENT_WINDOW_RESIZED || code == TL_EVENT_WINDOW_RESTORED) {
        size.x = event->data.u32[0];
        size.y = event->data.u32[1];
    }

    gl_resize(size);

    return TL_EVENT_STATUS_CONTUNE;
}

b8 tl_graphics_initialize(const TLSpecification* spec) {
    if (!gl_initialize(spec)) return false;
    tl_event_subscribe(TL_EVENT_WINDOW_RESIZED, tl_graphics_resize);
    tl_event_subscribe(TL_EVENT_WINDOW_MINIMIZED, tl_graphics_resize);
    tl_event_subscribe(TL_EVENT_WINDOW_MAXIMIZED, tl_graphics_resize);
    tl_event_subscribe(TL_EVENT_WINDOW_RESTORED, tl_graphics_resize);
}

void tl_graphics_clear(void) {
    gl_clear();
}

void tl_graphics_present(void) {
    gl_present();
}

b8 tl_graphics_terminate(void) {
    return gl_terminate();
}
