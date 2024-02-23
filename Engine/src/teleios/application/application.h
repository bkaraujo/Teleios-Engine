#ifndef TELEIOS_APPLICATION_H
#define TELEIOS_APPLICATION_H

#include "teleios/platform/detector.h"
#if defined(TL_PLATFORM_WINDOWS)
#   include <Windows.h>
#else
#   include <stdlib.h>
#endif // defined(TL_PLATFORM_WINDOWS)

#ifdef TELEIOS_GRAPHICS_OPENGL
#   include "glad/types.h"
#endif // TELEIOS_GRAPHICS_OPENGL

#include "teleios/engine/lifecycle.h"

static inline void tl_application_set(TLSpecification* spec);
static inline b8 tl_application_initialize(void);
static inline b8 tl_application_terminate(void);

#if defined(TELEIOS_DIST) && defined(TL_PLATFORM_WINDOWS)
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
#else
int main(int argc, char** argv) {
#endif // defined(TELEIOS_DIST) && defined(TL_PLATFORM_WINDOWS)
    {
        TLSpecification spec = { 0 };
        spec.version.major = 0;
        spec.version.minor = 1;
        spec.version.patch = 0;

        spec.window.title = "Teleios APP";
        spec.window.width = 1024;
        spec.window.height = 768;

        spec.graphics.vsync = false;
#ifdef TELEIOS_GRAPHICS_OPENGL
        spec.graphics.test.depth = false;
        spec.graphics.test.stencil = false;
        spec.graphics.test.scissor = false;
        spec.graphics.clear_color[0] = .37f;
        spec.graphics.clear_color[1] = .73f;
        spec.graphics.clear_color[2] = .48f;
        spec.graphics.clear_color[3] = 1;
        spec.graphics.depth_function = GL_LESS;
#endif // TELEIOS_GRAPHICS_OPENGL

        tl_application_set(&spec);

        if (!tl_engine_initialize(&spec)) {
            if (!tl_engine_terminate()) exit(99);
            exit(90);
        }
    }

    if (!tl_application_initialize()) {
        if (!tl_engine_terminate()) exit(99);
        exit(91);

    }

    if (!tl_engine_run()) {
        if (!tl_application_terminate()) {
            if (!tl_engine_terminate()) exit(99);
            exit(92);
        }

        if (!tl_engine_terminate()) exit(99);
        exit(93);
    }

    if (!tl_application_terminate()) {
        if (!tl_engine_terminate()) exit(99);
        exit(94);
    }

    if (!tl_engine_terminate()) exit(99);

    return 0;
}

#endif // TELEIOS_APPLICATION_H