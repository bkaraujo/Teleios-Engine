#ifndef TELEIOS_SCENE_H
#define TELEIOS_SCENE_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include "teleios/scene/structs.h"

TLAPI const TLIdentity* tl_scene_create_region(const TLIdentity* sceneid, const char* name);
TLAPI void tl_scene_destroy_region(const TLIdentity* sceneid, const TLIdentity* regionid);

void tl_scene_update(u64 delta);
void tl_scene_update_fixed(u64 delta);
void tl_scene_update_after(void);

#endif // TELEIOS_SCENE_H
