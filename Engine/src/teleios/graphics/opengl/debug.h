#ifndef TELEIOS_GRAPHICS_OPENGL_DEBUG_H
#define TELEIOS_GRAPHICS_OPENGL_DEBUG_H

#include "glad/glad.h"
#include "teleios/types.h"
#include "teleios/logger/console.h"

#define GLCHECK()                                                                               \
{                                                                                               \
    const char* error;                                                                          \
    u32 errorCode = glGetError();                                                               \
                                                                                                \
    if (errorCode != GL_NO_ERROR) {                                                             \
        switch (errorCode) {                                                                    \
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;                   \
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;                  \
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;              \
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;                 \
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;                \
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;                  \
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;  \
        default: error = "??";                                                                  \
        }                                                                                       \
                                                                                                \
        TLFATAL("GLCHECK: %s (%u): %s", __FILE__, __LINE__, error);                             \
    }                                                                                           \
}

#endif // TELEIOS_GRAPHICS_OPENGL_DEBUG_H