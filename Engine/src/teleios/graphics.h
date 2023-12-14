#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
b8 tl_graphics_terminate(void);
void tl_graphics_clear(void);
void tl_graphics_present(void);


typedef enum {
    TL_GRAPHICS_PRIMITIVE_MESH
} TLGraphcisType;

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
    } object;

} TLGraphics;

TLGraphics* tl_graphics_primitive_create(void);
void tl_graphics_primitive_destroy(TLGraphics* object);

#endif // TELEIOS_GRAPHICS_H
