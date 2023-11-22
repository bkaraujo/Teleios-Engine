#include "scene.h"
#include <teleios/application.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Flappy";

}

b8 tl_application_initialize(const TLSpecification* spec) {
    g_scene_initialize();
    return true;
}

b8 tl_application_terminate() {
    g_scene_terminate();
    return true;
}
