#include "teleios/time.h"
#include "teleios/platform/time.h"

TLAPI void tl_time_now(TLDateTime* dt) {
  tl_platform_time_now(dt);
}

TLAPI const u64 tl_time_epoch_seconds(void) {
  return tl_platform_time_epoch_seconds();
}

TLAPI const u64 tl_time_epoch_millis(void) {
  return tl_platform_time_epoch_millis();
}

TLAPI const u64 tl_time_epoch_micros(void) {
  return tl_platform_time_epoch_micros();
}

TLAPI const u64 tl_time_epoch_nanos(void) {
  return tl_platform_time_epoch_nanos();
}
