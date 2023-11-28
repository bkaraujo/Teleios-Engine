#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include "teleios/container.h"
#include "teleios/engine.h"

void tl_application_define(TLSpecification* spec);
b8 tl_application_initialize(const TLSpecification* spec);
b8 tl_application_terminate();

int main(int argc, char** argv) {
    if (!tl_engine_pre_initialize()) return -1;

    TLSpecification spec = { 0 };

    spec.name = "Teleios APP";
    spec.window.witdh = 640;
    spec.window.height = 480;
    spec.vulkan.device.extentions = tl_list_create();
    spec.vulkan.instance.layers = tl_list_create();
    spec.vulkan.instance.extentions = tl_list_create();

    tl_application_define(&spec);
    if (!tl_engine_initialize(&spec)) return -1;
    if (!tl_application_initialize(&spec)) return -99;

    tl_list_clear(spec.vulkan.device.extentions, tl_container_noop_dealocator);
    tl_list_destroy(spec.vulkan.device.extentions);
    spec.vulkan.device.extentions = NULL;

    tl_list_clear(spec.vulkan.instance.layers, tl_container_noop_dealocator);
    tl_list_destroy(spec.vulkan.instance.layers);
    spec.vulkan.instance.layers = NULL;

    tl_list_clear(spec.vulkan.instance.extentions, tl_container_noop_dealocator);
    tl_list_destroy(spec.vulkan.instance.extentions);
    spec.vulkan.instance.extentions = NULL;

    if (!tl_engine_run()) return -1;
    if (!tl_application_terminate()) return -99;
    if (!tl_engine_terminate()) return -1;

    return 0;
}

#endif // TELEIOS_APPLICATION_H
