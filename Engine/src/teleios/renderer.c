#include "teleios/graphics.h"
#include "teleios/renderer.h"


b8 tl_renderer_initialize(const TLSpecification* spec) {
    return true;
}

b8 tl_renderer_terminate(void) {
    return true;
}

void tl_renderer_frame_begin(void) {
    tl_graphics_clear();
}

void tl_renderer_frame_end(void) {
    tl_graphics_present();
}