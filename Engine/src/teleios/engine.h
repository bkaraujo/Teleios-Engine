#ifndef TELEIOS_ENGINE_H
#define TELEIOS_ENGINE_H

#include "teleios/defines.h"
#include "teleios/types.h"

// TODO: TLAPI when moving back to DLL

b8 tl_engine_pre_initialize(void);
b8 tl_engine_initialize(void);
b8 tl_engine_run(void);
b8 tl_engine_terminate(void);

#endif // TELEIOS_ENGINE_H
