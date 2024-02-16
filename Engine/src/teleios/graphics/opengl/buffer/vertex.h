#ifndef TELEIOS_GRAPHICS_OPENGL_BUFFER_VERTEX_H
#define TELEIOS_GRAPHICS_OPENGL_BUFFER_VERTEX_H

#include "teleios/types.h"
#include "teleios/graphics/types.h"

u32 tl_glvertexbuffer_create(void);
void tl_glvertexbuffer_layout(u8 count, TLBufferLayout* layouts);
void tl_glvertexbuffer_upload(u32 count, f32* vector);
void tl_glvertexbuffer_bind(u32 handle);
void tl_glvertexbuffer_unbind(u32 handle);
void tl_glvertexbuffer_destroy(u32 handle);

#endif // TELEIOS_GRAPHICS_OPENGL_BUFFER_VERTEX_H