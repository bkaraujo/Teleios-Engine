#ifndef TELEIOS_ECS_STRUCT_H
#define TELEIOS_ECS_STRUCT_H

#include "teleios/types.h"

typedef struct {
  TLIdentity identity;
  const char* name;
} TLEntity;

typedef struct {
  TLIdentity identity;

} TLComponent;

typedef struct {
  TLIdentity identity;

} TLSystem;

#endif // TELEIOS_ECS_STRUCT_H
