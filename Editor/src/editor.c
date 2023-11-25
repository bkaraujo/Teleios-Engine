#include <teleios/application.h>
#include <teleios/layer/stack.h>
#include <teleios/logger.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Teleios Editor";

}

static b8 ed_gui_on_attach(void);
static b8 ed_gui_on_detach(void);
static b8 ed_gui_update_variable(const u64 delta);
static b8 ed_gui_update_fixed(const u64 delta);
static b8 ed_gui_update_after(void);

static TLIdentity* layerid;
b8 tl_application_initialize(const TLSpecification* spec) {
    layerid = tl_layer_stack_create("Editor GUI", ed_gui_on_attach, ed_gui_on_detach, ed_gui_update_variable, ed_gui_update_fixed, ed_gui_update_after);
    if (layerid == NULL) {
        TLERROR("tl_application_initialize: Failed to create layer \"Editor GUI\"");
        return false;
    }

    return true;
}

b8 tl_application_terminate() {
    if (!tl_layer_stack_destroy(layerid)) {
        TLERROR("tl_application_terminate: Failed to destroy layer  \"Editor GUI\"");
        return false;
    }
    return true;
}

static b8 ed_gui_on_attach(void) {
    return true;
}

static b8 ed_gui_on_detach(void) {
    return true;
}

static b8 ed_gui_update_variable(const u64 delta) {
    return true;
}

static b8 ed_gui_update_fixed(const u64 delta) {
    return true;
}

static b8 ed_gui_update_after(void) {
    return true;
}