#ifndef TELEIOS_DEFINES_H
#define TELEIOS_DEFINES_H


#if !(defined(TLSTATIC) || defined(TLEXPORT) || defined(TLIMPORT))
#   error "One of the compiler symbles must be defined TLSTATIC/TLEXPORT/TLIMPORT"
#endif

#ifdef TLSTATIC
#   if defined(TLEXPORT) || defined(TLIMPORT)
#       error "Compiler symble TLEXPORT/TLIMPORT cannot be used with TLSTATIC"
#   endif
#endif

#ifdef TLEXPORT
#   if defined(TLIMPORT)
#       error "Compiler symble TLIMPORT cannot be used with TLEXPORT"
#   endif
#endif


#if defined(_MSC_VER)                                                   // For Windows
#   define TLINLINE __forceinline
#   if defined(TLEXPORT)
#       define TLAPI __declspec(dllexport)
#   elif defined (TLIMPORT)
#       define TLAPI __declspec(dllimport)
#   else
#       define TLAPI
#   endif
#else                                                                   // For Others
#   define TLINLINE static inline __attribute((always_inline))
#   if defined(TLEXPORT) || defined(TLIMPORT)
#       define TLAPI __attribute__((visibility("default")))
#   else
#       define TLAPI
#   endif
#endif


#endif // TELEIOS_DEFINES_H
