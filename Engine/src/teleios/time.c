#include "teleios/platform/time.h"
#include "teleios/time.h"

TLAPI void tl_time_now(TLDateTime* dt) {
    tl_platform_time_now(dt);
}

TLAPI const u64 tl_time_epoch_seconds(void) {
    return tl_platform_time_epoch_micros() / 1000000;
}

TLAPI const u64 tl_time_epoch_millis(void) {
    return tl_platform_time_epoch_micros() / 100;
}

TLAPI const u64 tl_time_epoch_micros(void) {
    return tl_platform_time_epoch_micros();
}
