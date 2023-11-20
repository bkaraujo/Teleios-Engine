#include "teleios/logger.h"
#include "teleios/identity.h"
#include "teleios/container.h"
#include "teleios/memory/allocator.h"
#include "teleios/scene/scene.h"
#include "teleios/scene/stack.h"

static const RSIZE = sizeof(TLRegion);
TLAPI const TLIdentity* tl_scene_create_region(const TLIdentity* sceneid, const char* name) {
  // ===========================================================
  // Ensure hosting scene
  // ===========================================================
  const TLScene* scene = tl_scene_stack_find(sceneid);
  if (scene == NULL) {
    TLERROR("tl_scene_create_region: Scene not found");
    return NULL;
  }

  // ===========================================================
  // Create the region
  // ===========================================================
  TLTRACE("tl_scene_create_region: scene \"%s\" region \"%s\"", scene->name, name);
  TLRegion* region = tl_memory_alloc(TL_MEMORY_TYPE_SCENE_REGION, RSIZE);

  region->name = name;
  region->entities = tl_list_create();
  tl_identity_initialize(&region->identity);
  // ===========================================================
  // Associate the Region with the Scene
  // ===========================================================
  if (!tl_list_append(scene->regions, region)) {
    TLERROR("tl_scene_create_region: Failed to append region to the scene");
    if (!tl_list_destroy(region->entities)) {
      TLERROR("tl_scene_create_region: Failed to detroys region's entity list");
      return NULL;
    }

    tl_memory_free(region, TL_MEMORY_TYPE_SCENE_REGION, RSIZE);
    return NULL;
  }

  return &region->identity;
}

TLAPI void tl_scene_destroy_region(const TLIdentity* sceneid, const TLIdentity* regionid) {
  // ===========================================================
  // Ensure hosting scene
  // ===========================================================
  const TLScene* scene = tl_scene_stack_find(sceneid);

  if (scene == NULL) {
    TLERROR("tl_scene_destroy_region: Scene not found");
    return;
  }
  // ===========================================================
  // Ensure region
  // ===========================================================
  const TLNode* node = NULL;
  TLNode* current = scene->regions->head;
  while (current != NULL) {
    const TLRegion* candidate = current->payload;
    if (tl_identity_compare(regionid, &candidate->identity)) {
      node = current;
      break;
    }

    current = current->next;
  }

  if (node == NULL) {
    TLERROR("tl_scene_destroy_region: Region not found within scene.");
    return;
  }
  
  const TLRegion* region = node->payload;
  TLTRACE("tl_scene_destroy_region: scene \"%s\" region \"%s\"", scene->name, region->name);
  // ===========================================================
  // Dealocate the region
  // ===========================================================
  if (!tl_list_clear(region->entities, tl_container_noop_dealocator)) {
    TLERROR("tl_scene_destroy_region: Failed to clear region's entities list");
    return;
  }

  if (!tl_list_destroy(region->entities)) {
    TLERROR("tl_scene_destroy_region: Failed to destroy region's entities list");
    return;
  }

  tl_memory_free(region, TL_MEMORY_TYPE_SCENE_REGION, RSIZE);
  if (!tl_list_remove_node(scene->regions, node)) {
    TLERROR("tl_scene_destroy_region: Failed to remove region from scene's region list");
    return;
  }
}


void tl_scene_update(u64 delta) {

}

void tl_scene_update_fixed(u64 delta) {

}

void tl_scene_update_after(void) {

}