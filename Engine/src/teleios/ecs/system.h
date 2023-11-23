#ifndef TELEIOS_ECS_SYSTEM_H
#define TELEIOS_ECS_SYSTEM_H

#include "teleios/defines.h"
#include "teleios/types.h"

typedef const b8(*PFN_SYSTEM_TIMED)(const u64 delta);
typedef const b8(*PFN_SYSTEM_UNTIEMD)(void);

TLAPI b8 tl_ecs_system_register_untimed(PFN_SYSTEM_UNTIEMD system);
TLAPI b8 tl_ecs_system_register(b8 time_fixed, PFN_SYSTEM_TIMED system);
TLAPI b8 tl_ecs_system_unregister(b8 time_fixed, PFN_SYSTEM_TIMED system);

b8 tl_ecs_system_process(void);
b8 tl_ecs_system_process_time_fixed(const u64 delta);
b8 tl_ecs_system_process_time_delta(const u64 delta);

#endif // TELEIOS_ECS_SYSTEM_H
