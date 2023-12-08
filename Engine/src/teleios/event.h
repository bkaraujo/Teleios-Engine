#ifndef TELEIOS_EVENT_H
#define TELEIOS_EVENT_H

#include "teleios/types.h"

b8 tl_event_initialize(void);
b8 tl_event_terminate(void);

void tl_event_fire(const u8 code, const TLEvent* event);

typedef TLEventStatus(*PFN_EventHandler)(const u8, const TLEvent*);
b8 tl_event_subscribe(const u8 code, PFN_EventHandler handler);

#endif // TELEIOS_EVENT_H
