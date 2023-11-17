#include "teleios/scene/stack.h"
#include "teleios/scene/region.h"
#include "teleios/logger.h"
#include "teleios/identity.h"
#include "teleios/container.h"

static const TLRegion* tl_region_find(const TLScene* scene, const TLIdentity* regionid) {
  TLNode* current = scene->regions->head;
  while (current != NULL) {
    const TLRegion* candidate = current->payload;
    if (tl_identity_compare(regionid, &candidate->identity)) {
      return candidate;
    }

    current = current->next;
  }

  return NULL;
}

TLAPI b8 tl_region_entity_attach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid) {
  const TLScene* scene = tl_scene_stack_find(sceneid);
  if (scene == NULL) {
    TLERROR("tl_region_entity_attach: Scene not found");
    return false;
  }

  const TLRegion* region = tl_region_find(scene, regionid);
  if (region == NULL) {
    TLERROR("tl_region_entity_attach: Region not found");
    return false;
  }

  TLNode* current = region->entities->head;
  while (current != NULL) {
    const TLIdentity* entity = current->payload;
    if (tl_identity_compare(entityid, entity)) {
      TLWARN("tl_region_entity_attach: Entity already attached to the region");
      return false;
    }

    current = current->next;
  }

  TLTRACE("tl_region_entity_attach: Scene \"%s\" Region \"%s\" Entity \"%s\"", scene->name, region->name, entityid->identity);
  return tl_list_append(region->entities, entityid);
}

TLAPI b8 tl_region_entity_detach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid) {
  const TLScene* scene = tl_scene_stack_find(sceneid);
  if (scene == NULL) {
    TLERROR("tl_region_entity_detach: Scene not found");
    return false;
  }

  const TLRegion* region = tl_region_find(scene, regionid);
  if (region == NULL) {
    TLERROR("tl_region_entity_detach: Region not found");
    return false;
  }

  TLTRACE("tl_region_entity_detach: Scene \"%s\" Region \"%s\" Entity \"%s\"", scene->name, region->name, entityid->identity);

  TLNode* current = region->entities->head;
  while (current != NULL) {
    const TLIdentity* entity = current->payload;
    if (tl_identity_compare(entityid, entity)) {
      if (!tl_list_remove_node(region->entities, current)) {
        TLERROR("tl_region_entity_detach: Failed to detach entity from region");
        return false;
      }

      return true;
    }

    current = current->next;
  }

  TLWARN("tl_region_entity_detach: Entity not found in region");
  return false;
}
