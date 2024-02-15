#include "teleios/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"
#include "teleios/event/registry.h"
#include "teleios/window/event.h"
#include "teleios/logger/console.h"

#include "teleios/graphics/draw.h"
#include "teleios/graphics/lifecycle.h"
#include "teleios/graphics/opengl/gldebug.h"

#include "glad/wgl.h"
#include "glad/glad.h"

static HDC m_device;
static HGLRC m_context;

static inline b8 gl_init_extentions(PIXELFORMATDESCRIPTOR* pfd);
static inline b8 gl_create(PIXELFORMATDESCRIPTOR* pfd);

static b8 tl_graphics_events(u16 code, const TLEvent* event) {
    glViewport(0, 0, event->payload.u32[0], event->payload.u32[1]);
    return false;
}

b8 tl_graphics_initialize(const TLSpecification* spec) {
    TLDEBUG("tl_graphics_initialize: Initializing the Graphics System");

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

    m_device = GetDC(e_window);

    if (!gl_init_extentions(&pfd)) { TLERROR("tl_graphics_initialize: Failed to initialize GL Extentions"); return false; }
    if (!gl_create(&pfd)) { TLERROR("tl_graphics_initialize: Failed to initialize GL Context"); return false; }
    if (!gladLoadGL()) { TLERROR("tl_graphics_initialize: Failed to initialize Opengl"); return false; }
    if (!gladLoadWGL(m_device)) { TLERROR("tl_graphics_initialize: Failed to initialize WGL"); return false; }

    TLINFO("GL_VENDOR: %s", glGetString(GL_VENDOR)); GLCHECK();
    TLINFO("GL_VERSION: %s", glGetString(GL_VERSION)); GLCHECK();

    glEnable(GL_DEPTH_TEST); GLCHECK();
    glEnable(GL_STENCIL_TEST); GLCHECK();
    glClearColor(.37f, .73f, .48f, 1); GLCHECK();

    if (wglSwapIntervalEXT == NULL) {
        TLWARN("tl_graphics_initialize: Impossible to set v-sync");
    }
    else {
        if (!wglSwapIntervalEXT(spec->graphics.vsync == true)) {
            TLWARN("tl_graphics_initialize: Failed to disable v-sync");
        }

        GLCHECK();
    }

    tl_event_register(TL_EVENT_WINDOW_RESIZED, tl_graphics_events);
    tl_event_register(TL_EVENT_WINDOW_RESTORED, tl_graphics_events);
    tl_event_register(TL_EVENT_WINDOW_MAXIMIZED, tl_graphics_events);
    tl_event_register(TL_EVENT_WINDOW_MINIMIZED, tl_graphics_events);

    return true;
}

static b8 gl_create(PIXELFORMATDESCRIPTOR* pfd) {
    HDC device = GetDC(e_window);

    i32 attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
        0,
    };

    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    i32 format;
    UINT formats;
    wglChoosePixelFormatARB(device, pixel_format_attribs, 0, 1, &format, &formats);
    if (!formats) {
        TLERROR("Failed to set the OpenGL %u.%u pixel format.", attribs[0], attribs[1]);
        return false;
    }

    if (!SetPixelFormat(device, format, pfd)) {
        TLERROR("Failed to set the OpenGL %u.%u pixel format.", attribs[0], attribs[1]);
        return false;
    }

    m_context = wglCreateContextAttribsARB(device, 0, attribs);
    if (!m_context) {
        TLERROR("Failed to create OpenGL %u.%u context.", attribs[0], attribs[1]);
        return false;
    }

    if (!wglMakeCurrent(device, m_context)) {
        TLERROR("Failed to activate OpenGL %u.%u rendering context.", attribs[0], attribs[1]);
        return false;
    }

    return true;
}

static b8 gl_init_extentions(PIXELFORMATDESCRIPTOR* pfd) {
    WNDCLASS window_class = {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "wgl",
    };

    if (!RegisterClass(&window_class)) { TLERROR("Failed to register dummy OpenGL window."); return false; }

    HWND hwnd = CreateWindowEx(0, window_class.lpszClassName, "x", 0, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, e_hinstance, 0);
    if (!hwnd) { TLERROR("Failed to create dummy OpenGL window."); return false; }

    HDC dc = GetDC(hwnd);
    int format = ChoosePixelFormat(dc, pfd);
    if (!format) { TLERROR("Failed to find a suitable pixel format."); return false; }
    if (!SetPixelFormat(dc, format, pfd)) { TLERROR("Failed to set the pixel format."); return false; }

    HGLRC cx = wglCreateContext(dc);
    if (!cx) { TLERROR("Failed to create a dummy OpenGL rendering context."); return false; }
    if (!wglMakeCurrent(dc, cx)) { TLERROR("Failed to activate dummy OpenGL rendering context."); return false; }

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

    wglMakeCurrent(dc, 0);
    wglDeleteContext(cx);
    ReleaseDC(hwnd, dc);
    DestroyWindow(hwnd);

    return true;
}

void tl_graphics_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); GLCHECK();
}

void tl_graphics_draw(u16 index) {
    glDrawElements(GL_TRIANGLES, index, GL_UNSIGNED_INT, 0); GLCHECK();
}

void tl_graphics_update(void) {
    if (!SwapBuffers(m_device)) {
        TLFATAL("tl_graphics_update: Failed to SwapBuffers: 0x%x", GetLastError());
    }
}

b8 tl_graphics_terminate(void) {
    TLDEBUG("tl_graphics_terminate: Terminating the Graphics System");

    wglMakeCurrent(m_device, NULL);
    wglDeleteContext(m_context);
    ReleaseDC(e_window, m_device);

    return true;
}

#endif // TL_PLATFORM_WINDOWS