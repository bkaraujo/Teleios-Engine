#include "teleios/container.h"
#include "teleios/graphics/vulkan/vkcontext.h"
#include "teleios/logger.h"
#include "teleios/memory/allocator.h"
#include "teleios/renderer/manager.h"

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_renderer_initialize(const TLSpecification* spec) {
    return true;
}

b8 tl_renderer_terminate(void) {
    return true;
}
