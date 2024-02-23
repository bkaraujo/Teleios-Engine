#ifdef TELEIOS_GRAPHICS_OPENGL
#include "teleios/logger/console.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"
#include "teleios/graphics/opengl/debug.h"
#include "teleios/graphics/geometry.h"
#include "glad/glad.h"

TLEXPORT const TLGeometry* tl_graphics_geometry_create(u32 vcount, f32* vertices, u32 ecount, u32* element, u32 lcount, TLBufferLayout* layouts) {
    if (vcount == 0 || vertices == NULL) { TLERROR("tl_graphics_geometry_create: Invalid vertices"); return NULL; }
    if (ecount == 0 || element == NULL) { TLERROR("tl_graphics_geometry_create: Invalid element"); return NULL; }
    if (lcount == 0 || layouts == NULL) { TLERROR("tl_graphics_geometry_create: Invalid layout"); return NULL; }
    
    const TLGeometry* geometry = tl_graphics_geometry_empty(lcount, layouts, vcount * sizeof(f32), ecount * sizeof(u32));
    if (geometry == NULL) return NULL;

    tl_memory_copy(vertices, geometry->vertex, geometry->vsize);
    tl_memory_copy(element, geometry->element, geometry->esize);
    
    return geometry;
}

TLEXPORT const TLGeometry* tl_graphics_geometry_empty(u32 lcount, TLBufferLayout* layouts, u32 vsize, u32 esize) {
    if (lcount == 0 || layouts == NULL) { TLERROR("tl_graphics_geometry_empty: Invalid layout"); return NULL; }
    if (vsize == 0 ) { TLERROR("tl_graphics_geometry_empty: Invalid vsize"); return NULL; }
    if (esize == 0) { TLERROR("tl_graphics_geometry_empty: Invalid esize"); return NULL; }

    TLGeometry* geometry = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(TLGeometry));
    if (geometry == NULL) { TLERROR("tl_graphics_geometry_empty: Failed to allocate memory"); return NULL; }
    
    *(u32*)&geometry->vsize = vsize;
    *(u32*)&geometry->esize = esize;

    u32 flags = 
          GL_MAP_READ_BIT           // Read from the buffer
        | GL_MAP_WRITE_BIT          // Write to the buffer
        | GL_MAP_PERSISTENT_BIT     // keep the mapping through subsequent OpenGL operations.
        | GL_MAP_COHERENT_BIT;      // Writes will be seen by the server when done from the client and vice versa
    
    glCreateBuffers(1, &geometry->vbo); GLCHECK();
    glNamedBufferStorage(geometry->vbo, geometry->vsize, NULL, GL_DYNAMIC_STORAGE_BIT  | flags); GLCHECK();
    geometry->vertex = glMapNamedBufferRange(geometry->vbo, 0, geometry->vsize, flags); GLCHECK();
    if (geometry->vertex == NULL) {
        tl_graphics_geometry_destroy(geometry);
        TLERROR("tl_graphics_geometry_empty: TLGeometry->vertex is null");
        return NULL;
    }

 
    glCreateBuffers(1, &geometry->ebo); GLCHECK();
    glNamedBufferStorage(geometry->ebo, geometry->esize, NULL, GL_DYNAMIC_STORAGE_BIT  | flags); GLCHECK();
    geometry->element = glMapNamedBufferRange(geometry->ebo, 0, geometry->esize, flags); GLCHECK();
    if (geometry->element == NULL) {
        tl_graphics_geometry_destroy(geometry);
        TLERROR("tl_graphics_geometry_empty: TLGeometry->element is null");
        return NULL;
    }

    tl_memory_zero(geometry->vertex, geometry->vsize);
    tl_memory_zero(geometry->element, geometry->esize);

    glCreateVertexArrays(1, &geometry->handle); GLCHECK();
    for (unsigned i = 0; i < lcount; ++i) {
        glEnableVertexArrayAttrib(geometry->handle, i); GLCHECK();

        switch (layouts[i].type) {
            case TL_BUFFER_TYPE_BYTE1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_BYTE2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_BYTE3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_BYTE4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_SHORT1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_SHORT2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_SHORT3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_SHORT4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_INT1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_INT2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_INT3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_INT4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_UBYTE1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_UNSIGNED_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UBYTE2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_UNSIGNED_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UBYTE3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_UNSIGNED_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UBYTE4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_UNSIGNED_BYTE, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_USHORT1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_UNSIGNED_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_USHORT2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_UNSIGNED_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_USHORT3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_UNSIGNED_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_USHORT4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_UNSIGNED_SHORT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_UINT1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_UNSIGNED_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UINT2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_UNSIGNED_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UINT3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_UNSIGNED_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_UINT4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_UNSIGNED_INT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;

            case TL_BUFFER_TYPE_FLOAT1: { glVertexArrayAttribFormat(geometry->handle, i, 1, GL_FLOAT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_FLOAT2: { glVertexArrayAttribFormat(geometry->handle, i, 2, GL_FLOAT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_FLOAT3: { glVertexArrayAttribFormat(geometry->handle, i, 3, GL_FLOAT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
            case TL_BUFFER_TYPE_FLOAT4: { glVertexArrayAttribFormat(geometry->handle, i, 4, GL_FLOAT, GL_FALSE, layouts[i].offset); GLCHECK(); } break;
        }

        glVertexArrayAttribBinding(geometry->handle, i, 0); GLCHECK();
    }

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
    
    if (geometry->vbo != GL_NONE) {
        glUnmapNamedBuffer(geometry->vbo);
        glDeleteBuffers(1, &geometry->vbo);
    }

    if (geometry->ebo != GL_NONE) {
        glUnmapNamedBuffer(geometry->ebo);
        glDeleteBuffers(1, &geometry->ebo);
    }

    if (geometry->handle != GL_NONE) {
        glDeleteVertexArrays(1, &geometry->handle);
    }

    if (geometry->vertex != NULL) tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)geometry->vertex, geometry->vsize);
    if (geometry->element != NULL) tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)geometry->element, geometry->esize);
    tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)geometry, sizeof(TLGeometry));
}
#endif // TELEIOS_GRAPHICS_OPENGL