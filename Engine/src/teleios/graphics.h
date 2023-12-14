#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/defines.h"
#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
b8 tl_graphics_terminate(void);
void tl_graphics_clear(void);
void tl_graphics_present(void);


typedef enum {
    TL_GRAPHICS_PRIMITIVE_MESH,

    TL_GRAPHICS_PRIMITIVE_SHADER
} TLGraphcisType;

typedef struct {
    const char* vertex;
    const char* fragment;
} TLShader;

typedef struct {
    b8(*bind)(void);
    b8(*unbind)(void);
} TLGraphicsBuffer;

typedef struct {
    TLGraphcisType type;
    u32 handler;

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

#endif // TELEIOS_GRAPHICS_H
