#ifndef TELEIOS_GRAPHICS_CONTEXT_H
#define TELEIOS_GRAPHICS_CONTEXT_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    VkInstance instance;
#ifdef TELEIOS_DEBUG
    VkDebugUtilsMessengerEXT messenger;
#endif
    VkAllocationCallbacks* allocator;
    TLList* extentions;
    TLList* layers;
    struct {
        VkSurfaceKHR handle;
        VkSurfaceCapabilitiesKHR capabilities;
        u32 format_count;
        VkSurfaceFormatKHR* formats;
        u32 present_mode_count;
        VkPresentModeKHR* present_modes;
    } surface;
    struct {
        struct {
            VkPhysicalDevice handle;

            VkPhysicalDeviceMemoryProperties memory;
            VkPhysicalDeviceFeatures features;
            VkPhysicalDeviceDriverProperties driver_properties;
            VkPhysicalDeviceProperties properties;
            TLList* extentions;

            u8 q_video;
            u8 q_compute;
            u8 q_graphics;
            u8 q_transfer;
            u8 q_present;
        } ph;
        struct {
            VkDevice handle;

            VkQueue q_video;
            VkQueue q_compute;
            VkQueue q_graphics;
            VkQueue q_transfer;
            VkQueue q_present;
        } lo;
    } device;
    struct {
        VkSwapchainKHR handle;
        VkPresentModeKHR present_mode;
        VkSurfaceFormatKHR image_format;
        VkExtent2D image_extent;
        u32 image_count;
        u32 frames_in_flight;

        u32 images_count;
        VkImage* images;
        VkImageView* images_view;
    } swapchain;
} VKContext;

extern VKContext context;

#endif // TELEIOS_GRAPHICS_CONTEXT_H
