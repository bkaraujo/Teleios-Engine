#ifndef TELEIOS_ENGINE_LIFECYCLE_H
#define TELEIOS_ENGINE_LIFECYCLE_H

#include "teleios/types.h"

TLEXPORT b8 tl_engine_initialize(const TLSpecification* spec);
TLEXPORT b8 tl_engine_run(void);
TLEXPORT b8 tl_engine_terminate(void);

#endif // TELEIOS_ENGINE_LIFECYCLE_H