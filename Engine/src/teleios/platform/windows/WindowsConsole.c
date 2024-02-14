#include "teleios/platform/Detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"
#include "teleios/platform/Console.h"
#include "teleios/string.h"

typedef enum {
    BlackFore = 0,
    MaroonFore = FOREGROUND_RED,
    GreenFore = FOREGROUND_GREEN,
    NavyFore = FOREGROUND_BLUE,
    TealFore = FOREGROUND_GREEN | FOREGROUND_BLUE,
    OliveFore = FOREGROUND_RED | FOREGROUND_GREEN,
    PurpleFore = FOREGROUND_RED | FOREGROUND_BLUE,
    GrayFore = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    SilverFore = FOREGROUND_INTENSITY,
    RedFore = FOREGROUND_INTENSITY | FOREGROUND_RED,
    LimeFore = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    BlueFore = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    AquaFore = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    YellowFore = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    FuchsiaFore = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    WhiteFore = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,

    BlackBack = 0,
    MaroonBack = BACKGROUND_RED,
    GreenBack = BACKGROUND_GREEN,
    NavyBack = BACKGROUND_BLUE,
    TealBack = BACKGROUND_GREEN | BACKGROUND_BLUE,
    OliveBack = BACKGROUND_RED | BACKGROUND_GREEN,
    PurpleBack = BACKGROUND_RED | BACKGROUND_BLUE,
    GrayBack = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    SilverBack = BACKGROUND_INTENSITY,
    RedBack = BACKGROUND_INTENSITY | BACKGROUND_RED,
    LimeBack = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
    BlueBack = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
    AquaBack = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
    YellowBack = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
    FuchsiaBack = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
    WhiteBack = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
} ConsoleColors;

void tl_platform_console_write(u8 color, const i8* message) {
    static u8 levels[6] = { FuchsiaBack | WhiteFore, MaroonFore, YellowFore, LimeFore, BlueFore, SilverFore };

    // #######################################################
    // Backup ScreenBufferInfo
    // #######################################################
    GetConsoleScreenBufferInfo(e_hstdout, &e_csbi);
    // #######################################################
    // Actual Logging
    // #######################################################
    SetConsoleTextAttribute(e_hstdout, levels[color]);
#ifdef TELEIOS_DEBUG
    OutputDebugString(message);
#endif
    WriteConsole(e_hstdout, message, (DWORD)tl_string_length(message), NULL, 0);
    // #######################################################
    // Restores previous ScreenBufferInfo
    // #######################################################
    SetConsoleTextAttribute(e_hstdout, e_csbi.wAttributes);
}
#endif // TL_PLATFORM_WINDOWS