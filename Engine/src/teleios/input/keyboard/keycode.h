#pragma once
#ifndef TELEIOS_INPUT_KEYBOARD_KEYCODE_H
#define TELEIOS_INPUT_KEYBOARD_KEYCODE_H

typedef enum {
    /** @brief The backspace key. */
    TL_KEY_BACKSPACE = 0x08,
    /** @brief The enter key. */
    TL_KEY_ENTER = 0x0D,
    /** @brief The tab key. */
    TL_KEY_TAB = 0x09,
    /** @brief The shift key. */
    TL_KEY_SHIFT = 0x10,
    /** @brief The Control/Ctrl key. */
    TL_KEY_CONTROL = 0x11,

    /** @brief The pause key. */
    TL_KEY_PAUSE = 0x13,
    /** @brief The Caps Lock key. */
    TL_KEY_CAPITAL = 0x14,

    /** @brief The Escape key. */
    TL_KEY_ESCAPE = 0x1B,

    TL_KEY_CONVERT = 0x1C,
    TL_KEY_NONCONVERT = 0x1D,
    TL_KEY_ACCEPT = 0x1E,
    TL_KEY_MODECHANGE = 0x1F,

    /** @brief The spacebar key. */
    TL_KEY_SPACE = 0x20,
    /** @brief The page up key. */
    TL_KEY_PAGEUP = 0x21,
    /** @brief The page down key. */
    TL_KEY_PAGEDOWN = 0x22,
    /** @brief The end key. */
    TL_KEY_END = 0x23,
    /** @brief The home key. */
    TL_KEY_HOME = 0x24,
    /** @brief The left arrow key. */
    TL_KEY_LEFT = 0x25,
    /** @brief The up arrow key. */
    TL_KEY_UP = 0x26,
    /** @brief The right arrow key. */
    TL_KEY_RIGHT = 0x27,
    /** @brief The down arrow key. */
    TL_KEY_DOWN = 0x28,
    TL_KEY_SELECT = 0x29,
    TL_KEY_PRINT = 0x2A,
    TL_KEY_EXECUTE = 0x2B,
    /** @brief The Print Screen key. */
    TL_KEY_PRINTSCREEN = 0x2C,
    /** @brief The insert key. */
    TL_KEY_INSERT = 0x2D,
    /** @brief The delete key. */
    TL_KEY_DELETE = 0x2E,
    TL_KEY_HELP = 0x2F,

    /** @brief The 0 key */
    TL_KEY_0 = 0x30,
    /** @brief The 1 key */
    TL_KEY_1 = 0x31,
    /** @brief The 2 key */
    TL_KEY_2 = 0x32,
    /** @brief The 3 key */
    TL_KEY_3 = 0x33,
    /** @brief The 4 key */
    TL_KEY_4 = 0x34,
    /** @brief The 5 key */
    TL_KEY_5 = 0x35,
    /** @brief The 6 key */
    TL_KEY_6 = 0x36,
    /** @brief The 7 key */
    TL_KEY_7 = 0x37,
    /** @brief The 8 key */
    TL_KEY_8 = 0x38,
    /** @brief The 9 key */
    TL_KEY_9 = 0x39,

    /** @brief The A key. */
    TL_KEY_A = 0x41,
    /** @brief The B key. */
    TL_KEY_B = 0x42,
    /** @brief The C key. */
    TL_KEY_C = 0x43,
    /** @brief The D key. */
    TL_KEY_D = 0x44,
    /** @brief The E key. */
    TL_KEY_E = 0x45,
    /** @brief The F key. */
    TL_KEY_F = 0x46,
    /** @brief The G key. */
    TL_KEY_G = 0x47,
    /** @brief The H key. */
    TL_KEY_H = 0x48,
    /** @brief The I key. */
    TL_KEY_I = 0x49,
    /** @brief The J key. */
    TL_KEY_J = 0x4A,
    /** @brief The K key. */
    TL_KEY_K = 0x4B,
    /** @brief The L key. */
    TL_KEY_L = 0x4C,
    /** @brief The M key. */
    TL_KEY_M = 0x4D,
    /** @brief The N key. */
    TL_KEY_N = 0x4E,
    /** @brief The O key. */
    TL_KEY_O = 0x4F,
    /** @brief The P key. */
    TL_KEY_P = 0x50,
    /** @brief The Q key. */
    TL_KEY_Q = 0x51,
    /** @brief The R key. */
    TL_KEY_R = 0x52,
    /** @brief The S key. */
    TL_KEY_S = 0x53,
    /** @brief The T key. */
    TL_KEY_T = 0x54,
    /** @brief The U key. */
    TL_KEY_U = 0x55,
    /** @brief The V key. */
    TL_KEY_V = 0x56,
    /** @brief The W key. */
    TL_KEY_W = 0x57,
    /** @brief The X key. */
    TL_KEY_X = 0x58,
    /** @brief The Y key. */
    TL_KEY_Y = 0x59,
    /** @brief The Z key. */
    TL_KEY_Z = 0x5A,

    /** @brief The left Windows/Super key. */
    TL_KEY_LSUPER = 0x5B,
    /** @brief The right Windows/Super key. */
    TL_KEY_RSUPER = 0x5C,
    /** @brief The applicatons key. */
    TL_KEY_APPS = 0x5D,

    /** @brief The sleep key. */
    TL_KEY_SLEEP = 0x5F,

    /** @brief The numberpad 0 key. */
    TL_KEY_NUMPAD0 = 0x60,
    /** @brief The numberpad 1 key. */
    TL_KEY_NUMPAD1 = 0x61,
    /** @brief The numberpad 2 key. */
    TL_KEY_NUMPAD2 = 0x62,
    /** @brief The numberpad 3 key. */
    TL_KEY_NUMPAD3 = 0x63,
    /** @brief The numberpad 4 key. */
    TL_KEY_NUMPAD4 = 0x64,
    /** @brief The numberpad 5 key. */
    TL_KEY_NUMPAD5 = 0x65,
    /** @brief The numberpad 6 key. */
    TL_KEY_NUMPAD6 = 0x66,
    /** @brief The numberpad 7 key. */
    TL_KEY_NUMPAD7 = 0x67,
    /** @brief The numberpad 8 key. */
    TL_KEY_NUMPAD8 = 0x68,
    /** @brief The numberpad 9 key. */
    TL_KEY_NUMPAD9 = 0x69,
    /** @brief The numberpad multiply key. */
    TL_KEY_MULTIPLY = 0x6A,
    /** @brief The numberpad add key. */
    TL_KEY_ADD = 0x6B,
    /** @brief The numberpad separator key. */
    TL_KEY_SEPARATOR = 0x6C,
    /** @brief The numberpad subtract key. */
    TL_KEY_SUBTRACT = 0x6D,
    /** @brief The numberpad decimal key. */
    TL_KEY_DECIMAL = 0x6E,
    /** @brief The numberpad divide key. */
    TL_KEY_DIVIDE = 0x6F,

    /** @brief The F1 key. */
    TL_KEY_F1 = 0x70,
    /** @brief The F2 key. */
    TL_KEY_F2 = 0x71,
    /** @brief The F3 key. */
    TL_KEY_F3 = 0x72,
    /** @brief The F4 key. */
    TL_KEY_F4 = 0x73,
    /** @brief The F5 key. */
    TL_KEY_F5 = 0x74,
    /** @brief The F6 key. */
    TL_KEY_F6 = 0x75,
    /** @brief The F7 key. */
    TL_KEY_F7 = 0x76,
    /** @brief The F8 key. */
    TL_KEY_F8 = 0x77,
    /** @brief The F9 key. */
    TL_KEY_F9 = 0x78,
    /** @brief The F10 key. */
    TL_KEY_F10 = 0x79,
    /** @brief The F11 key. */
    TL_KEY_F11 = 0x7A,
    /** @brief The F12 key. */
    TL_KEY_F12 = 0x7B,
    /** @brief The F13 key. */
    TL_KEY_F13 = 0x7C,
    /** @brief The F14 key. */
    TL_KEY_F14 = 0x7D,
    /** @brief The F15 key. */
    TL_KEY_F15 = 0x7E,
    /** @brief The F16 key. */
    TL_KEY_F16 = 0x7F,
    /** @brief The F17 key. */
    TL_KEY_F17 = 0x80,
    /** @brief The F18 key. */
    TL_KEY_F18 = 0x81,
    /** @brief The F19 key. */
    TL_KEY_F19 = 0x82,
    /** @brief The F20 key. */
    TL_KEY_F20 = 0x83,
    /** @brief The F21 key. */
    TL_KEY_F21 = 0x84,
    /** @brief The F22 key. */
    TL_KEY_F22 = 0x85,
    /** @brief The F23 key. */
    TL_KEY_F23 = 0x86,
    /** @brief The F24 key. */
    TL_KEY_F24 = 0x87,

    /** @brief The number lock key. */
    TL_KEY_NUMLOCK = 0x90,

    /** @brief The scroll lock key. */
    TL_KEY_SCROLL = 0x91,

    /** @brief The numberpad equal key. */
    TL_KEY_NUMPAD_EQUAL = 0x92,

    /** @brief The left shift key. */
    TL_KEY_LSHIFT = 0xA0,
    /** @brief The right shift key. */
    TL_KEY_RSHIFT = 0xA1,
    /** @brief The left control key. */
    TL_KEY_LCONTROL = 0xA2,
    /** @brief The right control key. */
    TL_KEY_RCONTROL = 0xA3,
    /** @brief The left alt key. */
    TL_KEY_LALT = 0xA4,
    /** @brief The right alt key. */
    TL_KEY_RALT = 0xA5,

    /** @brief The semicolon key. */
    TL_KEY_SEMICOLON = 0x3B,

    /** @brief The apostrophe/single-quote key */
    TL_KEY_APOSTROPHE = 0xDE,
    /** @brief An alias for TL_KEY_APOSTROPHE, apostrophe/single-quote key */
    TL_KEY_QUOTE = TL_KEY_APOSTROPHE,
    /** @brief The equal/plus key. */
    TL_KEY_EQUAL = 0xBB,
    /** @brief The comma key. */
    TL_KEY_COMMA = 0xBC,
    /** @brief The minus key. */
    TL_KEY_MINUS = 0xBD,
    /** @brief The period key. */
    TL_KEY_PERIOD = 0xBE,
    /** @brief The slash key. */
    TL_KEY_SLASH = 0xBF,

    /** @brief The grave key. */
    TL_KEY_GRAVE = 0xC0,

    /** @brief The left (square) bracket key e.g. [{ */
    TL_KEY_LBRACKET = 0xDB,
    /** @brief The pipe/backslash key */
    TL_KEY_PIPE = 0xDC,
    /** @brief An alias for the pipe/backslash key */
    TL_KEY_BACKSLASH = TL_KEY_PIPE,
    /** @brief The right (square) bracket key e.g. ]} */
    TL_KEY_RBRACKET = 0xDD,

    TL_KEY_MAXIMUM = 0xFF
} TLInputKeyboard;

#endif // TELEIOS_INPUT_KEYBOARD_KEYCODE_H