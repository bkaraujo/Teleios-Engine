#ifdef TELEIOS_GRAPHICS_OPENGL
#include "glad/glad.h"

#include "teleios/graphics/opengl/debug.h"
#include "teleios/graphics/opengl/buffer/buffer.h"
#include "teleios/graphics/opengl/buffer/element.h"

u32 tl_glelementbuffer_create(void) {
    return tl_glbuffer_create();
}

void tl_glelementbuffer_upload(u32 count, u32* vector) {
    tl_glbuffer_upload(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * count, vector, GL_STATIC_DRAW);
}

void tl_glelementbuffer_bind(u32 handle) {
    tl_glbuffer_bind(GL_ELEMENT_ARRAY_BUFFER, handle);
}

void tl_glelementbuffer_unbind(u32 handle) {
    tl_glbuffer_unbind(GL_ELEMENT_ARRAY_BUFFER, handle);
}

void tl_glelementbuffer_destroy(u32 handle) {
    tl_glbuffer_destroy(handle);
}
#endif // TELEIOS_GRAPHICS_OPENGL