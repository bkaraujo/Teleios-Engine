#include "teleios/logger/console.h"
#include "teleios/memory/allocator.h"
#include "teleios/graphics/texture.h"
#include "teleios/graphics/shader.h"
#include "teleios/renderer/material.h"

static inline b8 tl_material_bind_uniform(const TLMaterial* material);
static inline b8 tl_material_bind_texture(const TLMaterial* material);

b8 tl_graphics_material_bind(const TLMaterial* material) {
    if (material == NULL) { TLERROR("tl_graphics_material_bind: Material is null");  return false; }
    if (material->shader == NULL) { TLERROR("tl_graphics_material_bind: Material->Shader is null");  return false; }
    tl_graphics_shader_bind(material->shader);

    if (material->uniform_count > 0) tl_material_bind_uniform(material);
    if (material->texture != NULL) tl_material_bind_texture(material);

    return true;
}

static inline b8 tl_material_bind_uniform(const TLMaterial* material) {
    if (material->uniforms == NULL) {
        TLERROR("tl_graphics_material_bind: Uniform count grater than zero but no uniform informed");
        return false;
    }

    for (unsigned i = 0; i < material->uniform_count; ++i) {
        TLUniform uniform = material->uniforms[i];
        switch (uniform.type) {
        case TL_BUFFER_TYPE_BYTE:
        case TL_BUFFER_TYPE_SHORT: tl_graphics_shader_uploadi(material->shader, uniform.name, 1, uniform.payload); break;
        case TL_BUFFER_TYPE_BYTE2:
        case TL_BUFFER_TYPE_SHORT2: tl_graphics_shader_uploadi(material->shader, uniform.name, 2, uniform.payload); break;
        case TL_BUFFER_TYPE_BYTE3:
        case TL_BUFFER_TYPE_SHORT3: tl_graphics_shader_uploadi(material->shader, uniform.name, 3, uniform.payload); break;
        case TL_BUFFER_TYPE_BYTE4:
        case TL_BUFFER_TYPE_SHORT4: tl_graphics_shader_uploadi(material->shader, uniform.name, 4, uniform.payload); break;
        case TL_BUFFER_TYPE_FLOAT:  tl_graphics_shader_uploadf(material->shader, uniform.name, 1, uniform.payload); break;
        case TL_BUFFER_TYPE_FLOAT2: tl_graphics_shader_uploadf(material->shader, uniform.name, 2, uniform.payload); break;
        case TL_BUFFER_TYPE_FLOAT3: tl_graphics_shader_uploadf(material->shader, uniform.name, 3, uniform.payload); break;
        case TL_BUFFER_TYPE_FLOAT4: tl_graphics_shader_uploadf(material->shader, uniform.name, 4, uniform.payload); break;
        case TL_BUFFER_TYPE_UINT:
        case TL_BUFFER_TYPE_UBYTE:
        case TL_BUFFER_TYPE_USHORT: tl_graphics_shader_uploadu(material->shader, uniform.name, 1, uniform.payload); break;
        case TL_BUFFER_TYPE_UINT2:
        case TL_BUFFER_TYPE_UBYTE2:
        case TL_BUFFER_TYPE_USHORT2: tl_graphics_shader_uploadu(material->shader, uniform.name, 2, uniform.payload);  break;
        case TL_BUFFER_TYPE_UINT3:
        case TL_BUFFER_TYPE_UBYTE3:
        case TL_BUFFER_TYPE_USHORT3: tl_graphics_shader_uploadu(material->shader, uniform.name, 3, uniform.payload); break;
        case TL_BUFFER_TYPE_UINT4:
        case TL_BUFFER_TYPE_UBYTE4:
        case TL_BUFFER_TYPE_USHORT4: tl_graphics_shader_uploadu(material->shader, uniform.name, 4, uniform.payload);  break;
        }
    }
    return true;
}

static inline b8 tl_material_bind_texture(const TLMaterial* material) {
    tl_graphics_texture_bind(material->texture);
    return true;
}

TLEXPORT b8 tl_graphics_material_attach(TLMaterial* material) {
    material->uniforms = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, material->uniform_count * sizeof(TLUniform));
    if (material->uniforms == NULL) {
        TLERROR("tl_graphics_material_attach: Failed to allocate uniform array");
        return false;
    }

    return true;
}

b8 tl_graphics_material_unbind(const TLMaterial* material) {
    if (material == NULL) { TLERROR("tl_graphics_material_unbind: Material is null");  return false; }
    if (material->shader == NULL) { TLERROR("tl_graphics_material_unbind: Material->Shader is null");  return false; }

    tl_graphics_shader_unbind(material->shader);

    return true;
}