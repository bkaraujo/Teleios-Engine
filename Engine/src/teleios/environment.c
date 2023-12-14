#include "teleios/container.h"
#include "teleios/environment.h"
#include "teleios/logger.h"
#include "teleios/memory.h"

typedef struct {
    const TLEnvironmentVariables key;
    const void* value;
} TLEntry;

static TLList* environment;

b8 tl_environment_initialize(void) {
    environment = tl_list_create();
    if (environment == NULL) {
        TLERROR("tl_environment_initialize: Failed to create environment list");
        return false;
    }

    return true;
}

b8 tl_environment_set(const TLEnvironmentVariables key, const void* value) {
    TLNode* node = environment->head;
    while (node != NULL) {
        TLEntry* entry = (TLEntry*)node->payload;
        if (entry->key == key) {

            entry->value = value;
            return true;
        }
        node = node->next;
    }

    TLEntry temp = { key, value };
    TLEntry* entry = tl_memory_alloc(TL_MEMORY_TYPE_ENVIRONMENT, sizeof(TLEntry));
    if (entry == NULL) {
        TLERROR("tl_environment_set: Failed to tl_memory_alloc");
        return false;
    }

    tl_memory_copy(&temp, entry, sizeof(TLEntry));

    return tl_list_append(environment, entry);
}

b8 tl_environment_remove(const TLEnvironmentVariables key) {
    TLNode* node = environment->head;
    while (node != NULL) {
        const TLEntry* entry = node->payload;
        if (entry->key == key) {
            return tl_list_remove_node(environment, node);
        }
        node = node->next;
    }

    return false;
}

const void* tl_environment_get(const TLEnvironmentVariables key) {
    TLNode* node = environment->head;
    while (node != NULL) {
        const TLEntry* entry = node->payload;
        if (entry->key == key) return entry->value;
        node = node->next;
    }
    return NULL;
}

static b8 tl_environment_dealocator(const void* payload) {
    tl_memory_free(payload, TL_MEMORY_TYPE_ENVIRONMENT, sizeof(TLEntry));
    return true;
}

b8 tl_environment_terminate(void) {
    if (environment != NULL) {
        if (!tl_list_clear(environment, tl_environment_dealocator)) {
            TLERROR("tl_environment_terminate: Failed to clear environment list");
            return false;
        }

        if (!tl_list_destroy(environment)) {
            TLERROR("tl_environment_terminate: Failed to destroy environment list");
            return false;
        }

        environment = NULL;
    }
    return true;
}