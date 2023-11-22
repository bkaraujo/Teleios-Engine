#ifndef TELEIOS_SCENE_STACK_H
#define TELEIOS_SCENE_STACK_H

#include "teleios/defines.h"
#include "teleios/scene/structs.h"
#include "teleios/types.h"

void tl_scene_stack_activate(const TLIdentity* sceneid);
TLAPI const TLIdentity* tl_scene_stack_create(const char* name);
TLAPI void tl_scene_stack_destroy(const TLIdentity* sceneid);

#endif // TELEIOS_SCENE_STACK_H
