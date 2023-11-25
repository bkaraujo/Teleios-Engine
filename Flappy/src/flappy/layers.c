// ##############################################################################################
//
//                                        WORLD
//
// ##############################################################################################
#include "flappy/layer/world.h"
#include "flappy/scene/world.h"
b8 flappy_world_on_attach(void) {
    flappy_scene_world_initialize();
    return true;
}

b8 flappy_world_on_detach(void) {
    flappy_scene_world_terminate();
    return true;
}

b8 flappy_world_update_variable(const u64 delta) {
    return true;
}

b8 flappy_world_update_fixed(const u64 delta) {
    return true;
}

b8 flappy_world_update_after(void) {
    return true;
}
// ##############################################################################################
//
//                                        GUI
//
// ##############################################################################################
#include "flappy/layer/gui.h"
b8 flappy_gui_on_attach(void) {
    return true;
}

b8 flappy_gui_on_detach(void) {
    return true;
}

b8 flappy_gui_update_variable(const u64 delta) {
    return true;
}

b8 flappy_gui_update_fixed(const u64 delta) {
    return true;
}

b8 flappy_gui_update_after(void) {
    return true;
}
