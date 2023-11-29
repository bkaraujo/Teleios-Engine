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

typedef enum {
    TL_MOUSE_BUTTON_LEFT,
    TL_MOUSE_BUTTON_RIGHT,
    TL_MOUSE_BUTTON_MIDDLE,

    TL_MOUSE_BUTTON_MAXIMUM
} TLMouseButton;

typedef enum {
    TL_KEY_BACKSPACE = 0x08,
    TL_KEY_ENTER = 0x0D,
    TL_KEY_TAB = 0x09,
    TL_KEY_SHIFT = 0x10,
    TL_KEY_CONTROL = 0x11,

    TL_KEY_PAUSE = 0x13,
    TL_KEY_CAPITAL = 0x14,

    TL_KEY_ESCAPE = 0x1B,

    TL_KEY_CONVERT = 0x1C,
    TL_KEY_NONCONVERT = 0x1D,
    TL_KEY_ACCEPT = 0x1E,
    TL_KEY_MODECHANGE = 0x1F,

    TL_KEY_SPACE = 0x20,
    TL_KEY_PAGEUP = 0x21,
    TL_KEY_PAGEDOWN = 0x22,
    TL_KEY_END = 0x23,
    TL_KEY_HOME = 0x24,
    TL_KEY_LEFT = 0x25,
    TL_KEY_UP = 0x26,
    TL_KEY_RIGHT = 0x27,
    TL_KEY_DOWN = 0x28,
    TL_KEY_SELECT = 0x29,
    TL_KEY_PRINT = 0x2A,
    TL_KEY_EXECUTE = 0x2B,
    TL_KEY_PRINTSCREEN = 0x2C,
    TL_KEY_INSERT = 0x2D,
    TL_KEY_DELETE = 0x2E,
    TL_KEY_HELP = 0x2F,

    TL_KEY_0 = 0x30,
    TL_KEY_1 = 0x31,
    TL_KEY_2 = 0x32,
    TL_KEY_3 = 0x33,
    TL_KEY_4 = 0x34,
    TL_KEY_5 = 0x35,
    TL_KEY_6 = 0x36,
    TL_KEY_7 = 0x37,
    TL_KEY_8 = 0x38,
    TL_KEY_9 = 0x39,

    TL_KEY_A = 0x41,
    TL_KEY_B = 0x42,
    TL_KEY_C = 0x43,
    TL_KEY_D = 0x44,
    TL_KEY_E = 0x45,
    TL_KEY_F = 0x46,
    TL_KEY_G = 0x47,
    TL_KEY_H = 0x48,
    TL_KEY_I = 0x49,
    TL_KEY_J = 0x4A,
    TL_KEY_K = 0x4B,
    TL_KEY_L = 0x4C,
    TL_KEY_M = 0x4D,
    TL_KEY_N = 0x4E,
    TL_KEY_O = 0x4F,
    TL_KEY_P = 0x50,
    TL_KEY_Q = 0x51,
    TL_KEY_R = 0x52,
    TL_KEY_S = 0x53,
    TL_KEY_T = 0x54,
    TL_KEY_U = 0x55,
    TL_KEY_V = 0x56,
    TL_KEY_W = 0x57,
    TL_KEY_X = 0x58,
    TL_KEY_Y = 0x59,
    TL_KEY_Z = 0x5A,

    TL_KEY_LSUPER = 0x5B,
    TL_KEY_RSUPER = 0x5C,
    TL_KEY_APPS = 0x5D,
    TL_KEY_SLEEP = 0x5F,

    TL_KEY_NUMPAD0 = 0x60,
    TL_KEY_NUMPAD1 = 0x61,
    TL_KEY_NUMPAD2 = 0x62,
    TL_KEY_NUMPAD3 = 0x63,
    TL_KEY_NUMPAD4 = 0x64,
    TL_KEY_NUMPAD5 = 0x65,
    TL_KEY_NUMPAD6 = 0x66,
    TL_KEY_NUMPAD7 = 0x67,
    TL_KEY_NUMPAD8 = 0x68,
    TL_KEY_NUMPAD9 = 0x69,
    TL_KEY_MULTIPLY = 0x6A,
    TL_KEY_ADD = 0x6B,
    TL_KEY_SEPARATOR = 0x6C,
    TL_KEY_SUBTRACT = 0x6D,
    TL_KEY_DECIMAL = 0x6E,
    TL_KEY_DIVIDE = 0x6F,

    TL_KEY_F1 = 0x70,
    TL_KEY_F2 = 0x71,
    TL_KEY_F3 = 0x72,
    TL_KEY_F4 = 0x73,
    TL_KEY_F5 = 0x74,
    TL_KEY_F6 = 0x75,
    TL_KEY_F7 = 0x76,
    TL_KEY_F8 = 0x77,
    TL_KEY_F9 = 0x78,
    TL_KEY_F10 = 0x79,
    TL_KEY_F11 = 0x7A,
    TL_KEY_F12 = 0x7B,
    TL_KEY_F13 = 0x7C,
    TL_KEY_F14 = 0x7D,
    TL_KEY_F15 = 0x7E,
    TL_KEY_F16 = 0x7F,
    TL_KEY_F17 = 0x80,
    TL_KEY_F18 = 0x81,
    TL_KEY_F19 = 0x82,
    TL_KEY_F20 = 0x83,
    TL_KEY_F21 = 0x84,
    TL_KEY_F22 = 0x85,
    TL_KEY_F23 = 0x86,
    TL_KEY_F24 = 0x87,

    TL_KEY_NUMLOCK = 0x90,
    TL_KEY_SCROLL = 0x91,
    TL_KEY_NUMPAD_EQUAL = 0x92,

    TL_KEY_LSHIFT = 0xA0,
    TL_KEY_RSHIFT = 0xA1,
    TL_KEY_LCONTROL = 0xA2,
    TL_KEY_RCONTROL = 0xA3,
    TL_KEY_LALT = 0xA4,
    TL_KEY_RALT = 0xA5,

    TL_KEY_SEMICOLON = 0x3B,

    TL_KEY_APOSTROPHE = 0xDE,
    TL_KEY_QUOTE = TL_KEY_APOSTROPHE,
    TL_KEY_EQUAL = 0xBB,
    TL_KEY_COMMA = 0xBC,
    TL_KEY_MINUS = 0xBD,
    TL_KEY_PERIOD = 0xBE,
    TL_KEY_SLASH = 0xBF,

    TL_KEY_GRAVE = 0xC0,

    TL_KEY_LBRACKET = 0xDB,
    TL_KEY_PIPE = 0xDC,
    TL_KEY_BACKSLASH = TL_KEY_PIPE,
    TL_KEY_RBRACKET = 0xDD,

    TL_KEYS_MAXIMUM = 0xFF
} TLKeyboardKey;

typedef struct {
    const char* name;

    struct {
        u32 witdh;
        u32 height;
    } window;

    struct {
        struct {
            TLList* extentions;
            TLList* layers;
        } instance;
        struct {
            TLList* extentions;
        } device;
    } vulkan;
} TLSpecification;

#endif // TELEIOS_TYPES_H
