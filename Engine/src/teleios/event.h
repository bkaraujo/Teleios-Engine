#ifndef TELEIOS_EVENT_H
#define TELEIOS_EVENT_H

#include "teleios/types.h"

#define TL_EVENT_APPLICATION_QUIT  200

typedef enum {
    /// <summary>
    /// The event should keep walking the event chain.
    /// </summary>
    TL_EVENT_STATUS_CONTUNE,

    /// <summary>
    /// The event should be consumed and stop walkin the event chain.
    /// </summary>
    TL_EVENT_STATUS_STOP
} TLEventStatus;

typedef struct {
    // 128 bytes
    union {
        i64 i64[2];
        u64 u64[2];

        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];

        u16 u16[8];

        i8 i8[16];
        u8 u8[16];

        char c[16];
    } data;
} TLEvent;

b8 tl_event_initialize(void);
b8 tl_event_terminate(void);

void tl_event_fire(const u8 code, const TLEvent* event);

typedef TLEventStatus(*PFN_EventHandler)(const u8, const TLEvent*);
b8 tl_event_subscribe(const u8 code, PFN_EventHandler handler);

#endif // TELEIOS_EVENT_H
