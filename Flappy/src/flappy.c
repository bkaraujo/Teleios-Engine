#include "flappy/layer/gui.h"
#include "flappy/layer/world.h"
#include <teleios/application.h>
#include <teleios/layer/stack.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Flappy";

}

static TLIdentity* lWorld;
static TLIdentity* lGui;
b8 tl_application_initialize(const TLSpecification* spec) {
    lWorld = tl_layer_stack_create(
        "World",
        flappy_world_on_attach,
        flappy_world_on_detach,
        flappy_world_update_variable,
        flappy_world_update_fixed,
        flappy_world_update_after
    );

    lGui = tl_layer_stack_create(
        "GUI",
        flappy_gui_on_attach,
        flappy_gui_on_detach,
        flappy_gui_update_variable,
        flappy_gui_update_fixed,
        flappy_gui_update_after
    );

    return true;
}

b8 tl_application_terminate() {
    // if (!tl_layer_stack_destroy(lGui)) return false;
    // if (!tl_layer_stack_destroy(lWorld)) return false;

    return true;
}
