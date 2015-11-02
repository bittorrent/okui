#pragma once

#include "onair/platform.h"

#if ONAIR_IOS || ONAIR_TVOS
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
    #define OPENGL_ES 1
#elif defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl3.h>
#elif defined(__ANDROID__)
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    #define OPENGL_ES 1
#elif defined(__linux__)
    #if !defined(GL_GLEXT_PROTOTYPES)
        #define GL_GLEXT_PROTOTYPES
    #endif
    #include <GL/gl.h>
#endif

#if OPENGL_ES
#define ONAIR_OKUI_COMMON_SHADER_HEADER \
    "precision highp float;\n" \
    "#define VARYING_IN varying\n" \
    "#define VARYING_OUT varying\n" \
    "#define ATTRIBUTE_IN attribute\n" \
    "#define SAMPLE texture2D\n" \

#define ONAIR_OKUI_VERTEX_SHADER_HEADER ONAIR_OKUI_COMMON_SHADER_HEADER
#define ONAIR_OKUI_FRAGMENT_SHADER_HEADER ONAIR_OKUI_COMMON_SHADER_HEADER \
    "#define COLOR_OUT gl_FragColor\n"

#else
#define ONAIR_OKUI_COMMON_SHADER_HEADER \
    "#version 140\n" \
    "#define VARYING_IN in\n" \
    "#define VARYING_OUT out\n" \
    "#define ATTRIBUTE_IN in\n" \
    "#define SAMPLE texture\n" \

#define ONAIR_OKUI_VERTEX_SHADER_HEADER ONAIR_OKUI_COMMON_SHADER_HEADER
#define ONAIR_OKUI_FRAGMENT_SHADER_HEADER ONAIR_OKUI_COMMON_SHADER_HEADER \
    "out vec4 COLOR_OUT;\n"

#endif

#if !NDEBUG
#define ONAIR_OKUI_GL_ERROR_CHECK() \
    { \
        auto err = glGetError(); \
        if (err != GL_NO_ERROR) { \
            ONAIR_LOGF_WARNING("opengl error 0x%x", err);\
        } \
    }
#else
#define ONAIR_OKUI_GL_ERROR_CHECK()
#endif
