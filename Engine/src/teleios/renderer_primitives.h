#ifndef TELEIOS_RENDERER_PRIMITIVES
#define TELEIOS_RENDERER_PRIMITIVES

#include "teleios/graphics.h"

typedef struct {
    u32 handler;
} TLShader;

typedef struct {
    TLShader* shader;
    TLGraphics* graphics;
} TLRenderable;

#endif // TELEIOS_RENDERER_PRIMITIVES
