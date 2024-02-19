#ifndef TELEIOS_GRAPHICS_TYPES_H
#define TELEIOS_GRAPHICS_TYPES_H

#include "Teleios/Types.h"

typedef enum {
    TL_SHADER_VERTEX,
    TL_SHADER_FRAGMENT
} TLShaderType;

typedef struct TLShaderSource {
    TLShaderType type;
    const char* path;
    const char* source;
} TLShaderSource;

typedef struct TLShader {
    const u32 handle;
    const u8 source_count;
    const TLShaderSource* sources;
} TLShader;

typedef struct {
    const u32 handle;
    const u32 indices;
} TLGeometry;

typedef enum TLTextureType {
    TL_TEXTURE_2D,
    TL_TEXTURE_3D
} TLTextureType;

typedef struct TLTexture {
    const u32 handle;
    u8 slot;
    TLTextureType type;
    const char* path;
} TLTexture;

typedef enum {
    TL_BUFFER_TYPE_BYTE, TL_BUFFER_TYPE_BYTE2, TL_BUFFER_TYPE_BYTE3, TL_BUFFER_TYPE_BYTE4,
    TL_BUFFER_TYPE_SHORT, TL_BUFFER_TYPE_SHORT2, TL_BUFFER_TYPE_SHORT3, TL_BUFFER_TYPE_SHORT4,
    TL_BUFFER_TYPE_INT, TL_BUFFER_TYPE_INT2, TL_BUFFER_TYPE_INT3, TL_BUFFER_TYPE_INT4,
    TL_BUFFER_TYPE_FLOAT, TL_BUFFER_TYPE_FLOAT2, TL_BUFFER_TYPE_FLOAT3, TL_BUFFER_TYPE_FLOAT4,

    TL_BUFFER_TYPE_UBYTE, TL_BUFFER_TYPE_UBYTE2, TL_BUFFER_TYPE_UBYTE3, TL_BUFFER_TYPE_UBYTE4,
    TL_BUFFER_TYPE_USHORT, TL_BUFFER_TYPE_USHORT2, TL_BUFFER_TYPE_USHORT3, TL_BUFFER_TYPE_USHORT4,
    TL_BUFFER_TYPE_UINT, TL_BUFFER_TYPE_UINT2, TL_BUFFER_TYPE_UINT3, TL_BUFFER_TYPE_UINT4
} TLBufferType;

typedef struct TLBufferLayout {
    const char* name;
    TLBufferType type;
} TLBufferLayout;

typedef struct TLUniform {
    const char* name;
    TLBufferType type;
    void* payload;
} TLUniform;

typedef struct TLMaterial {
    const TLShader* shader;
    u8 uniform_count;
    TLUniform* uniforms;
    TLTexture* texture;
} TLMaterial;

#endif // TELEIOS_GRAPHICS_TYPES_H