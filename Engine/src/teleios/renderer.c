#include "teleios/logger.h"
#include "teleios/renderer/manager.h"
#include "teleios/renderer/vulkan/vkpipeline.h"
#include "teleios/renderer/vulkan/vkrenderpass.h"

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_renderer_initialize(const TLSpecification* spec) {
    if (!tl_vulkan_pipeline_initialize(spec)) return false;
    if (!tl_vulkan_renderpass_initialize(spec)) return false;

    return true;
}

b8 tl_renderer_terminate(void) {
    if (!tl_vulkan_pipeline_terminate()) return false;
    if (!tl_vulkan_renderpass_terminate()) return false;

    return true;
}
