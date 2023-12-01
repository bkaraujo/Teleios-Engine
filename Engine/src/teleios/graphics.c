#include "teleios/container.h"
#include "teleios/graphics/context.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/platform/detector.h"
#include "teleios/string.h"
#include <vulkan/vulkan.h>

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

static VKContext context = { 0 };
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/graphics/manager.h"

static b8 vkinstance_initialize(const TLSpecification* spec);
static b8 vksurface_initialize(const TLSpecification* spec);
static b8 vkdevice_initialize(const TLSpecification* spec);
static b8 vkswapchain_initialize(const TLSpecification* spec);

b8 tl_graphics_initialize(const TLSpecification* spec) {
    tl_memory_zero(&context, sizeof(VKContext));

    if (!vkinstance_initialize(spec)) return false;
    if (!vksurface_initialize(spec)) return false;
    if (!vkdevice_initialize(spec)) return false;
    if (!vkswapchain_initialize(spec)) return false;

    return true;
}

static b8 vkinstance_terminate(void);
static b8 vksurface_terminate(void);
static b8 vkdevice_terminate(void);
static b8 vkswapchain_terminate(void);

b8 tl_graphics_terminate(void) {
    if (!vkswapchain_terminate()) return false;
    if (!vkdevice_terminate()) return false;
    if (!vksurface_terminate()) return false;
    if (!vkinstance_terminate()) return false;

    return true;
}

// ##############################################################################################
//
//                                        PRIMITIVE
//
// ##############################################################################################
#include "teleios/graphics/primitive.h"


// ##############################################################################################
//
//                                        HELPERS
//
// ##############################################################################################
static const char* vkresult(VkResult result);
static const char* vkformat(VkFormat format);
static const char* vkcolor(VkColorSpaceKHR space);
static const char* vkpresent(VkPresentModeKHR mode);

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
// ##############################################################################################
//
//                                        VULKAN INSTANCE
//
// ##############################################################################################

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
static b8 vkinstance_initialize(const TLSpecification* spec) {
    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    {
        u32 api_version = 0;
        VKCHECK("vkEnumerateInstanceVersion", vkEnumerateInstanceVersion(&api_version));

        app_info.apiVersion = VK_MAKE_API_VERSION(0, VK_VERSION_MAJOR(api_version), VK_VERSION_MINOR(api_version), VK_VERSION_PATCH(api_version));
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
            TLERROR("vkinstance_initialize: Failed to merge spec into final required instance extentions");
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
                    found = true;
                    break;
                }
            }

            if (!found) {
                TLERROR("vkinstance_initialize: Required extention \"%s\" not found.", required);
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
            TLERROR("vkinstance_initialize: Failed to merge spec into final required instance layers");
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
                    TLERROR("vkinstance_initialize: Required layer \"%s\" not found.", required);
                    not_found = true;
                }

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

static b8 vkinstance_terminate(void) {
    if (context.extentions != NULL) {
        if (!tl_list_clear(context.extentions, tl_container_noop_dealocator)) {
            TLERROR("vksurface_initialize: Failed to clean context.extentions");
            return false;
        }

        if (!tl_list_destroy(context.extentions)) {
            TLERROR("vksurface_initialize: Failed to destroy context.extentions");
            return false;
        }
        context.extentions = NULL;
    }

    if (context.layers != NULL) {
        if (!tl_list_clear(context.layers, tl_container_noop_dealocator)) {
            TLERROR("vksurface_initialize: Failed to clean context.layers");
            return false;
        }

        if (!tl_list_destroy(context.layers)) {
            TLERROR("vksurface_initialize: Failed to destroy context.layers");
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
// ##############################################################################################
//
//                                        VULKAN SURFACE
//
// ##############################################################################################
#ifdef TELEIOS_PLATFORM_WINDOWS
#include "teleios/platform/info.h"
#endif

static b8 vksurface_initialize(const TLSpecification* spec) {

#ifdef TELEIOS_PLATFORM_WINDOWS
    VkWin32SurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hwnd = *((HWND*)tl_platform_window_handle());
    create_info.hinstance = *((HINSTANCE*)tl_platform_handle());
    VKCHECK("vkCreateWin32SurfaceKHR", vkCreateWin32SurfaceKHR(context.instance, &create_info, context.allocator, &context.surface.handle));
#endif
    return true;
}

static b8 vksurface_terminate(void) {
    if (context.surface.handle != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(context.instance, context.surface.handle, context.allocator);
        context.surface.handle = VK_NULL_HANDLE;
    }

    return true;
}

// ##############################################################################################
//
//                                        VULKAN DEVICE
//
// ##############################################################################################
static b8 vkdevice_physical_select(const TLSpecification* spec);
static b8 vkdevice_logical_create(const TLSpecification* spec);

static b8 vkdevice_initialize(const TLSpecification* spec) {
    if (!vkdevice_physical_select(spec)) {
        TLERROR("vkdevice_initialize: Failed to select VkPhysicalDevice");
        return false;
    }

    TLTRACE("vkdevice_initialize: Device %s", context.device.ph.properties.deviceName);
    TLTRACE("vkdevice_initialize: Driver %s %s", context.device.ph.driver_properties.driverName, context.device.ph.driver_properties.driverInfo);

    if (!vkdevice_logical_create(spec)) {
        TLERROR("vkdevice_initialize: Failed to create VkDevice");
        return false;
    }

    return true;
}

static b8 vkdevice_physical_select(const TLSpecification* spec) {
    {
        u32 avaliable = 0;
        VKCHECK("vkEnumeratePhysicalDevices", vkEnumeratePhysicalDevices(context.instance, &avaliable, NULL));
        if (avaliable == 0) {
            TLERROR("vkdevice_physical_select: No VkPhysicalDevice found");
            return false;
        }

        VkPhysicalDevice* devices = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkPhysicalDevice));
        VkResult result = vkEnumeratePhysicalDevices(context.instance, &avaliable, devices);
        if (result != VK_SUCCESS) {
            TLERROR("vkEnumerateDeviceExtensionProperties: Failed with: %s", vkresult(result));
            tl_memory_free(devices, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkPhysicalDevice));
            return false;
        }
        // =================================================
        // Select the device with most memory
        // =================================================
        context.device.ph.handle = devices[0];
        vkGetPhysicalDeviceMemoryProperties(context.device.ph.handle, &context.device.ph.memory);

        f64 local_memory = 0;
        for (unsigned j = 0; j < context.device.ph.memory.memoryHeapCount; ++j) {
            if (context.device.ph.memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                local_memory = GiB(context.device.ph.memory.memoryHeaps[j].size);
                break;
            }
        }

        for (unsigned i = 1; i < avaliable; ++i) {
            VkPhysicalDevice device = devices[i];
            VkPhysicalDeviceMemoryProperties memory = { 0 };
            vkGetPhysicalDeviceMemoryProperties(device, &memory);

            for (unsigned j = 0; j < memory.memoryHeapCount; ++j) {
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    if (GiB(memory.memoryHeaps[j].size) > local_memory) {

                        context.device.ph.handle = devices[i];
                        context.device.ph.memory = memory;
                        break;
                    }
                }
            }
        }

        tl_memory_free(devices, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkPhysicalDevice));
    }
    // =================================================
    // Load the selected device's properties
    // =================================================
    {
        VkPhysicalDeviceProperties2 properties2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2 };
        VkPhysicalDeviceDriverProperties driverProperties = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES };
        properties2.pNext = &driverProperties;
        vkGetPhysicalDeviceProperties2(context.device.ph.handle, &properties2);
        tl_memory_copy(&properties2.properties, &context.device.ph.properties, sizeof(VkPhysicalDeviceProperties));
        tl_memory_copy(&driverProperties, &context.device.ph.driver_properties, sizeof(VkPhysicalDeviceDriverProperties));
    }
    // =================================================
    // Load the selected device's features
    // =================================================
    {
        vkGetPhysicalDeviceFeatures(context.device.ph.handle, &context.device.ph.features);
    }
    // =================================================
    // Load the selected device's supported extentions
    // =================================================
    {
        u32 avaliable = 0;
        VKCHECK("vkEnumerateDeviceExtensionProperties", vkEnumerateDeviceExtensionProperties(context.device.ph.handle, NULL, &avaliable, NULL));
        VkExtensionProperties* extension_properties = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkExtensionProperties));
        VkResult result = vkEnumerateDeviceExtensionProperties(context.device.ph.handle, NULL, &avaliable, extension_properties);
        if (result != VK_SUCCESS) {
            TLERROR("vkEnumerateDeviceExtensionProperties: Failed with: %s", vkresult(result));
            tl_memory_free(extension_properties, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkExtensionProperties));
            return false;
        }

        tl_list_append(spec->vulkan.device.extentions, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        TLNode* current = spec->vulkan.device.extentions->head;
        while (current != NULL) {
            b8 found = false;
            for (unsigned i = 0; i < avaliable; ++i) {
                if (tl_string_equals(current->payload, extension_properties[i].extensionName)) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                TLERROR("vkdevice_physical_select: Device extention %s not supported.", current->payload);
                tl_memory_free(extension_properties, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkExtensionProperties));
                return false;
            }

            current = current->next;
        }

        context.device.ph.extentions = tl_list_create();
        for (unsigned i = 0; i < avaliable; ++i) {
            tl_list_append(context.device.ph.extentions, extension_properties[i].extensionName);
        }

        tl_memory_free(extension_properties, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkExtensionProperties));
    }
    // =================================================
    // Load the selected device's queue details
    // =================================================
    {
        u8 video = U8MAX;
        u8 compute = U8MAX;
        u8 graphics = U8MAX;
        u8 transfer = U8MAX;
        u8 present = U8MAX;

        u32 avaliable = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.ph.handle, &avaliable, 0);
        VkQueueFamilyProperties* queue_family_properties = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.ph.handle, &avaliable, queue_family_properties);
        for (unsigned i = 0; i < avaliable; ++i) {
            b8 is_video = queue_family_properties[i].queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR;
            b8 is_graphics = queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            b8 is_transfer = queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
            b8 is_compute = queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

            if (is_video) {
                if (video == U8MAX) video = i;
            }

            if (is_graphics) {
                if (graphics == U8MAX) graphics = i;
                if (graphics != U8MAX && !is_video && !is_compute && !is_transfer) graphics = i;
            }

            if (is_transfer && !is_graphics) {
                if (transfer == U8MAX) transfer = i;
                if (transfer != U8MAX && !is_graphics && !is_video && !is_compute)
                    if (queue_family_properties[i].queueCount > queue_family_properties[transfer].queueCount)
                        transfer = i;
            }

            if (is_compute && !is_graphics) {
                if (compute == U8MAX) compute = i;
                if (compute != U8MAX && !is_graphics && !is_video && !is_transfer)
                    if (queue_family_properties[i].queueCount > queue_family_properties[compute].queueCount)
                        compute = i;
            }
        }

        tl_memory_free(queue_family_properties, TL_MEMORY_TYPE_GRAPHICS, avaliable * sizeof(VkQueueFamilyProperties));
        // =================================================
        // Select device's presentation queue
        // =================================================
        if (present == U8MAX) {
            VkBool32 supports_present = VK_FALSE;
            VKCHECK("vkGetPhysicalDeviceSurfaceSupportKHR", vkGetPhysicalDeviceSurfaceSupportKHR(context.device.ph.handle, compute, context.surface.handle, &supports_present));
            present = supports_present ? compute : U8MAX;
        }

        if (present == U8MAX) {
            VkBool32 supports_present = VK_FALSE;
            VKCHECK("vkGetPhysicalDeviceSurfaceSupportKHR", vkGetPhysicalDeviceSurfaceSupportKHR(context.device.ph.handle, transfer, context.surface.handle, &supports_present));
            present = supports_present ? compute : U8MAX;
        }
        // =================================================
        // Store device's selected index for future use
        // =================================================
        context.device.ph.q_video = video;
        context.device.ph.q_compute = compute;
        context.device.ph.q_graphics = graphics;
        context.device.ph.q_transfer = transfer;
        context.device.ph.q_present = present;
    }

    return true;
}

static b8 vkdevice_logical_create(const TLSpecification* spec) {
    VkDeviceCreateInfo device_create_info = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    device_create_info.queueCreateInfoCount = 0;
    // =================================================
    // VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO
    // =================================================]
    {
        u32 count;
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.ph.handle, &count, 0);
        VkQueueFamilyProperties* props = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, count * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(context.device.ph.handle, &count, props);

        f32 queue_priorities[2] = { 0.9f, 1.0f };
        if (context.device.ph.q_graphics != U8MAX) device_create_info.queueCreateInfoCount++;
        if (context.device.ph.q_video != U8MAX) device_create_info.queueCreateInfoCount++;
        if (context.device.ph.q_transfer != U8MAX) device_create_info.queueCreateInfoCount++;
        if (context.device.ph.q_compute != U8MAX) device_create_info.queueCreateInfoCount++;

        VkDeviceQueueCreateInfo* queue_create_infos = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, device_create_info.queueCreateInfoCount * sizeof(VkDeviceQueueCreateInfo));

        u8 index = 0;
        if (context.device.ph.q_graphics != U8MAX) {
            queue_create_infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[index].queueFamilyIndex = context.device.ph.q_graphics;
            queue_create_infos[index].pQueuePriorities = queue_priorities;
            queue_create_infos[index].queueCount = 1;
            index++;
        }

        if (context.device.ph.q_video != U8MAX) {
            queue_create_infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[index].queueFamilyIndex = context.device.ph.q_video;
            queue_create_infos[index].pQueuePriorities = queue_priorities;
            queue_create_infos[index].queueCount = 1;
            index++;
        }

        if (context.device.ph.q_transfer != U8MAX) {
            queue_create_infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[index].queueFamilyIndex = context.device.ph.q_transfer;
            queue_create_infos[index].pQueuePriorities = queue_priorities;
            queue_create_infos[index].queueCount = 1;
            if (context.device.ph.q_transfer == context.device.ph.q_present) {
                if (props[context.device.ph.q_transfer].queueCount > 1) {
                    queue_create_infos[index].queueCount = 2;
                }
            }
            index++;
        }

        if (context.device.ph.q_transfer != U8MAX) {
            queue_create_infos[index].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_infos[index].queueFamilyIndex = context.device.ph.q_compute;
            queue_create_infos[index].pQueuePriorities = queue_priorities;
            queue_create_infos[index].queueCount = 1;
            if (context.device.ph.q_compute == context.device.ph.q_present) {
                if (props[context.device.ph.q_compute].queueCount > 1) {
                    queue_create_infos[index].queueCount = 2;
                }
            }
        }
        tl_memory_free(props, TL_MEMORY_TYPE_GRAPHICS, count * sizeof(VkQueueFamilyProperties));
        device_create_info.pQueueCreateInfos = queue_create_infos;
    }
    // =================================================
    // Device Features
    // =================================================
    device_create_info.pEnabledFeatures = NULL;
    // =================================================
    // Device Extentions
    // =================================================
    {
        const char** extension_names = NULL;
        if (spec->vulkan.device.extentions->size > 0) {
            device_create_info.enabledExtensionCount = spec->vulkan.device.extentions->size;
            extension_names = tl_memory_alloc(TL_MEMORY_TYPE_STRING, device_create_info.enabledExtensionCount * sizeof(char*));

            u32 index = 0;
            TLNode* current = spec->vulkan.device.extentions->head;
            while (current != NULL) {
                extension_names[index++] = current->payload;
                current = current->next;
            }
        }

        device_create_info.ppEnabledExtensionNames = extension_names;
    }
    // =================================================
    // Create the Logical Device
    // =================================================
    {
        VkResult result = vkCreateDevice(context.device.ph.handle, &device_create_info, context.allocator, &context.device.lo.handle);

        tl_memory_free(device_create_info.pQueueCreateInfos, TL_MEMORY_TYPE_GRAPHICS, device_create_info.queueCreateInfoCount * sizeof(VkDeviceQueueCreateInfo));
        if (device_create_info.enabledExtensionCount > 0) {
            tl_memory_free(device_create_info.ppEnabledExtensionNames, TL_MEMORY_TYPE_STRING, device_create_info.enabledExtensionCount * sizeof(char*));
        }

        if (result != VK_SUCCESS) {
            TLERROR("vkCreateDevice: Failed with: %s", vkresult(result));
            return false;
        }
    }
    // =================================================
    // Retrieve Logical Device's Queue Handle
    // =================================================
    {
        context.device.lo.q_video = VK_NULL_HANDLE;
        context.device.lo.q_graphics = VK_NULL_HANDLE;
        context.device.lo.q_transfer = VK_NULL_HANDLE;
        context.device.lo.q_compute = VK_NULL_HANDLE;
        context.device.lo.q_present = VK_NULL_HANDLE;

        if (context.device.ph.q_video != U8MAX)
            vkGetDeviceQueue(context.device.lo.handle, context.device.ph.q_video, 0, &context.device.lo.q_video);

        if (context.device.ph.q_graphics != U8MAX)
            vkGetDeviceQueue(context.device.lo.handle, context.device.ph.q_graphics, 0, &context.device.lo.q_graphics);

        if (context.device.ph.q_transfer != U8MAX)
            vkGetDeviceQueue(context.device.lo.handle, context.device.ph.q_transfer, 0, &context.device.lo.q_transfer);

        if (context.device.ph.q_compute != U8MAX)
            vkGetDeviceQueue(context.device.lo.handle, context.device.ph.q_compute, 0, &context.device.lo.q_compute);

        vkGetDeviceQueue(context.device.lo.handle, context.device.ph.q_present, 1, &context.device.lo.q_present);
    }
    // =================================================
    // Create Queue's Command Pool
    // =================================================
    // TODO: Create Queue's Command Pool
    return true;
}

static b8 vkdevice_terminate(void) {
    if (context.device.ph.extentions != NULL) {
        tl_list_clear(context.device.ph.extentions, tl_container_noop_dealocator);
        tl_list_destroy(context.device.ph.extentions);
        context.device.ph.extentions = NULL;
    }

    tl_memory_zero(&context.device.ph, sizeof(context.device.ph));

    if (context.device.lo.handle != VK_NULL_HANDLE) {
        vkDestroyDevice(context.device.lo.handle, context.allocator);
        context.device.lo.handle = VK_NULL_HANDLE;
    }

    tl_memory_zero(&context.device.lo, sizeof(context.device.lo));

    return true;
}

// ##############################################################################################
//
//                                        VULKAN SWAPCHAIN
//
// ##############################################################################################
static b8 vkswapchain_initialize(const TLSpecification* spec) {
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
            TLERROR("vkswapchain_initialize: Unsupported format VK_FORMAT_B8G8R8A8_UNORM + VK_COLOR_SPACE_SRGB_NONLINEAR_KHR");
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
                break;
            }

            if ((properties.optimalTilingFeatures & flags) == flags) {
                context.swapchain.depth_format = candidate[i];
                break;
            }
        }

        if (context.swapchain.depth_format == VK_FORMAT_UNDEFINED) {
            TLERROR("No suitable Depth Format");
            return false;
        }
    }
    return true;
}

static b8 vkswapchain_terminate(void) {
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

    if (context.swapchain.handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(context.device.lo.handle, context.swapchain.handle, context.allocator);
    }

    return true;
}

// ##############################################################################################
//
//                                              VULKAN STRINGS
//
// ##############################################################################################

static const char* vkresult(VkResult result) {
    switch (result) {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_EVENT_SET: return "VK_EVENT_SET";
    case VK_EVENT_RESET: return "VK_EVENT_RESET";
    case VK_INCOMPLETE: return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY(_KHR)";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE(_KHR)";
    case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION(_EXT)";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_(DEVICE|OPAQUE_CAPTURE)_ADDRESS";
    case VK_PIPELINE_COMPILE_REQUIRED: return "VK_(ERROR)_PIPELINE_COMPILE_REQUIRED(_EXT)";
    case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_NOT_PERMITTED_KHR: return "VK_ERROR_NOT_PERMITTED_(KHR|EXT)";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
    case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT: return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
    case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
    default: return "vkresult ????";
    }
}

static const char* vkformat(VkFormat format) {
    switch (format) {
    case VK_FORMAT_UNDEFINED: return "VK_FORMAT_UNDEFINED";
    case VK_FORMAT_R4G4_UNORM_PACK8: return "VK_FORMAT_R4G4_UNORM_PACK8";
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return "VK_FORMAT_R4G4B4A4_UNORM_PACK16";
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return "VK_FORMAT_B4G4R4A4_UNORM_PACK16";
    case VK_FORMAT_R5G6B5_UNORM_PACK16: return "VK_FORMAT_R5G6B5_UNORM_PACK16";
    case VK_FORMAT_B5G6R5_UNORM_PACK16: return "VK_FORMAT_B5G6R5_UNORM_PACK16";
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return "VK_FORMAT_R5G5B5A1_UNORM_PACK16";
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return "VK_FORMAT_B5G5R5A1_UNORM_PACK16";
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return "VK_FORMAT_A1R5G5B5_UNORM_PACK16";
    case VK_FORMAT_R8_UNORM: return "VK_FORMAT_R8_UNORM";
    case VK_FORMAT_R8_SNORM: return "VK_FORMAT_R8_SNORM";
    case VK_FORMAT_R8_USCALED: return "VK_FORMAT_R8_USCALED";
    case VK_FORMAT_R8_SSCALED: return "VK_FORMAT_R8_SSCALED";
    case VK_FORMAT_R8_UINT: return "VK_FORMAT_R8_UINT";
    case VK_FORMAT_R8_SINT: return "VK_FORMAT_R8_SINT";
    case VK_FORMAT_R8_SRGB: return "VK_FORMAT_R8_SRGB";
    case VK_FORMAT_R8G8_UNORM: return "VK_FORMAT_R8G8_UNORM";
    case VK_FORMAT_R8G8_SNORM: return "VK_FORMAT_R8G8_SNORM";
    case VK_FORMAT_R8G8_USCALED: return "VK_FORMAT_R8G8_USCALED";
    case VK_FORMAT_R8G8_SSCALED: return "VK_FORMAT_R8G8_SSCALED";
    case VK_FORMAT_R8G8_UINT: return "VK_FORMAT_R8G8_UINT";
    case VK_FORMAT_R8G8_SINT: return "VK_FORMAT_R8G8_SINT";
    case VK_FORMAT_R8G8_SRGB: return "VK_FORMAT_R8G8_SRGB";
    case VK_FORMAT_R8G8B8_UNORM: return "VK_FORMAT_R8G8B8_UNORM";
    case VK_FORMAT_R8G8B8_SNORM: return "VK_FORMAT_R8G8B8_SNORM";
    case VK_FORMAT_R8G8B8_USCALED: return "VK_FORMAT_R8G8B8_USCALED";
    case VK_FORMAT_R8G8B8_SSCALED: return "VK_FORMAT_R8G8B8_SSCALED";
    case VK_FORMAT_R8G8B8_UINT: return "VK_FORMAT_R8G8B8_UINT";
    case VK_FORMAT_R8G8B8_SINT: return "VK_FORMAT_R8G8B8_SINT";
    case VK_FORMAT_R8G8B8_SRGB: return "VK_FORMAT_R8G8B8_SRGB";
    case VK_FORMAT_B8G8R8_UNORM: return "VK_FORMAT_B8G8R8_UNORM";
    case VK_FORMAT_B8G8R8_SNORM: return "VK_FORMAT_B8G8R8_SNORM";
    case VK_FORMAT_B8G8R8_USCALED: return "VK_FORMAT_B8G8R8_USCALED";
    case VK_FORMAT_B8G8R8_SSCALED: return "VK_FORMAT_B8G8R8_SSCALED";
    case VK_FORMAT_B8G8R8_UINT: return "VK_FORMAT_B8G8R8_UINT";
    case VK_FORMAT_B8G8R8_SINT: return "VK_FORMAT_B8G8R8_SINT";
    case VK_FORMAT_B8G8R8_SRGB: return "VK_FORMAT_B8G8R8_SRGB";
    case VK_FORMAT_R8G8B8A8_UNORM: return "VK_FORMAT_R8G8B8A8_UNORM";
    case VK_FORMAT_R8G8B8A8_SNORM: return "VK_FORMAT_R8G8B8A8_SNORM";
    case VK_FORMAT_R8G8B8A8_USCALED: return "VK_FORMAT_R8G8B8A8_USCALED";
    case VK_FORMAT_R8G8B8A8_SSCALED: return "VK_FORMAT_R8G8B8A8_SSCALED";
    case VK_FORMAT_R8G8B8A8_UINT: return "VK_FORMAT_R8G8B8A8_UINT";
    case VK_FORMAT_R8G8B8A8_SINT: return "VK_FORMAT_R8G8B8A8_SINT";
    case VK_FORMAT_R8G8B8A8_SRGB: return "VK_FORMAT_R8G8B8A8_SRGB";
    case VK_FORMAT_B8G8R8A8_UNORM: return "VK_FORMAT_B8G8R8A8_UNORM";
    case VK_FORMAT_B8G8R8A8_SNORM: return "VK_FORMAT_B8G8R8A8_SNORM";
    case VK_FORMAT_B8G8R8A8_USCALED: return "VK_FORMAT_B8G8R8A8_USCALED";
    case VK_FORMAT_B8G8R8A8_SSCALED: return "VK_FORMAT_B8G8R8A8_SSCALED";
    case VK_FORMAT_B8G8R8A8_UINT: return "VK_FORMAT_B8G8R8A8_UINT";
    case VK_FORMAT_B8G8R8A8_SINT: return "VK_FORMAT_B8G8R8A8_SINT";
    case VK_FORMAT_B8G8R8A8_SRGB: return "VK_FORMAT_B8G8R8A8_SRGB";
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return "VK_FORMAT_A8B8G8R8_UNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return "VK_FORMAT_A8B8G8R8_SNORM_PACK32";
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return "VK_FORMAT_A8B8G8R8_USCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return "VK_FORMAT_A8B8G8R8_SSCALED_PACK32";
    case VK_FORMAT_A8B8G8R8_UINT_PACK32: return "VK_FORMAT_A8B8G8R8_UINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SINT_PACK32: return "VK_FORMAT_A8B8G8R8_SINT_PACK32";
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return "VK_FORMAT_A8B8G8R8_SRGB_PACK32";
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return "VK_FORMAT_A2R10G10B10_UNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return "VK_FORMAT_A2R10G10B10_SNORM_PACK32";
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return "VK_FORMAT_A2R10G10B10_USCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return "VK_FORMAT_A2R10G10B10_SSCALED_PACK32";
    case VK_FORMAT_A2R10G10B10_UINT_PACK32: return "VK_FORMAT_A2R10G10B10_UINT_PACK32";
    case VK_FORMAT_A2R10G10B10_SINT_PACK32: return "VK_FORMAT_A2R10G10B10_SINT_PACK32";
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return "VK_FORMAT_A2B10G10R10_UNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return "VK_FORMAT_A2B10G10R10_SNORM_PACK32";
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return "VK_FORMAT_A2B10G10R10_USCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return "VK_FORMAT_A2B10G10R10_SSCALED_PACK32";
    case VK_FORMAT_A2B10G10R10_UINT_PACK32: return "VK_FORMAT_A2B10G10R10_UINT_PACK32";
    case VK_FORMAT_A2B10G10R10_SINT_PACK32: return "VK_FORMAT_A2B10G10R10_SINT_PACK32";
    case VK_FORMAT_R16_UNORM: return "VK_FORMAT_R16_UNORM";
    case VK_FORMAT_R16_SNORM: return "VK_FORMAT_R16_SNORM";
    case VK_FORMAT_R16_USCALED: return "VK_FORMAT_R16_USCALED";
    case VK_FORMAT_R16_SSCALED: return "VK_FORMAT_R16_SSCALED";
    case VK_FORMAT_R16_UINT: return "VK_FORMAT_R16_UINT";
    case VK_FORMAT_R16_SINT: return "VK_FORMAT_R16_SINT";
    case VK_FORMAT_R16_SFLOAT: return "VK_FORMAT_R16_SFLOAT";
    case VK_FORMAT_R16G16_UNORM: return "VK_FORMAT_R16G16_UNORM";
    case VK_FORMAT_R16G16_SNORM: return "VK_FORMAT_R16G16_SNORM";
    case VK_FORMAT_R16G16_USCALED: return "VK_FORMAT_R16G16_USCALED";
    case VK_FORMAT_R16G16_SSCALED: return "VK_FORMAT_R16G16_SSCALED";
    case VK_FORMAT_R16G16_UINT: return "VK_FORMAT_R16G16_UINT";
    case VK_FORMAT_R16G16_SINT: return "VK_FORMAT_R16G16_SINT";
    case VK_FORMAT_R16G16_SFLOAT: return "VK_FORMAT_R16G16_SFLOAT";
    case VK_FORMAT_R16G16B16_UNORM: return "VK_FORMAT_R16G16B16_UNORM";
    case VK_FORMAT_R16G16B16_SNORM: return "VK_FORMAT_R16G16B16_SNORM";
    case VK_FORMAT_R16G16B16_USCALED: return "VK_FORMAT_R16G16B16_USCALED";
    case VK_FORMAT_R16G16B16_SSCALED: return "VK_FORMAT_R16G16B16_SSCALED";
    case VK_FORMAT_R16G16B16_UINT: return "VK_FORMAT_R16G16B16_UINT";
    case VK_FORMAT_R16G16B16_SINT: return "VK_FORMAT_R16G16B16_SINT";
    case VK_FORMAT_R16G16B16_SFLOAT: return "VK_FORMAT_R16G16B16_SFLOAT";
    case VK_FORMAT_R16G16B16A16_UNORM: return "VK_FORMAT_R16G16B16A16_UNORM";
    case VK_FORMAT_R16G16B16A16_SNORM: return "VK_FORMAT_R16G16B16A16_SNORM";
    case VK_FORMAT_R16G16B16A16_USCALED: return "VK_FORMAT_R16G16B16A16_USCALED";
    case VK_FORMAT_R16G16B16A16_SSCALED: return "VK_FORMAT_R16G16B16A16_SSCALED";
    case VK_FORMAT_R16G16B16A16_UINT: return "VK_FORMAT_R16G16B16A16_UINT";
    case VK_FORMAT_R16G16B16A16_SINT: return "VK_FORMAT_R16G16B16A16_SINT";
    case VK_FORMAT_R16G16B16A16_SFLOAT: return "VK_FORMAT_R16G16B16A16_SFLOAT";
    case VK_FORMAT_R32_UINT: return "VK_FORMAT_R32_UINT";
    case VK_FORMAT_R32_SINT: return "VK_FORMAT_R32_SINT";
    case VK_FORMAT_R32_SFLOAT: return "VK_FORMAT_R32_SFLOAT";
    case VK_FORMAT_R32G32_UINT: return "VK_FORMAT_R32G32_UINT";
    case VK_FORMAT_R32G32_SINT: return "VK_FORMAT_R32G32_SINT";
    case VK_FORMAT_R32G32_SFLOAT: return "VK_FORMAT_R32G32_SFLOAT";
    case VK_FORMAT_R32G32B32_UINT: return "VK_FORMAT_R32G32B32_UINT";
    case VK_FORMAT_R32G32B32_SINT: return "VK_FORMAT_R32G32B32_SINT";
    case VK_FORMAT_R32G32B32_SFLOAT: return "VK_FORMAT_R32G32B32_SFLOAT";
    case VK_FORMAT_R32G32B32A32_UINT: return "VK_FORMAT_R32G32B32A32_UINT";
    case VK_FORMAT_R32G32B32A32_SINT: return "VK_FORMAT_R32G32B32A32_SINT";
    case VK_FORMAT_R32G32B32A32_SFLOAT: return "VK_FORMAT_R32G32B32A32_SFLOAT";
    case VK_FORMAT_R64_UINT: return "VK_FORMAT_R64_UINT";
    case VK_FORMAT_R64_SINT: return "VK_FORMAT_R64_SINT";
    case VK_FORMAT_R64_SFLOAT: return "VK_FORMAT_R64_SFLOAT";
    case VK_FORMAT_R64G64_UINT: return "VK_FORMAT_R64G64_UINT";
    case VK_FORMAT_R64G64_SINT: return "VK_FORMAT_R64G64_SINT";
    case VK_FORMAT_R64G64_SFLOAT: return "VK_FORMAT_R64G64_SFLOAT";
    case VK_FORMAT_R64G64B64_UINT: return "VK_FORMAT_R64G64B64_UINT";
    case VK_FORMAT_R64G64B64_SINT: return "VK_FORMAT_R64G64B64_SINT";
    case VK_FORMAT_R64G64B64_SFLOAT: return "VK_FORMAT_R64G64B64_SFLOAT";
    case VK_FORMAT_R64G64B64A64_UINT: return "VK_FORMAT_R64G64B64A64_UINT";
    case VK_FORMAT_R64G64B64A64_SINT: return "VK_FORMAT_R64G64B64A64_SINT";
    case VK_FORMAT_R64G64B64A64_SFLOAT: return "VK_FORMAT_R64G64B64A64_SFLOAT";
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return "VK_FORMAT_B10G11R11_UFLOAT_PACK32";
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return "VK_FORMAT_E5B9G9R9_UFLOAT_PACK32";
    case VK_FORMAT_D16_UNORM: return "VK_FORMAT_D16_UNORM";
    case VK_FORMAT_X8_D24_UNORM_PACK32: return "VK_FORMAT_X8_D24_UNORM_PACK32";
    case VK_FORMAT_D32_SFLOAT: return "VK_FORMAT_D32_SFLOAT";
    case VK_FORMAT_S8_UINT: return "VK_FORMAT_S8_UINT";
    case VK_FORMAT_D16_UNORM_S8_UINT: return "VK_FORMAT_D16_UNORM_S8_UINT";
    case VK_FORMAT_D24_UNORM_S8_UINT: return "VK_FORMAT_D24_UNORM_S8_UINT";
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return "VK_FORMAT_D32_SFLOAT_S8_UINT";
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return "VK_FORMAT_BC1_RGB_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return "VK_FORMAT_BC1_RGB_SRGB_BLOCK";
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return "VK_FORMAT_BC1_RGBA_UNORM_BLOCK";
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return "VK_FORMAT_BC1_RGBA_SRGB_BLOCK";
    case VK_FORMAT_BC2_UNORM_BLOCK: return "VK_FORMAT_BC2_UNORM_BLOCK";
    case VK_FORMAT_BC2_SRGB_BLOCK: return "VK_FORMAT_BC2_SRGB_BLOCK";
    case VK_FORMAT_BC3_UNORM_BLOCK: return "VK_FORMAT_BC3_UNORM_BLOCK";
    case VK_FORMAT_BC3_SRGB_BLOCK: return "VK_FORMAT_BC3_SRGB_BLOCK";
    case VK_FORMAT_BC4_UNORM_BLOCK: return "VK_FORMAT_BC4_UNORM_BLOCK";
    case VK_FORMAT_BC4_SNORM_BLOCK: return "VK_FORMAT_BC4_SNORM_BLOCK";
    case VK_FORMAT_BC5_UNORM_BLOCK: return "VK_FORMAT_BC5_UNORM_BLOCK";
    case VK_FORMAT_BC5_SNORM_BLOCK: return "VK_FORMAT_BC5_SNORM_BLOCK";
    case VK_FORMAT_BC6H_UFLOAT_BLOCK: return "VK_FORMAT_BC6H_UFLOAT_BLOCK";
    case VK_FORMAT_BC6H_SFLOAT_BLOCK: return "VK_FORMAT_BC6H_SFLOAT_BLOCK";
    case VK_FORMAT_BC7_UNORM_BLOCK: return "VK_FORMAT_BC7_UNORM_BLOCK";
    case VK_FORMAT_BC7_SRGB_BLOCK: return "VK_FORMAT_BC7_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return "VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return "VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return "VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return "VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return "VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK";
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return "VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK";
    case VK_FORMAT_EAC_R11_UNORM_BLOCK: return "VK_FORMAT_EAC_R11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11_SNORM_BLOCK: return "VK_FORMAT_EAC_R11_SNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return "VK_FORMAT_EAC_R11G11_UNORM_BLOCK";
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return "VK_FORMAT_EAC_R11G11_SNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return "VK_FORMAT_ASTC_4x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return "VK_FORMAT_ASTC_4x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return "VK_FORMAT_ASTC_5x4_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return "VK_FORMAT_ASTC_5x4_SRGB_BLOCK";
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return "VK_FORMAT_ASTC_5x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return "VK_FORMAT_ASTC_5x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return "VK_FORMAT_ASTC_6x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return "VK_FORMAT_ASTC_6x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return "VK_FORMAT_ASTC_6x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return "VK_FORMAT_ASTC_6x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return "VK_FORMAT_ASTC_8x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return "VK_FORMAT_ASTC_8x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return "VK_FORMAT_ASTC_8x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return "VK_FORMAT_ASTC_8x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return "VK_FORMAT_ASTC_8x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return "VK_FORMAT_ASTC_8x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return "VK_FORMAT_ASTC_10x5_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return "VK_FORMAT_ASTC_10x5_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return "VK_FORMAT_ASTC_10x6_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return "VK_FORMAT_ASTC_10x6_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return "VK_FORMAT_ASTC_10x8_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return "VK_FORMAT_ASTC_10x8_SRGB_BLOCK";
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return "VK_FORMAT_ASTC_10x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return "VK_FORMAT_ASTC_10x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return "VK_FORMAT_ASTC_12x10_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return "VK_FORMAT_ASTC_12x10_SRGB_BLOCK";
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return "VK_FORMAT_ASTC_12x12_UNORM_BLOCK";
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return "VK_FORMAT_ASTC_12x12_SRGB_BLOCK";
    case VK_FORMAT_G8B8G8R8_422_UNORM: return "VK_FORMAT_G8B8G8R8_422_UNORM(_KHR)";
    case VK_FORMAT_B8G8R8G8_422_UNORM: return "VK_FORMAT_B8G8R8G8_422_UNORM(_KHR)";
    case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: return "VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM(_KHR)";
    case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: return "VK_FORMAT_G8_B8R8_2PLANE_420_UNORM(_KHR)";
    case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM: return "VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM(_KHR)";
    case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM: return "VK_FORMAT_G8_B8R8_2PLANE_422_UNORM(_KHR)";
    case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM: return "VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM(_KHR)";
    case VK_FORMAT_R10X6_UNORM_PACK16: return "VK_FORMAT_R10X6_UNORM_PACK16(_KHR)";
    case VK_FORMAT_R10X6G10X6_UNORM_2PACK16: return "VK_FORMAT_R10X6G10X6_UNORM_2PACK16(_KHR)";
    case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: return "VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return "VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return "VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_R12X4_UNORM_PACK16: return "VK_FORMAT_R12X4_UNORM_PACK16(_KHR)";
    case VK_FORMAT_R12X4G12X4_UNORM_2PACK16: return "VK_FORMAT_R12X4G12X4_UNORM_2PACK16(_KHR)";
    case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16: return "VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return "VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return "VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G16B16G16R16_422_UNORM: return "VK_FORMAT_G16B16G16R16_422_UNORM(_KHR)";
    case VK_FORMAT_B16G16R16G16_422_UNORM: return "VK_FORMAT_B16G16R16G16_422_UNORM(_KHR)";
    case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM: return "VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM(_KHR)";
    case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM: return "VK_FORMAT_G16_B16R16_2PLANE_420_UNORM(_KHR)";
    case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM: return "VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM(_KHR)";
    case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM: return "VK_FORMAT_G16_B16R16_2PLANE_422_UNORM(_KHR)";
    case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM: return "VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM(_KHR)";
    case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM: return "VK_FORMAT_G8_B8R8_2PLANE_444_UNORM(_KHR)";
    case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16: return "VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16: return "VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16(_KHR)";
    case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM: return "VK_FORMAT_G16_B16R16_2PLANE_444_UNORM(_KHR)";
    case VK_FORMAT_A4R4G4B4_UNORM_PACK16: return "VK_FORMAT_A4R4G4B4_UNORM_PACK16(_KHR)";
    case VK_FORMAT_A4B4G4R4_UNORM_PACK16: return "VK_FORMAT_A4B4G4R4_UNORM_PACK16(_KHR)";
    case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK: return "VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK(_EXT)";
    case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: return "VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: return "VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG: return "VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG: return "VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: return "VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: return "VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG: return "VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG: return "VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG";
    case VK_FORMAT_R16G16_S10_5_NV: return "VK_FORMAT_R16G16_S10_5_NV";
    case VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR: return "VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR";
    case VK_FORMAT_A8_UNORM_KHR: return "VK_FORMAT_A8_UNORM_KHR";
    default: return "vkformat ????";
    }
}

static const char* vkcolor(VkColorSpaceKHR space) {
    switch (space) {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR: return "VK_COLOR(_)SPACE_SRGB_NONLINEAR_KHR";
    case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT: return "VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT";
    case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT: return "VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT";
    case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT: return "VK_COLOR_SPACE_(DISPLAY|DCI)_P3_LINEAR_EXT";
    case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT: return "VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT";
    case VK_COLOR_SPACE_BT709_LINEAR_EXT: return "VK_COLOR_SPACE_BT709_LINEAR_EXT";
    case VK_COLOR_SPACE_BT709_NONLINEAR_EXT: return "VK_COLOR_SPACE_BT709_NONLINEAR_EXT";
    case VK_COLOR_SPACE_BT2020_LINEAR_EXT: return "VK_COLOR_SPACE_BT2020_LINEAR_EXT";
    case VK_COLOR_SPACE_HDR10_ST2084_EXT: return "VK_COLOR_SPACE_HDR10_ST2084_EXT";
    case VK_COLOR_SPACE_DOLBYVISION_EXT: return "VK_COLOR_SPACE_DOLBYVISION_EXT";
    case VK_COLOR_SPACE_HDR10_HLG_EXT: return "VK_COLOR_SPACE_HDR10_HLG_EXT";
    case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT: return "VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT";
    case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT: return "VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT";
    case VK_COLOR_SPACE_PASS_THROUGH_EXT: return "VK_COLOR_SPACE_PASS_THROUGH_EXT";
    case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT: return "VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT";
    case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD: return "VK_COLOR_SPACE_DISPLAY_NATIVE_AMD";
    case VK_COLOR_SPACE_MAX_ENUM_KHR: return "VK_COLOR_SPACE_MAX_ENUM_KHR";
    default: return "vkcolor ????";
    }
}

static const char* vkpresent(VkPresentModeKHR mode) {
    switch (mode) {
    case VK_PRESENT_MODE_IMMEDIATE_KHR: return "VK_PRESENT_MODE_IMMEDIATE_KHR";
    case VK_PRESENT_MODE_MAILBOX_KHR: return "VK_PRESENT_MODE_MAILBOX_KHR";
    case VK_PRESENT_MODE_FIFO_KHR: return "VK_PRESENT_MODE_FIFO_KHR";
    case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
    case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR: return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
    case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR: return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
    case VK_PRESENT_MODE_MAX_ENUM_KHR: return "VK_PRESENT_MODE_MAX_ENUM_KHR";
    default: return "vkpresent ????";
    }
}