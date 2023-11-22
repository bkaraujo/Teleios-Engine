#ifndef TELEIOS_IDENTITY_H
#define TELEIOS_IDENTITY_H

#include "teleios/types.h"

void tl_identity_initialize(const TLIdentity* identity);
b8 tl_identity_empty(const TLIdentity* identity);
b8 tl_identity_compare(const TLIdentity* first, const TLIdentity* second);

#endif // TELEIOS_IDENTITY_H
