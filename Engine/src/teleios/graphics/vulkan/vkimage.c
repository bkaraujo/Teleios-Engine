#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkimage.h"
#include "teleios/graphics/vulkan/vktools.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

void tl_vulkan_image_create_view(VkFormat format, VkImageAspectFlags aspect_flags, VKImage* image) {
    VkImageViewCreateInfo view_create_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    view_create_info.image = image->handle;
    view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_create_info.format = format;

    view_create_info.subresourceRange.aspectMask = aspect_flags;
    view_create_info.subresourceRange.levelCount = 1;
    view_create_info.subresourceRange.baseMipLevel = 0;
    view_create_info.subresourceRange.layerCount = 1;
    view_create_info.subresourceRange.baseArrayLayer = 0;

    VkResult result = vkCreateImageView(context.device.lo.handle, &view_create_info, context.allocator, &image->view);
    if (result != VK_SUCCESS) {
        TLERROR("tl_vulkan_image_create_view: vkCreateImageView failed with %s", vkresult(result));
        image->view = NULL;
    }
}

VKImage* tl_vulkan_image_create(TLVKImageCreateInfo* info) {
    VKImage* image = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
    image->extent = info->extent;
    image->format = info->format;
    image->tilling = info->tilling;

    VkImageCreateInfo image_create_info = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent = info->extent;
    image_create_info.mipLevels = 4;
    image_create_info.arrayLayers = 1;
    image_create_info.format = info->format;
    image_create_info.tiling = info->tilling;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = info->usage_flags;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(context.device.lo.handle, &image_create_info, context.allocator, &image->handle);
    if (result != VK_SUCCESS) {
        tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
        TLERROR("tl_vulkan_image_create: vkCreateImage failed with %s", vkresult(result));
        return NULL;
    }

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(context.device.lo.handle, image->handle, &requirements);
    u32 index = tl_vulkan_memory_index(requirements.memoryTypeBits, info->memory_flags);
    if (index == U32MAX) {
        tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
        TLERROR("tl_vulkan_image_create: Failed to find image memory index");
        return false;
    }

    VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    memory_allocate_info.allocationSize = requirements.size;
    memory_allocate_info.memoryTypeIndex = index;
    result = vkAllocateMemory(context.device.lo.handle, &memory_allocate_info, context.allocator, &image->memory);
    if (result != VK_SUCCESS) {
        tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
        TLERROR("tl_vulkan_image_create: vkAllocateMemory failed with %s", vkresult(result));
        return NULL;
    }

    result = vkBindImageMemory(context.device.lo.handle, image->handle, image->memory, 0);
    if (result != VK_SUCCESS) {
        tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
        vkFreeMemory(context.device.lo.handle, image->memory, context.allocator);
        TLERROR("tl_vulkan_image_create: vkBindImageMemory failed with %s", vkresult(result));
        return NULL;
    }

    if (info->create_view) {
        tl_vulkan_image_create_view(info->format, info->aspect_flags, image);
        if (image->view == NULL) {
            tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
            vkFreeMemory(context.device.lo.handle, image->memory, context.allocator);
            TLERROR("tl_vulkan_image_create: Failed to create Image View");
            return NULL;
        }
    }

    return image;
}

void tl_vulkan_image_destroy(VKImage* image) {
    if (image == NULL) return;

    if (image->view != NULL) {
        vkDestroyImageView(context.device.lo.handle, image->view, context.allocator);
        image->view = NULL;
    }

    if (image->memory != NULL) {
        vkFreeMemory(context.device.lo.handle, image->memory, context.allocator);
        image->memory = NULL;
    }

    if (image->handle != NULL) {
        vkDestroyImage(context.device.lo.handle, image->handle, context.allocator);
        image->handle = NULL;
    }

    tl_memory_free(image, TL_MEMORY_TYPE_GRAPHICS, sizeof(VKImage));
}