#include "scene.h"

#include <teleios/scene/scene.h>

static const TLIdentity* sceneid;
void g_scene_initialize(void) {
  sceneid = tl_scene_create("Main");
}

void g_scene_terminate(void) {
  tl_scene_destroy(sceneid);
}
