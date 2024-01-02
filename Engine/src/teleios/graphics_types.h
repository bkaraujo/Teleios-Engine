#ifndef TELEIOS_GRAPHICS_TYPES_H
#define TELEIOS_GRAPHICS_TYPES_H

#include "teleios/types.h"

typedef enum {
    TL_SHADER_TYPE_VERTEX,
    TL_SHADER_TYPE_FRAGMENT
} TLShaderType;

typedef struct {
    TLShaderType type;
    const TLFile* file;
} TLShaderSource;

typedef struct {
    const char* vertex;
    const char* fragment;
} TLShader;

typedef struct {
    u32 vbo;
    u32 ebo;
    u32 indices;
} TLVertexBuffer;

typedef enum {
    TL_GRAPHICS_PRIMITIVE_SHADER,
    TL_GRAPHICS_PRIMITIVE_GEOMETRY
} TLGraphcisType;

typedef struct {
    TLGraphcisType type;
    u32 handle;

    union {
        TLVertexBuffer vertex;
        TLShader shader;
    } payload;

} TLGraphics;

typedef enum {
    TL_BUFFER_LAYOUT_TYPE_U8,
    TL_BUFFER_LAYOUT_TYPE_U16,
    TL_BUFFER_LAYOUT_TYPE_U32,
    TL_BUFFER_LAYOUT_TYPE_I8,
    TL_BUFFER_LAYOUT_TYPE_I16,
    TL_BUFFER_LAYOUT_TYPE_I32,
    TL_BUFFER_LAYOUT_TYPE_F32,
    TL_BUFFER_LAYOUT_TYPE_F64
} TLBufferLayoutType;

typedef struct {
    u8 components;
    const char* name;
    TLBufferLayoutType type;
} TLBufferLayout;

typedef struct {
    u32 vsize; f32* vertices;
    u32 isize; u32* indices;
    u32 lsize; TLBufferLayout* layout;
} TLGeometry;

#endif // TELEIOS_GRAPHICS_TYPES_H
