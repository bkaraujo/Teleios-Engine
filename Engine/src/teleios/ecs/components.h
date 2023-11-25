#ifndef TELEIOS_ECS_COMPONENTS_H
#define TELEIOS_ECS_COMPONENTS_H

#include "teleios/types.h"

typedef struct {
    const TLIdentity* identity;
} TLComponent;

typedef struct {
    TLComponent owner;
    ivec3s scale;
    ivec3s position;
} TLComponentTransform;

typedef struct {
    TLComponent owner;
    const char* name;
} TLComponentName;

#endif // TELEIOS_ECS_COMPONENST_H
