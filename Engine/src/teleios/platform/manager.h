#ifndef TELEIOS_PLATFORM_MANAGER_H
#define TELEIOS_PLATFORM_MANAGER_H

#include "teleios/types.h"

b8 tl_platform_initialize(TLSpecification* spec);
b8 tl_platform_terminate(void);
b8 tl_platform_update(void);
#endif // TELEIOS_PLATFORM_MANAGER_H
