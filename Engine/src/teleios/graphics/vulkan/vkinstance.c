#include "teleios/container.h"
#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkinstance.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/platform/detector.h"
#include "teleios/string.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#   include <Windows.h>
#   include <vulkan/vulkan_win32.h>
#endif

VKAPI_ATTR VkBool32 VKAPI_CALL
vkdebug(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data)
{
    switch (message_severity) {
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: TLERROR("%s", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: TLWARN("%s", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: TLINFO("%s", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: TLTRACE("%s", callback_data->pMessage); break;
    }

    return VK_FALSE;
}

b8 tl_vulkan_instance_initialize(const TLSpecification* spec) {
    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    {
        u32 api_version = 0;
        VKCHECK("vkEnumerateInstanceVersion", vkEnumerateInstanceVersion(&api_version));
        TLDEBUG("Vulkan Instance %u.%u.%u", VK_VERSION_MAJOR(api_version), VK_VERSION_MINOR(api_version), VK_VERSION_PATCH(api_version));
        app_info.apiVersion = api_version;
        app_info.pApplicationName = "";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Teleios Engine";
        app_info.engineVersion = VK_MAKE_VERSION(0, 4, 0);

        create_info.pApplicationInfo = &app_info;
    }

    // =================================================
    // Extentions
    // =================================================
    const char** ppEnabledExtensionNames = NULL;
    {
        context.extentions = tl_list_create();
        tl_list_append(context.extentions, &VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(TELEIOS_DEBUG) || defined(TELEIOS_DEBUG_GRAPHICS)
        tl_list_append(context.extentions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef TELEIOS_PLATFORM_WINDOWS
        tl_list_append(context.extentions, VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

        if (!tl_list_append_all(context.extentions, spec->vulkan.instance.extentions)) {
            TLERROR("tl_vulkan_instance_initialize: Failed to merge spec into final required instance extentions");
            return false;
        }

        u32 available = 0;
        VKCHECK("vkEnumerateInstanceExtensionProperties", vkEnumerateInstanceExtensionProperties(0, &available, 0));
        VkExtensionProperties* extensions = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, available * sizeof(VkExtensionProperties));
        VKCHECK("vkEnumerateInstanceExtensionProperties", vkEnumerateInstanceExtensionProperties(0, &available, extensions));

        create_info.enabledExtensionCount = context.extentions->size;
        ppEnabledExtensionNames = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, create_info.enabledExtensionCount * sizeof(char*));

        u8 index = 0;
        b8 not_found = false;
        TLNode* current = context.extentions->head;
        while (current != NULL) {
            const char* required = current->payload;
            ppEnabledExtensionNames[index++] = required;

            b8 found = false;
            for (unsigned i = 0; i < available; ++i) {
                if (tl_string_equals(required, extensions[i].extensionName)) {
                    TLDEBUG("Vulkan Instance Extention: %s", required);
                    found = true;
                    break;
                }
            }

            if (!found) {
                TLERROR("tl_vulkan_instance_initialize: Required extention \"%s\" not found.", required);
                not_found = true;
            }

            current = current->next;
        }

        tl_memory_free(extensions, TL_MEMORY_TYPE_GRAPHICS, available * sizeof(VkExtensionProperties));
        extensions = NULL;

        if (not_found) {
            tl_memory_free(ppEnabledExtensionNames, TL_MEMORY_TYPE_GRAPHICS, create_info.enabledExtensionCount * sizeof(char*));
            ppEnabledExtensionNames = NULL;
            return false;
        }
    }
    // =================================================
    // Layers
    // =================================================
    const char** ppEnabledLayerNames = NULL;
    {
        context.layers = tl_list_create();
#if defined(TELEIOS_DEBUG) || defined(TELEIOS_DEBUG_GRAPHICS)
        tl_list_append(context.layers, "VK_LAYER_KHRONOS_validation");
#endif

        if (!tl_list_append_all(context.layers, spec->vulkan.instance.layers)) {
            TLERROR("tl_vulkan_instance_initialize: Failed to merge spec into final required instance layers");
            return false;
        }

        if (context.layers->size > 0) {
            create_info.enabledLayerCount = context.layers->size;
            ppEnabledLayerNames = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, create_info.enabledLayerCount * sizeof(char*));

            u32 available = 0;
            VKCHECK("vkEnumerateInstanceLayerProperties", vkEnumerateInstanceLayerProperties(&available, 0));
            VkLayerProperties* layers = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, available * sizeof(VkLayerProperties));
            VKCHECK("vkEnumerateInstanceLayerProperties", vkEnumerateInstanceLayerProperties(&available, layers));

            u8 index = 0;
            b8 not_found = false;
            TLNode* current = context.layers->head;
            while (current != NULL) {

                b8 found = false;
                const char* required = current->payload;
                ppEnabledLayerNames[index++] = required;
                for (unsigned i = 0; i < available; ++i) {
                    if (tl_string_equals(required, layers[i].layerName)) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    TLERROR("tl_vulkan_instance_initialize: Required layer \"%s\" not found.", required);
                    not_found = true;
                }

                TLDEBUG("Vulkan Instance Layer: %s", required);
                current = current->next;
            }

            tl_memory_free(layers, TL_MEMORY_TYPE_GRAPHICS, available * sizeof(VkLayerProperties));
            layers = NULL;

            if (not_found) {
                tl_memory_free(ppEnabledExtensionNames, TL_MEMORY_TYPE_GRAPHICS, create_info.enabledExtensionCount * sizeof(char*));
                tl_memory_free(ppEnabledLayerNames, TL_MEMORY_TYPE_GRAPHICS, create_info.enabledLayerCount * sizeof(char*));
                return false;
            }
        }
    }
    // =================================================
    // Instance Creation
    // =================================================
    {
        create_info.ppEnabledLayerNames = ppEnabledLayerNames;
        create_info.ppEnabledExtensionNames = ppEnabledExtensionNames;

        VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
        if (create_info.enabledExtensionCount > 0) {
            tl_memory_free(ppEnabledExtensionNames, TL_MEMORY_TYPE_GRAPHICS, create_info.enabledExtensionCount * sizeof(char*));
        }

        if (create_info.enabledLayerCount > 0) {
            tl_memory_free(ppEnabledLayerNames, TL_MEMORY_TYPE_GRAPHICS, create_info.enabledLayerCount * sizeof(char*));
        }

        if (result != VK_SUCCESS) {
            TLERROR("vkCreateInstance: Failed with %s", vkresult(result));
            return false;
        }
    }
    // =================================================
    // Instance Debug Messenger
    // =================================================
    {
#ifdef TELEIOS_DEBUG
        VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debug_create_info.pfnUserCallback = vkdebug;
        debug_create_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
        debug_create_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;

        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
        VKCHECK("vkCreateDebugUtilsMessengerEXT", func(context.instance, &debug_create_info, context.allocator, &context.messenger));
#endif
    }

    return true;
}

b8 tl_vulkan_instance_terminate(void) {
    if (context.extentions != NULL) {
        if (!tl_list_clear(context.extentions, tl_container_noop_dealocator)) {
            TLERROR("tl_vulkan_surface_initialize: Failed to clean context.extentions");
            return false;
        }

        if (!tl_list_destroy(context.extentions)) {
            TLERROR("tl_vulkan_surface_initialize: Failed to destroy context.extentions");
            return false;
        }
        context.extentions = NULL;
    }

    if (context.layers != NULL) {
        if (!tl_list_clear(context.layers, tl_container_noop_dealocator)) {
            TLERROR("tl_vulkan_surface_initialize: Failed to clean context.layers");
            return false;
        }

        if (!tl_list_destroy(context.layers)) {
            TLERROR("tl_vulkan_surface_initialize: Failed to destroy context.layers");
            return false;
        }
        context.layers = NULL;
    }

#ifdef TELEIOS_DEBUG
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
    func(context.instance, context.messenger, context.allocator);
#endif
    if (context.instance != VK_NULL_HANDLE)
        vkDestroyInstance(context.instance, context.allocator);

    return true;
}
