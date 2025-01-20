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
#define GFX_INVALID_CONTEXT_VERSION           (int)12
#define GFX_FAILED_TO_LOAD_GL_DEBUG_PROCS     (int)13
#define GFX_WGL_PROC_INVALID                  (int)14

/* Input */
#define GFX_KEY_DOWN (int)0x00
#define GFX_KEY_UP   (int)0x01

#define GFX_KEY_A (int)0x41
#define GFX_KEY_B (int)0x42
#define GFX_KEY_C (int)0x43
#define GFX_KEY_D (int)0x44
#define GFX_KEY_E (int)0x45
#define GFX_KEY_F (int)0x46
#define GFX_KEY_G (int)0x47
#define GFX_KEY_H (int)0x48
#define GFX_KEY_I (int)0x49
#define GFX_KEY_J (int)0x4A
#define GFX_KEY_K (int)0x4B
#define GFX_KEY_L (int)0x4C
#define GFX_KEY_M (int)0x4D
#define GFX_KEY_N (int)0x4E
#define GFX_KEY_O (int)0x4F
#define GFX_KEY_P (int)0x50
#define GFX_KEY_Q (int)0x51
#define GFX_KEY_R (int)0x52
#define GFX_KEY_S (int)0x53
#define GFX_KEY_T (int)0x54
#define GFX_KEY_U (int)0x55
#define GFX_KEY_V (int)0x56
#define GFX_KEY_W (int)0x57
#define GFX_KEY_X (int)0x58
#define GFX_KEY_Y (int)0x59
#define GFX_KEY_Z (int)0x5A

/* OpenGL Context Options (legacy versions not supported) */

/* Core */
#define GFX_OPENGL_CORE_3_0 (int)0x50
#define GFX_OPENGL_CORE_3_1 (int)0x51
#define GFX_OPENGL_CORE_3_2 (int)0x52
#define GFX_OPENGL_CORE_3_3 (int)0x53
#define GFX_OPENGL_CORE_4_0 (int)0x54
#define GFX_OPENGL_CORE_4_1 (int)0x55
#define GFX_OPENGL_CORE_4_2 (int)0x56
#define GFX_OPENGL_CORE_4_3 (int)0x57
#define GFX_OPENGL_CORE_4_4 (int)0x58
#define GFX_OPENGL_CORE_4_5 (int)0x59
#define GFX_OPENGL_CORE_4_6 (int)0x5A

/* Compatibility */
#define GFX_OPENGL_COMPATIBILITY_3_2 (int)0x5B
#define GFX_OPENGL_COMPATIBILITY_3_3 (int)0x5C
#define GFX_OPENGL_COMPATIBILITY_4_0 (int)0x5D
#define GFX_OPENGL_COMPATIBILITY_4_1 (int)0x5E
#define GFX_OPENGL_COMPATIBILITY_4_2 (int)0x5F
#define GFX_OPENGL_COMPATIBILITY_4_3 (int)0x60
#define GFX_OPENGL_COMPATIBILITY_4_4 (int)0x61
#define GFX_OPENGL_COMPATIBILITY_4_5 (int)0x62
#define GFX_OPENGL_COMPATIBILITY_4_6 (int)0x63

/* Core Debug */
#define GFX_OPENGL_CORE_4_3_DEBUG (int)0x64
#define GFX_OPENGL_CORE_4_4_DEBUG (int)0x65
#define GFX_OPENGL_CORE_4_5_DEBUG (int)0x66
#define GFX_OPENGL_CORE_4_6_DEBUG (int)0x67

/* Compatibility Debug */
#define GFX_OPENGL_COMPATIBILITY_4_3_DEBUG (int)0x68
#define GFX_OPENGL_COMPATIBILITY_4_4_DEBUG (int)0x69
#define GFX_OPENGL_COMPATIBILITY_4_5_DEBUG (int)0x6A
#define GFX_OPENGL_COMPATIBILITY_4_6_DEBUG (int)0x6B

#endif // LIBGFX_DEFS_H