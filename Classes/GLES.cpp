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

#include "GLES.hpp"
#include "stdio.h"
#include "stdlib.h"

std::stack<Mat16> glStack;
Mat16 currentModelViewMatrix;

unsigned int shaderProgram = 0, shaderVertexPosition = 0, shaderVertexNormal = 0;
unsigned int shaderModelViewMatrix = 0, shaderProjectionMatrix = 0, shaderNormalMatrix = 0;
unsigned int shaderObjColor = 0, shaderUseVertexColor = 0, shaderVertexColor = 0;

const GLchar *vertexShader =
"attribute vec4 vertexPosition; \n" // INPUT is vec3; the 4th element is automatically set to 1
"attribute vec4 vertexNormal; \n"
"uniform mat4 modelviewMatrix; \n"
"uniform mat4 normalMatrix; \n" // FIXME: should be mat3
"uniform mat4 projectionMatrix; \n"
"uniform vec4 objColor; \n" // FIXME: Do we need alpha?
"uniform float useVertexColor; \n" // FIXME: should use #define for performance
"attribute vec4 vertexColor; \n"
" \n"
"varying vec4 colorOut; \n"
"void main() {\n"
" \n"
"     vec4 diffuseLight1 = vec4(0.6, 0.6, 0.6, 1); \n"
"     vec4 ambientLight1 = vec4(0.2, 0.2, 0.2, 1); \n"
"     vec4 positionLight1 = vec4(0.0, 0.0, 1.0, 1.0); \n"
"     vec4 diffuseLight2 = vec4(0.1, 0.1, 0.1, 1); \n"
"     vec4 positionLight2 = vec4(0.0, 0.0, -1.0, 1.0); \n"
"     gl_Position = projectionMatrix * modelviewMatrix * vertexPosition; \n"
"     vec3 transformedNormal = normalize(mat3(normalMatrix) * vertexNormal.xyz); \n"
"     vec4 matColor = (useVertexColor > 0.5) ? vertexColor: objColor; \n"
"     vec4 diffuse = matColor * diffuseLight1 * abs(dot(vec4(transformedNormal, 1), positionLight1));"
"     diffuse += matColor * diffuseLight2 * abs(dot(vec4(transformedNormal, 1), positionLight2));"
"     vec4 ambient = matColor * ambientLight1;"
"     colorOut = diffuse + ambient; \n"
"     colorOut.a = 1.0; \n"
"} \n";

const GLchar *fragmentShader =
"precision mediump float; \n"
"varying vec4 colorOut; \n"
"void main() { \n"
"    gl_FragColor = colorOut; \n"
"} \n";

void glPushMatrix() {
    glStack.push(currentModelViewMatrix);
}

void glPopMatrix() {
    currentModelViewMatrix = glStack.top();
    glStack.pop();
}

GLuint CreateShader(const GLchar *vs, const GLchar *fs) {
    char buf[1024];
    int status = 0;
	
    GLuint prog = glCreateProgram();
	
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vs, NULL);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(vshader, 1024, NULL, buf);
        printf("Vertex shader compilation failed: %s\n", buf);
        glDeleteShader(vshader);
        glDeleteProgram(prog);
        return 0;
    }
	glAttachShader(prog, vshader);
    glDeleteShader(vshader); // to-be deleted flag
	
	
    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fs, NULL);
    glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        glGetShaderInfoLog(fshader, 1024, NULL, buf);
        printf("Fragment shader compilation failed: %s\n", buf);
        glDeleteShader(fshader);
        glDeleteProgram(prog);
        return 0;
    }
	glAttachShader(prog, fshader);
    glDeleteShader(fshader); // to-be deleted flag

    glLinkProgram(prog);
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        printf("Shader Link failed.\n");
        return 0;
    }
	
    return prog;
}

