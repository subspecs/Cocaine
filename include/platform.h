#pragma once
#define DEBUG

//Default Includes.
#define GLFW_INCLUDE_NONE //Forces GLFW to use GLAD.
#include "glfw/glfw3.h"
#if defined(DEBUG)
#include <stdio.h>
#endif
#include <stdlib.h>

//Very helpful defines.
#define false 0
#define true 1
typedef int bool;
typedef unsigned char byte;

//Work Constants.
#define MaxProgramShaderSourceSize 512 * 1024 //512kb.

//Platform Specific.
#if defined(Cocaine_Win_64)
#include "glad/gl.h"
#include "windows.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"
#elif defined(Cocaine_Win_32)
#include "glad/gl.h"
#include "windows.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/glfw3native.h"
#elif defined(Cocaine_Win_ARM64)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_MacOS_x64)
#include "glad/gl.h"
#elif defined(Cocaine_MacOS_ARM64)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_MacOS_Universal)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_Linux_x64)
#include "glad/gl.h"
#elif defined(Cocaine_Linux_x32)
#include "glad/gl.h"
#elif defined(Cocaine_Linux_ARM64)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_Linux_ARM32)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_iOS_ARM32)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_iOS_ARM64)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_Android_ARM32)
#include "glad/gles2.h"
#define Cocaine_GLES
#elif defined(Cocaine_Android_ARM64)
#include "glad/gles2.h"
#define Cocaine_GLES
#endif

