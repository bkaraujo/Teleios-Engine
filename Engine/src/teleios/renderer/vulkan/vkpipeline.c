#include "teleios/filesystem/files.h"
#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/renderer/vulkan/vkpipeline.h"
#include "teleios/string.h"

static VkShaderModule sm_vertex;
static VkShaderModule sm_fragment;

static b8 tl_vulkan_pipeline_shader(VkShaderModule* module, const char* path);

b8 tl_vulkan_pipeline_initialize(const TLSpecification* spec) {
    // ===============================================
    // Shaders
    // ===============================================
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
    // ===============================================
    // Vertex input
    // ===============================================
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = NULL; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = NULL; // Optional

    // ===============================================
    // Input assembly
    // ===============================================
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    // ===============================================
    // Viewports and scissors
    // ===============================================
    VkViewport viewport = { 0 };
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)context.swapchain.extent.width;
    viewport.height = (float)context.swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.extent = context.swapchain.extent;

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    // ===============================================
    // Rasterizer
    // ===============================================
    VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
    // ===============================================
    // Multisampling
    // ===============================================
    VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
    // ===============================================
    // Depth and stencil testing
    // ===============================================
    VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    depthStencil.depthBoundsTestEnable = VK_TRUE;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.minDepthBounds = -1;
    depthStencil.maxDepthBounds = 1;
    // ===============================================
    // Color blending
    // ===============================================
    VkPipelineColorBlendAttachmentState colorBlendAttachment = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional
    // ===============================================
    // Pipeline layout
    // ===============================================
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = NULL; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    context.layout = VK_NULL_HANDLE;
    VKCHECK("vkCreatePipelineLayout", vkCreatePipelineLayout(context.device.lo.handle, &pipelineLayoutInfo, context.allocator, &context.layout));
    return true;
}

static b8 tl_vulkan_pipeline_shader(VkShaderModule* module, const char* path) {
    if (path == NULL) {
        TLERROR("tl_vulkan_pipeline_shader: Path is null");
        return false;
    }

    const u32* shader_code = tl_filesystem_file_tou32(path);
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
    if (context.layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(context.device.lo.handle, context.layout, context.allocator);
        context.layout = VK_NULL_HANDLE;
    }

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
