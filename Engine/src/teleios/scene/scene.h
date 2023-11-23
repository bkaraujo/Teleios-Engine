#ifndef TELEIOS_SCENE_H
#define TELEIOS_SCENE_H

#include "teleios/defines.h"
#include "teleios/scene/structs.h"
#include "teleios/types.h"

TLAPI const TLIdentity* tl_scene_create_region(const TLIdentity* sceneid, const char* name);
TLAPI void tl_scene_destroy_region(const TLIdentity* sceneid, const TLIdentity* regionid);
void tl_scene_activate_region(const TLIdentity* regionid);

#endif // TELEIOS_SCENE_H
