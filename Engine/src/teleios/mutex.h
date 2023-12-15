#ifndef TELEIOS_MUTEX_H
#define TELEIOS_MUTEX_H

#include "teleios/types.h"

void* tl_mutex_create(void);
b8 tl_mutex_lock(void*);
b8 tl_mutex_unlock(void*);
void tl_mutex_destroy(void*);

#endif // TELEIOS_MUTEX_H
