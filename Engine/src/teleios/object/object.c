#include "teleios/logger.h"
#include "teleios/memory/allocator.h"

#include <stdlib.h>
// ################################################################################
//
//                                     FACTORY
//
// ################################################################################
#include "teleios/object/factory.h"

static u64 OSIZE = sizeof(TLObject);
static const char UUID[] = "0123456789abcdef";

TLAPI TLObject* tl_object_create(const char* name, const u64 stride, void (*destructor)(const void*)) {
  TLObject* object = tl_memory_alloc(TL_MEMORY_TYPE_OBJECT, OSIZE);
  if (object == NULL) {
    TLERROR("tl_object_create: Failed to allocate TLObject");
    return NULL;
  }

  object->pointer = tl_memory_alloc(TL_MEMORY_TYPE_OBJECT_DEFINED, stride);
  if (object->pointer == NULL) {
    TLERROR("tl_object_create: Failed to allocate TLObject->pointer");
    tl_memory_free(object, TL_MEMORY_TYPE_OBJECT, OSIZE);
    return NULL;
  }

  *((u8*)&object->refs) = 1;
  *((u64*)&object->stride) = stride;
  object->name = name;

  for (unsigned i = 0; i < 37; ++i) {
    *((i8*)&object->identity[i]) = UUID[rand() % 16];
  }

  *((i8*)&object->identity[8]) = '-';
  *((i8*)&object->identity[13]) = '-';
  *((i8*)&object->identity[18]) = '-';
  *((i8*)&object->identity[23]) = '-';
  *((i8*)&object->identity[37]) = 0; // null-terminated
  
  object->destructor = destructor;

  return object;
}

// ################################################################################
//
//                                     COMPARATOR
//
// ################################################################################
#include "teleios/string.h"
#include "teleios/object/comparator.h"

TLAPI b8 tl_object_equals(const TLObject* first, const TLObject* second) {
  if (first == NULL) return false;
  if (second == NULL) return false;

  return tl_string_equals(first->identity, second->identity);
}
// ################################################################################
//
//                                     REFERENCE
//
// ################################################################################
#include "teleios/object/reference.h"

TLAPI void tl_memory_ref_increase(const TLObject* object) {
  if (object == NULL) return;
  *((u8*)&object->refs) = object->refs + 1;
}

TLAPI b8 tl_object_ref_decrease(const TLObject* object) {
  if (object == NULL) return false;

  *((u8*)&object->refs) = object->refs - 1;
  if (object->refs > 0) return false;

  if (object->destructor != NULL) object->destructor(object->pointer);
  tl_memory_free(object->pointer, TL_MEMORY_TYPE_OBJECT_DEFINED, object->stride);
  tl_memory_free(object, TL_MEMORY_TYPE_OBJECT, OSIZE);

  return true;
}