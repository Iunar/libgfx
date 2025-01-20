#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <assert.h>

#include <gl/gl.h>
#include "wglext.h"
#include "glcorearb.h"

#include "libgfx.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_LIBGFX ((HINSTANCE)&__ImageBase)

/* Error codes */
#define GFX_SUCCESS                           (int)0
#define GFX_FAILED_TO_REGISTER_CLASSEXA       (int)1
#define GFX_FAILED_TO_CREATE_WINDOW           (int)2
#define GFX_FAILED_TO_GET_DC                  (int)3
#define GFX_FAILED_TO_DESCRIBE_PIXEL_FORMAT   (int)4
#define GFX_FAILED_TO_SET_PIXEL_FORMAT        (int)5
#define GFX_FAILED_TO_CREATE_GL_CONTEXT       (int)6
#define GFX_FAILED_TO_MAKE_CONTEXT_CURRENT    (int)7
#define GFX_FAILED_TO_LOAD_WGL_PROC           (int)8
#define GFX_FAILED_TO_LOAD_WGL_PROCS          (int)9
#define GFX_FAILED_TO_CHOOSE_PIXEL_FORMAT_ARB (int)10
#define GFX_FAILED_TO_CREATE_GL_CONTEXT_ARB   (int)11

/* wgl Procs */
PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC     wglGetPixelFormatAttribivARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB = NULL;
PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT = NULL;

LRESULT gfx_def_winproc(
  HWND window,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
);

/* LibGFX */

typedef struct {
    WNDCLASSEXA class;
    HWND handle;
    HDC dc;
    HGLRC gl_context;

    int width;
    int height;
    int should_close;
} gfx_window;

int gfx_load_wgl_extensions();
int gfx_create_window(gfx_window* window, int width, int height, const char* title);
int gfx_create_opengl_context(gfx_window* window);
void gfx_poll_events(gfx_window window);

/* Utility */
int check_wgl_proc(void* proc);

static gfx_window window;

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

    glViewport(0, 0, window.width, window.height);
    while(!window.should_close) {
        gfx_poll_events(window);

        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SwapBuffers(window.dc);
    }

    return 0;
}

LRESULT gfx_def_winproc(
  HWND window,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
) {
    LRESULT result = 0;

    switch(message) {
        default: {
            result = DefWindowProcA(window, message, wparam, lparam);
        } break;
    }

    return result;
}

/* Load required wgl functions to create a modern opengl context */
int gfx_load_wgl_extensions() {
    WNDCLASSEXA dummy_class = { 0 };

    /* Class */
    dummy_class.cbSize = sizeof(WNDCLASSEXA);
    dummy_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    dummy_class.lpfnWndProc = gfx_def_winproc;
    dummy_class.cbClsExtra = 0;
    dummy_class.cbWndExtra = 0;
    dummy_class.hInstance = HINST_LIBGFX;
    dummy_class.hIcon = 0;
    dummy_class.hCursor = 0;
    dummy_class.hbrBackground = 0;
    dummy_class.lpszMenuName = 0;
    dummy_class.lpszClassName = "wgl load functions window";
    dummy_class.hIconSm = 0;

    /* Register */
    if(!RegisterClassExA(&dummy_class)) {
        return GFX_FAILED_TO_REGISTER_CLASSEXA;
    }

    /* Create */
    HWND dummy_handle = 
    CreateWindowExA(
        0,                              // dwExStyle
        dummy_class.lpszClassName,      // lpClassName
        0,                              // lpWindowName
        0,                              // dwStyle
        CW_USEDEFAULT,                  // X
        CW_USEDEFAULT,                  // Y                 
        CW_USEDEFAULT,                  // W 
        CW_USEDEFAULT,                  // H
        0,                              // hWndParent
        0,                              // hMenu
        dummy_class.hInstance,          // hInstance
        0                               // lpParam
    );
    if(!dummy_handle) {
        UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_CREATE_WINDOW;
    }

    /* DC */
    HDC dummy_context = GetDC(dummy_handle);
    if(!dummy_context) {
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_GET_DC;
    }

    /* Pixel format */
    int pixel_format = 1; // No need to specify a particular format
    PIXELFORMATDESCRIPTOR format_description;
    if (!DescribePixelFormat(dummy_context, pixel_format, sizeof(format_description), &format_description)) {
    	ReleaseDC(dummy_handle, dummy_context);
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
    	return GFX_FAILED_TO_DESCRIBE_PIXEL_FORMAT;
    }

    if(!SetPixelFormat(dummy_context, pixel_format, &format_description)) {
        ReleaseDC(dummy_handle, dummy_context);
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_SET_PIXEL_FORMAT;
    }
    
    /* Context */
    HGLRC dummy_gl_context = wglCreateContext(dummy_context);
    if(!dummy_gl_context) {
        ReleaseDC(dummy_handle, dummy_context);
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_CREATE_GL_CONTEXT;
    }
    
    if(!wglMakeCurrent(dummy_context, dummy_gl_context)) {
        wglDeleteContext(dummy_gl_context);
        ReleaseDC(dummy_handle, dummy_context);
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_MAKE_CONTEXT_CURRENT;
    }

    /* Load functions */
    wglCreateContextAttribsARB =    (void*)wglGetProcAddress("wglCreateContextAttribsARB");
    wglGetPixelFormatAttribivARB =  (void*)wglGetProcAddress("wglGetPixelFormatAttribivARB");
    wglChoosePixelFormatARB =       (void*)wglGetProcAddress("wglChoosePixelFormatARB");
    wglSwapIntervalEXT =            (void*)wglGetProcAddress("wglSwapIntervalEXT");

    if(
        check_wgl_proc((void*)wglCreateContextAttribsARB  ) != GFX_SUCCESS ||
        check_wgl_proc((void*)wglGetPixelFormatAttribivARB) != GFX_SUCCESS ||
        check_wgl_proc((void*)wglChoosePixelFormatARB     ) != GFX_SUCCESS ||
        check_wgl_proc((void*)wglSwapIntervalEXT          ) != GFX_SUCCESS
    ) {
        wglMakeCurrent(dummy_context, 0);
        wglDeleteContext(dummy_gl_context);
        ReleaseDC(dummy_handle, dummy_context);
    	DestroyWindow(dummy_handle);
    	UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);
        return GFX_FAILED_TO_LOAD_WGL_PROCS;
    }

    /* Clean up */

    // TODO: Maybe add error checking
    wglMakeCurrent(dummy_context, 0);

    wglDeleteContext(dummy_gl_context);

    ReleaseDC(dummy_handle, dummy_context);

    DestroyWindow(dummy_handle);

    UnregisterClassA(dummy_class.lpszClassName, dummy_class.hInstance);

    return GFX_SUCCESS;
}

/* Create window without setting the pixel format */
int gfx_create_window(gfx_window* window, int width, int height, const char* title) {
    // TODO: Window proc option
    window->width = width;
    window->height = height;
    window->should_close = 0;

    /* Class */
    window->class.cbSize = sizeof(WNDCLASSEXA);
    window->class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window->class.lpfnWndProc = gfx_def_winproc;
    window->class.cbClsExtra = 0;
    window->class.cbWndExtra = 0;
    window->class.hInstance = HINST_LIBGFX;
    window->class.hIcon = 0;
    window->class.hCursor = 0;
    window->class.hbrBackground = 0;
    window->class.lpszMenuName = 0;
    window->class.lpszClassName = "WINDOW0"; // TODO: Change this
    window->class.hIconSm = 0;

    /* Register */
    if(!RegisterClassExA(&window->class)) {
        return GFX_FAILED_TO_REGISTER_CLASSEXA;
    }

    /* Create */
    window->handle = 
    CreateWindowExA(
        WS_EX_OVERLAPPEDWINDOW,             // dwExStyle
        window->class.lpszClassName,        // lpClassName
        title,                              // lpWindowName
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,   // dwStyle
        CW_USEDEFAULT,                      // X
        CW_USEDEFAULT,                      // Y                 
        width,                              // W 
        height,                             // H
        0,                                  // hWndParent
        0,                                  // hMenu
        window->class.hInstance,            // hInstance
        0                                   // lpParam
    );
    if(!window->handle) {
        UnregisterClassA(window->class.lpszClassName, window->class.hInstance);
        return GFX_FAILED_TO_CREATE_WINDOW;
    }

    /* DC */
    window->dc = GetDC(window->handle);
    if(!window->dc) {
    	DestroyWindow(window->handle);
    	UnregisterClassA(window->class.lpszClassName, window->class.hInstance);
        return GFX_FAILED_TO_GET_DC;
    }

    return GFX_SUCCESS;
}

/* Set pixel format and create opengl context */
int gfx_create_opengl_context(gfx_window* window) {
    /* Set pixel format */
    const int pixel_attribute_list[] = { // TMP
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0, // End
    };

    UINT request_count = 1;
    int pixel_format = 0;
    UINT format_count = 0;
    if(!wglChoosePixelFormatARB(window->dc, pixel_attribute_list, 0, request_count, &pixel_format, &format_count)) {
        return GFX_FAILED_TO_CHOOSE_PIXEL_FORMAT_ARB;
    }

    PIXELFORMATDESCRIPTOR format_description = { 0 };
    if(!DescribePixelFormat(window->dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &format_description)) {
        return GFX_FAILED_TO_DESCRIBE_PIXEL_FORMAT;
    }

    if(!SetPixelFormat(window->dc, pixel_format, &format_description)) {
        return GFX_FAILED_TO_SET_PIXEL_FORMAT;
    } printf("SetPixelFormat: %d\n", GetLastError());

    /* Create Context */
    const int context_attribute_list[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    window->gl_context = wglCreateContextAttribsARB(window->dc, 0, context_attribute_list);
    if(!window->gl_context) {
        return GFX_FAILED_TO_CREATE_GL_CONTEXT_ARB;
    } printf("Context: %d\n", GetLastError());

    if(!wglMakeCurrent(window->dc, window->gl_context)) {
        return GFX_FAILED_TO_MAKE_CONTEXT_CURRENT;
    } printf("Make Context Current: %d\n", GetLastError());

    return GFX_SUCCESS;
}

/* Process messages in event queue */
void gfx_poll_events(gfx_window window) { // TODO: Error checking
	MSG msg = {0};
	while(PeekMessageA(&msg, window.handle, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

// TODO: Make internal?
int check_wgl_proc(void* proc) {
	// Check validity
	if(proc == 0 || 
	(proc == (void*)0x1) || 
	(proc == (void*)0x2) ||
	(proc == (void*)0x3) ||
    (proc == (void*)-1)) {
        return GFX_FAILED_TO_LOAD_WGL_PROC;
    }
    return GFX_SUCCESS;
}