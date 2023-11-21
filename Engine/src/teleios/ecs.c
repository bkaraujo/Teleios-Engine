#include "teleios/logger.h"
#include "teleios/container.h"
#include "teleios/identity.h"
#include "teleios/memory/allocator.h"
#include "teleios/ecs/component.h"
#include "teleios/ecs/manager.h"

typedef struct {
  const TLIdentity entityid;
} TLTransform;

static TLList* transforms;
// ##############################################################################################
//
//                                       COMPONENT
//
// ##############################################################################################
static b8 tl_ecs_component_contains(TLList* list, const TLIdentity* entityid) {
  TLIdentity identity;
  TLNode* current = list->head;
  while (current != NULL) {
    tl_memory_copy(current->payload, &identity.identity, sizeof(TLIdentity));
    if (tl_identity_compare(entityid, &identity)) {
      return true;
    }

    current = current->next;
  }

  return false;
}

b8 tl_ecs_component_create(const TLIdentity* entityid, TL_COMPONENT_TYPES type) {
  if (entityid == NULL) {
    TLERROR("tl_ecs_component_create: entityid is null");
    return false;
  }

  switch (type) {
    case TL_COMPONENT_TRANSFORM: {
      if (tl_ecs_component_contains(transforms, entityid)) {
        TLWARN("tl_ecs_component_create: Entity %s already have component TL_COMPONENT_TRANSFORM", entityid->identity);
        return false;
      }

      TLTransform* transform = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLTransform));
      tl_memory_copy(entityid, &transform->entityid, sizeof(TLIdentity));

      if (!tl_list_append(transforms, transform)) {
        TLERROR("tl_ecs_component_create: Failed to create TL_COMPONENT_TRANSFORM");
        return false;
      }
    } break;

    default: {
      TLERROR("tl_ecs_component_create: unexpected TL_COMPONENT_TYPE %lu", type);
      return false;
    }
  }

  return true;
}
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_ecs_initialize(void) {
  transforms = tl_list_create();
  if (transforms == NULL) {
    TLERROR("tl_ecs_initialize: Failed to create transform component list");
    return false;
  }

  return true;
}

b8 tl_dealocator_transform(const void* pointer) {
  tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLTransform));
  return true;
}

b8 tl_ecs_terminate(void) {
  if (!tl_list_clear(transforms, tl_dealocator_transform)) {
    TLERROR("tl_ecs_terminate: Failed to clear transform component list");
    return false;
  }

  if (!tl_list_destroy(transforms)) {
    TLERROR("tl_ecs_terminate: Failed to destroy transform component list");
    return false;
  }

  return true;
}