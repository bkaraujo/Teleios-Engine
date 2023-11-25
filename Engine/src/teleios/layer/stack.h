#ifndef TELEIOS_LAYER_STACK_H
#define TELEIOS_LAYER_STACK_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI TLIdentity* tl_layer_stack_create(
    const char* name,
    b8(*on_attach)(void),
    b8(*on_detach)(void),
    b8(*update_variable)(const u64 delta),
    b8(*update_fixed)(const u64 delta),
    b8(*update_after)(void)
);

TLAPI b8 tl_layer_stack_destroy(TLIdentity* layerid);

#endif // TELEIOS_LAYER_STACK_H
