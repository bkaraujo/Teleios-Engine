#ifndef TELEIOS_ENGINE_H
#define TELEIOS_ENGINE_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI b8 tl_engine_pre_initialize(void);
TLAPI b8 tl_engine_initialize(const TLSpecification* spec);
TLAPI b8 tl_engine_run(void);
TLAPI b8 tl_engine_terminate(void);

TLAPI b8 tl_engine_layer_append(const TLLayer* layer);
TLAPI b8 tl_engine_layer_remove(const TLLayer* layer);

#endif // TELEIOS_ENGINE_H
