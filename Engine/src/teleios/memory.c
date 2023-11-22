#include "teleios/logger.h"
#include "teleios/platform/memory.h"

#include <stdlib.h>

struct TLRegistry {
  u64 allocated;
  u64 pool[TL_MEMORY_TYPE_MAXIMUM];
};

static u64 OSIZE = sizeof(TLObject);
static struct TLRegistry registry = { 0 };
// ################################################################################
//
//                                     ALLOCATOR
//
// ################################################################################
#include "teleios/memory/allocator.h"

TLAPI void* tl_memory_alloc(TLEMemoryType type, u64 size) {
  void* block = tl_platform_memory_alloc(size);
  if (block == NULL) {
    TLERROR("tl_memory_alloc: Failed to allocate %llu bytes", size);
    return NULL;
  }

  registry.allocated += size;
  registry.pool[type] += size;

  return block;
}

TLAPI void tl_memory_free(const void* target, TLEMemoryType type, u64 size) {
  tl_platform_memory_free(target);

  registry.allocated -= size;
  registry.pool[type] -= size;
}

TLAPI void tl_memory_zero(const void* target, u64 size) {
  tl_platform_memory_set(target, 0, size);
}

TLAPI void tl_memory_set(const void* target, i32 value, u64 size) {
  tl_platform_memory_set(target, value, size);
}

TLAPI void tl_memory_copy(const void* source, const void* target, u64 size) {
  tl_platform_memory_copy(source, target, size);
}

// ################################################################################
//
//                                     MANAGER
//
// ################################################################################
#include "teleios/memory/manager.h"
b8 tl_memory_initialize(void) {
  TLTRACE("tl_memory_initialize");
  return true;
}

static const char* tl_memory_type(TLEMemoryType type) {
  switch (type) {
    case TL_MEMORY_TYPE_CONTAINER_LIST: return "TL_MEMORY_TYPE_CONTAINER_LIST";
    case TL_MEMORY_TYPE_CONTAINER_NODE: return "TL_MEMORY_TYPE_CONTAINER_NODE";
    case TL_MEMORY_TYPE_SCENE:          return "TL_MEMORY_TYPE_SCENE         ";
    case TL_MEMORY_TYPE_SCENE_REGION:   return "TL_MEMORY_TYPE_SCENE_REGION  ";
    case TL_MEMORY_TYPE_ECS_ENTITY:     return "TL_MEMORY_TYPE_ECS_ENTITY    ";
    case TL_MEMORY_TYPE_ECS_SYSTEM:     return "TL_MEMORY_TYPE_ECS_SYSTEM    ";
    case TL_MEMORY_TYPE_ECS_COMPONENT:  return "TL_MEMORY_TYPE_ECS_COMPONENT ";
    case TL_MEMORY_TYPE_MAXIMUM:        return "TL_MEMORY_TYPE_MAXIMUM       ";
  }

  return "TL_MEMORY_TYPE_ ???";
}

b8 tl_memory_terminate(void) {
  TLTRACE("tl_memory_terminate");
  if (registry.allocated == 0) return true;
  
  for (unsigned i = 0; i < TL_MEMORY_TYPE_MAXIMUM; ++i)
    if (registry.pool[i] > 0)
      TLERROR("tl_memory_terminate: Leak %s - %u bytes", tl_memory_type(i), registry.pool[i]);

  return false;
}
