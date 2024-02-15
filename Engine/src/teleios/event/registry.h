#ifndef TELEIOS_EVENT_REGISTRY_H
#define TELEIOS_EVENT_REGISTRY_H

#include "Teleios/Types.h"

typedef b8(*PFN_handler)(u16 code, const TLEvent* event);

b8 tl_event_register(u16 code, PFN_handler handler);

#endif // TELEIOS_EVENT_REGISTRY_H
