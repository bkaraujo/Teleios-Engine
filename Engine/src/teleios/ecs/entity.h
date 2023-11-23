#ifndef TELEIOS_ECS_ENTITY_H
#define TELEIOS_ECS_ENTITY_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name);
TLAPI b8 tl_ecs_entity_destroy(const TLIdentity* entityid);

#endif // TELEIOS_ECS_ENTITY_H
