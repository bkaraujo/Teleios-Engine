#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
b8 tl_graphics_terminate(void);
void tl_graphics_clear(void);
void tl_graphics_present(void);


typedef enum {
    TL_GRAPHICS_PRIMITIVE_SHADER,
    TL_GRAPHICS_PRIMITIVE_GEOMETRY
} TLGraphcisType;

typedef struct {
    const char* vertex;
    const char* fragment;
} TLShader;

typedef struct {
    u32 vbo;
    u32 ebo;
    u32 indices;
} TLGraphicsBuffer;

typedef struct {
    TLGraphcisType type;
    u32 handle;

    // Polimorphic contet based on 'type'
    union {
        TLGraphicsBuffer buffer;
        TLShader shader;
    } object;

} TLGraphics;

TLAPI TLGraphics* tl_graphics_primitive_create(const TLGraphcisType type);
TLAPI void tl_graphics_primitive_destroy(TLGraphics* object);

typedef enum {
    TL_SHADER_TYPE_VERTEX,
    TL_SHADER_TYPE_FRAGMENT
} TLShaderType;

typedef struct {
    TLShaderType type;
    const TLFile* file;
} TLShaderSource;

TLAPI TLGraphics* tl_graphics_shader(const TLShaderSource* sources, const u8 count);

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
} TLGraphicsGeometry;

TLAPI TLGraphics* tl_graphics_geometry(const TLGraphicsGeometry* geometry);

#endif // TELEIOS_GRAPHICS_H
