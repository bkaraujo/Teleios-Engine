#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include <stdlib.h>
#include "teleios/engine/lifecycle.h"

static inline void tl_application_set(TLSpecification* spec);
static inline b8 tl_application_initialize(void);
static inline b8 tl_application_terminate(void);

int main(int argc, char** argv) {

    {
        TLSpecification spec = { 0 };
        spec.version.major = 0;
        spec.version.minor = 1;
        spec.version.patch = 0;

        spec.window.title = "Teleios APP";
        spec.window.width = 1024;
        spec.window.height = 768;

        spec.graphics.vsync = false;

        tl_application_set(&spec);

        if (!tl_engine_initialize(&spec)) {
            if (!tl_engine_terminate()) exit(99);
            exit(90);
        }
    }

    if (!tl_application_initialize()) {
        if (!tl_engine_terminate()) exit(99);
        exit(91);

    }

    if (!tl_engine_run()) {
        if (!tl_application_terminate()) {
            if (!tl_engine_terminate()) exit(99);
            exit(92);
        }

        if (!tl_engine_terminate()) exit(99);
        exit(93);
    }

    if (!tl_application_terminate()) {
        if (!tl_engine_terminate()) exit(99);
        exit(94);
    }

    if (!tl_engine_terminate()) exit(99);

}

#endif // TELEIOS_APPLICATION_H