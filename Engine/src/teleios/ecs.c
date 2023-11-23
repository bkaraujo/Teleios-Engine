#include "teleios/container.h"
#include "teleios/ecs/component.h"
#include "teleios/ecs/entity.h"
#include "teleios/ecs/manager.h"
#include "teleios/identity.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

static TLList* transforms;
typedef struct {
    const TLIdentity* entityid;
} TLComponentTransform;

b8 tl_dealocator_transform(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
    return true;
}

static TLList* names;
typedef struct {
    const TLIdentity* entityid;
    const char* name;
} TLComponentName;

b8 tl_dealocator_name(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
    return true;
}

b8 tl_dealocator_identity(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
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
        if (tl_identity_equals(entityid, &identity)) {
            return true;
        }

        current = current->next;
    }

    return false;
}

TLAPI void* tl_ecs_component_attach(const TLIdentity* entityid, TL_COMPONENT_TYPE type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_attach: entityid is null");
        return false;
    }

    switch (type) {
    case TL_COMPONENT_TRANSFORM: {
        if (tl_ecs_component_contains(transforms, entityid)) {
            TLWARN("tl_ecs_component_attach: Entity %s already have component TL_COMPONENT_TRANSFORM", entityid->identity);
            return NULL;
        }

        TLComponentTransform* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
        component->entityid = entityid;
        if (!tl_list_append(transforms, component)) {
            TLERROR("tl_ecs_component_attach: Failed to create TL_COMPONENT_TRANSFORM");
            return NULL;
        }

        return component;
    } break;

    case TL_COMPONENT_NAME: {
        if (tl_ecs_component_contains(names, entityid)) {
            TLWARN("tl_ecs_component_attach: Entity %s already have component TL_COMPONENT_NAME", entityid->identity);
            return NULL;
        }

        TLComponentName* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
        component->entityid = entityid;
        if (!tl_list_append(names, component)) {
            TLERROR("tl_ecs_component_attach: Failed to create TL_COMPONENT_TRANSFORM");
            return NULL;
        }

        return component;

    } break;

    default: {
        TLERROR("tl_ecs_component_attach: unexpected tl_identity_equals %lu", type);
    }
    }

    return NULL;
}

TLAPI void tl_ecs_component_detach(const TLIdentity* entityid, TL_COMPONENT_TYPE type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_remove: entityid is null");
        return;
    }

    switch (type) {
    case TL_COMPONENT_NAME: {
        TLNode* current = names->head;
        while (current != NULL) {
            const TLComponentName* component = current->payload;
            if (tl_identity_equals(entityid, component->entityid)) {
                if (!tl_list_remove_node(names, current)) {
                    TLERROR("tl_ecs_component_remove: Failed to remove component TLComponentName from entity %s ", entityid->identity);
                    return;
                }

                tl_memory_free(component, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
                return;
            }
            current = current->next;
        }
    }break;
    case TL_COMPONENT_TRANSFORM: {
        TLNode* current = transforms->head;
        while (current != NULL) {
            const TLComponentTransform* component = current->payload;
            if (tl_identity_equals(entityid, component->entityid)) {
                if (!tl_list_remove_node(transforms, current)) {
                    TLERROR("tl_ecs_component_remove: Failed to remove component TLComponentTransform from entity %s ", entityid->identity);
                    return;
                }

                tl_memory_free(component, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
                return;
            }

            current = current->next;
        }
    } break;
    default: TLERROR("tl_ecs_component_remove: unexpected tl_identity_equals %lu", type); break;
    }
}
// ##############################################################################################
//
//                                        ENTITY
//
// ##############################################################################################
static TLList* entities;

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name) {
    TLIdentity* identity = tl_memory_alloc(TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
    tl_identity_initialize(identity);

    if (tl_ecs_component_attach(identity, TL_COMPONENT_TRANSFORM) == NULL) {
        TLERROR("tl_ecs_entity_create: Failed to attach TL_COMPONENT_TRANSFORM");
        tl_memory_free(identity, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
        return NULL;
    }

    if (tl_ecs_component_attach(identity, TL_COMPONENT_NAME) == NULL) {
        TLERROR("tl_ecs_entity_create: Failed to attach TL_COMPONENT_NAME");
        tl_memory_free(identity, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
        tl_ecs_component_detach(identity, TL_COMPONENT_TRANSFORM);
        return NULL;
    }

    if (!tl_list_append(entities, identity)) {
        tl_ecs_component_detach(identity, TL_COMPONENT_NAME);
        tl_ecs_component_detach(identity, TL_COMPONENT_TRANSFORM);
        tl_memory_free(identity, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
        TLERROR("tl_ecs_entity_create: Failed to append identity to entities list");
        return NULL;
    }

    return identity;
}

TLAPI b8 tl_ecs_entity_destroy(const TLIdentity* entityid) {
    TLNode* current = entities->head;
    while (current != NULL) {
        const TLIdentity* candidate = current->payload;
        if (tl_identity_equals(entityid, candidate)) {
            tl_ecs_component_detach(entityid, TL_COMPONENT_NAME);
            tl_ecs_component_detach(entityid, TL_COMPONENT_TRANSFORM);
            if (!tl_list_remove_node(entities, current)) {
                TLERROR("tl_ecs_entity_destroy: Failed to remove entity from list after dealocating");
                return false;
            }

            tl_memory_free(entityid, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
            return true;
        }

        current = current->next;
    }

    return false;
}
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/memory/allocator.h"

b8 tl_ecs_initialize(void) {
    entities = tl_list_create();
    if (entities == NULL) {
        TLERROR("tl_ecs_initialize: Failed to create entities identity list");
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
    TLNode* current = entities->head;
    while (current != NULL) {
        const TLIdentity* entityid = current->payload;
        current = current->next;

        TLWARN("tl_ecs_terminate: Removing entity %s. Please remove it yourself at tl_application_terminate(void)", entityid->identity);
        if (!tl_ecs_entity_destroy(entityid)) {
            TLERROR("tl_ecs_terminate: Failed to destroy entity.");
        }
    }

    if (!tl_list_destroy(entities)) {
        TLERROR("tl_ecs_terminate: Failed to destroy identities pool list");
        return false;
    }
    if (!tl_list_destroy(names)) {
        TLERROR("tl_ecs_terminate: Failed to destroy names component list");
        return false;
    }

    if (!tl_list_destroy(transforms)) {
        TLERROR("tl_ecs_terminate: Failed to destroy transform component list");
        return false;
    }

    return true;
}