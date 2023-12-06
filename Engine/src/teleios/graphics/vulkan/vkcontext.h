#ifndef TELEIOS_GRAPHICS_CONTEXT_H
#define TELEIOS_GRAPHICS_CONTEXT_H

#include "teleios/defines.h"
#include "teleios/types.h"
#include <vulkan/vulkan_core.h>

typedef struct {
    VkImage handle;
    VkDeviceMemory memory;
    VkFormat format;
    VkImageTiling tilling;
    VkExtent3D extent;
    VkImageView view;
} VKImage;

typedef enum {
    RENDERPASS_READY,
    RENDERPASS_RECORDING,
    RENDERPASS_IN_RENDER_PASS,
    RENDERPASS_REDORDING_ENDED,
    RENDERPASS_SUBMITTED,
    RENDERPASS_NOT_ALLOCATED
} VKRenderpassState;

typedef enum {
    COMMAND_BUFFER_READY,
    COMMAND_BUFFER_RECORDING,
    COMMAND_BUFFER_IN_RENDER_PASS,
    COMMAND_BUFFER_REDORDING_ENDED,
    COMMAND_BUFFER_SUBMITTED,
    COMMAND_BUFFER_NOT_ALLOCATED
} VKCommandState;

typedef struct {
    VkCommandBuffer handle;
    VKCommandState state;
} VKCommandBuffer;

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

            u8 video_q;
            u8 compute_q;
            u8 graphics_q;
            u8 transfer_q;
            u8 present_q;
        } ph;
        struct {
            VkDevice handle;

            VkQueue video_q;
            VkCommandPool video_cp;
            VkQueue compute_q;
            VkCommandPool compute_cp;
            VkQueue graphics_q;
            VkCommandPool graphics_cp;
            VkQueue transfer_q;
            VkCommandPool transfer_cp;
            VkQueue present_q;
            VkCommandPool present_cp;
        } lo;
    } device;

    struct {
        VkRenderPass handle;
        VKRenderpassState state;
        vec4s area;
        vec4s color;
        f32 depth;
        u32 stencil;
    } renderpass;

    struct {
        VkSwapchainKHR handle;
        VkPresentModeKHR present_mode;
        VkSurfaceFormatKHR format;
        VkExtent2D extent;
        u32 image_count;
        u32 frames_in_flight;

        VkFormat depth_format;
        VKImage* depth;

        u32 images_count;
        VkImage* images;
        VkImageView* images_view;
    } swapchain;

    VkPipelineLayout layout;
} VKContext;

extern VKContext context;

#include "teleios/graphics/vulkan/vktools.h"

#endif // TELEIOS_GRAPHICS_CONTEXT_H
