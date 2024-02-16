#include "glad/glad.h"
#include "stb/image.h"
#include "teleios/filesystem.h"
#include "teleios/logger/console.h"
#include "teleios/memory/allocator.h"
#include "teleios/graphics/texture.h"
#include "teleios/graphics/opengl/debug.h"

static inline void tl_graphics_texture_load2d(TLTexture* texture, const char* path);
static inline void tl_graphics_texture_load3d(TLTexture* texture, const char* path);

TLEXPORT const TLTexture* tl_graphics_texture_create(TLTextureType type, const char* path) {
    if (path == NULL) { TLERROR("tl_graphics_texture_create: Texture path is null"); return NULL; }

    TLTexture* texture = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(TLTexture));
    if (texture == NULL) { TLERROR("tl_graphics_texture_create: Failed to allocate memory"); return NULL; }


    switch (type) {
    case TL_TEXTURE_2D: tl_graphics_texture_load2d(texture, path); break;
    case TL_TEXTURE_3D: tl_graphics_texture_load3d(texture, path); break;
    }

    return texture;
}

static inline void tl_graphics_texture_load2d(TLTexture* texture, const char* path) {
    glGenTextures(1, &(u32)texture->handle); GLCHECK();
    TLTRACE("tl_texture_load2d: [%u] Loading %s", texture->handle, path);
    texture->type = TL_TEXTURE_2D;
    texture->path = path;

    glBindTexture(GL_TEXTURE_2D, texture->handle); GLCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GLCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GLCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GLCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLCHECK();

    i32 width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    const TLFile* file = tl_filesystem_read_raw(path);
    if (file == NULL) {
        glDeleteTextures(1, &texture->handle);
        tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)texture, sizeof(TLTexture));
        TLERROR("tl_graphics_texture_load2d: Failed to load texture file");
        return;
    }

    u8* data = stbi_load_from_memory(file->payload.raw, (int)file->size, &width, &height, &nrChannels, 0);
    if (data == NULL) {
        tl_filesystem_free(file);
        glDeleteTextures(1, &texture->handle); GLCHECK();
        tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)texture, sizeof(TLTexture));
        TLERROR("tl_graphics_texture_load2d: STB Image failed");
        return;
    }

    TLTRACE("tl_graphics_texture_load2d: [%u] uploaing %llu bytes", texture->handle, file->size);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); GLCHECK();
    glGenerateMipmap(GL_TEXTURE_2D); GLCHECK();

    stbi_image_free(data);
    tl_filesystem_free(file);
}

static inline void tl_graphics_texture_load3d(TLTexture* texture, const char* path) {
    TLFATAL("tl_texture_load3d: Implementation missing");
}

TLEXPORT void tl_graphics_texture_bind(const TLTexture* texture) {
    glActiveTexture(GL_TEXTURE0 + texture->slot); GLCHECK();

    switch (texture->type) {
    case TL_TEXTURE_2D: glBindTexture(GL_TEXTURE_2D, texture->handle); GLCHECK(); break;
    case TL_TEXTURE_3D: glBindTexture(GL_TEXTURE_3D, texture->handle); GLCHECK(); break;
    }

}

TLEXPORT void tl_graphics_texture_destroy(const TLTexture* texture) {
    if (texture == NULL) { TLERROR("tl_graphics_texture_destroy: Texture is null"); return; }
    TLTRACE("tl_graphics_texture_destroy: [%u] Releasing %s", texture->handle, texture->path);
    glDeleteTextures(1, &texture->handle); GLCHECK();
    tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)texture, sizeof(TLTexture));
}