#ifndef TELEIOS_ECS_SEARCH_H
#define TELEIOS_ECS_SEARCH_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI const TLList* tl_ecs_search_for(const u32 mask);
TLAPI void tl_ecs_search_release(TLList* search);

#endif // TELEIOS_ECS_SEARCH_H
