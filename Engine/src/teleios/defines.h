#ifndef TELEIOS_DEFINES_H
#define TELEIOS_DEFINES_H


#if !(defined(TELEIOS_STATIC) || defined(TELEIOS_EXPORT) || defined(TELEIOS_IMPORT))
#   error "One of the compiler symbles must be defined TELEIOS_STATIC/TELEIOS_EXPORT/TELEIOS_IMPORT"
#endif

#ifdef TELEIOS_STATIC
#   if defined(TELEIOS_EXPORT) || defined(TELEIOS_IMPORT)
#       error "Compiler symble TELEIOS_EXPORT/TELEIOS_IMPORT cannot be used with TELEIOS_STATIC"
#   endif
#endif

#ifdef TELEIOS_EXPORT
#   if defined(TELEIOS_IMPORT)
#       error "Compiler symble TELEIOS_IMPORT cannot be used with TELEIOS_EXPORT"
#   endif
#endif


#if defined(_MSC_VER)                                                   // For Windows
#   define TLINLINE __forceinline
#   if defined(TELEIOS_EXPORT)
#       define TLAPI __declspec(dllexport)
#   elif defined (TELEIOS_IMPORT)
#       define TLAPI __declspec(dllimport)
#   else
#       define TLAPI
#   endif
#else                                                                   // For Others
#   define TLINLINE static inline __attribute((always_inline))
#   if defined(TELEIOS_EXPORT) || defined(TELEIOS_IMPORT)
#       define TLAPI __attribute__((visibility("default")))
#   else
#       define TLAPI
#   endif
#endif


#endif // TELEIOS_DEFINES_H
