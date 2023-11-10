#include "teleios/platform/winapi/externals.h"
#include "teleios/platform/console.h"
#include "teleios/platform/memory.h"
#include "teleios/sring.h"
#include <malloc.h>
#define TL_CONSOLE_BLACK        ( 0 )
#define TL_CONSOLE_DARKBLUE     ( FOREGROUND_BLUE )
#define TL_CONSOLE_DARKGREEN    ( FOREGROUND_GREEN )
#define TL_CONSOLE_DARKCYAN     ( FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_DARKRED      ( FOREGROUND_RED )
#define TL_CONSOLE_DARKMAGENTA  ( FOREGROUND_RED | FOREGROUND_BLUE )
#define TL_CONSOLE_DARKYELLOW   ( FOREGROUND_RED | FOREGROUND_GREEN )
#define TL_CONSOLE_DARKGRAY     ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_GRAY         ( FOREGROUND_INTENSITY )
#define TL_CONSOLE_BLUE         ( FOREGROUND_INTENSITY | FOREGROUND_BLUE )
#define TL_CONSOLE_GREEN        ( FOREGROUND_INTENSITY | FOREGROUND_GREEN )
#define TL_CONSOLE_CYAN         ( FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE )
#define TL_CONSOLE_RED          ( FOREGROUND_INTENSITY | FOREGROUND_RED )
#define TL_CONSOLE_MAGENTA      ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE )
#define TL_CONSOLE_YELLOW       ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN )
#define TL_CONSOLE_WHITE        ( FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE )

static const u8 levels[6] = { 
  BACKGROUND_RED | TL_CONSOLE_WHITE,
  TL_CONSOLE_DARKRED,
  TL_CONSOLE_DARKYELLOW,
  TL_CONSOLE_DARKGRAY,
  TL_CONSOLE_DARKCYAN,
  TL_CONSOLE_DARKMAGENTA
};

void tl_platform_stdout(u8 level, const char* message) {
  
  // Backup console screen buffer info
  CONSOLE_SCREEN_BUFFER_INFO csbi; 
  GetConsoleScreenBufferInfo(hconsole, &csbi);
  // transforme the ansi to utf8
  i32 needed = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message, -1, NULL, 0);
  u16* utf8 = _malloca(sizeof(u16) * needed);
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, message, -1, utf8, needed);
  
  // Apply the console color and print the message
  SetConsoleTextAttribute(hconsole, levels[level]);
  WriteConsole(hconsole, utf8, needed, NULL, 0);
  
  // Free and Restore resources
  _freea(utf8);
  SetConsoleTextAttribute(hconsole, csbi.wAttributes);
}
