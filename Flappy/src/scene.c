#include "scene.h"

#include <teleios/scene/stack.h>
#include <teleios/scene/scene.h>

static const TLIdentity* sceneid;
static const TLIdentity* regionid;
void g_scene_initialize(void) {
  sceneid = tl_scene_stack_create("Main");
  regionid = tl_scene_create_region(sceneid, "R1");
}

void g_scene_terminate(void) {
  tl_scene_stack_destroy(sceneid);
}
