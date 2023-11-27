#ifndef TELEIOS_GRAPHICS_CONTEXT_H
#define TELEIOS_GRAPHICS_CONTEXT_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    TLList* extentions;
    TLList* layers;
    VkSurfaceKHR surface;
    struct {
        VkPhysicalDevice ph;
        TLList* extentions;
        VkDevice handle;
    } device;
    VkSwapchainKHR swapchain;
} VKContext;

extern VKContext context;

#endif // TELEIOS_GRAPHICS_CONTEXT_H
