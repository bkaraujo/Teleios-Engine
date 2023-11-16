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
    TLERROR("tl_scene_sestroy_region: Scene not found");
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
    TLERROR("tl_scene_sestroy_region: Region not found within scene.");
    return;
  }
  // ===========================================================
  // Dealocate the region
  // ===========================================================
  const TLRegion* region = node->payload;
  if (!tl_list_destroy(region->entities)) {
    TLERROR("tl_scene_sestroy_region: Failed to remove entities from region");
    return;
  }

  tl_memory_free(region, TL_MEMORY_TYPE_SCENE_REGION, RSIZE);
  if (!tl_list_remove_node(scene->regions, node)) {
    TLERROR("tl_scene_sestroy_region: Failed to remove region from scene's region list");
    return;
  }
}
