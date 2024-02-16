#ifndef TELEIOS_GRAPHICS_TEXTURE_H
#define TELEIOS_GRAPHICS_TEXTURE_H

#include "teleios/types.h"
#include "teleios/graphics/types.h"

TLEXPORT const TLTexture* tl_graphics_texture_create(TLTextureType type, const char* path);
TLEXPORT void tl_graphics_texture_bind(const TLTexture* texture);
TLEXPORT void tl_graphics_texture_destroy(const TLTexture* texture);

#endif // TELEIOS_GRAPHICS_TEXTURE_H