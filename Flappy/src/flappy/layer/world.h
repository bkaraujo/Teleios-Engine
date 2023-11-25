#pragma once
#include <teleios/types.h>

b8 flappy_world_on_attach(void);
b8 flappy_world_on_detach(void);
b8 flappy_world_update_variable(const u64 delta);
b8 flappy_world_update_fixed(const u64 delta);
b8 flappy_world_update_after(void);
