#ifndef TELEIOS_EVENT_SUBSCRIBER_H
#define TELEIOS_EVENT_SUBSCRIBER_H

#include "teleios/types.h"

typedef b8(*PFN_EventHandler)(u8,TLEvent*);

/// <summary>
/// Subscribe to an event for handling.
/// </summary>
/// <param name="code">Event code, or TL_EVENT_CODE_MAXIMUM for all events</param>
/// <param name="handler">Pointer to handler function</param>
b8 tl_event_subscribe(u8 code, PFN_EventHandler handler);

#endif // TELEIOS_EVENT_SUBSCRIBER_H
