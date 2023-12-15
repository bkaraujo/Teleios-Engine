#ifndef TELEIOS_THREAD_H
#define TELEIOS_THREAD_H

#include "teleios/types.h"

typedef struct {
    void* handle;
    u64 id;
    b8 running;
    b8 suspended;
} TLThread;

TLThread* tl_thread_create(b8 detach, const void* funtion, const void* parameters);
b8 tl_thread_destroy(TLThread* thread);
b8 tl_thread_detach(TLThread* thread);
b8 tl_thread_wait(TLThread* thread);

u64 tl_thread_current_id(void);
void tl_thread_current_sleep(u64 millis);

#endif // TELEIOS_THREAD_H