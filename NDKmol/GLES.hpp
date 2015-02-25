/*  NDKmol - Molecular Viewer on Android NDK
 
 (C) Copyright 2011 - 2012, biochem_fan
 
 This file is part of NDKmol.
 
 NDKmol is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef GLES_INCLUDED
#define GLES_INCLUDED

#ifdef __ANDROID__
#include <android/log.h>
#elif __APPLE__
#include "TargetConditionals.h"
#endif

// debug
#define OPENGL_ES1

#ifdef OPENGL_ES1

#ifdef __ANDROID__
#include <GLES/gl.h>
#elif TARGET_OS_IPHONE
#include <OpenGLES/ES1/gl.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include <GL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

// OPENGL_ES2
#else
#ifdef __ANDROID__
#include <GLES2/gl2.h>
#elif TARGET_OS_IPHONE
#include <OpenGLES/ES2/gl.h>
#elif __APPLE__
#include <OpenGL/gl.h>
#else
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#endif

#include "Matrix.hpp"
#include <stack>

extern std::stack<Mat16> glStack;
extern unsigned int shaderProgram, shaderVertexPosition, shaderVertexNormal;
extern unsigned int shaderModelViewMatrix, shaderProjectionMatrix, shaderNormalMatrix;
extern unsigned int shaderVertexColor;
extern Mat16 currentModelViewMatrix;

#ifdef _WIN32
extern PFNGLGENBUFFERSPROC          glGenBuffers;
extern PFNGLBINDBUFFERPROC          glBindBuffer;
extern PFNGLBUFFERDATAPROC          glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLDELETEBUFFERSPROC       glDeleteBuffers;
void PrepareGlFunctions();
#endif

#ifndef OPENGL_ES1
extern const GLchar *vertexShader, *fragmentShader;
GLuint CreateShader(const GLchar *vs, const GLchar *fs);
void glPushMatrix();
void glPopMatrix();
#endif

#endif
