#include "teleios/teleios.h"

b8 tl_renderer_initialize(const TLSpecification* spec) {
    TLTRACE("tl_renderer_initialize: Initializing Renderer");
    return true;
}

void tl_renderer_frame_begin(void) {
    tl_graphics_clear();
}

TLAPI void tl_renderer_submmit(TLRenderable* renderable) {

}

void tl_renderer_frame_end(void) {
    tl_graphics_present();
}

b8 tl_renderer_terminate(void) {
    return true;
}
