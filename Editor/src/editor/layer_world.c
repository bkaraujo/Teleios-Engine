#include "editor/layer_world.h"
#include <glad/glad.h>
#include <teleios/teleios.h>

unsigned int VAO = GL_NONE;
unsigned int VBO = GL_NONE;
unsigned int shaderProgram = GL_NONE;

static b8 editor_layer_initialize(void) {
    TLDEBUG("editor_layer_initialize: Initializing Editor World Layer");

    {
        unsigned int vertexShader = GL_NONE;
        {
            const TLFile* file = tl_filesystem_file_tochar("C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/vertex.glsl");
            vertexShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexShader, 1, &file->payload.string, 0);
            tl_filesystem_file_free(file);

            glCompileShader(vertexShader);

            int  success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                TLERROR("editor_layer_initialize: Failed to compile vertex shader: %s", infoLog);
                return false;
            }
        }

        unsigned int fragmentShader = GL_NONE;
        {
            const TLFile* file = tl_filesystem_file_tochar("C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/fragment.glsl");
            vertexShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(vertexShader, 1, &file->payload.string, 0);
            tl_filesystem_file_free(file);

            glCompileShader(vertexShader);

            int  success;
            char infoLog[512];
            glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
                TLERROR("editor_layer_initialize: Failed to compile fragment shader: %s", infoLog);
                return false;
            }
        }

        {
            shaderProgram = glCreateProgram();

            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            glLinkProgram(shaderProgram);

            int  success;
            char infoLog[512];
            glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
                TLERROR("editor_layer_initialize: Failed to compile shader program: %s", infoLog);
                return false;
            }
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    {
        {
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);
        }

        {
            f32 vertices[] = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
            };

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
    }
    return true;
}

static b8 editor_layer_terminate(void) {
    TLDEBUG("editor_layer_terminate: Terminating Editor World Layer");
    return true;
}

static b8 editor_layer_update(const u64 delta) {
    return true;
}

static b8 editor_ayer_update_fixed(const u64 delta) {
    return true;
}

static b8 editor_layer_update_late(void) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

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