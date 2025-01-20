
/*
    TODO: 
        virtual keys
        mouse input
*/

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <assert.h>

#include <gl/gl.h>
#include "wglext.h"
#include "glcorearb.h"

#include "libgfx.h"

static gfx_window window;
void key_callback(int key, int state);

#include <stdio.h>
int main() {
    int status = 0;
    /* Load wgl extensions */
    if(gfx_load_wgl_extensions() != GFX_SUCCESS) {
        printf("Failed to load wgl extensions, %d\n", gfx_get_last_error());
        return -1;
    }

    /* Create main window*/
    if(gfx_create_window(&window, 512, 512, "[GFX]") != GFX_SUCCESS) {
        printf("Failed to create window, %d\n", gfx_get_last_error());
        return -1;
    }

    if(gfx_create_opengl_context(&window, GFX_OPENGL_CORE_4_6_DEBUG) != GFX_SUCCESS) {
        printf("Failed to create opengl context, %d\n", gfx_get_last_error());
        return -1;
    } printf("GL version: %s\n", glGetString(GL_VERSION));

    gfx_set_key_callback(key_callback);
    glViewport(0, 0, window.width, window.height);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

    while(!window.should_close) {
        gfx_poll_events(window);

        glClear(GL_COLOR_BUFFER_BIT);

        SwapBuffers(window.dc);
    }

    gfx_destroy_window(&window);
    return 0;
}

void key_callback(int key, int state) { // why are both the key and state 0 sometimes ??
    if((key == GFX_KEY_R) && (state == GFX_KEY_DOWN)) {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    } else if((key == GFX_KEY_G) && (state == GFX_KEY_DOWN)) {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    } else if((key == GFX_KEY_B) && (state == GFX_KEY_DOWN)) {
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }

    if((key == GFX_KEY_Q) && (state == GFX_KEY_DOWN)) {
        window.should_close = 1;
    }
}