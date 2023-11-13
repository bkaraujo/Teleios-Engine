#ifndef TELEIOS_OBJECT_REFERENCE_H
#define TELEIOS_OBJECT_REFERENCE_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI void tl_object_ref_increase(const TLObject* object);
TLAPI b8 tl_object_ref_decrease(const TLObject* object);

#endif // TELEIOS_OBJECT_REFERENCE_H
