#ifndef TELEIOS_ECS_COMPONENTS_H
#define TELEIOS_ECS_COMPONENTS_H

#include "teleios/types.h"

typedef struct {
    const TLIdentity* entityid;
} TLComponentTransform;

typedef struct {
    const TLIdentity* entityid;
    const char* name;
} TLComponentName;

#endif // TELEIOS_ECS_COMPONENST_H
