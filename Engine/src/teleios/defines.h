#ifndef TELEIOS_DEFINES_H
#define TELEIOS_DEFINES_H

// Check the usage of compiler symbols
#if ! ( defined(TLSTATIC) || defined(TLEXPORT) || defined(TLIMPORT) )
#   error "TWhen TLSTATIC is undefined TLEXPORT or TLIMPORT must be defined"
#endif

#ifdef TLSTATIC
#   if defined(TLEXPORT) || defined(TLIMPORT)
#       error "TWhen TLSTATIC is defined TLEXPORT/TLIMPORT must be undefined"
#   endif
#endif

#ifdef TLEXPORT
#   if defined(TLIMPORT)
#       error "When TLEXPORT is defined TLIMPORT must be undefined"
#   endif
#endif

#ifdef _MSC_VER // For Windows 
#   define TLINLINE __forceinline
#   ifdef TLSTATIC
#       define TLAPI
#   elif defined(TLEXPORT)
#       define TLAPI __declspec(dllexport)
#   elif defined(TLIMPORT)
#       define TLAPI __declspec(dllimport)
#   endif
#else // For Linux
#   define TLINLINE static inline __attribute((always_inline))
#   ifdef TLSTATIC
#       define TLAPI
#   else
#       define TLAPI __attribute__((visibility("default")))
#   endif
#endif

#endif // TELEIOS_DEFINES_H
