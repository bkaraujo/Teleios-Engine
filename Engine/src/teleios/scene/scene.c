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
  const TLIdentity identity;
  const char* name;
  TLList* regions;
} TLScene;

static const TLScene* active;
static const SSIZE = sizeof(TLScene);

TLAPI void tl_scene_activate(const TLIdentity* sceneid) {
  TLNode* current = scenes->head;
  while (current != NULL) {
    const TLScene* scene = current->payload;
    if (tl_identity_compare(sceneid, &scene->identity)) {
      active = scene;
      break;
    }

    current = current->next;
  }
}

TLAPI const TLIdentity* tl_scene_create(const char* name) {
  TLScene* scene = tl_memory_alloc(TL_MEMORY_TYPE_SCENE, SSIZE);
  if (scene == NULL) {
    TLERROR("tl_scene_create: Failed to allocate TLScene");
    return NULL;
  }

  scene->name = name;
  scene->regions = tl_list_create();
  tl_identity_initialize(&scene->identity);

  if (!tl_list_append(scenes, scene)) {
    tl_memory_free(scene, TL_MEMORY_TYPE_SCENE, SSIZE);
    TLERROR("tl_scene_create: Failed to append scene to list");
    return NULL;
  }

  if (active == NULL) active = scene;
  return &scene->identity;
}

TLAPI void tl_scene_destroy(const TLIdentity* sceneid) {
  TLNode* current = scenes->head;
  while (current != NULL) {
    const TLScene* scene = current->payload;
    
    if (tl_identity_compare(&scene->identity, sceneid)) {
      if (!tl_list_remove_node(scenes, current)) {
        TLERROR("tl_scene_destroy: Failed to remove scene from list");
      }

      tl_list_destroy(scene->regions);
      tl_memory_free(scene, TL_MEMORY_TYPE_SCENE, SSIZE);
      break; 
    }

    current = current->next;
  }
}
// ##############################################################################################
//
//                                        REGION
//
// ##############################################################################################
#include "teleios/scene/region.h"

TLAPI const TLIdentity* tl_region_create(const TLIdentity* sceneid, const char* name) {
  return NULL;
}
               
TLAPI void tl_region_destroy(const TLIdentity* sceneid, const TLIdentity* regionid) {

}

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/scene/manager.h"

b8 tl_scene_initialize(void) {
  scenes = tl_list_create();
  if (scenes == NULL) {
    TLERROR("tl_scene_initialize: Failed to create scene list");
    return false;
  }

  return true;
}

b8 tl_scene_terminate(void) {
  if (scenes->size > 0) {
    TLERROR("tl_scene_terminate: Scene list is not empty.");
    return false;
  }

  return tl_list_destroy(scenes);
}