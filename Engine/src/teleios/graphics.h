#ifndef TELEIOS_GRAPHICS_H
#define TELEIOS_GRAPHICS_H

#include "teleios/types.h"

b8 tl_graphics_initialize(const TLSpecification* spec);
b8 tl_graphics_terminate(void);
void tl_graphics_being(void);
void tl_graphics_present(void);

#endif // TELEIOS_GRAPHICS_H
