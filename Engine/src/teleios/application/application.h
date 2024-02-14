#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include <stdlib.h>
#include "teleios/engine/Lifecycle.h"

int main(int argc, char** argv) {

    if (!tl_engine_initialize()) {
        if (!tl_engine_terminate()) exit(99);
        exit(90);
    }

    if (!tl_engine_run()) {
        if (!tl_engine_terminate()) exit(99);
        exit(91);
    }

    if (!tl_engine_terminate()) exit(99);

}

#endif // TELEIOS_APPLICATION_H