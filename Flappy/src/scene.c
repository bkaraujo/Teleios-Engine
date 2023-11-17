#include "scene.h"

#include <teleios/scene/stack.h>
#include <teleios/scene/scene.h>
#include <teleios/scene/region.h>
#include <teleios/ecs/entity.h>

static const TLIdentity* sceneid;
static const TLIdentity* regionid;
static const TLIdentity* birdid;

void g_scene_initialize(void) {
  sceneid = tl_scene_stack_create("Main");
  regionid = tl_scene_create_region(sceneid, "R1");

  birdid = tl_ecs_entity_create("bird");
  // tl_ecs_entity_attach(birdid, TL_COMPONENT_TRANSFORM);
  // tl_ecs_entity_attach(birdid, TL_COMPONENT_MESH);
  // tl_ecs_entity_attach(birdid, TL_COMPONENT_RIGID_BODY);
  // tl_ecs_entity_attach(birdid, TL_COMPONENT_SCRIPT);
  // 
  tl_region_entity_attach(sceneid, regionid, birdid);
}

void g_scene_terminate(void) {
  tl_ecs_entity_destroy(birdid);
  tl_scene_stack_destroy(sceneid);
}
