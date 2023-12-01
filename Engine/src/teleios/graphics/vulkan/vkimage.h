#ifndef TELELIOS_GRAPHICS_VULKAN_IMAGE_H
#define TELELIOS_GRAPHICS_VULKAN_IMAGE_H

#include "teleios/graphics/vulkan/vkcontext.h"

typedef struct {
    VkImageType type;
    VkExtent3D extent;
    VkFormat format;
    VkImageTiling tilling;
    VkImageUsageFlags usage_flags;
    VkMemoryPropertyFlags memory_flags;
    b8 create_view;
    VkImageAspectFlags aspect_flags;
} TLVKImageCreateInfo;

void tl_vulkan_image_create_view(VkFormat format, VkImageAspectFlags aspect_flags, VKImage* image);
VKImage* tl_vulkan_image_create(TLVKImageCreateInfo* info);
void tl_vulkan_image_destroy(VKImage* image);

#endif // TELELIOS_GRAPHICS_VULKAN_IMAGE_H
