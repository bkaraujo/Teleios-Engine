#ifndef TELEIOS_MEMORY_OPERATOR_H
#define TELEIOS_MEMORY_OPERATOR_H

#include "teleios/types.h"

TLEXPORT void tl_memory_set(void* pointer, i32 value, u64 size);
TLEXPORT void tl_memory_zero(void* pointer, u64 size);
TLEXPORT void tl_memory_copy(void* source, void* target, u64 size);

#endif // TELEIOS_MEMORY_OPERATOR_H