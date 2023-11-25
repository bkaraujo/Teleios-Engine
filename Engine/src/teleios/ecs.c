#include "teleios/container.h"
#include "teleios/ecs/component.h"
#include "teleios/ecs/components.h"
#include "teleios/ecs/entity.h"
#include "teleios/ecs/manager.h"
#include "teleios/identity/generator.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

static TLList* components[TL_COMPONENT_MAXIMUM];

b8 tl_dealocator_transform(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
    return true;
}


b8 tl_dealocator_name(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
    return true;
}

b8 tl_dealocator_identity(const void* pointer) {
    tl_memory_free(pointer, TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
    return true;
}

static const char* tl_ecs_component_name(const TL_COMPONENT_TYPE type) {
    switch (type) {
    case TL_COMPONENT_TRANSFORM: return "TL_COMPONENT_TRANSFORM";
    case TL_COMPONENT_NAME: return "TL_COMPONENT_NAME";
    case TL_COMPONENT_VELOCITY: return "TL_COMPONENT_VELOCITY";
    case TL_COMPONENT_MASS: return "TL_COMPONENT_MASS";
    case TL_COMPONENT_RIGIDBODY: return "TL_COMPONENT_RIGIDBODY";
    case TL_COMPONENT_SPRITE: return "TL_COMPONENT_SPRITE";
    case TL_COMPONENT_MESH: return "TL_COMPONENT_MESH";
    case TL_COMPONENT_MAXIMUM: return "TL_COMPONENT_MAXIMUM";
    }
    return "tl_ecs_component_name: ? ? ?";
}
// ##############################################################################################
//
//                                       COMPONENT
//
// ##############################################################################################

static b8 tl_ecs_component_contains(const TLList* list, const TLIdentity* entityid) {
    TLNode* current = list->head;
    while (current != NULL) {
        const TLComponent* component = current->payload;
        if (tl_identity_equals(entityid, component->identity)) {
            return true;
        }

        current = current->next;
    }

    return false;
}

TLAPI void* tl_ecs_component_attach(const TLIdentity* entityid, const TL_COMPONENT_TYPE type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_attach: entityid is null");
        return false;
    }

    if (tl_ecs_component_contains(components[type], entityid)) {
        TLWARN("tl_ecs_component_attach: Entity %s already have component %s", entityid->identity, tl_ecs_component_name(type));
        return NULL;
    }

    switch (type) {
    case TL_COMPONENT_TRANSFORM: {
        TLComponentTransform* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform));
        component->owner.identity = entityid;
        if (!tl_list_append(components[type], component)) {
            TLERROR("tl_ecs_component_attach: Failed to create %s", tl_ecs_component_name(type));
            return NULL;
        }

        return component;
    } break;

    case TL_COMPONENT_NAME: {
        TLComponentName* component = tl_memory_alloc(TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName));
        component->owner.identity = entityid;
        if (!tl_list_append(components[type], component)) {
            TLERROR("tl_ecs_component_attach: Failed to create %s", tl_ecs_component_name(type));
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

TLAPI void tl_ecs_component_detach(const TLIdentity* entityid, const TL_COMPONENT_TYPE type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_remove: entityid is null");
        return;
    }

    TLNode* current = components[type]->head;
    while (current != NULL) {
        TLNode* next = current->next;
        const TLComponent* component = current->payload;
        if (tl_identity_equals(entityid, component->identity)) {
            if (!tl_list_remove_node(components[type], current)) {
                TLERROR("tl_ecs_component_remove: Failed to remove component %s from entity %s ", tl_ecs_component_name(type), entityid->identity);
                return;
            }

            switch (type) {
            case TL_COMPONENT_NAME: tl_memory_free(component, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentName)); break;
            case TL_COMPONENT_TRANSFORM: tl_memory_free(component, TL_MEMORY_TYPE_ECS_COMPONENT, sizeof(TLComponentTransform)); break;
            default: TLERROR("tl_ecs_component_remove: unexpected tl_identity_equals %lu", type); break;
                return;
            }
        }

        current = next;
    }
}

TLAPI const void* tl_ecs_component_get(const TLIdentity* entityid, const TL_COMPONENT_TYPE type) {
    if (entityid == NULL) {
        TLERROR("tl_ecs_component_get: entityid is null");
        return NULL;
    }

    TLNode* current = components[type]->head;
    while (current != NULL) {
        const TLComponent* component = current->payload;
        if (tl_identity_equals(entityid, component->identity)) {
            return component;
        }
        current = current->next;
    }

    return NULL;
}
// ##############################################################################################
//
//                                        ENTITY
//
// ##############################################################################################
static TLList* entities;

TLAPI const TLIdentity* tl_ecs_entity_create(const char* name) {
    TLIdentity* identity = tl_memory_alloc(TL_MEMORY_TYPE_IDENTITY, sizeof(TLIdentity));
    tl_identity_generate(identity);

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
//                                        SYSTEM
//
// ##############################################################################################
#include "teleios/ecs/system.h"

typedef enum {
    TL_SYSTEM_TYPE_UNTIMED,
    TL_SYSTEM_TYPE_FIXED,
    TL_SYSTEM_TYPE_VARIABLE,
    TL_SYSTEM_TYPE_MAXIMUM
} TL_SYSTEM_TYPE;

static TLList* system[3];

TLAPI b8 tl_ecs_system_register_untimed(PFN_SYSTEM_UNTIEMD pfn) {
    return tl_list_append(system[TL_SYSTEM_TYPE_UNTIMED], pfn);
}

TLAPI b8 tl_ecs_system_register_timed(b8 time_fixed, PFN_SYSTEM_TIMED pfn) {
    return tl_list_append(time_fixed ? system[TL_SYSTEM_TYPE_FIXED] : system[TL_SYSTEM_TYPE_VARIABLE], pfn);
}

TLAPI b8 tl_ecs_system_unregister(PFN_SYSTEM_TIMED pfn) {
    if (tl_list_remove_payload(system[TL_SYSTEM_TYPE_VARIABLE], pfn)) return true;
    if (tl_list_remove_payload(system[TL_SYSTEM_TYPE_FIXED], pfn)) return true;
    return tl_list_remove_payload(system[TL_SYSTEM_TYPE_UNTIMED], pfn);
}

b8 tl_ecs_system_process_time_fixed(const u64 delta) {
    TLNode* current = system[TL_SYSTEM_TYPE_FIXED]->head;
    while (current != NULL) {
        if (!((PFN_SYSTEM_TIMED)current->payload)(delta)) {
            TLERROR("tl_ecs_system_process_time_fixed: System return false");
            return false;
        }
        current = current->next;
    }

    return true;
}

b8 tl_ecs_system_process_time_delta(const u64 delta) {
    TLNode* current = system[TL_SYSTEM_TYPE_VARIABLE]->head;
    while (current != NULL) {
        if (!((PFN_SYSTEM_TIMED)current->payload)(delta)) {
            TLERROR("tl_ecs_system_process_time_delta: System return false");
            return false;
        }
        current = current->next;
    }

    return true;
}

b8 tl_ecs_system_process(void) {
    TLNode* current = system[TL_SYSTEM_TYPE_UNTIMED]->head;
    while (current != NULL) {
        if (!((PFN_SYSTEM_UNTIEMD)current->payload)()) {
            TLERROR("tl_ecs_system_process: System return false");
            return false;
        }
        current = current->next;
    }

    return true;
}
// ##############################################################################################
//
//                                        SEARCH
//
// ##############################################################################################
static b8 tl_ecs_compare_component(const void* first, const void* second) {
    const TLComponent* component = first;
    return tl_identity_equals(component->identity, second);
}

TLAPI const TLList* tl_ecs_search_for(const u32 mask) {
    TLList* result = tl_list_create();

    {
        TLNode* current = components[TL_COMPONENT_TRANSFORM]->head;
        while (current != NULL) {
            const TLComponentTransform* component = current->payload;
            if (!tl_list_append(result, component->owner.identity)) {
                TLERROR("tl_ecs_search: Failed to append to result");
                tl_list_clear(result, tl_container_noop_dealocator);
                tl_list_destroy(result);
                return NULL;
            }
            current = current->next;
        }
    }

    {
        TLNode* current = result->head;
        while (current != NULL) {
            const TLIdentity* entityid = current->payload;
            if (!tl_list_contains(components[TL_COMPONENT_NAME], tl_ecs_compare_component, entityid)) {
                if (!tl_list_remove_payload(result, entityid)) {
                    TLERROR("tl_ecs_search_for: Failed to remove entity from result");
                    tl_list_clear(result, tl_container_noop_dealocator);
                    tl_list_destroy(result);
                    return NULL;
                }
            }
            current = current->next;
        }
    }

    return result;
}

TLAPI void tl_ecs_search_release(TLList* search) {
    tl_list_clear(search, tl_container_noop_dealocator);
    tl_list_destroy(search);
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

    for (unsigned i = 0; i < TL_COMPONENT_MAXIMUM; ++i) {
        components[i] = tl_list_create();
        if (components[i] == NULL) {
            TLERROR("tl_ecs_initialize: Failed to create component %u list", i);
            return false;
        }
    }

    for (unsigned i = 0; i < TL_SYSTEM_TYPE_MAXIMUM; ++i) {
        system[i] = tl_list_create();
        if (system[i] == NULL) {
            TLERROR("tl_ecs_initialize: Failed to create system list");
            return false;
        }
    }

    return true;
}

b8 tl_ecs_terminate(void) {
    {
        if (entities->size > 0) {
            TLNode* current = entities->head;
            while (current != NULL) {
                const TLIdentity* entityid = current->payload;
                current = current->next;

                TLWARN("tl_ecs_terminate: Removing entity %s. %s", entityid->identity, MSG_PLEASE_DO_IT_YOURSELF);
                if (!tl_ecs_entity_destroy(entityid)) {
                    TLERROR("tl_ecs_terminate: Failed to destroy entity.");
                }
            }
        }

        if (!tl_list_destroy(entities)) {
            TLERROR("tl_ecs_terminate: Failed to destroy identities pool list");
            return false;
        }

        entities = NULL;
    }

    for (unsigned i = 0; i < TL_COMPONENT_MAXIMUM; ++i) {
        if (!tl_list_destroy(components[i])) {
            TLERROR("tl_ecs_terminate: Failed to destroy %u component list", i);
            return false;
        }

        components[i] = NULL;
    }

    for (unsigned i = 0; i < TL_SYSTEM_TYPE_MAXIMUM; ++i) {
        if (system[i]->size > 0) {
            TLWARN("tl_ecs_terminate: Removing unregistered systems. Please remove it yourself at tl_application_terminate(void)");
            if (!tl_list_clear(system[i], tl_container_noop_dealocator)) {
                TLERROR("tl_ecs_terminate: Failed to clear system list");
                return false;
            }
        }

        if (!tl_list_destroy(system[i])) {
            TLERROR("tl_ecs_terminate: Failed to destroy system list");
            return false;
        }

        system[i] = NULL;
    }

    return true;
}