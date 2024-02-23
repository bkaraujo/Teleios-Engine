#ifndef TELEIOS_GRAPHICS_GEOMETRY_H
#define TELEIOS_GRAPHICS_GEOMETRY_H

#include "teleios/types.h"
#include "teleios/graphics/types.h"

TLEXPORT const TLGeometry* tl_graphics_geometry_empty(u32 lcount, TLBufferLayout* layouts, u32 vsize, u32 esize);
TLEXPORT const TLGeometry* tl_graphics_geometry_create(u32 vcount, f32* vertices, u32 ecount, u32* element, u32 lcount, TLBufferLayout* layouts);
void tl_graphics_geometry_bind(const TLGeometry* geometry);
void tl_graphics_geometry_unbind(const TLGeometry* geometry);
TLEXPORT void tl_graphics_geometry_destroy(const TLGeometry* geometry);

#endif // TELEIOS_GRAPHICS_GEOMETRY_H