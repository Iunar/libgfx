
/*
    TODO: 
        virtual keys
*/

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <assert.h>

#include "libgfx.h"

static gfx_window window;
void key_callback(int key, int state);
void mouse_pos_callback(double x, double y);
void mouse_button_callback(int button, double x, double y);
void window_size_callback(int width, int height);

/*
    [win32/opengl problem]
    Hey all! I've recently been working on a very small windowing and input library
    using win32 and c with the primary objective of using it in my opengl projects,
    however, I've run into an odd issue with glViewport. The issue is that when 
    I set the opengl viewport to be the same size as the client rect of the win32
    window, the renders are vertically squished by half of the height. The only hack
    I've managed to find is simply multiplying the height by two whenever you set the
    viewport... If anyone has any idea what the problem might be, I would love to hear
    it. Here is the _link_ to the source code. Thanks!
*/

#include <stdio.h>
int main() {
    /* Load wgl extensions */
    if(gfx_load_wgl_extensions() != GFX_SUCCESS) {
        printf("Failed to load wgl extensions, %d\n", gfx_get_last_error());
        return -1;
    }

    /* Create main window */
    if(gfx_create_window(&window, 512, 512, "[GFX]") != GFX_SUCCESS) {
        printf("Failed to create window, %d\n", gfx_get_last_error());
        return -1;
    }

    /* Create main window */
    if(gfx_create_opengl_context(&window, GFX_OPENGL_CORE_4_6_DEBUG) != GFX_SUCCESS) {
        printf("Failed to create opengl context, %d\n", gfx_get_last_error());
        return -1;
    } printf("GL version: %s\n", glGetString(GL_VERSION));

    gfx_set_key_callback(key_callback);
    gfx_set_mouse_pos_callback(mouse_pos_callback);
    gfx_set_mouse_button_callback(mouse_button_callback);
    gfx_set_window_size_callback(window_size_callback);

    /* Triangle */
    float verts[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), (void*)verts, GL_STATIC_DRAW);

    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), (void*)indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    unsigned int shader = gfx_create_shader("shaders\\vertex.glsl", "shaders\\fragment.glsl");

    glViewport(0, 0, window.width, window.height * 2); // HACK (idk why the viewport is rendering squashed)
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

    while(!window.should_close) {
        gfx_poll_events(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);
        glUseProgram(shader);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)0);

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

void mouse_pos_callback(double x, double y) {
    printf("[%.4f %.4f]\n", x, y);
}

void mouse_button_callback(int button, double x, double y) {
    printf("[%d %.4f  %.4f]\n", button, x, y);
}

void window_size_callback(int width, int height) {
    window.width = width;
    window.height = height;
    gfx_set_window_size(&window, window.width, window.height);
    glViewport(0, 0, window.width, window.height * 2); // HACK (idk why the viewport is rendering squashed )
}