#include "teleios/platform_detector.h"
#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "glad/glad.h" // must be after Windows.h
#include "glad/wgl.h" // must be after Windows.h
#include "teleios/graphics_opengl.h"
#include "teleios/logger.h"
#include "teleios/platform.h"

static b8 gl_context_load(void);


#ifdef TELEIOS_PLATFORM_WINDOWS
#include <GL/GL.h> // must be after glad.h
static HDC device;
static HGLRC context;
#endif

b8 gl_initialize(const TLSpecification* spec) {
    if (!gl_context_load()) {
        TLERROR("gl_initialize: Failed to gladLoadGL");
        return false;
    }


    TLDEBUG("GL_VENDOR: %s", glGetString(GL_VENDOR));
    TLDEBUG("GL_RENDERER: %s", glGetString(GL_RENDERER));
    TLDEBUG("GL_VERSION: %s", glGetString(GL_VERSION));

    glClearColor(.75f, .23f, .38f, 1);

    // disable v-sync
    if (wglSwapIntervalEXT != NULL) {
        wglSwapIntervalEXT(0);
    }
    return true;
}

static b8 gl_context_load(void) {
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

    device = GetDC(*((HWND*)tl_platform_window_handle()));
    i32 pixelFormat = ChoosePixelFormat(device, &pfd);
    if (pixelFormat == 0) {
        TLERROR("gl_context_load: Failed to ChoosePixelFormat");
        return false;
    }

    if (!SetPixelFormat(device, pixelFormat, &pfd)) {
        TLERROR("gl_context_load: Failed to SetPixelFormat");
        return false;
    }

    context = wglCreateContext(device);
    if (context == NULL) return false;
    if (!wglMakeCurrent(device, context)) {
        TLERROR("gl_context_load: Failed to wglMakeCurrent");
        return false;
    }

    if (!gladLoadGL()) return false;
    return gladLoadWGL(device);
#else
    return false;
#endif
}


b8 gl_present(void) {
    return SwapBuffers(device);
}

b8 gl_terminamte(void) {
#ifdef TELEIOS_PLATFORM_WINDOWS
    if (context != NULL) {
        wglDeleteContext(context);
        gladUnloadWGL();
    }
#endif

    return true;
}