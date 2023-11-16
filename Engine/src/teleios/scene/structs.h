#ifndef TELEIOS_SCENE_SCTRUCTS_H
#define TELEIOS_SCENE_SCTRUCTS_H

#include "teleios/types.h"

typedef struct {
  const TLIdentity identity;
  const char* name;
  TLList* regions;
} TLScene;

typedef struct {
  const TLIdentity identity;
  const char* name;
  TLList* entities;
} TLRegion;



#endif // TELEIOS_SCENE_SCTRUCTS_H
