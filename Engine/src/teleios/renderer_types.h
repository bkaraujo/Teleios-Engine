#ifndef TELEIOS_RENDERER_TYPES_H
#define TELEIOS_RENDERER_TYPES_H

#include "teleios/graphics_types.h"
#include "teleios/types.h"

typedef struct {
    const TLGraphics* shader;
    const TLGraphics* geometry;
} TLRenderable;

#endif // TELEIOS_RENDERER_TYPES_H
