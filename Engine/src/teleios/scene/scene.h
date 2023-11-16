#ifndef TELEIOS_SCENE_H
#define TELEIOS_SCENE_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void tl_scene_activate(const TLIdentity* sceneid);
TLAPI const TLIdentity* tl_scene_create(const char* name);
TLAPI void tl_scene_destroy(const TLIdentity* sceneid);

#endif // TELEIOS_SCENE_H
