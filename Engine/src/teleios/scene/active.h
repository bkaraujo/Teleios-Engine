#ifndef TELEIOS_SCENE_ACTIVE_H
#define TELEIOS_SCENE_ACTIVE_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void tl_scene_activate(const TLIdentity* sceneid);

b8 tl_scene_prepare(void);
b8 tl_scene_update(u64 delta);
b8 tl_scene_update_fixed(u64 delta);
b8 tl_scene_update_after(void);

#endif // TELEIOS_SCENE_ACTIVE_H
