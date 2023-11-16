#include "teleios/scene/stack.h"
#include "teleios/scene/scene.h"
#include "teleios/logger.h"
#include "teleios/identity.h"
#include "teleios/container.h"
#include "teleios/memory/allocator.h"

static TLList* scenes;
// ##############################################################################################
//
//                                        STACK
//
// ##############################################################################################
static const SSIZE = sizeof(TLScene);

TLAPI const TLIdentity* tl_scene_stack_create(const char* name){
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

  return &scene->identity;
}

static const TLScene* last;
const TLScene* tl_scene_stack_find(const TLIdentity* sceneid) {
  if (last != NULL && tl_identity_compare(&last->identity, sceneid)) {
    return last;
  }

  TLNode* current = scenes->head;
  while (current != NULL) {
    const TLScene* canditate = current->payload;
    if (tl_identity_compare(&canditate->identity, sceneid)) {
      last = canditate;
      break;
    }
  }

  return last;
}

TLAPI void tl_scene_stack_destroy(const TLIdentity* sceneid) {
  // ===========================================================
// Ensure scene
// ===========================================================
  const TLScene* scene = tl_scene_stack_find(sceneid);

  if (scene == NULL) {
    TLERROR("tl_scene_destroy: Scene not found");
    return;
  }
  // ===========================================================
  // Ensure scene has no region
  // ===========================================================
  TLNode* current = scene->regions->head;
  while (current != NULL) {
    const TLRegion* region = current->payload;
    tl_scene_destroy_region(sceneid, &region->identity);
    current = current->next;
  }

  if (scene->regions->size > 0) {
    TLERROR("tl_scene_destroy: Failed to destroy scene's regions");
    return;
  }
  // ===========================================================
  // Dealocate scene
  // ===========================================================
  if (!tl_list_remove_payload(scenes, scene)) {
    TLERROR("tl_scene_destroy: Failed to remove scene from scenes list");
    return;
  }

  tl_list_destroy(scene->regions);
  tl_memory_free(scene, TL_MEMORY_TYPE_SCENE, SSIZE);
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