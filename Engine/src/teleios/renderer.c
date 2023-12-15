#include "teleios/renderer_types.h"
#include "teleios/teleios.h"
#include <glad/glad.h>

static TLList* queue;

static b8 tl_renderable_dealocator(const void* renderable);

// ####################################################################
// ####################################################################
//                              Public API
// ####################################################################
// ####################################################################

TLAPI void tl_renderer_submmit(const TLGraphics* shader, const TLGraphics* geometry) {
    TLRenderable* renderable = tl_memory_alloc(TL_MEMORY_TYPE_RENDERER, sizeof(TLRenderable));
    renderable->shader = shader;
    renderable->geometry = geometry;
    tl_list_append(queue, renderable);
}

// ####################################################################
// ####################################################################
//                          Internal API
// ####################################################################
// ####################################################################

b8 tl_renderer_initialize(const TLSpecification* spec) {
    TLTRACE("tl_renderer_initialize: Initializing Renderer");
    queue = tl_list_create();
    if (queue == NULL) {
        TLERROR("tl_renderer_initialize: Failed to create render queue");
        return false;
    }
    return true;
}

void tl_renderer_frame_begin(void) {
    if (!tl_list_clear(queue, tl_renderable_dealocator)) {
        TLFATAL("tl_renderer_frame_begin: Failed to clear the render queue");
    }

    if (!tl_list_remove_all(queue)) {
        TLFATAL("tl_renderer_frame_begin: Failed to empty the render queue");
    }

    tl_graphics_clear();
}

void tl_renderer_frame_end(void) {
    // =========================================================
    // Sort for camera proximity
    // =========================================================

    // =========================================================
    // Sort for shader usage
    // =========================================================

    // =========================================================
    // Actual draw calls
    // =========================================================
    TLNode* node = queue->head;
    while (node != NULL) {
        const TLRenderable* renderable = node->payload;

        glUseProgram(renderable->shader->handle);
        glBindVertexArray(renderable->geometry->handle);
        glDrawElements(GL_TRIANGLES, renderable->geometry->payload.vertex.indices, GL_UNSIGNED_INT, 0);
        node = node->next;
    }

    tl_graphics_present();
}

b8 tl_renderer_terminate(void) {
    if (!tl_list_clear(queue, tl_renderable_dealocator)) {
        TLERROR("tl_renderer_terminate: Failed to clear the render queue");
        return false;
    }
    if (!tl_list_destroy(queue)) {
        TLERROR("tl_renderer_terminate: Failed to destroy the render queue");
        return false;
    }
    return true;
}

// ####################################################################
// ####################################################################
//                              Private API
// ####################################################################
// ####################################################################

static b8 tl_renderable_dealocator(const void* renderable) {
    tl_memory_free(renderable, TL_MEMORY_TYPE_RENDERER, sizeof(TLRenderable));
    return true;
}
