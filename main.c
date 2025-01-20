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
    if((status = gfx_load_wgl_extensions()) != GFX_SUCCESS) {
        printf("Failed to load wgl extensions, %d\n", status);
        return -1;
    } printf("Successfully loaded wgl extensions, %d\n", status);

    /* Create main window*/
    if((status = gfx_create_window(&window, 512, 512, "[GFX]")) != GFX_SUCCESS) {
        printf("Failed to create window, %d\n", status);
        return -1;
    } printf("Successfully created gfx window, %d\n", status);

    if((status = gfx_create_opengl_context(&window)) != GFX_SUCCESS) {
        printf("Failed to create opengl context, %d\n", status);
        return -1;
    } printf("Successfully created opengl context, %d\n", status);
    printf("GL version: %s\n", glGetString(GL_VERSION));

    gfx_set_key_callback(key_callback);

    glViewport(0, 0, window.width, window.height);
    while(!window.should_close) {
        gfx_poll_events(window);

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SwapBuffers(window.dc);
    }

    // TODO: gfx_close_window();
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