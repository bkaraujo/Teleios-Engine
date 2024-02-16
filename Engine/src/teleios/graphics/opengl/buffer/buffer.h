#ifndef TELEIOS_GRAPHICS_OPENGL_BUFFER_H
#define TELEIOS_GRAPHICS_OPENGL_BUFFER_H

#include "teleios/types.h"

u32 tl_glbuffer_create(void);
void tl_glbuffer_upload(u32 handle, u32 count, void* vector, u32 usage);
void tl_glbuffer_bind(u32 kind, u32 handle);
void tl_glbuffer_unbind(u32 kind, u32 handle);
void tl_glbuffer_destroy(u32 handle);

#endif // TELEIOS_GRAPHICS_OPENGL_BUFFER_H