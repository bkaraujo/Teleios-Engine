#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
void tl_graphics_clear(void);
void tl_graphics_update(void);
b8 tl_graphics_terminate(void);

#endif // TELEIOS_GRAPHICS_H