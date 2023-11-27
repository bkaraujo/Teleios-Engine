#include "teleios/container.h"
#include "teleios/identity/generator.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/scene/structs.h"

static const SSIZE = sizeof(TLScene);
static const RSIZE = sizeof(TLRegion);

// ##############################################################################################
//
//                                        STACK
//
// ##############################################################################################
#include "teleios/scene/stack.h"
#include "teleios/scene/scene.h"

static TLList* scenes;

b8 tl_scene_stack_initialize(void) {
    scenes = tl_list_create();
    if (scenes == NULL) {
        TLERROR("tl_scene_stack_initialize: Failed to create scene list");
        return false;
    }

    return true;
}

TLAPI const TLIdentity* tl_scene_stack_create(const char* name) {
    TLScene* scene = tl_memory_alloc(TL_MEMORY_TYPE_SCENE, SSIZE);
    if (scene == NULL) {
        TLERROR("tl_scene_create: Failed to allocate TLScene");
        return NULL;
    }

    scene->name = name;
    scene->regions = tl_list_create();
    tl_identity_generate(&scene->identity);

    if (!tl_list_append(scenes, scene)) {
        tl_memory_free(scene, TL_MEMORY_TYPE_SCENE, SSIZE);
        TLERROR("tl_scene_create: Failed to append scene to list");
        return NULL;
    }

    return &scene->identity;
}

TLAPI b8 tl_scene_stack_destroy(const TLIdentity* sceneid) {
    // ===========================================================
    // Ensure scene
    // ===========================================================
    const TLScene* scene = tl_scene_stack_get(sceneid);
    if (scene == NULL) {
        TLERROR("tl_scene_destroy: Scene not found");
        return false;
    }
    // ===========================================================
    // Ensure scene has no region
    // ===========================================================
    while (scene->regions->head != NULL) {
        const TLRegion* region = scene->regions->head->payload;
        tl_scene_destroy_region(sceneid, &region->identity);
    }

    if (scene->regions->size > 0) {
        TLERROR("tl_scene_destroy: Failed to destroy scene's regions");
        return false;
    }
    // ===========================================================
    // Dealocate scene
    // ===========================================================
    if (!tl_list_remove_payload(scenes, scene)) {
        TLERROR("tl_scene_destroy: Failed to remove scene from scenes list");
        return false;
    }

    tl_list_destroy(scene->regions);
    tl_memory_free(scene, TL_MEMORY_TYPE_SCENE, SSIZE);
    return true;
}

static const TLScene* found = NULL;
const TLScene* tl_scene_stack_get(const TLIdentity* sceneid) {
    if (sceneid == NULL) {
        TLERROR("tl_scene_stack_get: sceneid is null");
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

b8 tl_scene_stack_terminate(void) {
    TLNode* current = scenes->head;
    while (current != NULL) {
        const TLScene* scene = current->payload;
        TLNode* next = current->next;
        TLWARN("tl_scene_stack_terminate: Removing scene %s. %s", scene->identity.identity, MSG_PLEASE_DO_IT_YOURSELF);

        tl_scene_stack_destroy(&scene->identity);
        current = next;
    }

    if (!tl_list_destroy(scenes)) {
        TLERROR("tl_scene_stack_terminate: Failed to destroy scenes list");
        return false;
    }
    scenes = NULL;

    return true;
}
