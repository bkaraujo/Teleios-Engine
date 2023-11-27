#ifndef TELEIOS_SCENE_STACK_H
#define TELEIOS_SCENE_STACK_H

#include "teleios/defines.h"
#include "teleios/scene/structs.h"
#include "teleios/types.h"

b8 tl_scene_stack_initialize(void);
b8 tl_scene_stack_terminate(void);

TLAPI const TLIdentity* tl_scene_stack_create(const char* name);
TLAPI b8 tl_scene_stack_destroy(const TLIdentity* sceneid);
const TLScene* tl_scene_stack_get(const TLIdentity* sceneid);

#endif // TELEIOS_SCENE_STACK_H
