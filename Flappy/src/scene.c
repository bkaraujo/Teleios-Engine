#include "scene.h"

#include <teleios/logger.h>
#include <teleios/identity.h>
#include <teleios/scene/stack.h>
#include <teleios/scene/scene.h>
#include <teleios/scene/region.h>
#include <teleios/ecs/component.h>

static const TLIdentity* sceneid;
static const TLIdentity* regionid;
const TLIdentity birdid;

void g_scene_initialize(void) {
  sceneid = tl_scene_stack_create("Main");
  regionid = tl_scene_create_region(sceneid, "R1");
  
  tl_identity_initialize(&birdid);
  if (!tl_ecs_component_create(&birdid, TL_COMPONENT_TRANSFORM)) {
    TLFATAL("g_scene_initialize: Failed to create TL_COMPONENT_TRANSFORM");
  }

  tl_scene_stack_activate(sceneid);
  tl_scene_activate_region(regionid);
}

void g_scene_terminate(void) {
  tl_scene_stack_destroy(sceneid); 
}
