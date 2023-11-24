#ifndef TELEIOS_IDENTITY_GENERATOR_H
#define TELEIOS_IDENTITY_GENERATOR_H

#include "teleios/types.h"

void tl_identity_generate(const TLIdentity* identity);
b8 tl_identity_empty(const TLIdentity* identity);
b8 tl_identity_equals(const TLIdentity* first, const TLIdentity* second);

#endif // TELEIOS_IDENTITY_GENERATOR_H
