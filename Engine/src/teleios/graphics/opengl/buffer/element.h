#ifndef TELEIOS_GRAPHICS_OPENGL_BUFFER_ELEMENT_H
#define TELEIOS_GRAPHICS_OPENGL_BUFFER_ELEMENT_H

#include "teleios/types.h"

u32 tl_glelementbuffer_create(void);
void tl_glelementbuffer_upload(u32 count, u32* vector);
void tl_glelementbuffer_bind(u32 handle);
void tl_glelementbuffer_unbind(u32 handle);
void tl_glelementbuffer_destroy(u32 handle);

#endif // TELEIOS_GRAPHICS_OPENGL_BUFFER_ELEMENT_H