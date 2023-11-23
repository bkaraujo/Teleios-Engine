#include "teleios/container.h"
#include "teleios/identity.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

#include "teleios/scene/active.h"
#include "teleios/scene/manager.h"
#include "teleios/scene/region.h"
#include "teleios/scene/scene.h"
#include "teleios/scene/stack.h"

static TLList* scenes;
static TLList* regions;
static const TLScene* active;

static const SSIZE = sizeof(TLScene);
static const RSIZE = sizeof(TLRegion);

static const TLScene* tl_scene_find(const TLIdentity* sceneid);

// ##############################################################################################
//
//                                        STACK
//
// ##############################################################################################
TLAPI const TLIdentity* tl_scene_stack_create(const char* name) {
    TLTRACE("tl_scene_stack_create: scene \"%s\"", name);

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

TLAPI void tl_scene_stack_destroy(const TLIdentity* sceneid) {
    // ===========================================================
    // Ensure scene
    // ===========================================================
    const TLScene* scene = tl_scene_find(sceneid);
    if (scene == NULL) {
        TLERROR("tl_scene_destroy: Scene not found");
        return;
    }
    // ===========================================================
    // Ensure scene has no region
    // ===========================================================
    TLTRACE("tl_scene_stack_destroy: scene \"%s\"", scene->name);
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

void tl_scene_stack_activate(const TLIdentity* sceneid) {
    if (active != NULL && tl_identity_equals(&active->identity, sceneid)) {
        TLWARN("tl_scene_stack_activate: Scene already active");
        return;
    }

    const TLScene* scene = tl_scene_find(sceneid);
    if (scene == NULL) {
        TLERROR("tl_region_entity_attach: Scene not found");
        return;
    }

    active = scene;
}
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
    const TLScene* scene = tl_scene_find(sceneid);
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

    TLTRACE("tl_region_entity_attach: Scene \"%s\" Region \"%s\" Entity \"%s\"", scene->name, region->name, entityid->identity);
    return tl_list_append(region->entities, entityid);
}

TLAPI b8 tl_region_entity_detach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid) {
    const TLScene* scene = tl_scene_find(sceneid);
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
    const TLScene* scene = tl_scene_find(sceneid);
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
    const TLScene* scene = tl_scene_find(sceneid);
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

void tl_scene_activate_region(const TLIdentity* regionid) {
    TLNode* current = active->regions->head;
    while (current != NULL) {
        const TLRegion* candidate = current->payload;
        if (tl_identity_equals(regionid, &candidate->identity)) {
            *((b8*)&candidate->active) = true;
            tl_list_append(regions, candidate);
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
    TLTRACE("tl_scene_initialize");
    scenes = tl_list_create();
    if (scenes == NULL) {
        TLERROR("tl_scene_initialize: Failed to create scene list");
        return false;
    }

    regions = tl_list_create();
    if (regions == NULL) {
        TLERROR("tl_scene_initialize: Failed to create regions list");
        return false;
    }

    return true;
}

b8 tl_scene_terminate(void) {
    TLTRACE("tl_scene_terminate");
    if (scenes->size > 0) {
        TLERROR("tl_scene_terminate: Scene list is not empty.");
        return false;
    }

    if (!tl_list_destroy(scenes)) {
        TLERROR("tl_scene_terminate: Failed to destroy scenes list");
        return false;
    }

    if (!tl_list_clear(regions, tl_container_noop_dealocator)) {
        TLERROR("tl_scene_terminate: Failed to clear regions list");
        return false;
    }

    if (!tl_list_destroy(regions)) {
        TLERROR("tl_scene_terminate: Failed to destroy regions list");
        return false;
    }

    return true;
}
// ##############################################################################################
//
//                                        ACTIVE
//
// ##############################################################################################
b8 tl_scene_prepare(void) {
    if (regions == NULL || regions->size == 0) {
        TLERROR("tl_scene_prepare: No region active");
        return false;
    }

    return true;
}

void tl_scene_update_fixed(u64 delta) {

}

void tl_scene_update(u64 delta) {

}

void tl_scene_update_after(void) {

}
// ##############################################################################################
//
//                                        HELPERS
//
// ##############################################################################################
static const TLScene* found;
static const TLScene* tl_scene_find(const TLIdentity* sceneid) {
    if (sceneid == NULL) {
        TLERROR("tl_scene_find: sceneid is null");
        return NULL;
    }

    if (found != NULL && tl_identity_equals(&found->identity, sceneid)) {
        return found;
    }

    TLNode* current = scenes->head;
    while (current != NULL) {
        const TLScene* canditate = current->payload;
        if (tl_identity_equals(&canditate->identity, sceneid)) {
            found = canditate;
            break;
        }

        current = current->next;
    }

    return found;
}
