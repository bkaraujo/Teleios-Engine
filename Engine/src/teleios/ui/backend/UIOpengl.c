#ifdef TELEIOS_GRAPHICS_OPENGL

#include "nuklear/nuklear.h"
#include "teleios/ui/backend.h"
#include "teleios/ui/backend/common.h"
#include "teleios/memory/pool.h"
#include "teleios/logger/console.h"
#include "teleios/graphics/shader.h"
#include "teleios/graphics/geometry.h"

static TLMemoryPool* memory;
struct nk_context* nkcontext;

#define NK_SHADER_VERSION "#version 450 core\n"
#define NK_SHADER_BINDLESS "#extension GL_ARB_bindless_texture : require\n"
#define NK_SHADER_64BIT "#extension GL_ARB_gpu_shader_int64 : require\n"

static const TLShader* shader = NULL;
static const TLGeometry* geometry = NULL;


b8 tl_uibackend_initialize(void) {
    memory = tl_memory_pool_create();
    if (memory == NULL) { TLERROR("tl_uibackend_initialize: Failed to create TLMemoryPool"); return false; }

    TLShaderSource vertex = { 0 };
    vertex.type = TL_SHADER_VERTEX;
    vertex.source =
        NK_SHADER_VERSION
        NK_SHADER_BINDLESS
        NK_SHADER_64BIT
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";

    TLShaderSource fragment = { 0 };
    fragment.type = TL_SHADER_FRAGMENT;
    fragment.source =
        NK_SHADER_VERSION
        NK_SHADER_BINDLESS
        NK_SHADER_64BIT
        "precision mediump float;\n"
        "uniform uint64_t Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   sampler2D smp = sampler2D(Texture);\n"
        "   Out_Color = Frag_Color * texture(smp, Frag_UV.st);\n"
        "}\n";

    TLShaderSource sources[2] = { vertex, fragment };
    shader = tl_graphics_shader_create(TLARRSIZE(sources), sources);

    return true;
}

b8 tl_uibackend_update(void) {
    const struct nk_command* command = 0;
    nk_foreach(command, &nkcontext) {
        switch (command->type) {
        case NK_COMMAND_NOP:
        case NK_COMMAND_SCISSOR:
        case NK_COMMAND_LINE:
        case NK_COMMAND_CURVE:
        case NK_COMMAND_RECT:
        case NK_COMMAND_RECT_FILLED:
        case NK_COMMAND_RECT_MULTI_COLOR:
        case NK_COMMAND_CIRCLE:
        case NK_COMMAND_CIRCLE_FILLED:
        case NK_COMMAND_ARC:
        case NK_COMMAND_ARC_FILLED:
        case NK_COMMAND_TRIANGLE:
        case NK_COMMAND_TRIANGLE_FILLED:
        case NK_COMMAND_POLYGON:
        case NK_COMMAND_POLYGON_FILLED:
        case NK_COMMAND_POLYLINE:
        case NK_COMMAND_TEXT:
        case NK_COMMAND_IMAGE:
        case NK_COMMAND_CUSTOM: break;
        }
    }
    return true;
}

b8 tl_uibackend_terminate(void) {
    tl_graphics_shader_destroy(shader);
    tl_memory_pool_destroy(memory);

    return true;
}

#endif // TELEIOS_GRAPHICS_OPENGL