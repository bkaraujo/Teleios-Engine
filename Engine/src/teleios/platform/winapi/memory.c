#include "teleios/logger.h"
#include "teleios/platform/memory.h"
#include "teleios/platform/winapi/externals.h"

void* tl_platform_memory_alloc(u64 size) {
  return HeapAlloc(heap, HEAP_ZERO_MEMORY, size);
}

void tl_platform_memory_free(void* block) {
  HeapFree(heap, HEAP_NO_SERIALIZE, block);
}

void tl_platform_memory_copy(void* source, void* target, u64 size) {
  i32 error = memcpy_s(target, size, source, size);
  if (error != 0) {
    TLFATAL("tl_platform_memory_copy: Failed with platform code: %d", error);
  }
}

void tl_platform_memory_set(void* target, i32 value, u64 size) {
  memset(target, value, size);
}
