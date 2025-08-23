#pragma once

// #define GL_SILENCE_DEPRECATION

#ifndef GL_FRAGMENT_PRECISION_HIGH
#define GL_FRAGMENT_PRECISION_HIGH 1
#endif

#include <GL/gl.h>

// #ifndef GL_GLEXT_PROTOTYPES
// #define GL_GLEXT_PROTOTYPES 1
// #endif

#include <GL/glext.h>

// #define GLFW_INCLUDE_GLEXT 1
// #include <GLFW/glfw3.h>
// #include <webgl/webgl2.h>  //  much better performance without

#define GL_GLES_PROTOTYPES 1
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
// #include <GLES3/gl31.h>
// #include <GLES3/gl32.h>
#define EGL_EGL_PROTOTYPES 1
#define EGL_EGLEXT_PROTOTYPES 1
#include <EGL/egl.h>
#include <EGL/eglext.h>
//  #include <EGL/eglplatform.h>
// #include <KHR/khrplatform.h>
