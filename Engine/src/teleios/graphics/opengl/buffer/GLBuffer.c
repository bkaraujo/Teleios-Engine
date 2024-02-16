#ifdef TELEIOS_GRAPHICS_OPENGL
#include "glad/glad.h"

#include "teleios/graphics/opengl/debug.h"
#include "teleios/graphics/opengl/buffer/buffer.h"

u32 tl_glbuffer_create(void) {
    u32 handle;

    glGenBuffers(1, &handle);
    GLCHECK();

    return handle;
}

void tl_glbuffer_upload(u32 kind, u32 size, void* vector, u32 usage) {
    glBufferData(kind, size, vector, usage);
    GLCHECK();
}

void tl_glbuffer_bind(u32 kind, u32 handle) {
    glBindBuffer(kind, handle);
    GLCHECK();
}

void tl_glbuffer_unbind(u32 kind, u32 handle) {
    glBindBuffer(kind, GL_NONE);
    GLCHECK();
}

void tl_glbuffer_destroy(u32 handle) {
    glDeleteBuffers(1, &handle);
    GLCHECK();
}

#endif // TELEIOS_GRAPHICS_OPENGL
