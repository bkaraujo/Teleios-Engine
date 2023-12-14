#ifndef TELEIOS_RENDERER_H
#define TELEIOS_RENDERER_H

#include "teleios/renderer_primitives.h"
#include "teleios/types.h"

b8 tl_renderer_initialize(const TLSpecification* spec);
b8 tl_renderer_terminate(void);

void tl_renderer_frame_begin(void);
void tl_renderer_frame_end(void);
void tl_renderer_submmit(TLRenderable* renderable);

#endif // TELEIOS_RENDERER_H
