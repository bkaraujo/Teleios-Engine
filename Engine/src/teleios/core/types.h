#ifndef TELEIOS_CORE_TYPES_H
#define TELEIOS_CORE_TYPES_H

#include "teleios/types.h"

typedef struct TLArray {
    u32 lenth;
    u32 size;
    TLALIGN(PSIZE) const void** payload;
} TLArray;

#endif 
