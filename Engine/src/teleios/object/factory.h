#ifndef TELEIOS_OBJECT_FACTORY_H
#define TELEIOS_OBJECT_FACTORY_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLObject* tl_object_create(const char* name, const u64 stride, void (*destructor)(const void*));

#endif // TELEIOS_OBJECT_FACTORY_H
