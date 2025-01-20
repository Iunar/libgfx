#ifndef LIBGFX_H
#define LIBGFX_H 1

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

#include <gl/gl.h>
#include "wglext.h"
#include "glcorearb.h"

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

/* Callback function types */
typedef void (*GFX_KEY_CALLBACK)(int, int);
typedef void (*GFX_MOUSE_POS_CALLBACK)(double, double);
typedef void (*GFX_MOUSE_BUTTON_CALLBACK)(int, double, double);
typedef void (*GFX_WINDOW_SIZE_CALLBACK)(int, int);

/* Get the previous error of a gfx_* function. */
int  gfx_get_last_error();

/* Load opengl windows extensions required to create a real opengl context */
int  gfx_load_wgl_extensions();

/* Create win32 window */
int  gfx_create_window(gfx_window* window, int width, int height, const char* title);

/* Close window and free resources */
void gfx_destroy_window(gfx_window* window);

/* Create real opengl context (see libgfx_defs.h for context_version defines) */
int  gfx_create_opengl_context(gfx_window* window, int context_version);
/* Process window message queue */
void gfx_poll_events(gfx_window window);

/* Callback setters */
void gfx_set_key_callback(GFX_KEY_CALLBACK key_callback);
void gfx_set_mouse_pos_callback(GFX_MOUSE_POS_CALLBACK mouse_pos_callback);
void gfx_set_mouse_button_callback(GFX_MOUSE_BUTTON_CALLBACK mouse_button_callback);
void gfx_set_window_size_callback(GFX_WINDOW_SIZE_CALLBACK window_size_callback);

/* Set the size of a gfx_window adjusting for the title bar */
int gfx_set_window_size(gfx_window* window, int w, int h);

/* Compile and create shader */
GLuint gfx_create_shader(const char* VertexSource, const char* FragmentSource);

#endif // LIBGFX_H