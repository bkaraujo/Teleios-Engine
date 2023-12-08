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

#define KiB(b)              ((f64)(b / 1024.0))
#define MiB(b)              ((f64)(KiB(b) / 1024.0))
#define GiB(b)              ((f64)(MiB(b) / 1024.0))

#define NANOSECOND         1000000000
#define MICROSECOND        1000000
#define MILISECOND         1000
#define SECOND             1

#define BIT(x)             (1 << x)
#define BITVALUE(x, n)     (((x) >> (n)) & 0x1)

#define MSG_PLEASE_DO_IT_YOURSELF "Please do it yourself at tl_application_terminate(void)"

#include "cglm/types-struct.h"

typedef enum {
    TL_MEMORY_TYPE_IDENTITY,
    TL_MEMORY_TYPE_LAYER,
    TL_MEMORY_TYPE_CONTAINER_LIST,
    TL_MEMORY_TYPE_CONTAINER_NODE,
    TL_MEMORY_TYPE_SCENE,
    TL_MEMORY_TYPE_SCENE_REGION,
    TL_MEMORY_TYPE_ECS_ENTITY,
    TL_MEMORY_TYPE_ECS_SYSTEM,
    TL_MEMORY_TYPE_ECS_COMPONENT,
    TL_MEMORY_TYPE_GRAPHICS,
    TL_MEMORY_TYPE_STRING,
    TL_MEMORY_TYPE_FILE,
    TL_MEMORY_TYPE_MAXIMUM
} TLEMemoryType;

typedef enum {
    /// <summary>
    /// The event should keep walking the event chain.
    /// </summary>
    TL_EVENT_STATUS_CONTUNE,

    /// <summary>
    /// The event should be consumed and stop walkin the event chain.
    /// </summary>
    TL_EVENT_STATUS_STOP
} TLEventStatus;

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
    u64 start;   // Ticks from boot
    u64 current; // Ticks from 'start'
} TLTimer;

typedef struct TLNode {
    const void* payload;
    struct TLNode* next;
    struct TLNode* previous;
} TLNode;

typedef struct {
    TLNode* head;
    TLNode* tail;
    u16 size;
} TLList;

typedef struct {
    const char identity[38];
} TLIdentity;

typedef struct {
    const TLIdentity* identity;
} TLComponent;

typedef struct {
    TLComponent owner;
    ivec3s scale;
    ivec3s position;
} TLComponentTransform;

typedef struct {
    TLComponent owner;
    const char* name;
} TLComponentName;

typedef struct {
    TLIdentity* identity;
    const char* name;

    b8(*on_attach)(void);
    b8(*on_detach)(void);

    b8(*update_variable)(const u64 delta);
    b8(*update_fixed)(const u64 delta);
    b8(*update_after)(void);
} TLLayer;

typedef struct {
    /* General Data */
    const char* name;
    const char identity[38];
    const u8 refs;

    /* Particular Data */
    void* pointer;
    const u64 stride;
    void (*destructor)(const void*);

} TLObject;

typedef struct {
    const char* name;
    struct {
        u8 per_second;
    } simulation;
    struct {
        u32 witdh;
        u32 height;
    } window;

} TLSpecification;

#endif // TELEIOS_TYPES_H
