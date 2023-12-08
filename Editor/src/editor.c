#include <teleios/application.h>
#include <teleios/logger.h>

void tl_application_define(TLSpecification* spec) {
    spec->name = "Teleios Editor";
}

b8 tl_application_initialize(const TLSpecification* spec) {
    return true;
}

b8 tl_application_terminate(void) {
    return true;
}