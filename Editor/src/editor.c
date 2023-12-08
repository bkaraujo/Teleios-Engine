#include "editor/layer/gui.h"
#include "editor/layer/player.h"
#include <teleios/application.h>
#include <teleios/layer/stack.h>
#include <teleios/logger.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Teleios Editor";
    spec->vulkan.pipeline.fragment = "C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/frag.spv";
    spec->vulkan.pipeline.vertex = "C:/Users/bkrau/OneDrive/Trabalho/BKraujo/TeleiosEngine/Editor/Assets/Shader/vert.spv";
}

static TLIdentity* lGuiid;
static TLIdentity* lPlayerid;
b8 tl_application_initialize(const TLSpecification* spec) {
    lGuiid = tl_layer_stack_create("GUI", editor_gui_on_attach, editor_gui_on_detach, editor_gui_update_variable, editor_gui_update_fixed, editor_gui_update_after);
    if (lGuiid == NULL) {
        TLERROR("tl_application_initialize: Failed to create layer \"Editor GUI\"");
        return false;
    }

    lPlayerid = tl_layer_stack_create("Player", editor_player_on_attach, editor_player_on_detach, editor_player_update_variable, editor_player_update_fixed, editor_player_update_after);
    if (lPlayerid == NULL) {
        TLERROR("tl_application_initialize: Failed to create layer \"Editor Player\"");
        return false;
    }

    return true;
}

b8 tl_application_terminate() {
    if (!tl_layer_stack_destroy(lPlayerid)) {
        TLERROR("tl_application_terminate: Failed to destroy layer  \"Editor Player\"");
        return false;
    }
    lPlayerid = NULL;

    if (!tl_layer_stack_destroy(lGuiid)) {
        TLERROR("tl_application_terminate: Failed to destroy layer  \"Editor GUI\"");
        return false;
    }
    lGuiid = NULL;

    return true;
}
