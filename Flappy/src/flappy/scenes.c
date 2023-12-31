#include "flappy/scene/world.h"

#include <teleios/ecs/entity.h>
#include <teleios/logger.h>
#include <teleios/scene/active.h>
#include <teleios/scene/region.h>
#include <teleios/scene/scene.h>
#include <teleios/scene/stack.h>

static const TLIdentity* sceneid;
static const TLIdentity* regionid;
static const TLIdentity* birdid;
static const TLIdentity* pipeid;

void flappy_scene_world_initialize(void) {
    sceneid = tl_scene_stack_create("Main");
    if (sceneid == NULL) TLFATAL("g_scene_initialize: Failed to create scene");

    regionid = tl_scene_create_region(sceneid, "R1");
    if (regionid == NULL) TLFATAL("g_scene_initialize: Failed to create region");

    birdid = tl_ecs_entity_create("bird");
    if (birdid == NULL) TLFATAL("g_scene_initialize: Failed to create entity");
    if (!tl_region_entity_attach(sceneid, regionid, birdid)) TLFATAL("g_scene_initialize: Failed to attach entity");

    pipeid = tl_ecs_entity_create("Pipe");
    if (pipeid == NULL) TLFATAL("g_scene_initialize: Failed to create entity");
    if (!tl_region_entity_attach(sceneid, regionid, pipeid)) TLFATAL("g_scene_initialize: Failed to attach entity");

    tl_scene_activate(sceneid);
    tl_scene_activate_region(regionid);
}

void flappy_scene_world_terminate(void) {
    //tl_ecs_entity_destroy(birdid);
    //tl_ecs_entity_destroy(pipeid);
    //tl_scene_stack_destroy(sceneid);
}
