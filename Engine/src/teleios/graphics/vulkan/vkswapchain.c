#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkimage.h"
#include "teleios/graphics/vulkan/vkswapchain.h"
#include "teleios/graphics/vulkan/vktools.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

b8 tl_vulkan_swapchain_initialize(const TLSpecification* spec) {
    // =================================================
    // Surface Capabilities
    // =================================================
    VKCHECK("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.device.ph.handle, context.surface.handle, &context.surface.capabilities));
    // =================================================
    // Surface Formats
    // =================================================
    {
        VKCHECK("vkGetPhysicalDeviceSurfaceFormatsKHR", vkGetPhysicalDeviceSurfaceFormatsKHR(context.device.ph.handle, context.surface.handle, &context.surface.format_count, NULL));
        context.surface.formats = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, context.surface.format_count * sizeof(VkSurfaceFormatKHR));
        VKCHECK("vkGetPhysicalDeviceSurfaceFormatsKHR", vkGetPhysicalDeviceSurfaceFormatsKHR(context.device.ph.handle, context.surface.handle, &context.surface.format_count, context.surface.formats));

        context.swapchain.image_format.format = VK_FORMAT_UNDEFINED;
        context.swapchain.image_format.colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

        for (unsigned i = 0; i < context.surface.format_count; ++i) {
            VkSurfaceFormatKHR format = context.surface.formats[i];
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                context.swapchain.image_format = format;
                break;
            }
        }

        if (context.swapchain.image_format.format == VK_FORMAT_UNDEFINED) {
            TLERROR("tl_vulkan_swapchain_initialize: Unsupported format VK_FORMAT_B8G8R8A8_UNORM + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
            return false;
        }
    }
    // =================================================
    // Surface Present Modes
    // =================================================
    {
        VKCHECK("vkGetPhysicalDeviceSurfacePresentModesKHR", vkGetPhysicalDeviceSurfacePresentModesKHR(context.device.ph.handle, context.surface.handle, &context.surface.present_mode_count, NULL));
        context.surface.present_modes = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, context.surface.present_mode_count * sizeof(VkPresentModeKHR));
        VKCHECK("vkGetPhysicalDeviceSurfacePresentModesKHR", vkGetPhysicalDeviceSurfacePresentModesKHR(context.device.ph.handle, context.surface.handle, &context.surface.present_mode_count, context.surface.present_modes));

        context.swapchain.present_mode = VK_PRESENT_MODE_MAX_ENUM_KHR;
        for (unsigned i = 0; i < context.surface.present_mode_count; ++i) {
            VkPresentModeKHR present_mode = context.surface.present_modes[i];
            if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                context.swapchain.present_mode = present_mode;
                break;
            }
        }

        if (context.swapchain.present_mode == VK_PRESENT_MODE_MAX_ENUM_KHR) {
            context.swapchain.present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }
    // =================================================
    // Image Extent
    // =================================================
    context.swapchain.image_extent = context.surface.capabilities.currentExtent;
    // =================================================
    // Image Count
    // =================================================
    {
        context.swapchain.image_count = context.surface.capabilities.minImageCount + 1;
        if (context.surface.capabilities.maxImageCount > 0 && context.swapchain.image_count > context.surface.capabilities.maxImageCount) {
            context.swapchain.image_count = context.surface.capabilities.maxImageCount;
        }

        context.swapchain.frames_in_flight = context.swapchain.image_count - 1;
    }
    // =================================================
    // Create the Swapchain
    // =================================================
    VkSwapchainCreateInfoKHR swapchain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapchain_create_info.surface = context.surface.handle;
    swapchain_create_info.minImageCount = context.swapchain.image_count;
    swapchain_create_info.imageFormat = context.swapchain.image_format.format;
    swapchain_create_info.imageColorSpace = context.swapchain.image_format.colorSpace;
    swapchain_create_info.imageExtent = context.swapchain.image_extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (context.device.ph.q_graphics != context.device.ph.q_present) {
        u32 indexes[2] = { context.device.ph.q_graphics, context.device.ph.q_present };
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = indexes;
    }

    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.preTransform = context.surface.capabilities.currentTransform;
    swapchain_create_info.presentMode = context.swapchain.present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;

    VKCHECK("vkCreateSwapchainKHR", vkCreateSwapchainKHR(context.device.lo.handle, &swapchain_create_info, context.allocator, &context.swapchain.handle));
    // =================================================
    // Acquire Swapchain Images
    // =================================================
    VKCHECK("vkGetSwapchainImagesKHR", vkGetSwapchainImagesKHR(context.device.lo.handle, context.swapchain.handle, &context.swapchain.images_count, NULL));
    context.swapchain.images = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, context.swapchain.images_count * sizeof(VkImage));
    VKCHECK("vkGetSwapchainImagesKHR", vkGetSwapchainImagesKHR(context.device.lo.handle, context.swapchain.handle, &context.swapchain.images_count, context.swapchain.images));
    // =================================================
    // Create Swapchain Image Views
    // =================================================
    context.swapchain.images_view = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, context.swapchain.images_count * sizeof(VkImageView));
    for (unsigned i = 0; i < context.swapchain.images_count; ++i) {
        VkImageViewCreateInfo image_view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        image_view_create_info.image = context.swapchain.images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = context.swapchain.image_format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        VKCHECK("vkCreateImageView", vkCreateImageView(context.device.lo.handle, &image_view_create_info, context.allocator, &context.swapchain.images_view[i]));
    }
    // =================================================
    // Detect Swapchain Depth Format
    // =================================================
    {
        const u64 candidate_count = 3;
        VkFormat candidate[3] = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };

        context.swapchain.depth_format = VK_FORMAT_UNDEFINED;
        u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
        for (unsigned i = 0; i < candidate_count; ++i) {
            VkFormatProperties properties;
            vkGetPhysicalDeviceFormatProperties(context.device.ph.handle, candidate[i], &properties);

            if ((properties.linearTilingFeatures & flags) == flags) {
                context.swapchain.depth_format = candidate[i];
                TLDEBUG("Vulkan Device Depth Format: %s", i == 0 ? "VK_FORMAT_D32_SFLOAT" : (i == 1 ? "VK_FORMAT_D32_SFLOAT_S8_UINT" : "VK_FORMAT_D24_UNORM_S8_UINT"));
                break;
            }

            if ((properties.optimalTilingFeatures & flags) == flags) {
                context.swapchain.depth_format = candidate[i];
                TLDEBUG("Vulkan Device Depth Format: %s", i == 0 ? "VK_FORMAT_D32_SFLOAT" : (i == 1 ? "VK_FORMAT_D32_SFLOAT_S8_UINT" : "VK_FORMAT_D24_UNORM_S8_UINT"));
                break;
            }
        }

        if (context.swapchain.depth_format == VK_FORMAT_UNDEFINED) {
            TLERROR("tl_vulkan_swapchain_initialize: No suitable Depth Format");
            return false;
        }
    }
    // =================================================
    // Create Swapchain Depth Buffer
    // =================================================
    {
        TLVKImageCreateInfo info = { 0 };
        info.type = VK_IMAGE_TYPE_2D;
        info.extent.width = context.swapchain.image_extent.width;
        info.extent.height = context.swapchain.image_extent.height;
        info.extent.depth = 1;
        info.format = context.swapchain.depth_format;
        info.tilling = VK_IMAGE_TILING_OPTIMAL;
        info.usage_flags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        info.memory_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        info.create_view = true;
        info.aspect_flags = VK_IMAGE_ASPECT_DEPTH_BIT;

        context.swapchain.depth = tl_vulkan_image_create(&info);
        if (context.swapchain.depth == NULL) {
            TLERROR("tl_vulkan_swapchain_initialize: Failed to create depth buffer");
            return false;
        }
    }

    return true;
}

b8 tl_vulkan_swapchain_terminate(void) {
    if (context.surface.formats != NULL) {
        tl_memory_free(context.surface.formats, TL_MEMORY_TYPE_GRAPHICS, context.surface.format_count * sizeof(VkSurfaceFormatKHR));
        context.surface.formats = NULL;
        context.surface.format_count = 0;
    }

    if (context.surface.present_modes != NULL) {
        tl_memory_free(context.surface.present_modes, TL_MEMORY_TYPE_GRAPHICS, context.surface.present_mode_count * sizeof(VkPresentModeKHR));
        context.surface.present_modes = NULL;
        context.surface.present_mode_count = 0;
    }

    if (context.swapchain.images_view != NULL) {
        for (unsigned i = 0; i < context.swapchain.images_count; ++i) {
            if (context.swapchain.images_view[i] != VK_NULL_HANDLE) {
                vkDestroyImageView(context.device.lo.handle, context.swapchain.images_view[i], context.allocator);
            }
        }

        tl_memory_free(context.swapchain.images_view, TL_MEMORY_TYPE_GRAPHICS, context.swapchain.images_count * sizeof(VkImageView));
        context.swapchain.images_view = NULL;
    }

    if (context.swapchain.images != NULL) {
        tl_memory_free(context.swapchain.images, TL_MEMORY_TYPE_GRAPHICS, context.swapchain.images_count * sizeof(VkImage));
        context.swapchain.images = NULL;
        context.swapchain.images_count = 0;
    }

    if (context.swapchain.depth != NULL) {
        tl_vulkan_image_destroy(context.swapchain.depth);
        context.swapchain.depth = NULL;
    }

    if (context.swapchain.handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(context.device.lo.handle, context.swapchain.handle, context.allocator);
    }

    return true;
}