#ifndef TELEIOS_STRING_H
#define TELEIOS_STRING_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI u64 tl_string_length(const char* string);
TLAPI b8 tl_string_equals(const char* first, const char* second);

#endif // TELEIOS_STRING_H
