#ifndef TELEIOS_SCENE_H
#define TELEIOS_SCENE_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void tl_scene_activate(TLIdentity* sceneid);
TLAPI TLIdentity* tl_scene_create(const char* name);
TLAPI void tl_scene_destroy(TLIdentity* sceneid);

#endif // TELEIOS_SCENE_H
