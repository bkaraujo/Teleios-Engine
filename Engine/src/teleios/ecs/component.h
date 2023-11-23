#ifndef TELEIOS_ECS_COMPONENT_H
#define TELEIOS_ECS_COMPONENT_H

#include "teleios/types.h"

typedef enum {
    TL_COMPONENT_TRANSFORM,
    TL_COMPONENT_NAME,
    TL_COMPONENT_MAXIMUM
} TL_COMPONENT_TYPE;

TLAPI void* tl_ecs_component_attach(const TLIdentity* entityid, TL_COMPONENT_TYPE type);
TLAPI void tl_ecs_component_detach(const TLIdentity* entityid, TL_COMPONENT_TYPE type);

#endif // TELEIOS_ECS_COMPONENT_H
