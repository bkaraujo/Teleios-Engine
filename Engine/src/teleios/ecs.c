#include "teleios/container.h"
#include "teleios/ecs/component.h"
#include "teleios/ecs/entity.h"
#include "teleios/ecs/manager.h"
#include "teleios/identity.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

static TLList* transforms;
typedef struct {
    const TLIdentity entityid;
} TLComponentTransform;

b8 tl_dealocator_transform(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
    return true;
}

static TLList* names;
typedef struct {
    const TLIdentity entityid;
    const char* name;
} TLComponentName;

b8 tl_dealocator_name(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
    return true;
}

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

void* tl_ecs_component_create(const TLIdentity* entityid, TL_COMPONENT_TYPES type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_create: entityid is null");
        return false;
    }

    switch (type) {
    case TL_COMPONENT_TRANSFORM: {
        if (tl_ecs_component_contains(transforms, entityid)) {
            TLWARN("tl_ecs_component_create: Entity %s already have component TL_COMPONENT_TRANSFORM", entityid->identity);
            return NULL;
        }

        TLComponentTransform* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
        tl_memory_copy(entityid, &component->entityid, sizeof(TLIdentity));
        if (!tl_list_append(transforms, component)) {
            TLERROR("tl_ecs_component_create: Failed to create TL_COMPONENT_TRANSFORM");
            return NULL;
        }

        return component;
    } break;

    case TL_COMPONENT_NAME: {
        if (tl_ecs_component_contains(names, entityid)) {
            TLWARN("tl_ecs_component_create: Entity %s already have component TL_COMPONENT_NAME", entityid->identity);
            return NULL;
        }

        TLComponentName* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
        tl_memory_copy(entityid, &component->entityid, sizeof(TLIdentity));
        if (!tl_list_append(names, component)) {
            TLERROR("tl_ecs_component_create: Failed to create TL_COMPONENT_TRANSFORM");
            return NULL;
        }

        return component;

    } break;

    default: {
        TLERROR("tl_ecs_component_create: unexpected TL_COMPONENT_TYPE %lu", type);
    }
    }

    return NULL;
}
// ##############################################################################################
//
//                                        ENTITY
//
// ##############################################################################################
static TLList* identities;

static void tl_ecs_entity_identities_free(void) {
    TLNode* current = identities->head;
    while (current != NULL) {
        tl_memory_free(current->payload, TL_MEMORY_TYPE_IDENTITY, U8MAX * sizeof(TLIdentity));
        current = current->next;
    }
}

static b8 tl_ecs_entity_identities_increase(void) {
    void* block = tl_memory_alloc(TL_MEMORY_TYPE_IDENTITY, U8MAX * sizeof(TLIdentity));
    if (!tl_list_append(identities, block)) {
        TLERROR("tl_ecs_entity_identities_increase: Failed to pool TLIdentity");
        tl_memory_free(block, TL_MEMORY_TYPE_IDENTITY, U8MAX * sizeof(TLIdentity));
        tl_ecs_entity_identities_free();
        return false;
    }

    return true;
}

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name) {
    const TLIdentity* identity = NULL;
    // ==================================================
    // Search for avaliable identity from a pool
    // ==================================================
    TLNode* current = identities->head;
    while (current != NULL) {
        const TLIdentity* pool = current->payload;
        for (unsigned i = 0; i < U8MAX; ++i) {
            const TLIdentity candidate = pool[i];
            if (tl_identity_empty(&candidate)) {
                identity = &candidate;
                break;
            }
        }

        if (identity != NULL) break;
        current = current->next;
    }
    // ==================================================
    // Create new pool and consumes the first element
    // ==================================================
    if (identity == NULL) {
        if (!tl_ecs_entity_identities_increase()) {
            TLERROR("tl_ecs_entity_create: Failed to pool TLIdentity");
            return false;
        }

        TLNode* current = identities->tail;
        const TLIdentity* pool = (TLIdentity*)current->payload;
        identity = *(&pool + 0);
    }
    // ==================================================
    // Setup mandatory components
    // ==================================================
    tl_identity_initialize(identity);
    tl_ecs_component_create(identity, TL_COMPONENT_TRANSFORM);
    TLComponentName* cName = tl_ecs_component_create(identity, TL_COMPONENT_NAME);
    cName->name = name;

    return &cName->entityid;
}
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/memory/allocator.h"

b8 tl_ecs_initialize(void) {
    identities = tl_list_create();
    if (identities == NULL) {
        TLERROR("tl_ecs_initialize: Failed to create identities pool list");
        return false;
    }

    names = tl_list_create();
    if (names == NULL) {
        TLERROR("tl_ecs_initialize: Failed to create names component list");
        return false;
    }

    transforms = tl_list_create();
    if (transforms == NULL) {
        TLERROR("tl_ecs_initialize: Failed to create transform component list");
        return false;
    }

    return true;
}

b8 tl_ecs_terminate(void) {
    if (!tl_list_clear(names, tl_dealocator_name) || !tl_list_destroy(names)) {
        TLERROR("tl_ecs_terminate: Failed to destroy names component list");
        return false;
    }

    if (!tl_list_clear(transforms, tl_dealocator_transform) || !tl_list_destroy(transforms)) {
        TLERROR("tl_ecs_terminate: Failed to destroy transform component list");
        return false;
    }

    tl_ecs_entity_identities_free();

    if (!tl_list_clear(identities, tl_container_noop_dealocator) || !tl_list_destroy(identities)) {
        TLERROR("tl_ecs_terminate: Failed to destroy identities pool list");
        return false;
    }

    return true;
}