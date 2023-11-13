#include "teleios/sring.h"

#include <string.h>

TLAPI u64 tl_string_length(const char* string) {
  return strlen(string);
}

TLAPI b8 tl_string_equals(const char* first, const char* second) {
  u64 length = tl_string_length(first);
  if (tl_string_length(second) != length) return false;
  
  for (unsigned i = 0 ; i < length; ++i)
    if (first[i] != second[i])
      return false;

  return true;
}