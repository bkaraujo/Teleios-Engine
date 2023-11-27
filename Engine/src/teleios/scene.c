#include "teleios/container.h"
#include "teleios/identity/generator.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

#include "teleios/scene/active.h"
#include "teleios/scene/manager.h"
#include "teleios/scene/region.h"
#include "teleios/scene/scene.h"
#include "teleios/scene/stack.h"

static TLList* active_regions;
static const TLScene* active_scene;

static const RSIZE = sizeof(TLRegion);

// ##############################################################################################
//
//                                        REGION
//
// ##############################################################################################
static const TLRegion* tl_region_find(const TLScene* scene, const TLIdentity* regionid) {
    TLNode* current = scene->regions->head;
    while (current != NULL) {
        const TLRegion* candidate = current->payload;
        if (tl_identity_equals(regionid, &candidate->identity)) {
            return candidate;
        }

        current = current->next;
    }

    return NULL;
}

TLAPI b8 tl_region_entity_attach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid) {
    const TLScene* scene = tl_scene_stack_get(sceneid);
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
        if (tl_identity_equals(entityid, entity)) {
            TLWARN("tl_region_entity_attach: Entity already attached to the region");
            return false;
        }

        current = current->next;
    }

    return tl_list_append(region->entities, entityid);
}

TLAPI b8 tl_region_entity_detach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid) {
    const TLScene* scene = tl_scene_stack_get(sceneid);
    if (scene == NULL) {
        TLERROR("tl_region_entity_detach: Scene not found");
        return false;
    }

    const TLRegion* region = tl_region_find(scene, regionid);
    if (region == NULL) {
        TLERROR("tl_region_entity_detach: Region not found");
        return false;
    }

    TLNode* current = region->entities->head;
    while (current != NULL) {
        const TLIdentity* entity = current->payload;
        if (tl_identity_equals(entityid, entity)) {
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
// ##############################################################################################
//
//                                        SCENE
//
// ##############################################################################################
TLAPI const TLIdentity* tl_scene_create_region(const TLIdentity* sceneid, const char* name) {
    // ===========================================================
    // Ensure hosting scene
    // ===========================================================
    const TLScene* scene = tl_scene_stack_get(sceneid);
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
    tl_identity_generate(&region->identity);
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
    const TLScene* scene = tl_scene_stack_get(sceneid);
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
        if (tl_identity_equals(regionid, &candidate->identity)) {
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

void tl_scene_activate_region(const TLIdentity* regionid) {
    TLNode* current = active_scene->regions->head;
    while (current != NULL) {
        const TLRegion* candidate = current->payload;
        if (tl_identity_equals(regionid, &candidate->identity)) {
            *((b8*)&candidate->active) = true;
            tl_list_append(active_regions, candidate);
            break;
        }

        current = current->next;
    }
}
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_scene_initialize(void) {
    if (!tl_scene_stack_initialize()) {
        return false;
    }

    active_regions = tl_list_create();
    if (active_regions == NULL) {
        TLERROR("tl_scene_initialize: Failed to create regions list");
        return false;
    }

    return true;
}

b8 tl_scene_terminate(void) {
    active_scene = NULL;

    if (!tl_scene_stack_terminate()) {
        return false;
    }

    {
        if (!tl_list_clear(active_regions, tl_container_noop_dealocator)) {
            TLERROR("tl_scene_terminate: Failed to clear regions list");
            return false;
        }

        if (!tl_list_destroy(active_regions)) {
            TLERROR("tl_scene_terminate: Failed to destroy regions list");
            return false;
        }

        active_regions = NULL;
    }

    return true;
}
// ##############################################################################################
//
//                                        ACTIVE
//
// ##############################################################################################
#include "teleios/ecs/system.h"

TLAPI void tl_scene_activate(const TLIdentity* sceneid) {
    if (active_scene != NULL && tl_identity_equals(&active_scene->identity, sceneid)) {
        TLWARN("tl_scene_stack_activate: Scene already active");
        return;
    }

    const TLScene* scene = tl_scene_stack_get(sceneid);
    if (scene == NULL) {
        TLERROR("tl_region_entity_attach: Scene not found");
        return;
    }

    active_scene = scene;
}

b8 tl_scene_prepare(void) {
    if (active_regions == NULL || active_regions->size == 0) {
        TLERROR("tl_scene_prepare: No region active");
        return false;
    }

    return true;
}

b8 tl_scene_update_fixed(u64 delta) {
    return tl_ecs_system_process_time_fixed(delta);
}

b8 tl_scene_update(u64 delta) {
    return tl_ecs_system_process_time_delta(delta);
}

b8 tl_scene_update_after(void) {
    return tl_ecs_system_process();
}
