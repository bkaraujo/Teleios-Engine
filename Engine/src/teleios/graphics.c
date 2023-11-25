#include "teleios/container.h"
#include "teleios/graphics/context.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

VKContext context;
// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
#include "teleios/graphics/manager.h"

static b8 vkinstance_initialize(void);
static b8 vksurface_initialize(void);
static b8 vkdevice_initialize(void);
static b8 vkswapchain_initialize(void);

b8 tl_graphics_initialize(const TLSpecification* spec) {
    TLTRACE("tl_graphics_initialize");
    tl_memory_zero(&context, sizeof(VKContext));

    if (!vkinstance_initialize()) return false;
    if (!vksurface_initialize()) return false;
    if (!vkdevice_initialize()) return false;
    if (!vkswapchain_initialize()) return false;

    return true;
}

static b8 vkinstance_terminate(void);
static b8 vksurface_terminate(void);
static b8 vkdevice_terminate(void);
static b8 vkswapchain_terminate(void);

b8 tl_graphics_terminate(void) {
    TLTRACE("tl_graphics_terminate");
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
//                                        VULKAN INSTANCE
//
// ##############################################################################################
static b8 vkinstance_initialize(void) {
    return true;
}

static b8 vkinstance_terminate(void) {
    return true;
}
// ##############################################################################################
//
//                                        VULKAN SURFACE
//
// ##############################################################################################
static b8 vksurface_initialize(void) {
    return true;
}

static b8 vksurface_terminate(void) {
    return true;
}

// ##############################################################################################
//
//                                        VULKAN DEVICE
//
// ##############################################################################################
static b8 vkdevice_initialize(void) {
    return true;
}

static b8 vkdevice_terminate(void) {
    return true;
}

// ##############################################################################################
//
//                                        VULKAN SWAPCHAIN
//
// ##############################################################################################
static b8 vkswapchain_initialize(void) {
    return true;
}

static b8 vkswapchain_terminate(void) {
    return true;
}