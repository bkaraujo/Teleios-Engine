#ifndef TELEIOS_SCENE_REGION_H
#define TELEIOS_SCENE_REGION_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI b8 tl_region_entity_attach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid);
TLAPI b8 tl_region_entity_detach(const TLIdentity* sceneid, const TLIdentity* regionid, const TLIdentity* entityid);

#endif // TELEIOS_SCENE_REGION_H
