#include "libgfx.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <stdio.h>

#include <gl/gl.h>
#include "glcorearb.h"
#include "wglext.h"

PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;


#define internal static

/* wgl Procs */
PFNWGLCREATECONTEXTATTRIBSARBPROC       wglCreateContextAttribsARB = NULL;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC     wglGetPixelFormatAttribivARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC          wglChoosePixelFormatARB = NULL;
PFNWGLSWAPINTERVALEXTPROC               wglSwapIntervalEXT = NULL;

/* Utility */
int check_wgl_proc(void* proc);

GFX_KEY_CALLBACK user_key_callback = NULL;
void APIENTRY gfx_default_debug_callback(GLenum source,GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char *message, const void *userParam);

LRESULT gfx_def_winproc(
  HWND window,
  UINT message,
  WPARAM wparam,
  LPARAM lparam
) {
    LRESULT result = 0;

    switch(message) {
        case WM_CHAR:
        case WM_KEYUP:
        case WM_KEYDOWN: {
        
            int key_code = 0;
            switch(wparam) { // TODO: Does any casting need to happen?
                case GFX_KEY_A: {
                    key_code = GFX_KEY_A;
                } break;
                case GFX_KEY_B: {
                    key_code = GFX_KEY_B;
                } break;
                case GFX_KEY_C: {
                    key_code = GFX_KEY_C;
                } break;
                case GFX_KEY_D: {
                    key_code = GFX_KEY_D;
                } break;
                case GFX_KEY_E: {
                    key_code = GFX_KEY_R;
                } break;
                case GFX_KEY_F: {
                    key_code = GFX_KEY_F;
                } break;
                case GFX_KEY_G: {
                    key_code = GFX_KEY_G;
                } break;
                case GFX_KEY_H: {
                    key_code = GFX_KEY_H;
                } break;
                case GFX_KEY_I: {
                    key_code = GFX_KEY_I;
                } break;
                case GFX_KEY_J: {
                    key_code = GFX_KEY_J;
                } break;
                case GFX_KEY_K: {
                    key_code = GFX_KEY_K;
                } break;
                case GFX_KEY_L: {
                    key_code = GFX_KEY_L;
                } break;
                case GFX_KEY_M: {
                    key_code = GFX_KEY_M;
                } break;
                case GFX_KEY_N: {
                    key_code = GFX_KEY_N;
                } break;
                case GFX_KEY_O: {
                    key_code = GFX_KEY_O;
                } break;
                case GFX_KEY_P: {
                    key_code = GFX_KEY_P;
                } break;
                case GFX_KEY_Q: {
                    key_code = GFX_KEY_Q;
                } break;
                case GFX_KEY_R: {
                    key_code = GFX_KEY_R;
                } break;
                case GFX_KEY_S: {
                    key_code = GFX_KEY_S;
                } break;
                case GFX_KEY_T: {
                    key_code = GFX_KEY_T;
                } break;
                case GFX_KEY_U: {
                    key_code = GFX_KEY_U;
                } break;
                case GFX_KEY_V: {
                    key_code = GFX_KEY_V;
                } break;
                case GFX_KEY_W: {
                    key_code = GFX_KEY_W;
                } break;
                case GFX_KEY_X: {
                    key_code = GFX_KEY_X;
                } break;
                case GFX_KEY_Y: {
                    key_code = GFX_KEY_Y;
                } break;
                case GFX_KEY_Z: {
                    key_code = GFX_KEY_Z;
                } break;
                default: break;
            }

            int key_state = GFX_KEY_UP; // If transition flag is set to 1
            if( ((lparam >> 31) & 1) == 0) {
                key_state = GFX_KEY_DOWN; // If transition flag is set to 0
            }

            if(user_key_callback) {
                user_key_callback(key_code, key_state);
            }

        } break;
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

/* Close the window and free resources. Doesn't guarentee that any of the resources are successfully freed */
void gfx_destroy_window(gfx_window* window) {
    wglMakeCurrent(window->dc, 0);
    wglDeleteContext(window->glrc);
    ReleaseDC(window->handle, window->dc);
    DestroyWindow(window->handle);
    UnregisterClassA(window->class.lpszClassName, window->class.hInstance);
}

/* Set pixel format and create opengl context */
int gfx_create_opengl_context(gfx_window* window, int context_version) {
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
    }

    /* Create Context */
    /*
        Please note that if the extension 'WGL_ARB_create_context_profile' is present, implementations
        are allowed to return gl versions 3.0+ even if not directly asked for as long as the returned version
        has the desired functionality from the requested version. For example, asking for version 3.0 and
        receiving version 4.6 with the compatability profile.
    */
    // Build context attribute list from context_version
    int context_attribute_list[9];
    switch (context_version) {
        case GFX_OPENGL_CORE_3_0: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 0;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_3_1: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 1;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_3_2: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 2;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_3_3: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_0: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 0;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_1: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 1;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_2: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 2;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_3: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_4: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 4;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_5: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 5;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_6: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 6;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;

        case GFX_OPENGL_COMPATIBILITY_3_2: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 2;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_3_3: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 3;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_0: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 0;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_1: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 1;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_2: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 2;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_3: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_4: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 4;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_5: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 5;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_6: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 6;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = 0;
        } break;
        case GFX_OPENGL_CORE_4_3_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_CORE_4_4_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 4;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_CORE_4_5_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 5;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_CORE_4_6_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 6;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_3_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 3;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_4_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 4;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_5_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 5;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        case GFX_OPENGL_COMPATIBILITY_4_6_DEBUG: {
            context_attribute_list[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
            context_attribute_list[1] = 4;
            context_attribute_list[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
            context_attribute_list[3] = 6;
            context_attribute_list[4] = WGL_CONTEXT_PROFILE_MASK_ARB;
            context_attribute_list[5] = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
            context_attribute_list[6] = WGL_CONTEXT_FLAGS_ARB;
            context_attribute_list[7] = WGL_CONTEXT_DEBUG_BIT_ARB;
            context_attribute_list[8] = 0;
        } break;
        default: {
            return GFX_INVALID_CONTEXT_VERSION; // TODO: Free stuff
        }
    }

    /* 
    For some reason, on my main machine with the rx 6750 xt, this call to wglCreateContextAttribsARB
    causes GetLastError() to return 126, and the subsequent SwapBuffers call causes GetLastError() to
    generate 6. However, on a laptop with an nvidia gpu neither codes are generated. It seems to work
    just fine on both computers however so idk. */
    window->glrc = wglCreateContextAttribsARB(window->dc, 0, context_attribute_list);
    if(!window->glrc) {
        return GFX_FAILED_TO_CREATE_GL_CONTEXT_ARB; // TODO: Free resources??
    }

    if(!wglMakeCurrent(window->dc, window->glrc)) {
        return GFX_FAILED_TO_MAKE_CONTEXT_CURRENT;
    } printf("Make Context Current: %d\n", GetLastError());

    /* Load OpenGL Procedures */
    //HINSTANCE openglInstance = GetModuleHandleA("opengl32.lib");
    //glDebugMessageCallback = (void*)GetProcAddress(openglInstance, "glDebugMessageCallback");
    //if(!glDebugMessageCallback) {
    //    printf("Failed to load glDebugmessageCallback\n");
    //}
//
    //// Set up debug logging
	//int flags;
	//glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	//if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	//	//printf("[DEBUG]: Debug context created.\n");
	//	glEnable(GL_DEBUG_OUTPUT);
	//	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//	glDebugMessageCallback(gfx_default_debug_callback, NULL);
	//	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	//}

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

void gfx_set_key_callback(GFX_KEY_CALLBACK key_callback) {
    user_key_callback = key_callback;
}

// TODO: Make internal?
internal
int check_wgl_proc(void* proc) {
	// Check validity
	if(proc == 0 || 
	(proc == (void*)0x1) || 
	(proc == (void*)0x2) ||
	(proc == (void*)0x3) ||
    (proc == (void*)-1)) {
        return 0;
    }
    return 1;
}

/* Default Debug Callback */
static void APIENTRY gfx_default_debug_callback(GLenum source,GLenum type, unsigned int id, GLenum severity,
    GLsizei length, const char *message, const void *userParam) 
{

    // Ignore insignificant warnings/errors
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    fprintf(stderr, "[Debug]:");
    fprintf(stderr, " { ");
    switch (source) 
	{
        case GL_DEBUG_SOURCE_API:             fprintf(stderr, "SRC: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   fprintf(stderr, "SRC: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "SRC: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     fprintf(stderr, "SRC: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     fprintf(stderr, "SRC: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           fprintf(stderr, "SRC: Other"); break;
    };
    fprintf(stderr, " | ");
    switch (type) 
	{
        case GL_DEBUG_TYPE_ERROR:               fprintf(stderr, "TYPE: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "TYPE: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  fprintf(stderr, "TYPE: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY:         fprintf(stderr, "TYPE: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         fprintf(stderr, "TYPE: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              fprintf(stderr, "TYPE: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          fprintf(stderr, "TYPE: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           fprintf(stderr, "TYPE: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               fprintf(stderr, "TYPE: Other"); break;
    };
    printf(" | ");
    switch (severity) 
	{
        case GL_DEBUG_SEVERITY_HIGH:         fprintf(stderr, "Severity"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       fprintf(stderr, "Severity"); break;
        case GL_DEBUG_SEVERITY_LOW:          fprintf(stderr, "Severity"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity"); break;
    };
    fprintf(stderr, " }: %s\n", message);
}


//LRESULT Win32WindowProc(
//	HWND WindowHandle,
//	UINT Message,
//	WPARAM WParam,
//	LPARAM LParam
//) {
//	LRESULT result = 0;
//	switch(Message) {
//		case WM_CLOSE: {
//			WindowShouldClose = 1;
//		} break;
//
//		// Key-up events
//		// Docs: https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input
//		case WM_KEYUP: {
//			WORD vkCode = LOWORD(WParam);		// Virtual
//			WORD keyFlags = HIWORD(LParam);		// Flags
//			//WORD scanCode = LOBYTE(keyFlags);
//
//			switch(vkCode) {
//				case VK_ESCAPE: {
//					WindowShouldClose = 1;
//				}break;
//				default:
//					break;
//			}
//			result = DefWindowProc(WindowHandle, Message, WParam, LParam);
//		} break;
//
//		default: {
//			result = DefWindowProc(WindowHandle, Message, WParam, LParam);
//		}
//	}
//	return result;
//}
