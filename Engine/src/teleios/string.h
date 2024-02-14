#ifndef TELEIOS_STRING_H
#define TELEIOS_STRING_H

#include "teleios/types.h"

TLEXPORT b8 tl_string_is_empty(const char* string);
TLEXPORT u64 tl_string_length(const char* string);
TLEXPORT b8 tl_string_starts_with(const char* string, const char* candidate);
TLEXPORT b8 tl_string_equals(const char* first, const char* second);

#endif // TELEIOS_STRING_H