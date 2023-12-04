#include "teleios/filesystem/files.h"
#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkpipeline.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/string.h"

static VkShaderModule sm_vertex;
static VkShaderModule sm_fragment;

static b8 tl_vulkan_pipeline_shader(VkShaderModule* module, const char* path);

b8 tl_vulkan_pipeline_initialize(const TLSpecification* spec) {
    if (!tl_vulkan_pipeline_shader(&sm_vertex, spec->vulkan.pipeline.vertex)) return false;
    VkPipelineShaderStageCreateInfo vert_stage_info = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = sm_vertex;
    vert_stage_info.pName = "main";

    if (!tl_vulkan_pipeline_shader(&sm_fragment, spec->vulkan.pipeline.fragment)) return false;
    VkPipelineShaderStageCreateInfo frag_stage_info = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = sm_fragment;
    frag_stage_info.pName = "main";

    VkPipelineShaderStageCreateInfo create_info[] = { vert_stage_info, frag_stage_info };
    return true;
}

static b8 tl_vulkan_pipeline_shader(VkShaderModule* module, const char* path) {
    if (path == NULL) {
        TLERROR("tl_vulkan_pipeline_shader: Path is null");
        return false;
    }

    const u32* shader_code = tl_filesystem_file_loadu32(path);
    if (shader_code == NULL) {
        TLERROR("tl_vulkan_pipeline_shader: Failed to load framgment shader");
        return false;
    }

    VkShaderModuleCreateInfo create_info = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
    create_info.codeSize = tl_filesyste_file_size(path);
    create_info.pCode = shader_code;

    VkResult result = vkCreateShaderModule(context.device.lo.handle, &create_info, context.allocator, module);
    if (result != VK_SUCCESS) {
        tl_memory_free(shader_code, TL_MEMORY_TYPE_FILE, create_info.codeSize);
        TLERROR("vkCreateShaderModule: Failed with: %s", vkresult(result));
        return false;
    }

    tl_memory_free(shader_code, TL_MEMORY_TYPE_FILE, create_info.codeSize);

    return true;
}

b8 tl_vulkan_pipeline_terminate(void) {
    if (sm_fragment != NULL) {
        vkDestroyShaderModule(context.device.lo.handle, sm_fragment, NULL);
        sm_fragment = NULL;
    }

    if (sm_vertex != NULL) {
        vkDestroyShaderModule(context.device.lo.handle, sm_vertex, NULL);
        sm_vertex = NULL;
    }

    return true;
}
