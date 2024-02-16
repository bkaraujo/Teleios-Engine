#ifndef TELEIOS_UI_BACKEND_OPENGL4_H
#define TELEIOS_UI_BACKEND_OPENGL4_H

#include <glad/glad.h>
#include "nuklear/nuklear.h"

NK_API struct nk_context* tl_uibackend_initialize(void);
NK_API void               tl_uibackend_shutdown(void);
NK_API void               tl_uibackend_font_stash_begin(struct nk_font_atlas** atlas);
NK_API void               tl_uibackend_font_stash_end(void);
NK_API void               tl_uibackend_new_frame(void);
NK_API void               tl_uibackend_render(enum nk_anti_aliasing);

NK_API void               tl_uibackend_device_destroy(void);
NK_API void               tl_uibackend_device_create(void);

NK_API GLuint             tl_uibackend_get_tex_ogl_id(int tex_index);
NK_API GLuint64           tl_uibackend_get_tex_ogl_handle(int tex_index);
NK_API int                tl_uibackend_create_texture(const void* image, int width, int height);
NK_API void               tl_uibackend_destroy_texture(int tex_index);


#endif // TELEIOS_UI_BACKEND_OPENGL4_H
