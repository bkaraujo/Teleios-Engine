#ifndef TELEIOS_PLATFORM_H
#define TELEIOS_PLATFORM_H

#include "teleios/types.h"

b8 tl_platform_initialize(void);
b8 tl_platform_terminate(void);
b8 tl_platform_update(void);

void* tl_platform_handle(void);

void tl_platform_stdout(const u8 level, const char* message);

void* tl_platform_memory_alloc(u64 size);
void tl_platform_memory_free(const void* block);
void tl_platform_memory_copy(const void* source, const void* target, u64 size);
void tl_platform_memory_set(const void* target, i32 value, u64 size);

void tl_platform_time_now(TLDateTime* dt);
const u64 tl_platform_time_epoch_micros(void);

void* tl_platform_window_handle(void);
b8 tl_platform_window_create(const TLSpecification* spec);
void tl_platform_window_set_title(const char* title, ...);
void tl_platform_window_show(void);
void tl_platform_window_hide(void);

#endif // TELEIOS_PLATFORM_H
