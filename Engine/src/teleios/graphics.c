#include "teleios/platform_detector.h"
#ifdef TELEIOS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "teleios/teleios.h"
#include "teleios/eventcodes.h"
#include "teleios/graphics.h"
#include "teleios/platform.h"

#include "glad/glad.h" // must be after Windows.h
#include "glad/wgl.h"  // must be after Windows.h

#ifdef TELEIOS_PLATFORM_WINDOWS
#include <GL/GL.h> // must be after glad.h
static HDC device;
static HGLRC context;
#endif

TLAPI TLGraphics* tl_graphics_shader(const TLShaderSource* sources, const u8 count) {
    if (count == 0) {
        TLERROR("tl_graphics_shader: Invalid TLShaderSource count value");
        return NULL;
    }

    TLGraphics* program = tl_graphics_primitive_create(TL_GRAPHICS_PRIMITIVE_SHADER);
    program->handler = glCreateProgram();

    int  success;
    char infoLog[512];

    u32* shader = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, count * sizeof(u32));
    for (unsigned i = 0; i < count; ++i) {
        const TLShaderSource source = sources[i];
        u32 handle = shader[i];
        u32 type = GL_NONE;
        if (type == GL_NONE && source.type == TL_SHADER_TYPE_VERTEX) {
            type = GL_VERTEX_SHADER;
            program->object.shader.vertex = source.file->path;
        }
        if (type == GL_NONE && source.type == TL_SHADER_TYPE_FRAGMENT) {
            type = GL_FRAGMENT_SHADER;
            program->object.shader.fragment = source.file->path;
        }

        handle = glCreateShader(type);
        glShaderSource(handle, 1, &source.file->payload.string, 0);
        glCompileShader(handle);
        glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
        if (!success) {
            tl_memory_zero(infoLog, 512);
            glGetShaderInfoLog(handle, 512, NULL, infoLog);
            TLERROR("editor_layer_initialize: Failed to compile shader %s: %s", source.file->path, infoLog);

            // Delete all previously create shader
            for (unsigned j = 0; j < count; ++j) {
                if (shader[j] > 0) {
                    glDeleteShader(shader[i]);
                }
            }

            // Destroy the primitive
            tl_graphics_primitive_destroy(program);
            tl_memory_free(shader, TL_MEMORY_TYPE_GRAPHICS, count * sizeof(u32));
            return NULL;
        }

        glAttachShader(program->handler, handle);
    }

    {
        glLinkProgram(program->handler);
        glGetProgramiv(program->handler, GL_LINK_STATUS, &success);

        if (!success) {
            tl_memory_zero(infoLog, 512);
            glGetProgramInfoLog(program->handler, 512, NULL, infoLog);
            TLERROR("editor_layer_initialize: Failed to compile shader program: %s", infoLog);

            // Delete all create shaders
            for (unsigned i = 0; i < count; ++i) {
                if (shader[i] > 0) {
                    glDeleteShader(shader[i]);
                }
            }

            // Destroy the primitive
            tl_graphics_primitive_destroy(program);
            tl_memory_free(shader, TL_MEMORY_TYPE_GRAPHICS, count * sizeof(u32));
            return NULL;
        }
    }

    tl_memory_free(shader, TL_MEMORY_TYPE_GRAPHICS, count * sizeof(u32));
    return program;
}

TLAPI TLGraphics* tl_graphics_primitive_create(const TLGraphcisType type) {
    TLGraphics* primitive = tl_memory_alloc(TL_MEMORY_TYPE_GRAPHICS, sizeof(TLGraphics));
    if (primitive == NULL) {
        TLERROR("tl_graphics_primitive_create: Failed to create primitive");
        return NULL;
    }

    tl_memory_copy(&type, &primitive->type, sizeof(TLGraphcisType));
    return primitive;
}

TLAPI void tl_graphics_primitive_destroy(TLGraphics* primitive) {
    if (primitive->type == TL_GRAPHICS_PRIMITIVE_SHADER) {
        glDeleteProgram(primitive->handler);
    }

    tl_memory_free(primitive, TL_MEMORY_TYPE_GRAPHICS, sizeof(TLGraphics));
}

#define SUBSCRIBE(event,callback)                                       \
    if (!tl_event_subscribe(event, callback)) {                         \
        TLERROR("tl_graphics_initialize: Failed to tl_event_subscribe");\
        return false;                                                   \
    }

static TLEventStatus tl_graphics_resize(const u8 code, const TLEvent* event) {
    ivec2s size = { 0 };

    if (code == TL_EVENT_WINDOW_RESIZED || code == TL_EVENT_WINDOW_RESTORED) {
        size.x = event->data.u32[0];
        size.y = event->data.u32[1];
    }

    glViewport(0, 0, size.x, size.y);
    return TL_EVENT_STATUS_CONTUNE;
}

b8 tl_graphics_initialize(const TLSpecification* spec) {
    TLTRACE("tl_graphics_initialize: Loading OpenGL");
#ifdef TELEIOS_PLATFORM_WINDOWS

    {
        // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
        // We use a dummy window because you can only set the pixel format for a window once. For the
        // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
        // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
        // have a context.
        WNDCLASSA window_class = {
            .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
            .lpfnWndProc = DefWindowProcA,
            .hInstance = GetModuleHandle(0),
            .lpszClassName = "Dummy_WGL_djuasiodwa",
        };

        if (!RegisterClassA(&window_class)) {
            TLERROR("Failed to register dummy OpenGL window.");
            return false;
        }

        HWND dummy_window = CreateWindowExA(
            0,
            window_class.lpszClassName,
            "Dummy OpenGL Window",
            0,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            window_class.hInstance,
            0);

        if (!dummy_window) {
            TLERROR("Failed to create dummy OpenGL window.");
            return false;
        }

        HDC dummy_dc = GetDC(dummy_window);

        PIXELFORMATDESCRIPTOR pfd = {
            .nSize = sizeof(pfd),
            .nVersion = 1,
            .iPixelType = PFD_TYPE_RGBA,
            .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            .cColorBits = 32,
            .cAlphaBits = 8,
            .iLayerType = PFD_MAIN_PLANE,
            .cDepthBits = 24,
            .cStencilBits = 8,
        };

        int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
        if (!pixel_format) {
            TLERROR("Failed to find a suitable pixel format.");
            return false;
        }
        if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
            TLERROR("Failed to set the pixel format.");
            return false;
        }

        HGLRC dummy_context = wglCreateContext(dummy_dc);
        if (!dummy_context) {
            TLERROR("Failed to create a dummy OpenGL rendering context.");
            return false;
        }

        if (!wglMakeCurrent(dummy_dc, dummy_context)) {
            TLERROR("Failed to activate dummy OpenGL rendering context.");
            return false;
        }

        wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

        wglMakeCurrent(dummy_dc, 0);
        wglDeleteContext(dummy_context);
        ReleaseDC(dummy_window, dummy_dc);
        DestroyWindow(dummy_window);
    }

    {
        device = GetDC(*((HWND*)tl_platform_window_handle()));

        // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
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

        int pixel_format;
        UINT num_formats;
        wglChoosePixelFormatARB(device, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
        if (!num_formats) {
            TLERROR("tl_graphics_initialize: Failed to wglChoosePixelFormatARB");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd;
        DescribePixelFormat(device, pixel_format, sizeof(pfd), &pfd);
        if (!SetPixelFormat(device, pixel_format, &pfd)) {
            TLERROR("tl_graphics_initialize: Failed to SetPixelFormat");
            return false;
        }

        // Specify that we want to create an OpenGL 4.6 core profile context
        int gl33_attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0,
        };

        context = wglCreateContextAttribsARB(device, 0, gl33_attribs);
        if (context == NULL) return false;
        if (!wglMakeCurrent(device, context)) {
            TLERROR("tl_graphics_initialize: Failed to wglMakeCurrent");
            return false;
        }

        if (!gladLoadGL()) {
            TLERROR("tl_graphics_initialize: Failed to gladLoadGL");
            return false;
        }

        if (!gladLoadWGL(device)) {
            TLERROR("tl_graphics_initialize: Failed to gladLoadWGL");
            return false;
        }
    }

    TLDEBUG("GL_VERSION: %s", glGetString(GL_VERSION));
    TLDEBUG("GL_RENDERER: %s", glGetString(GL_RENDERER));

    glClearColor(.75f, .23f, .38f, 1);

    // disable v-sync
    if (wglSwapIntervalEXT != NULL) {
        wglSwapIntervalEXT(0);
    }

    SUBSCRIBE(TL_EVENT_WINDOW_RESIZED, tl_graphics_resize);
    SUBSCRIBE(TL_EVENT_WINDOW_MINIMIZED, tl_graphics_resize);
    SUBSCRIBE(TL_EVENT_WINDOW_MAXIMIZED, tl_graphics_resize);
    SUBSCRIBE(TL_EVENT_WINDOW_RESTORED, tl_graphics_resize);

    return true;
#else
    return false;
#endif
}

void tl_graphics_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void tl_graphics_present(void) {
    if (!SwapBuffers(device)) {
        TLFATAL("Openbgl failed to swap buffers");
    }
}

b8 tl_graphics_terminate(void) {
#ifdef TELEIOS_PLATFORM_WINDOWS
    if (context != NULL) {
        wglDeleteContext(context);
        gladUnloadWGL();
    }
#endif

    return true;
}
