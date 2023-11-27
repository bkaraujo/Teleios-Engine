#include "teleios/container.h"
#include "teleios/identity/generator.h"
#include "teleios/identity/manager.h"
#include "teleios/logger.h"
#include "teleios/string.h"

#include <stdlib.h>

static const char UUID[] = "0123456789abcdef";
static TLList* known;

static b8 tl_identity_comparator(const void* first, const void* second);
static void tl_identity_fill(const TLIdentity* identity);
// ##############################################################################################
//
//                                        GENERATOR
//
// ##############################################################################################
void tl_identity_generate(const TLIdentity* identity) {
    while (true) {
        tl_identity_fill(identity);
        if (!tl_list_contains(known, tl_identity_comparator, identity->identity))
            break;
    }

    tl_list_append(known, identity->identity);
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

b8 tl_identity_equals(const TLIdentity* first, const TLIdentity* second) {
    return tl_string_equals(first->identity, second->identity);
}

// ##############################################################################################
//
//                                        MANAGER
//
// ##############################################################################################
b8 tl_identity_initialize(void) {
    known = tl_list_create();
    if (known == NULL) {
        TLERROR("tl_identity_initialize: Failed to initialize known list");
        return false;
    }

    return true;
}

b8 tl_identity_terminate(void) {
    if (!tl_list_clear(known, tl_container_noop_dealocator)) {
        TLERROR("tl_identity_terminate: Failed to clear the known list");
        return false;
    }

    if (!tl_list_destroy(known)) {
        TLERROR("tl_identity_terminate: Failed to destroy the known list");
        return false;
    }

    known = NULL;

    return true;
}
// ##############################################################################################
//
//                                        HELPERS
//
// ##############################################################################################
static void tl_identity_fill(const TLIdentity* identity) {
    for (unsigned i = 0; i < 37; ++i) {
        *((i8*)&identity->identity[i]) = UUID[rand() % 16];
    }

    *((i8*)&identity->identity[8]) = '-';
    *((i8*)&identity->identity[13]) = '-';
    *((i8*)&identity->identity[18]) = '-';
    *((i8*)&identity->identity[23]) = '-';
    *((i8*)&identity->identity[37]) = 0; // null-terminated
}

static b8 tl_identity_comparator(const void* first, const void* second) {
    return tl_string_equals(first, second);
}