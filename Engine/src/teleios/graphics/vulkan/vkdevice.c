#include "teleios/container.h"
#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/graphics/vulkan/vkdevice.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/tools.h"
#include "teleios/string.h"

static b8 vkdevice_physical_select(const TLSpecification* spec);
static b8 vkdevice_logical_create(const TLSpecification* spec);

b8 tl_vulkan_device_initialize(const TLSpecification* spec) {
    if (!vkdevice_physical_select(spec)) {
        TLERROR("tl_vulkan_device_initialize: Failed to select VkPhysicalDevice");
        return false;
    }

    if (!vkdevice_logical_create(spec)) {
        TLERROR("tl_vulkan_device_initialize: Failed to create VkDevice");
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
        TLDEBUG("Vulkan Device %s", context.device.ph.properties.deviceName);
        tl_memory_copy(&driverProperties, &context.device.ph.driver_properties, sizeof(VkPhysicalDeviceDriverProperties));
        TLDEBUG("Vulkan Device Driver %s %s", context.device.ph.driver_properties.driverName, context.device.ph.driver_properties.driverInfo);
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

            TLDEBUG("Vulkan Device Extention: %s", current->payload);
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
        TLDEBUG("Vulkan Device Queue Video Family: %u", video);
        TLDEBUG("Vulkan Device Queue Graphics Family: %u", graphics);
        TLDEBUG("Vulkan Device Queue Compute Family: %u %s", compute, compute == present ? " with present" : "");
        TLDEBUG("Vulkan Device Queue Transfer Family: %u %s", transfer, transfer == present ? " with present" : "");

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

b8 tl_vulkan_device_terminate(void) {
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