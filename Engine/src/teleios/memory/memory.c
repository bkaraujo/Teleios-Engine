#include "teleios/platform/memory.h"
#include "teleios/memory/allocator.h"
#include "teleios/memory/manager.h"
#include "teleios/logger.h"

#include <stdlib.h>

struct TLRegistry {
  u64 allocated;
  u64 type[TL_MEMORY_TYPE_MAXIMUM];
};

static u64 OSIZE = sizeof(TLObject);
static const char UUID[] = "0123456789abcdef";
static struct TLRegistry registry = { 0 };


TLAPI TLObject* tl_memory_alloc(TLEMemoryType type, u64 size) {
  TLObject* object = tl_platform_memory_alloc(OSIZE);
  if (object == NULL) {
    TLERROR("tl_memory_alloc: Failed to allocate TLObject");
    return NULL;
  }

  object->pointer = tl_platform_memory_alloc(size);
  if (object->pointer == NULL) {
    TLERROR("tl_memory_alloc: Failed to allocate TLObject->pointer");
    tl_memory_free(object);
    return NULL;
  }

  *((u8*)&object->refs) = 1;
  *((u64*) &object->allocated) = size;
  *((TLEMemoryType*) &object->type) = type;
 
  for (unsigned i = 0; i < 37;  ++i) {
    *((i8*) &object->identity[i]) = UUID[rand() % 16];
  }

  *((i8*) &object->identity[ 8]) = '-';
  *((i8*) &object->identity[13]) = '-';
  *((i8*) &object->identity[18]) = '-';
  *((i8*) &object->identity[23]) = '-';
  *((i8*) &object->identity[37]) = 0; // null-terminated

  registry.type[type] += size;
  registry.type[TL_MEMORY_TYPE_OBJECT] += OSIZE;
  registry.allocated += OSIZE + size;

  return object;
}

TLAPI void tl_memory_free(const TLObject* object) {
  if (object == NULL) return;

  if (object->refs > 1) {
    TLERROR("tl_memory_free: object->refs (%u) > 1", object->refs);
    return;
  }

   registry.type[object->type] -= object->allocated;
   registry.type[TL_MEMORY_TYPE_OBJECT] -= OSIZE;
   registry.allocated -= OSIZE + object->allocated;

   tl_platform_memory_free(object->pointer);
   tl_platform_memory_free(object);
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

b8 tl_memory_initialize(void) {
  return true;
}

static const char* tl_memory_type(TLEMemoryType type) {
  switch (type) {
  case TL_MEMORY_TYPE_OBJECT: return "TL_MEMORY_TYPE_OBJECT";
  case TL_MEMORY_TYPE_SCENE: return "TL_MEMORY_TYPE_SCENE";
  case TL_MEMORY_TYPE_REGION: return "TL_MEMORY_TYPE_REGION";
  case TL_MEMORY_TYPE_LAYER: return "TL_MEMORY_TYPE_LAYER";
  case TL_MEMORY_TYPE_ENTITY: return "TL_MEMORY_TYPE_ENTITY";
  case TL_MEMORY_TYPE_COMPONENT: return "TL_MEMORY_TYPE_COMPONENT";
  case TL_MEMORY_TYPE_MAXIMUM: return "TL_MEMORY_TYPE_MAXIMUM";
  }
}

b8 tl_memory_terminate(void) {
  if (registry.allocated == 0) return true;
  
  for (unsigned i = 0; i < TL_MEMORY_TYPE_MAXIMUM; ++i)
    if (registry.type[i] > 0)
      TLERROR("tl_memory_terminate: Leak %s - %u bytes", tl_memory_type(i), registry.type[i]);

  return false;
}
