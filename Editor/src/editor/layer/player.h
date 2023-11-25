#pragma once
#include <teleios/types.h>

b8 editor_player_on_attach(void);
b8 editor_player_on_detach(void);
b8 editor_player_update_variable(const u64 delta);
b8 editor_player_update_fixed(const u64 delta);
b8 editor_player_update_after(void);
