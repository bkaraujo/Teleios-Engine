#ifndef TELEIOS_SCENE_ACTIVE_H
#define TELEIOS_SCENE_ACTIVE_H

#include "teleios/types.h"

b8 tl_scene_prepare(void);

void tl_scene_update(u64 delta);
void tl_scene_update_fixed(u64 delta);
void tl_scene_update_after(void);

#endif // TELEIOS_SCENE_ACTIVE_H
