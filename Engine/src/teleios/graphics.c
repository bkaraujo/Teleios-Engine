#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkdevice.h"
#include "teleios/graphics/vulkan/vkinstance.h"
#include "teleios/graphics/vulkan/vksurface.h"
#include "teleios/graphics/vulkan/vkswapchain.h"
#include "teleios/memory/tools.h"

VKContext context = { 0 };
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/graphics/manager.h"


b8 tl_graphics_initialize(const TLSpecification* spec) {
    tl_memory_zero(&context, sizeof(VKContext));

    if (!tl_vulkan_instance_initialize(spec)) return false;
    if (!tl_vulkan_surface_initialize(spec)) return false;
    if (!tl_vulkan_device_initialize(spec)) return false;
    if (!tl_vulkan_swapchain_initialize(spec)) return false;

    return true;
}

b8 tl_graphics_terminate(void) {
    if (!tl_vulkan_swapchain_terminate()) return false;
    if (!tl_vulkan_device_terminate()) return false;
    if (!tl_vulkan_surface_terminate()) return false;
    if (!tl_vulkan_instance_terminate()) return false;

    return true;
}

// ##############################################################################################
//
//                                        PRIMITIVE
//
// ##############################################################################################
#include "teleios/graphics/primitive.h"

