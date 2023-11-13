#ifndef TELEIOS_EVENT_SUBSCRIBER_H
#define TELEIOS_EVENT_SUBSCRIBER_H

#include "teleios/types.h"

typedef b8(*PFN_EventHandler)(TLEvent*);
void tl_event_subscribe(u8 code, PFN_EventHandler handler);

#endif // TELEIOS_EVENT_SUBSCRIBER_H
