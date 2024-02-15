#include "teleios/application/application.h"

static inline void tl_application_set(TLSpecification* spec) {
    spec->window.title = "Teleios Editor";
}

static inline b8 tl_application_initialize(void) {
    return true;
}

static inline b8 tl_application_terminate(void) {
    return true;
}
