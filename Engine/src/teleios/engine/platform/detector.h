#ifndef TELEIOS_ENGINE_PLATFORM_DETECTOR_H
#define TELEIOS_ENGINE_PLATFORM_DETECTOR_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   define TL_PLATFORM_WINDOWS 1
#   ifndef _WIN64
#       error "64-bit is required on Windows!"
#   endif
#elif defined(__linux__) || defined(__gnu_linux__)
#   define TL_PLATFORM_LINUX 1
#   if defined(__ANDROID__)
#       define TL_PLATFORM_ANDROID 1
#   endif
#elif defined(__unix__)
#   define TL_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
#   define TL_PLATFORM_POSIX 1
#elif __APPLE__
// Apple platforms
#   define TL_PLATFORM_APPLE 1
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR
#       define TL_PLATFORM_IOS 1
#       define TL_PLATFORM_IOS_SIMULATOR 1
#   elif TARGET_OS_IPHONE
#       define TL_PLATFORM_IOS 1
#   elif TARGET_OS_MAC
#   else
#       error "Unknown Apple platform"
#   endif
#else
#   error "Unknown platform!"
#endif

#endif // TELEIOS_ENGINE_PLATFORM_DETECTOR_H