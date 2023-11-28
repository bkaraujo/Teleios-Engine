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
        struct {
            VkPhysicalDevice handle;
            VkPhysicalDeviceMemoryProperties memory;
            VkPhysicalDeviceFeatures features;
            VkPhysicalDeviceDriverProperties driver_properties;
            VkPhysicalDeviceProperties properties;
            TLList* extentions;
        } ph;
        struct {
            VkDevice handle;
        } lo;
    } device;
    VkSwapchainKHR swapchain;
} VKContext;

extern VKContext context;

#endif // TELEIOS_GRAPHICS_CONTEXT_H
