#ifndef TELEIOS_TYPES_H
#define TELEIOS_TYPES_H

#include <stddef.h>

typedef unsigned char       u8;   // 255
typedef unsigned short      u16;  // 65_535
typedef unsigned int        u32;  // 4_294_967_295
typedef unsigned long long  u64;  // 18_446_744_073_709_551_615

#define U8MAX               255
#define U16MAX              65535
#define U32MAX              4294967295
#define U64MAX              18446744073709551615

typedef signed char         i8;  // -128 +127
typedef signed short        i16; // -32_768 +32_767
typedef signed int          i32; // -2_147_483_648 to 2_147_483_647
typedef signed long long    i64; // -9_223_372_036_854_775_808 to 9_223_372_036_854_775_807

#define I8MIN               -128
#define I16MIN              -32768
#define I32MIN              -2147483648
#define I64MIN              -9223372036854775808

#define I8MAX               127
#define I16MAX              32767
#define I32MAX              2147483647
#define I64MAX              9223372036854775807

typedef float               f32; //	1.2E-38 to 3.4E+38
typedef double              f64; // 2.3E-308 to 1.7E+308

typedef signed char         b8;

#define true                1
#define false               0

#define KiB(b)              (b * 1024)
#define MiB(b)              (KiB(b) * 1024)
#define GiB(b)              (MiB(b) * 1024)

typedef enum {
  TL_MEMORY_TYPE_CONTAINER_LIST,
  TL_MEMORY_TYPE_CONTAINER_NODE,
  TL_MEMORY_TYPE_OBJECT,
  TL_MEMORY_TYPE_OBJECT_DEFINED,
  TL_MEMORY_TYPE_MAXIMUM
} TLEMemoryType;

#define TL_EVENT_STOP true
#define TL_EVENT_CONTINUE false

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

typedef struct {
  u16 year;
  u8  month;
  u8  day;
  u8  hour;
  u8  minute;
  u8  second;
  u16 milliseconds;
} TLDateTime;

typedef struct {
  u64 start;
  u64 current;
} TLTimer;

typedef struct TLNode{
  void* payload;
  struct TLNode* next;
  struct TLNode* previous;
} TLNode;

typedef struct {
  TLNode* head;
  TLNode* tail;
  u16 size;
} TLList;

typedef struct {
  /* General Data */
  const char* name;
  const char identity[38];
  const u8 refs;
  
  /* Particular Data */
  const void* pointer;
  const u64 stride;
  void (*destructor)(const void*);

} TLObject;

typedef struct {
  const char* name;

  struct {
    u32 witdh;
    u32 height;
  } window;
} TLSpecification;

#endif // TELEIOS_TYPES_H
