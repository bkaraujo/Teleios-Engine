#ifndef TELEIOS_SCENE_STACK_H
#define TELEIOS_SCENE_STACK_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include "teleios/scene/structs.h"

const TLScene* tl_scene_stack_find(const TLIdentity* sceneid);
TLAPI const TLIdentity* tl_scene_stack_create(const char* name);
TLAPI void tl_scene_stack_destroy(const TLIdentity* sceneid);

#endif // TELEIOS_SCENE_STACK_H
