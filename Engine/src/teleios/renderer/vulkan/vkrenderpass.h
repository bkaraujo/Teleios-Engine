#ifndef TELEIOS_RENDERER_RENDERPASS_H
#define TELEIOS_RENDERER_RENDERPASS_H

#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/types.h"

b8 tl_vulkan_renderpass_initialize(const TLSpecification* spec);
b8 tl_vulkan_renderpass_terminate(void);

void tl_vulkan_renderpass_begin(const VKCommandBuffer* command, const VkFramebuffer* frame);
void tl_vulkan_renderpass_end(const VKCommandBuffer* command);
void tl_vulkan_renderpass_submit(void);

#endif // TELEIOS_RENDERER_RENDERPASS_H
