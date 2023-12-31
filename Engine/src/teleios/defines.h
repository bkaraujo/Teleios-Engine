#ifndef TELEIOS_DEFINES_H
#define TELEIOS_DEFINES_H

#ifdef TELEIOS_EXPORT
# ifdef _MSC_VER
#   define TLAPI __declspec(dllexport)
# else
#   define TLAPI __attribute__((visibility("default")))
# endif
#elif TELEIOS_IMPORT
# ifdef _MSC_VER
#   define TLAPI __declspec(dllimport)
# else
#   define TLAPI
# endif
#else 
# define TLAPI
#endif // TELEIOS_EXPORT

#endif // TELEIOS_DEFINES_H
