#ifndef LIBGFX_H
#define LIBGFX_H 1

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

#include "libgfx_defs.h"

typedef struct {
    WNDCLASSEXA class;
    HWND handle;
    HDC dc; // Device Context
    HGLRC glrc; // OpenGL Render Context

    int width;
    int height;
    int should_close;
} gfx_window;

typedef void (*GFX_KEY_CALLBACK)(int, int);

int  gfx_load_wgl_extensions();
int  gfx_create_window(gfx_window* window, int width, int height, const char* title);
void gfx_destroy_window(gfx_window* window);
int  gfx_create_opengl_context(gfx_window* window, int context_version);
void gfx_poll_events(gfx_window window);
void gfx_set_key_callback(GFX_KEY_CALLBACK key_callback);

#endif // LIBGFX_H