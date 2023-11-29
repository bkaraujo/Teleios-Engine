#include "teleios/string.h"

#include <string.h>

TLAPI const u64 tl_string_length(const char* string) {
    return strlen(string);
}

TLAPI b8 tl_string_equals(const char* first, const char* second) {
    u64 length = tl_string_length(first);
    if (tl_string_length(second) != length) return false;

    return strcmp(first, second) == 0;
}