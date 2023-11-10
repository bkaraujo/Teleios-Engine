#ifndef TELEIOS_TYPES_H
#define TELEIOS_TYPES_H

#include <stddef.h>

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef signed char         i8;
typedef signed short        i16;
typedef signed int          i32;
typedef signed long long    i64;

typedef float               f32;
typedef double              f64;

typedef signed char         b8;

#define true                1
#define false               0

typedef struct {
  u16 year;
  u8 month;
  u8 day;
  u8  hour;
  u8 minute;
  u8 second;
  u16 milliseconds;
} TLDateTime;

#endif // TELEIOS_TYPES_H
