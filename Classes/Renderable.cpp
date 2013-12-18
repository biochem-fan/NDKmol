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

#include "Renderable.hpp"
//#import <OpenGLES/ES1/gl.h>
#include "GLES.hpp"
#include "math.h"

Renderable::Renderable() {
	scalex = 1; scaley = 1; scalez = 1;
	posx = 0; posy = 0; posz = 0;
	rot = 0; rotx = 1; roty = 1; rotz = 0;
	vertexColors = false;
	vertexBuffer = NULL; colorBuffer = NULL; vertexNormalBuffer = NULL;
	faceBuffer = NULL;
	nFaces = 0;
	children.clear();
}

Renderable::~Renderable() {
	if (vertexBuffer) {
		delete vertexBuffer;
		vertexBuffer = NULL;
	}
	if (vertexNormalBuffer) {
		delete vertexNormalBuffer;
		vertexNormalBuffer = NULL;
	}
	if (faceBuffer) {
		delete faceBuffer;
		faceBuffer = NULL;
	}
	if (colorBuffer) {
		delete colorBuffer;
		colorBuffer = NULL;
	}
	for (int i = 0, lim = children.size(); i < lim; i++) {
		if (children[i]) {
			delete children[i];
			children[i] = NULL;			
		}
	}
}

void Renderable::setMatrix() {
//	glTranslatef(posx, posy, posz);
    Mat16 tmp = translationMatrix(posx, posy, posz);
	currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, tmp);
//	glRotatef(rot, rotx, roty, rotz);
    tmp = rotationMatrix(rot / 180 * M_PI, rotx, roty, rotz);
    currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, tmp);
    tmp = scaleMatrix(scalex, scaley, scalez);
    currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, tmp);
//	glScalef(scalex, scaley, scalez);

    glUniformMatrix4fv(shaderModelViewMatrix, 1, GL_FALSE, currentModelViewMatrix.m);
    glUniformMatrix3fv(shaderNormalMatrix, 1, GL_FALSE, transposedInverseMatrix9(currentModelViewMatrix).m);
}

void Renderable::drawChildren() {
//	__android_log_print(ANDROID_LOG_DEBUG,"Renderable","rendering children started");
	for (int i = 0; i < children.size(); i++) {
//		__android_log_print(ANDROID_LOG_DEBUG,"Renderable","rendering children %d/%d", i + 1, children.size());
		children[i]->render();
	}
}

void Renderable::prepareVBO() {
    // TODO: Implement me. We need nVertices and nFaces here.
    GLuint vbo[4];
	glGenBuffers(4, vbo);
    
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float), vertexBuffer, GL_STATIC_DRAW);
	vertexVBO = vbo[0];
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float), vertexNormalBuffer, GL_STATIC_DRAW);
	vertexNormalVBO = vbo[1];
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, nFaces * sizeof(short), faceBuffer, GL_STATIC_DRAW);
	faceVBO = vbo[2];
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

    if (vertexColors) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
        glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(float) * 4 / 3, colorBuffer, GL_STATIC_DRAW);
        colorVBO = vbo[3];
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Renderable::render() {
    if (vertexVBO == -1) {
        prepareVBO();
     }
    
	glPushMatrix();
    setMatrix();
	drawChildren();

	if (vertexColors && colorBuffer != NULL) {
        glEnableVertexAttribArray(shaderVertexColor);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glVertexAttribPointer(shaderVertexColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glEnableClientState(GL_COLOR_ARRAY);
//		glColorPointer(4, GL_FLOAT, 0, colorBuffer);
	} else {
        glDisableVertexAttribArray(shaderVertexColor);
        glVertexAttrib4f(shaderVertexColor, objectColor.r, objectColor.g, objectColor.b, objectColor.a);
//		glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	}
	if (nFaces > 0) {
        glEnableVertexAttribArray(shaderVertexPosition);
    	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glVertexAttribPointer(shaderVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (vertexNormalBuffer != NULL) {
            glBindBuffer(GL_ARRAY_BUFFER, vertexNormalVBO);
            glEnableVertexAttribArray(shaderVertexNormal);
            glVertexAttribPointer(shaderVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
        glDrawElements(GL_TRIANGLES, nFaces, GL_UNSIGNED_SHORT, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
//        glDisableVertexAttribArray(shaderVertexPosition);
//        glUseProgram(0);

/*
        glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertexBuffer);
		if (vertexNormalBuffer != NULL) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glNormalPointer(GL_FLOAT, 0, vertexNormalBuffer);
		}
		glDrawElements(GL_TRIANGLES, nFaces, GL_UNSIGNED_SHORT, faceBuffer);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
 */
	}
	if (vertexColors) {
        glDisableVertexAttribArray(shaderVertexColor);
//		glDisableClientState(GL_COLOR_ARRAY);
	}
	glPopMatrix();
}

