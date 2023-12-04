#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vksurface.h"
#include "teleios/logger.h"
#include "teleios/platform/detector.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#   include "teleios/platform/info.h"
#   include <Windows.h>
#   include <vulkan/vulkan_win32.h>
#endif

b8 tl_vulkan_surface_initialize(const TLSpecification* spec) {

#ifdef TELEIOS_PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hwnd = *((HWND*)tl_platform_window_handle());
    create_info.hinstance = *((HINSTANCE*)tl_platform_handle());
    VKCHECK("vkCreateWin32SurfaceKHR", vkCreateWin32SurfaceKHR(context.instance, &create_info, context.allocator, &context.surface.handle));
#endif
    return true;
}

b8 tl_vulkan_surface_terminate(void) {
    if (context.surface.handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(context.instance, context.surface.handle, context.allocator);
        context.surface.handle = VK_NULL_HANDLE;
    }

    return true;
}