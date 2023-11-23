#include "scene.h"
#include <teleios/application.h>
#include <teleios/ecs/component.h>
#include <teleios/ecs/search.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Flappy";

}

b8 tl_application_initialize(const TLSpecification* spec) {
    g_scene_initialize();


    const TLList* result = tl_ecs_search_for(BIT(TL_COMPONENT_TRANSFORM) | BIT(TL_COMPONENT_NAME));
    if (result->size != 2) {
        TLFATAL("XXXX");
    }

    tl_ecs_search_release(result);
    return true;
}

b8 tl_application_terminate() {
    g_scene_terminate();
    return true;
}
