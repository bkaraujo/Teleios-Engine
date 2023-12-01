#ifndef TELEIOS_GRAPHICS_VULKAN_TOOLS_H
#define TELEIOS_GRAPHICS_VULKAN_TOOLS_H

#include "teleios/types.h"
#include "vulkan/vulkan_core.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#define VKCHECK(fname,function)                                 \
{                                                               \
    VkResult result = function;                                 \
    SetLastError(NO_ERROR);                                     \
    if (result != VK_SUCCESS) {                                 \
        TLERROR("%s: Failed with %s", fname, vkresult(result)); \
        return false;                                           \
    }                                                           \
}
#else
#define VKCHECK(fname,function)                                 \
{                                                               \
    VkResult result = function;                                 \
    if (result != VK_SUCCESS) {                                 \
        TLERROR("%s: Failed with %s", fname, vkresult(result)); \
        return false;                                           \
    }                                                           \
}
#endif

const char* vkresult(const VkResult result);
const char* vkformat(const VkFormat format);
const char* vkcolor(const VkColorSpaceKHR space);
const char* vkpresent(const VkPresentModeKHR mode);
const u32 tl_vulkan_memory_index(const u32 type, const u32 flags);
#endif // TELEIOS_GRAPHICS_VULKAN_TOOLS_H
