#include "teleios/ecs/struct.h"
#include "teleios/ecs/manager.h"
#include "teleios/ecs/component.h"
#include "teleios/ecs/system.h"
#include "teleios/ecs/entity.h"
#include "teleios/container.h"
#include "teleios/logger.h"
#include "teleios/identity.h"
#include "teleios/memory/allocator.h"
// ##############################################################################################
//
//                                        COMPONENT
//
// ##############################################################################################

// ##############################################################################################
//
//                                        ENTITY
//
// ##############################################################################################
static TLList* entities;

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name) {
  TLTRACE("tl_ecs_entity_create: \"%s\"", name);
  TLEntity* entity = tl_memory_alloc(TL_MEMORY_TYPE_ECS_ENTITY, sizeof(TLEntity));
  tl_identity_initialize(&entity->identity);
  entity->name = name;

  tl_list_append(entities, entity);
  return &entity->identity;
}

b8 tl_ecs_entity_comparator(const void* first, const void* second) {
  const TLEntity* entity = first;
  const TLIdentity* identity = second;

  return tl_identity_compare(&entity->identity, identity);
}

TLAPI void tl_ecs_entity_destroy(const TLIdentity* entityid) {
  const TLEntity* entity = tl_list_remove(entities, tl_ecs_entity_comparator, entityid);
  if (entity == NULL) {
    TLERROR("tl_ecs_entity_destroy: Failed remove entity from list");
    return;
  }

  TLTRACE("tl_ecs_entity_destroy: \"%s\"", entity->name);
  tl_memory_free(entity, TL_MEMORY_TYPE_ECS_ENTITY, sizeof(TLEntity));
}

TLAPI b8 tl_ecs_entity_attach(const TLIdentity* entityid, const TL_COMPONENT component) {
  return true;
}

TLAPI b8 tl_ecs_entity_detach(const TLIdentity* entityid, const TL_COMPONENT component) {
  return true;
}
// ##############################################################################################
//
//                                        SYSTEM
//
// ##############################################################################################
static TLList* systems;

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_ecs_initialize(void) {
  TLTRACE("tl_ecs_initialize"); 

  entities = tl_list_create();
  if (entities == NULL) {
    TLERROR("tl_ecs_initialize: Failed to create entities list");
    return false;
  }

  systems = tl_list_create();
  if (systems == NULL) {
    tl_list_destroy(entities);
    TLERROR("tl_ecs_initialize: Failed to create systems list");
    return false;
  }

  return true;
}

b8 tl_ecs_terminate(void) {
  TLTRACE("tl_ecs_terminate");

  if (systems->size > 0) {
    TLERROR("tl_ecs_terminate: systems->size > 0");
    return false;
  }

  if (!tl_list_destroy(systems)) {
    TLERROR("tl_ecs_terminate: Failed to destory systems list");
    return false;
  }

  if (entities->size > 0) {
    TLERROR("tl_ecs_terminate: entities->size > 0");
    return false;
  }
  
  if (!tl_list_destroy(entities)) {
    TLERROR("tl_ecs_terminate: Failed to destory entities list");
    return false;
  }

  return true;
}
