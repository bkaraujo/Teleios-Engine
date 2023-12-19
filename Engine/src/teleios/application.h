#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include "teleios/container.h"
#include "teleios/engine.h"

void tl_application_define(TLSpecification* spec);
b8 tl_application_initialize(const TLSpecification* spec);
b8 tl_application_terminate(void);

int main(int argc, char** argv) {
    if (!tl_engine_pre_initialize()) return -1;

    TLSpecification spec = { 0 };

    spec.name = "Teleios APP";
    spec.window.witdh = 640;
    spec.window.height = 480;
    spec.simulation.per_second = 75;

    tl_application_define(&spec);
    if (!tl_engine_initialize(&spec)) {
        if (!tl_engine_terminate()) {
            return 99;
        }

        return 10;
    }

    if (!tl_application_initialize(&spec)) {
        if (!tl_engine_terminate()) {
            return 99;
        }

        return 11;
    }

    if (!tl_engine_run()) {
        if (!tl_engine_terminate()) {
            return 99;
        }

        return 20;
    }

    if (!tl_application_terminate()) {
        if (!tl_engine_terminate()) {
            return 99;
        }

        return 31;
    }

    if (!tl_engine_terminate()) {
        return 99;
    }

    return 0;
}

#endif // TELEIOS_APPLICATION_H
