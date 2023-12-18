#ifndef TELEIOS_CONCURRENT_H
#define TELEIOS_CONCURRENT_H

#include "teleios/defines.h"
#include "teleios/types.h"

// ##############################################################################################
//
//                                        THREAD
//
// ##############################################################################################
u64 tl_thread_current_id(void);
void tl_thread_current_sleep(u64 millis);

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
// ##############################################################################################
//
//                                        MUTEX
//
// ##############################################################################################
#include "teleios/platform_detector.h"
#ifdef TELEIOS_PLATFORM_WINDOWS
#include <minwindef.h>
#include <minwinbase.h>

void tl_mutex_lock(CRITICAL_SECTION* mutex);
void tl_mutex_unlock(CRITICAL_SECTION* mutex);
#else
void* tl_mutex_create(void);
void tl_mutex_lock(void* mutex);
void tl_mutex_unlock(void* mutex);
void tl_mutex_destroy(void* mutex);
#endif // TELEIOS_PLATFORM_WINDOWS


#endif // TELEIOS_CONCURRENT_H