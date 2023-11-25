// ##############################################################################################
//
//                                        GUI
//
// ##############################################################################################
#include "editor/layer/gui.h"

b8 editor_gui_on_attach(void) {
    return true;
}

b8 editor_gui_on_detach(void) {
    return true;
}

b8 editor_gui_update_variable(const u64 delta) {
    return true;
}

b8 editor_gui_update_fixed(const u64 delta) {
    return true;
}

b8 editor_gui_update_after(void) {
    return true;
}

// ##############################################################################################
//
//                                        GUI
//
// ##############################################################################################
#include "editor/layer/player.h"

b8 editor_player_on_attach(void) {
    return true;
}

b8 editor_player_on_detach(void) {
    return true;
}

b8 editor_player_update_variable(const u64 delta) {
    return true;
}

b8 editor_player_update_fixed(const u64 delta) {
    return true;
}

b8 editor_player_update_after(void) {
    return true;
}
