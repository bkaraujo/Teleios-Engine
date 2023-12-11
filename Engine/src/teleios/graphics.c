#include "teleios/graphics.h"
#include "teleios/graphics_opengl.h"
#include "teleios/logger.h"

b8 tl_graphics_initialize(const TLSpecification* spec) {
    return gl_initialize(spec);
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
