#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/defines.h"
#include "teleios/graphics_types.h"
#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
b8 tl_graphics_terminate(void);
void tl_graphics_clear(void);
void tl_graphics_present(void);

TLAPI void tl_graphics_destroy(TLGraphics* object);
TLAPI TLGraphics* tl_graphics_shader(const TLShaderSource* sources, const u8 count);
TLAPI TLGraphics* tl_graphics_geometry(const TLGeometry* geometry);

#endif // TELEIOS_GRAPHICS_H
