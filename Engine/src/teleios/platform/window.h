#ifndef TELEIOS_PLATFORM_WINDOW_H
#define TELEIOS_PLATFORM_WINDOW_H

#include "teleios/types.h"

b8 tl_platform_window_create(TLSpecification* spec);
void tl_platform_window_show(void);
void tl_platform_window_hide(void);
void tl_platform_window_set_title(const char* title, ...);

#endif // TELEIOS_PLATFORM_WINDOW_H
