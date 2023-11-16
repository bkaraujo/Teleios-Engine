#include "teleios/identity.h"
#include "teleios/logger.h"

#include <stdlib.h>
#include <string.h>

static const char UUID[] = "0123456789abcdef";

void tl_identity_initialize(const TLIdentity* identity) {
  for (unsigned i = 0; i < 37; ++i) {
    *((i8*)&identity->identity[i]) = UUID[rand() % 16];
  }

  *((i8*)&identity->identity[8]) = '-';
  *((i8*)&identity->identity[13]) = '-';
  *((i8*)&identity->identity[18]) = '-';
  *((i8*)&identity->identity[23]) = '-';
  *((i8*)&identity->identity[37]) = 0; // null-terminated
}

b8 tl_identity_compare(const TLIdentity* first, const TLIdentity* second) {
  if (strcmp(first->identity, second->identity) != 0)
    return false;

  return true;
}