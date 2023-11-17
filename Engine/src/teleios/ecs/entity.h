#ifndef TELEIOS_ECS_ENTITY_H
#define TELEIOS_ECS_ENTITY_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include "teleios/ecs/component.h"

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name);
TLAPI void tl_ecs_entity_destroy(const TLIdentity* entityid);

TLAPI b8 tl_ecs_entity_attach(const TLIdentity* entityid, const TL_COMPONENT component);
TLAPI b8 tl_ecs_entity_detach(const TLIdentity* entityid, const TL_COMPONENT component);

#endif // TELEIOS_ECS_ENTITY_H
