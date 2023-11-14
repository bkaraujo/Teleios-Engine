#include "teleios/logger.h"
#include "teleios/identity.h"
#include "teleios/container.h"
#include "teleios/memory/allocator.h"

// ##############################################################################################
//
//                                        SCENE
//
// ##############################################################################################
#include "teleios/scene/scene.h"
static TLList* scenes;

typedef struct {
  TLIdentity identity;
  const char* name;
  TLList* regions;
} TLScene;

static const SSIZE = sizeof(TLScene);

TLAPI void tl_scene_activate(TLIdentity* sceneid) {

}

TLAPI TLIdentity* tl_scene_create(const char* name) {
  TLScene* scene = tl_memory_alloc(TL_MEMORY_TYPE_SCENE, SSIZE);
  if (scene == NULL) {
    TLERROR("tl_scene_create: Failed to allocate TLScene");
    return NULL;
  }

  tl_list_append(scenes, scene);
  tl_identity_initialize(&scene->identity);
  scene->name = name;

  return &scene->identity;
}

TLAPI void tl_scene_destroy(TLIdentity* sceneid) {
  TLNode* current = scenes->head;
  while (current != NULL) {
    TLScene* scene = current->payload;
    if (tl_identity_compare(&scene->identity, sceneid)) {
      if (!tl_list_remove_node(scenes, current)) {
        TLERROR("tl_scene_destroy: Failed to remove scene from list");
      }
    }
  }
}
// ##############################################################################################
//
//                                        REGION
//
// ##############################################################################################
#include "teleios/scene/region.h"

TLAPI TLIdentity* tl_region_create(TLIdentity* sceneid, const char* name) {
  return NULL;
}

TLAPI void tl_region_destroy(TLIdentity* sceneid, TLIdentity* regionid) {

}

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/scene/manager.h"

b8 tl_scene_initialize(void) {
  return true;
}

b8 tl_scene_terminate(void) {
  return true;
}