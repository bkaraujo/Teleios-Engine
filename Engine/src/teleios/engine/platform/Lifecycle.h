#ifndef TELEIOS_ENGINE_PLATFORM_LIFECYCLE_H
#define TELEIOS_ENGINE_PLATFORM_LIFECYCLE_H

#include "teleios/types.h"

b8 tl_platform_initialize(void);
void tl_platform_update(void);
b8 tl_platform_terminate(void);

#endif // TELEIOS_ENGINE_PLATFORM_LIFECYCLE_H