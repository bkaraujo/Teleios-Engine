#ifndef TELEIOS_REGION_H
#define TELEIOS_REGION_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI const TLIdentity* tl_region_create(const TLIdentity* sceneid, const char* name);
TLAPI void tl_region_destroy(const TLIdentity* sceneid, const TLIdentity* regionid);

#endif // TELEIOS_REGION_H
