#include "teleios/graphics.h"
#include "teleios/graphics_opengl.h"
#include "teleios/logger.h"

b8 tl_graphics_initialize(const TLSpecification* spec) {
    return gl_initialize(spec);
}

b8 tl_graphics_terminate(void) {
    return gl_terminamte();
}
