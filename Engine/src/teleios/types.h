#ifndef TELEIOS_TYPES_H
#define TELEIOS_TYPES_H

#include <stddef.h>
#include <stdbool.h>

#include "cglm/types-struct.h"

#if defined(_MSC_VER)
#   define TLINLINE __forceinline
#   define TLALIGN(bytes) __declspec(align(bytes))
#   define STATIC_ASSERT static_assert
#   define TLNORETURN __declspec(noreturn)
#   if defined(TELEIOS_EXPORT)
#       define TLEXPORT __declspec(dllexport)
#   else
#       define TLEXPORT __declspec(dllimport)
#   endif
#else
#   define STATIC_ASSERT _Static_assert
#   define TLALIGN(bytes) __attribute((aligned(bytes)))
#   define TLNORETURN __attribute__((noreturn))
#   define TLEXPORT __attribute__((visibility("default")))
#   define TLINLINE static inline __attribute((always_inline))
#endif

typedef unsigned char       u8;     // 255
typedef unsigned short      u16;    // 65_535
typedef unsigned int        u32;    // 2_147_483_647
typedef unsigned long long  u64;    // 18_446_744_073_709_551_615

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

#define U8MAX               255                     // 255
#define U16MAX              65535                   // 65_535
#define U32MAX              2147483647              // 2_147_483_647
#define U64MAX              18446744073709551615    // 18_446_744_073_709_551_615

typedef signed char         i8;     // -128 to 127
typedef signed short        i16;    // -32_768 to 32_767
typedef signed int          i32;    // -2_147_483_647 to 2_147_483_647
typedef signed long long    i64;    // -9_223_372_036_854_775_807 to 9_223_372_036_854_775_807

#define I8MAX               127                     // 127
#define I16MAX              32767                   // 32_767
#define I32MAX              2147483647              // 2_147_483_647
#define I64MAX              9223372036854775807     // 9_223_372_036_854_775_807

#define I8MIN               -128                    // -128
#define I16MIN              -32768                  // -32_768
#define I32MIN              -2147483647             // -2_147_483_647
#define I64MIN              -9223372036854775807    // -9_223_372_036_854_775_807

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

typedef float               f32;
typedef double              f64;

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

typedef u8                  b8;

#define TLSECONDS             1e+0    // (double)(1.0)
#define TLMILLIS              1e+3    // (double)(1_000.0)
#define TLMICROS              1e+6    // (double)(1_000_000.0)
#define TLNANOS               1e+9    // (double)(1_000_000_000.0)

#define PSIZE                 8

typedef enum {
    TL_MEMORY_TYPE_CONTAINER_ARRAY,
    TL_MEMORY_TYPE_MAXIMUM
} TLMemoryType;

typedef struct TLMap TLMap;
typedef struct TLArray TLArray;

#define TLArraySize 16

typedef struct TLEvent {
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
    } payload;
} TLEvent;

typedef struct TLTime {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u16 millis;
} TLTime;

typedef struct TLTimer {
    u64 start;
    u64 current;
} TLTimer;

typedef struct {
    struct {
        u8 major;
        u8 minor;
        u8 patch;
    } version;

    struct {
        const char* title;
        u32 width;
        u32 height;
    } window;

    struct {
        b8 vsync;
    } graphics;
} TLSpecification;

#endif // TELEIOS_TYPES_H