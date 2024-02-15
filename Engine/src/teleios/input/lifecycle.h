#ifndef TELEIOS_INPUT_LIFECYCLE_H
#define TELEIOS_INPUT_LIFECYCLE_H

#include "teleios/types.h"

b8 tl_input_initialize(void);
b8 tl_input_terminate(void);
void tl_input_update(void);

#endif // TELEIOS_INPUT_LIFECYCLE_H