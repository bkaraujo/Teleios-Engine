#include "teleios/logger.h"
#include "teleios/renderer/vulkan/vkrenderpass.h"

b8 tl_vulkan_renderpass_initialize(const TLSpecification* spec) {

    // ===============================================
    // Attachment description
    // ===============================================
    VkAttachmentDescription colorAttachment = { 0 };
    colorAttachment.format = context.swapchain.format.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    // ===============================================
    // Subpasses and attachment references
    // ===============================================
    VkAttachmentReference colorAttachmentRef = { 0 };
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // ===============================================
    // Render pass
    // ===============================================
    return true;
}

void tl_vulkan_renderpass_begin(const VKCommandBuffer* command, const VkFramebuffer* frame) {

}

void tl_vulkan_renderpass_end(const VKCommandBuffer* command) {

}

void tl_vulkan_renderpass_submit(void) {

}


b8 tl_vulkan_renderpass_terminate(void) {
    return true;
}
