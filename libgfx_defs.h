#ifndef LIBGFX_DEFS_H
#define LIBGFX_DEFS_H 1

// Raymond Chen is my goat
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

#endif // LIBGFX_DEFS_H