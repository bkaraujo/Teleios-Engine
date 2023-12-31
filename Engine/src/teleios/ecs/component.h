#ifndef TELEIOS_ECS_COMPONENT_H
#define TELEIOS_ECS_COMPONENT_H

#include "teleios/types.h"

typedef enum {
    TL_COMPONENT_TRANSFORM,
    TL_COMPONENT_NAME,
    TL_COMPONENT_VELOCITY,
    TL_COMPONENT_MASS,
    TL_COMPONENT_RIGIDBODY,
    TL_COMPONENT_SPRITE,
    TL_COMPONENT_MESH,
    TL_COMPONENT_MAXIMUM
} TL_COMPONENT_TYPE;

TLAPI void* tl_ecs_component_attach(const TLIdentity* entityid, const TL_COMPONENT_TYPE type);
TLAPI void  tl_ecs_component_detach(const TLIdentity* entityid, const TL_COMPONENT_TYPE type);
TLAPI const void* tl_ecs_component_get(const TLIdentity* entityid, const TL_COMPONENT_TYPE type);

#endif // TELEIOS_ECS_COMPONENT_H
