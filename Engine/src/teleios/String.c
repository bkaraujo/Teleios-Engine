#include <string.h>
#include "teleios/string.h"

TLEXPORT b8 tl_string_is_empty(const char* string) {
    if (string == NULL) return true;
    return string[0] == 0;
}

TLEXPORT u64 tl_string_length(const char* string) {
    return strlen(string);
}

TLEXPORT b8 tl_string_starts_with(const char* string, const char* candidate) {
    if (string == NULL || candidate == NULL) return true;

    u64 sLen = tl_string_length(string);
    u64 cLen = tl_string_length(candidate);
    if (sLen < cLen) return false;

    return strncmp(string, candidate, cLen) == 0;
}

TLEXPORT b8 tl_string_equals(const char* first, const char* second) {
    if (first == NULL) return false;
    if (second == NULL) return false;
    if (tl_string_length(first) != tl_string_length(second)) return false;

    return strcmp(first, second) == 0;
}
