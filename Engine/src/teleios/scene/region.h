#ifndef TELEIOS_REGION_H
#define TELEIOS_REGION_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLIdentity* tl_region_create(TLIdentity* sceneid, const char* name);
TLAPI void tl_region_destroy(TLIdentity* sceneid, TLIdentity* regionid);

#endif // TELEIOS_REGION_H
