#include "teleios/identity.h"
#include "teleios/logger.h"
#include "teleios/string.h"

#include <stdlib.h>

static const char UUID[] = "0123456789abcdef";

void tl_identity_initialize(const TLIdentity* identity) {
    for (unsigned i = 0; i < 37; ++i) {
        *((i8*)&identity->identity[i]) = UUID[rand() % 16];
    }

    *((i8*)&identity->identity[8]) = '-';
    *((i8*)&identity->identity[13]) = '-';
    *((i8*)&identity->identity[18]) = '-';
    *((i8*)&identity->identity[23]) = '-';
    *((i8*)&identity->identity[37]) = 0; // null-terminated
}

static const char     EMPTY[] = "";
static const char    ZEROES[] = "000000000000000000000000000000000000";
static const char UUID_ZERO[] = "00000000-0000-0000-0000-000000000000";

b8 tl_identity_empty(const TLIdentity* identity) {
    if (identity->identity == NULL) return true;
    if (tl_string_equals(identity->identity, EMPTY)) return true;
    if (tl_string_equals(identity->identity, ZEROES)) return true;
    return tl_string_equals(identity->identity, UUID_ZERO);
}

b8 tl_identity_compare(const TLIdentity* first, const TLIdentity* second) {
    return tl_string_equals(first->identity, second->identity);
}