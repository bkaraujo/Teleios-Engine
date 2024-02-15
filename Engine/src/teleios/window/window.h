#ifndef TELEIOS_WINDOW_H
#define TELEIOS_WINDOW_H

#include "teleios/types.h"

b8 tl_window_create(const TLSpecification* spec);

void tl_window_show(void);
void tl_window_hide(void);
void tl_window_destroy(void);

#endif // TELEIOS_WINDOW_H