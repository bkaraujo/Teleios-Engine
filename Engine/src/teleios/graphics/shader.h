#ifndef TELEIOS_GRAPHICS_SHADER_H
#define TELEIOS_GRAPHICS_SHADER_H

#include "Teleios/Types.h"
#include "Teleios/Graphics/Types.h"

TLEXPORT const TLShader* tl_graphics_shader_create(u8 count, const TLShaderSource* sources);
TLEXPORT void tl_graphics_shader_bind(const TLShader* shader);
TLEXPORT void tl_graphics_shader_unbind(const TLShader* shader);
TLEXPORT void tl_graphics_shader_destroy(const TLShader* shader);

TLEXPORT void tl_graphics_shader_uploadf(const TLShader* shader, const char* uniform, u8 count, f32* vector);
TLEXPORT void tl_graphics_shader_uploadi(const TLShader* shader, const char* uniform, u8 count, i32* vector);
TLEXPORT void tl_graphics_shader_uploadu(const TLShader* shader, const char* uniform, u8 count, u32* vector);
TLEXPORT void tl_graphics_shader_uploadm(const TLShader* shader, const char* uniform, u8 type, u8 count, void* vector);

#endif // TELEIOS_GRAPHICS_SHADER_H