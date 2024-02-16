#ifdef TELEIOS_GRAPHICS_OPENGL
#include "glad/glad.h"

#include "teleios/graphics/opengl/Debug.h"
#include "teleios/graphics/opengl/buffer/buffer.h"
#include "teleios/graphics/opengl/buffer/vertex.h"

u32 tl_glvertexbuffer_create(void) {
    return tl_glbuffer_create();
}

void tl_glvertexbuffer_upload(u32 count, f32* vector) {
    tl_glbuffer_upload(GL_ARRAY_BUFFER, sizeof(f32) * count, vector, GL_STATIC_DRAW);
}

void tl_glvertexbuffer_layout(u8 count, TLBufferLayout* layouts) {

    u8 stride = 0;
    for (unsigned i = 0; i < count; ++i) {
        switch (layouts[i].type) {

        case TL_BUFFER_TYPE_BYTE: { stride += 1 * sizeof(i8); } break;
        case TL_BUFFER_TYPE_BYTE2: { stride += 2 * sizeof(i8); } break;
        case TL_BUFFER_TYPE_BYTE3: { stride += 3 * sizeof(i8); } break;
        case TL_BUFFER_TYPE_BYTE4: { stride += 4 * sizeof(i8); } break;

        case TL_BUFFER_TYPE_SHORT: { stride += 1 * sizeof(i16); } break;
        case TL_BUFFER_TYPE_SHORT2: { stride += 2 * sizeof(i16); } break;
        case TL_BUFFER_TYPE_SHORT3: { stride += 3 * sizeof(i16); } break;
        case TL_BUFFER_TYPE_SHORT4: { stride += 4 * sizeof(i16); } break;

        case TL_BUFFER_TYPE_INT: { stride += 1 * sizeof(i32); } break;
        case TL_BUFFER_TYPE_INT2: { stride += 2 * sizeof(i32); } break;
        case TL_BUFFER_TYPE_INT3: { stride += 3 * sizeof(i32); } break;
        case TL_BUFFER_TYPE_INT4: { stride += 4 * sizeof(i32); } break;

        case TL_BUFFER_TYPE_UBYTE: { stride += 1 * sizeof(u8); } break;
        case TL_BUFFER_TYPE_UBYTE2: { stride += 2 * sizeof(u8); } break;
        case TL_BUFFER_TYPE_UBYTE3: { stride += 3 * sizeof(u8); } break;
        case TL_BUFFER_TYPE_UBYTE4: { stride += 4 * sizeof(u8); } break;

        case TL_BUFFER_TYPE_USHORT: { stride += 1 * sizeof(u16); } break;
        case TL_BUFFER_TYPE_USHORT2: { stride += 2 * sizeof(u16); } break;
        case TL_BUFFER_TYPE_USHORT3: { stride += 3 * sizeof(u16); } break;
        case TL_BUFFER_TYPE_USHORT4: { stride += 4 * sizeof(u16); } break;

        case TL_BUFFER_TYPE_UINT: { stride += 1 * sizeof(u32); } break;
        case TL_BUFFER_TYPE_UINT2: { stride += 2 * sizeof(u32); } break;
        case TL_BUFFER_TYPE_UINT3: { stride += 3 * sizeof(u32); } break;
        case TL_BUFFER_TYPE_UINT4: { stride += 4 * sizeof(u32); } break;

        case TL_BUFFER_TYPE_FLOAT: { stride += 1 * sizeof(f32); } break;
        case TL_BUFFER_TYPE_FLOAT2: { stride += 2 * sizeof(f32); } break;
        case TL_BUFFER_TYPE_FLOAT3: { stride += 3 * sizeof(f32); } break;
        case TL_BUFFER_TYPE_FLOAT4: { stride += 4 * sizeof(f32); } break;
        }
    }

    u8 offset = 0;
    for (unsigned i = 0; i < count; ++i) {
        glEnableVertexAttribArray(i); GLCHECK();

        switch (layouts[i].type) {

        case TL_BUFFER_TYPE_BYTE: { glVertexAttribPointer(i, 1, GL_BYTE, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(i8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_BYTE2: { glVertexAttribPointer(i, 2, GL_BYTE, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(i8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_BYTE3: { glVertexAttribPointer(i, 3, GL_BYTE, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(i8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_BYTE4: { glVertexAttribPointer(i, 4, GL_BYTE, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(i8); GLCHECK(); } break;

        case TL_BUFFER_TYPE_SHORT: { glVertexAttribPointer(i, 1, GL_SHORT, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(i16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_SHORT2: { glVertexAttribPointer(i, 2, GL_SHORT, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(i16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_SHORT3: { glVertexAttribPointer(i, 3, GL_SHORT, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(i16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_SHORT4: { glVertexAttribPointer(i, 4, GL_SHORT, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(i16); GLCHECK(); } break;

        case TL_BUFFER_TYPE_INT: { glVertexAttribPointer(i, 1, GL_INT, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(i32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_INT2: { glVertexAttribPointer(i, 2, GL_INT, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(i32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_INT3: { glVertexAttribPointer(i, 3, GL_INT, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(i32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_INT4: { glVertexAttribPointer(i, 4, GL_INT, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(i32); GLCHECK(); } break;

        case TL_BUFFER_TYPE_UBYTE: { glVertexAttribPointer(i, 1, GL_UNSIGNED_BYTE, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(u8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UBYTE2: { glVertexAttribPointer(i, 2, GL_UNSIGNED_BYTE, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(u8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UBYTE3: { glVertexAttribPointer(i, 3, GL_UNSIGNED_BYTE, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(u8); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UBYTE4: { glVertexAttribPointer(i, 4, GL_UNSIGNED_BYTE, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(u8); GLCHECK(); } break;

        case TL_BUFFER_TYPE_USHORT: { glVertexAttribPointer(i, 1, GL_UNSIGNED_SHORT, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(u16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_USHORT2: { glVertexAttribPointer(i, 2, GL_UNSIGNED_SHORT, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(u16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_USHORT3: { glVertexAttribPointer(i, 3, GL_UNSIGNED_SHORT, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(u16); GLCHECK(); } break;
        case TL_BUFFER_TYPE_USHORT4: { glVertexAttribPointer(i, 4, GL_UNSIGNED_SHORT, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(u16); GLCHECK(); } break;

        case TL_BUFFER_TYPE_UINT: { glVertexAttribPointer(i, 1, GL_UNSIGNED_INT, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(u32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UINT2: { glVertexAttribPointer(i, 2, GL_UNSIGNED_INT, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(u32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UINT3: { glVertexAttribPointer(i, 3, GL_UNSIGNED_INT, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(u32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_UINT4: { glVertexAttribPointer(i, 4, GL_UNSIGNED_INT, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(u32); GLCHECK(); } break;

        case TL_BUFFER_TYPE_FLOAT: { glVertexAttribPointer(i, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offset); offset += 1 * sizeof(f32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_FLOAT2: { glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offset); offset += 2 * sizeof(f32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_FLOAT3: { glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offset); offset += 3 * sizeof(f32); GLCHECK(); } break;
        case TL_BUFFER_TYPE_FLOAT4: { glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, stride, (const void*)offset); offset += 4 * sizeof(f32); GLCHECK(); } break;

        }
    }
}

void tl_glvertexbuffer_bind(u32 handle) {
    tl_glbuffer_bind(GL_ARRAY_BUFFER, handle);
}

void tl_glvertexbuffer_unbind(u32 handle) {
    tl_glbuffer_unbind(GL_ARRAY_BUFFER, handle);

}

void tl_glvertexbuffer_destroy(u32 handle) {
    tl_glbuffer_destroy(handle);
}
#endif // TELEIOS_GRAPHICS_OPENGL