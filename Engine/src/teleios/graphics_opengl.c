#include "glad/glad.h"
#include "teleios/graphics_opengl.h"
#include "teleios/logger.h"
#include "teleios/platform.h"
#include "teleios/platform_detector.h"

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>
#include <GL/GL.h>

static HGLRC context;
#endif

b8 gl_initialize(const TLSpecification* spec) {

#ifdef TELEIOS_PLATFORM_WINDOWS
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.cAuxBuffers = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC hDc = GetDC(*((HWND*)tl_platform_window_handle()));
    i32 pixelFormat = ChoosePixelFormat(hDc, &pfd);
    if (pixelFormat) {
        if (!SetPixelFormat(hDc, pixelFormat, &pfd)) {
            TLERROR("gl_initialize: Failed setting pixel format!");
            return false;
        }
    }

    context = wglCreateContext(hDc);
    if (context) {
        if (!wglMakeCurrent(hDc, context)) {
            TLERROR("gl_initialize: Failed setting OpenGL context!");
            return false;
        }
    }
    else {
        TLERROR("gl_initialize: Failed creating OpenGL context!");
    }

    if (!gladLoadGL()) {
        TLERROR("gl_initialize: Failed to gladLoadGL");
        return false;
    }
#endif

    TLDEBUG("GL_VENDOR: %s", glGetString(GL_VENDOR));
    TLDEBUG("GL_RENDERER: %s", glGetString(GL_RENDERER));
    TLDEBUG("GL_VERSION: %s", glGetString(GL_VERSION));

    return true;
}

b8 gl_terminamte(void) {

#ifdef TELEIOS_PLATFORM_WINDOWS
    wglDeleteContext(context);
#endif

    return true;
}