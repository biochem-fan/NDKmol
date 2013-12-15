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
//	glRotatef(rot, rotx, roty, rotz);
//	glScalef(scalex, scaley, scalez);
}

//public Renderable(FloatBuffer vertices, ShortBuffer faces) {
//	vertexBuffer = vertices;
//	faceBuffer = faces;
//}


void Renderable::drawChildren() {
//	__android_log_print(ANDROID_LOG_DEBUG,"Renderable","rendering children started");
	for (int i = 0; i < children.size(); i++) {
//		__android_log_print(ANDROID_LOG_DEBUG,"Renderable","rendering children %d/%d", i + 1, children.size());
		children[i]->render();
	}
}

void Renderable::render() {
//	glPushMatrix();
    setMatrix();
	drawChildren();

	if (vertexColors && colorBuffer != NULL) {
        glUniform1f(shaderUseVertexColor, 1);
        glEnableVertexAttribArray(shaderVertexColor);
        glVertexAttribPointer(shaderVertexColor, 4, GL_FLOAT, GL_FALSE, 0, colorBuffer);
//		glEnableClientState(GL_COLOR_ARRAY);
//		glColorPointer(4, GL_FLOAT, 0, colorBuffer);
	} else {
        glUniform1f(shaderUseVertexColor, 0);
        glUniform4f(shaderObjColor, objectColor.r, objectColor.g, objectColor.b, objectColor.a);
//		glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
	}
	if (nFaces > 0) {
//        glUseProgram(shaderProgram);
        glEnableVertexAttribArray(shaderVertexPosition);
        glVertexAttribPointer(shaderVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
//        glUniformMatrix4fv(shaderProjectionMatrix, 1, GL_FALSE, projectionMatrix.m);
		glDrawElements(GL_TRIANGLES, nFaces, GL_UNSIGNED_SHORT, faceBuffer);
        
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
//	glPopMatrix();
}

