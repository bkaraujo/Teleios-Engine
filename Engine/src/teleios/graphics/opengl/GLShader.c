#ifdef TELEIOS_GRAPHICS_OPENGL
#include "glad/glad.h"

#include "teleios/filesystem.h"
#include "teleios/logger/console.h"
#include "teleios/graphics/shader.h"
#include "teleios/graphics/opengl/debug.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/operator.h"

#define LOGSIZE 512
static const TLShader DEFAULT = { 0 };

static inline u32 __tl_parse_shader_type(TLShaderType type) {
    switch (type) {
    case TL_SHADER_FRAGMENT: return GL_FRAGMENT_SHADER;
    case TL_SHADER_VERTEX: return GL_VERTEX_SHADER;
    }

    return U32MAX;
}

static inline u32 tl_shader_compile(u32 shader, TLShaderType type, const char* program) {
    i32 success;
    i8 reason[LOGSIZE];

    u32 gltype = U32MAX;
    switch (type) {
    case TL_SHADER_FRAGMENT: gltype = GL_FRAGMENT_SHADER; break;
    case TL_SHADER_VERTEX: gltype = GL_VERTEX_SHADER;  break;
    }

    const u32 handle = glCreateShader(gltype); GLCHECK();
    glShaderSource(handle, 1, &program, NULL); GLCHECK();

    glCompileShader(handle); GLCHECK();
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success); GLCHECK();
    if (!success) {
        glGetShaderInfoLog(handle, LOGSIZE, NULL, reason); GLCHECK();
        const char* name = NULL;
        switch (type) {
        case TL_SHADER_FRAGMENT: name = "GL_FRAGMENT_SHADER"; break;
        case TL_SHADER_VERTEX: name = "GL_VERTEX_SHADER";  break;
        }

        TLWARN("tl_shader_compile: [%lu] Failed to compile \"%s\"\n\nreason: %s", shader, name, reason);
        return U32MAX;
    }

    return handle;
}

static inline u32 tl_shader_load_source(u32 shader, const TLShaderSource* source) {
    TLTRACE("tl_shader_load_source: [%lu] %s", shader, source->path);

    const TLFile* file = tl_filesystem_read_string(source->path);
    if (file == NULL) {
        TLERROR("tl_shader_load_source: Failed to read \"%s\"", source->path);
        return U32MAX;
    }

    u32 gltype = U32MAX;
    switch (source->type) {
    case TL_SHADER_FRAGMENT: gltype = GL_FRAGMENT_SHADER; break;
    case TL_SHADER_VERTEX: gltype = GL_VERTEX_SHADER;  break;
    }

    u32 handle = tl_shader_compile(shader, source->type, file->payload.string);
    tl_filesystem_free(file);

    return handle;
}

TLEXPORT const TLShader* tl_graphics_shader_create(u8 count, const TLShaderSource* sources) {
    if (sources == NULL) { TLERROR("tl_graphics_shader_create: sources is null"); return &DEFAULT; }
    if (count == 0) { TLERROR("tl_graphics_shader_create: count is 0 (zero)"); return &DEFAULT; }

    // ##########################################################
    // Create the shader program
    // ##########################################################
    TLShader* shader = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(TLShader));
    {
        if (shader == NULL) {
            TLERROR("tl_graphics_shader_create: Failed to allocate memory");
            return &DEFAULT;
        }

        *(u32*)&shader->handle = glCreateProgram(); GLCHECK();
        TLTRACE("tl_graphics_shader_create: Generated Shader Program [%lu]", shader->handle);
        *(u32*)&shader->source_count = count;
        shader->sources = tl_memory_halloc(TL_MEMORY_TYPE_GRAPHICS, shader->source_count * sizeof(TLShaderSource));
        tl_memory_copy((void*)shader->sources, (void*)sources, count * sizeof(TLShaderSource));
    }
    // ##########################################################
    // Attach Shader Steps
    // ##########################################################
    u32 vertex, fragment;
    {
        vertex = fragment = U32MAX;
        for (unsigned i = 0; i < count; ++i) {
            const u32 handle = tl_shader_load_source(shader->handle, &sources[i]);
            if (handle == U32MAX) continue;

            glAttachShader(shader->handle, handle); GLCHECK();

            switch (sources[i].type) {
            case TL_SHADER_VERTEX: vertex = handle; break;
            case TL_SHADER_FRAGMENT: fragment = handle; break;
            }
        }
    }
    // ##########################################################
    // Default Shader Steps
    // ##########################################################
    {
        if (vertex == U32MAX) {
            TLWARN("tl_graphics_shader_create: [%lu] Using detaulf Vertex Shader", shader->handle);
            const char* program =
                "#version 330 core\n"
                "layout (location = 0) in vec3 geometry;\n"
                "uniform mat4 u_projection;\n"
                "void main() { gl_Position = u_projection * vec4(geometry, 1.0); }";

            vertex = tl_shader_compile(shader->handle, TL_SHADER_VERTEX, program);
            glAttachShader(shader->handle, vertex); GLCHECK();
        }

        if (fragment == U32MAX) {
            TLWARN("tl_graphics_shader_create: [%lu] Using detaulf Fragment Shader", shader->handle);
            const char* program =
                "#version 330 core\n"
                "out vec4 FragColor;\n"
                "void main() { FragColor = vec4(0.97, 0.21, 0.97, 1.0); } ";

            fragment = tl_shader_compile(shader->handle, TL_SHADER_FRAGMENT, program);
            glAttachShader(shader->handle, fragment); GLCHECK();
        }
    }
    // ##########################################################
    // Link the shader program
    // ##########################################################
    {
        i32 success;
        i8 reason[LOGSIZE];

        glLinkProgram(shader->handle); GLCHECK();
        if (vertex != U32MAX) { glDeleteShader(vertex); GLCHECK(); }
        if (fragment != U32MAX) { glDeleteShader(fragment); GLCHECK(); }

        glGetProgramiv(shader->handle, GL_LINK_STATUS, &success); GLCHECK();
        if (!success) {
            tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, shader, sizeof(TLShader));
            glGetProgramInfoLog(shader->handle, LOGSIZE, NULL, reason); GLCHECK();
            glDeleteProgram(shader->handle); GLCHECK();
            TLWARN("tl_graphics_shader_create: [%lu] Failed to link shader\n\n%s\n", shader->handle, reason);
            return &DEFAULT;
        }
    }

    return shader;
}

TLEXPORT void tl_graphics_shader_bind(const TLShader* shader) {
    if (shader == NULL) { TLERROR("tl_graphics_shader_bind: shader is null"); return; }
    glUseProgram(shader->handle); GLCHECK();
}

void tl_graphics_shader_unbind(const TLShader* shader) {
    glUseProgram(GL_NONE); GLCHECK();
}

TLEXPORT void tl_graphics_shader_uploadf(const TLShader* shader, const char* uniform, u8 count, f32* vector) {
    if (shader == NULL) { TLERROR("tl_graphics_shader_uploadf: shader is null"); return; }
    if (uniform == NULL) { TLERROR("tl_graphics_shader_uploadf: uniform is null"); return; }

    i8 location = glGetUniformLocation(shader->handle, uniform);
    if (location == -1) { TLWARN("tl_graphics_shader_uploadf: unknown uniform \"%s\"", uniform); return; }

    switch (count) {
    case 1: glUniform1f(location, vector[0]); break;
    case 2: glUniform2f(location, vector[0], vector[1]); break;
    case 3: glUniform3f(location, vector[0], vector[1], vector[2]); break;
    case 4: glUniform4f(location, vector[0], vector[1], vector[2], vector[3]); break;
    } GLCHECK();
}

TLEXPORT void tl_graphics_shader_uploadi(const TLShader* shader, const char* uniform, u8 count, i32* vector) {
    if (shader == NULL) { TLERROR("tl_graphics_shader_uploadi: shader is null"); return; }
    if (uniform == NULL) { TLERROR("tl_graphics_shader_uploadi: uniform is null"); return; }

    i8 location = glGetUniformLocation(shader->handle, uniform);
    if (location == -1) { TLWARN("tl_graphics_shader_uploadi: unknown uniform \"%s\"", uniform); return; }

    switch (count) {
    case 1: glUniform1i(location, vector[0]); break;
    case 2: glUniform2i(location, vector[0], vector[1]); break;
    case 3: glUniform3i(location, vector[0], vector[1], vector[2]); break;
    case 4: glUniform4i(location, vector[0], vector[1], vector[2], vector[3]); break;
    } GLCHECK();
}

TLEXPORT void tl_graphics_shader_uploadu(const TLShader* shader, const char* uniform, u8 count, u32* vector) {
    if (shader == NULL) { TLERROR("tl_graphics_shader_uploadu: shader is null"); return; }
    if (uniform == NULL) { TLERROR("tl_graphics_shader_uploadu: uniform is null"); return; }

    i8 location = glGetUniformLocation(shader->handle, uniform);
    if (location == -1) { TLWARN("tl_graphics_shader_uploadu: unknown uniform \"%s\"", uniform); return; }

    switch (count) {
    case 1: glUniform1ui(location, vector[0]); break;
    case 2: glUniform2ui(location, vector[0], vector[1]); break;
    case 3: glUniform3ui(location, vector[0], vector[1], vector[2]); break;
    case 4: glUniform4ui(location, vector[0], vector[1], vector[2], vector[3]); break;
    } GLCHECK();
}

TLEXPORT void tl_graphics_shader_uploadm(const TLShader* shader, const char* uniform, u8 type, u8 count, void* vector) {
    if (shader == NULL) { TLERROR("tl_graphics_shader_uploadm: shader is null"); return; }
    if (uniform == NULL) { TLERROR("tl_graphics_shader_uploadm: uniform is null"); return; }

    i8 location = glGetUniformLocation(shader->handle, uniform);
    if (location == -1) { TLWARN("tl_graphics_shader_uploadi: unknown uniform \"%s\"", uniform); return; }


    switch (type) {
    case 2: {
        switch (count) {
        case 2: glUniformMatrix2fv(location, 1, GL_TRUE, vector);  GLCHECK(); break;
        case 3: glUniformMatrix2x3fv(location, 6, GL_TRUE, vector);  GLCHECK(); break;
        case 4: glUniformMatrix2x4fv(location, 8, GL_TRUE, vector);  GLCHECK(); break;
        default: TLWARN("tl_graphics_shader_uploadm: Unknown count %u for matrix %d", count, type); break;
        }
    } break;
    case 3: {
        switch (count) {
        case 2: glUniformMatrix3x2fv(location, 6, GL_TRUE, vector);  GLCHECK(); break;
        case 3: glUniformMatrix3fv(location, 1, GL_TRUE, vector);  GLCHECK(); break;
        case 4: glUniformMatrix3x4fv(location, 12, GL_TRUE, vector);  GLCHECK(); break;
        default: TLWARN("tl_graphics_shader_uploadm: Unknown count %u for matrix %d", count, type); break;
        }
    } break;
    case 4: {
        switch (count) {
        case 2: glUniformMatrix4x2fv(location, 8, GL_TRUE, vector);  GLCHECK(); break;
        case 3: glUniformMatrix4x3fv(location, 12, GL_TRUE, vector);  GLCHECK(); break;
        case 4: glUniformMatrix4fv(location, 1, GL_FALSE, vector);  GLCHECK(); break;
        default: TLWARN("tl_graphics_shader_uploadm: Unknown count %u for matrix %d", count, type); break;
        }
    } break;
    default: {
        TLWARN("tl_graphics_shader_uploadm: Unknown matrix %d", type);
        return;
    }
    }
}

TLEXPORT void tl_graphics_shader_destroy(const TLShader* shader) {
    if (shader == NULL) return;
    if (shader == &DEFAULT) return;

    if (shader->handle != GL_NONE) { glDeleteProgram(shader->handle); GLCHECK(); }

    tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)shader->sources, shader->source_count * sizeof(TLShaderSource));
    tl_memory_hfree(TL_MEMORY_TYPE_GRAPHICS, (void*)shader, sizeof(TLShader));
}
#endif // TELEIOS_GRAPHICS_OPENGL