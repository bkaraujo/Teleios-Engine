#include "editor/layer_world.h"
#include <glad/glad.h>
#include <teleios/teleios.h>

static TLGraphics* geometry = NULL;
static TLGraphics* shader = NULL;

static b8 editor_layer_initialize(void) {
    TLDEBUG("editor_layer_initialize: Initializing Editor World Layer");

    TLShaderSource sources[2];
    sources[0].type = TL_SHADER_TYPE_VERTEX;
    sources[0].file = tl_filesystem_file_tochar("C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/vertex.glsl");
    sources[1].type = TL_SHADER_TYPE_FRAGMENT;
    sources[1].file = tl_filesystem_file_tochar("C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/fragment.glsl");
    shader = tl_graphics_shader(sources, 2);

    tl_filesystem_file_free(sources[0].file);
    tl_filesystem_file_free(sources[1].file);

    if (shader == NULL) {
        TLERROR("editor_layer_initialize: Failed to compile shader");
        return false;
    }

    TLGraphicsGeometry create_info = { 0 };
    create_info.vsize = 4 * (3 + 4);
    float vertices[] = {
        // Position , color
         0.5f,  0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,    // top right
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,    // bottom right
        -0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,    // bottom left
        -0.5f,  0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f     // top left 
    };
    create_info.vertices = vertices;

    create_info.isize = 2 * 3;
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    create_info.indices = indices;

    TLBufferLayout layout[2];
    layout[0].name = "aPos";
    layout[0].components = 3;
    layout[0].type = TL_BUFFER_LAYOUT_TYPE_F32;

    layout[1].name = "aColor";
    layout[1].components = 4;
    layout[1].type = TL_BUFFER_LAYOUT_TYPE_F32;

    create_info.lsize = 2;
    create_info.layout = &layout;

    geometry = tl_graphics_geometry(&create_info);
    return true;
}

static b8 editor_layer_terminate(void) {
    TLDEBUG("editor_layer_terminate: Terminating Editor World Layer");
    tl_graphics_primitive_destroy(shader);
    tl_graphics_primitive_destroy(geometry);
    return true;
}

static b8 editor_layer_update(const u64 delta) {
    return true;
}

static b8 editor_ayer_update_fixed(const u64 delta) {
    return true;
}

static b8 editor_layer_update_late(void) {
    glUseProgram(shader->handle);
    glBindVertexArray(geometry->handle);
    glDrawElements(GL_TRIANGLES, geometry->object.buffer.indices, GL_UNSIGNED_INT, 0);

    return true;
}

static TLLayer* layer;
const TLLayer* editor_layer_world_get(void) {
    return layer;
}

b8 editor_layer_world_initialize(void) {
    layer = tl_memory_alloc(TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
    if (layer == NULL) {
        TLERROR("editor_layer_debug_initialize: Failed to create layer");
        return false;
    }

    layer->name = "Editor World";
    layer->initialize = editor_layer_initialize;
    layer->terminate = editor_layer_terminate;
    layer->update = editor_layer_update;
    layer->update_fixed = editor_ayer_update_fixed;
    layer->update_late = editor_layer_update_late;


    if (!tl_engine_layer_append(editor_layer_world_get())) {
        TLERROR("editor_layer_debug_initialize: Engine refused to append layer");
        return false;
    }

    return true;
}

b8 editor_layer_world_terminate(void) {
    if (layer != NULL) {
        if (!tl_engine_layer_remove(editor_layer_world_get())) {
            TLERROR("editor_layer_debug_terminate: Engine refused to remove layer");
            return false;
        }

        tl_memory_free(layer, TL_MEMORY_TYPE_LAYER, sizeof(TLLayer));
    }

    return true;
}