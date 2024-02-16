#ifdef TELEIOS_GRAPHICS_OPENGL
#include "teleios/logger/console.h"
#include "teleios/memory/allocator.h"
#include "teleios/graphics/geometry.h"
#include "teleios/graphics/opengl/debug.h"
#include "teleios/graphics/opengl/buffer/vertex.h"
#include "teleios/graphics/opengl/buffer/element.h"
#include "glad/glad.h"

TLEXPORT const TLGeometry* tl_graphics_geometry_create(u32 vcount, f32* vertices, u32 icount, u32* indices, u32 lcount, TLBufferLayout* layouts) {
    if (vcount == 0 || vertices == NULL) { TLERROR("tl_graphics_geometry_create: Invalid vertices"); return NULL; }
    if (icount == 0 || indices == NULL) { TLERROR("tl_graphics_geometry_create: Invalid indices"); return NULL; }
    if (lcount == 0 || layouts == NULL) { TLERROR("tl_graphics_geometry_create: Invalid layout"); return NULL; }

    TLGeometry* geometry = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(TLGeometry));
    if (geometry == NULL) { TLERROR("tl_graphics_geometry_create: Failed to allocate memory"); return NULL; }

    glGenVertexArrays(1, &(u32)geometry->handle); GLCHECK();
    TLTRACE("tl_graphics_geometry_create: Generated Vertex Array [%lu]", geometry->handle);
    *(u32*)&geometry->indices = icount;
    tl_graphics_geometry_bind(geometry);

    u32 VBO = tl_glvertexbuffer_create();
    tl_glvertexbuffer_bind(VBO);
    tl_glvertexbuffer_upload(vcount, vertices);
    TLTRACE("tl_graphics_geometry_create: [%lu] Vertices %llu bytes", geometry->handle, vcount * sizeof(vertices));
    tl_glvertexbuffer_layout(lcount, layouts);
    tl_glvertexbuffer_unbind(VBO);

    u32 EBO = tl_glelementbuffer_create();
    tl_glelementbuffer_bind(EBO);
    tl_glelementbuffer_upload(geometry->indices, indices);
    TLTRACE("tl_graphics_geometry_create: [%lu] Indices %llu bytes", geometry->handle, geometry->indices * sizeof(indices));

    tl_graphics_geometry_unbind(geometry);

    return geometry;
}

void tl_graphics_geometry_bind(const TLGeometry* geometry) {
    if (geometry == NULL) { TLERROR("tl_graphics_geometry_bind: geometry is null"); return; }
    glBindVertexArray(geometry->handle); GLCHECK();
}

void tl_graphics_geometry_unbind(const TLGeometry* geometry) {
    glBindVertexArray(GL_NONE); GLCHECK();
}

TLEXPORT void tl_graphics_geometry_destroy(const TLGeometry* geometry) {
    if (geometry == NULL) return;
    glDeleteVertexArrays(1, &geometry->handle); GLCHECK();
    tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)geometry, sizeof(TLGeometry));
}
#endif // TELEIOS_GRAPHICS_OPENGL