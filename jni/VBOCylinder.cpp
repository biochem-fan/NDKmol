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

#include "VBOCylinder.hpp"
#include "CylinderGeometry.hpp"
#include <GLES/gl.h>
#include <android/log.h>
#include <cmath>

int VBOCylinder::faceVBO = -1, VBOCylinder::vertexVBO = -1, VBOCylinder::vertexNormalVBO = -1, VBOCylinder::faceCount = -1;

VBOCylinder::VBOCylinder() {}

VBOCylinder::VBOCylinder(float x1, float y1, float z1, float x2, float y2, float z2, float radius, Color color) {
	objectColor = color;

	double dist = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
	if (dist < 0.001) return;

	posx = x1; posy = y1; posz = z1;
	if (std::abs(x1 - x2) > 0.0001 || std::abs(y1 - y2) > 0.001){
		rot = (float) (180 / M_PI * std::acos((z2 - z1) / dist));
		rotx = y1 - y2;
		roty = x2 - x1;
		rotz = 0;
	} else {
		rot = (float) (180 / M_PI * std::acos((z2 - z1) / dist));
		rotx = 1;
		roty = 0;
		rotz = 0;
	}

	scalex = scaley = radius; scalez = (float) dist;
}

void VBOCylinder::prepareVBO() {
	GLuint vbo[3];
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, CylinderGeometry::nVertices * 3 * 4, CylinderGeometry::getVertexBuffer(), GL_STATIC_DRAW);
	vertexVBO = vbo[0];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, CylinderGeometry::nVertices * 3 * 4, CylinderGeometry::getVertexNormalBuffer(), GL_STATIC_DRAW);
	vertexNormalVBO = vbo[1];

	faceCount = CylinderGeometry::nFaces;
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * 2, CylinderGeometry::getFaceBuffer(), GL_STATIC_DRAW);
	faceVBO = vbo[2];

	__android_log_print(ANDROID_LOG_DEBUG,"VBOCylinder", "prepared VBOs: vertex %d normal %d face %d", vertexVBO, vertexNormalVBO, faceVBO);

	// unbind -- IMPORTANT! otherwise, GL will crash!
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBOCylinder::render() {
	glPushMatrix();
	setMatrix();

//	__android_log_print(ANDROID_LOG_DEBUG,"VBOCylinder", "rendering VBOs: Num %d vertex %d normal %d face %d", faceCount, vertexVBO, vertexNormalVBO, faceVBO);
	glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalVBO);
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
	glDrawElements(GL_TRIANGLES, VBOCylinder::faceCount * 3, GL_UNSIGNED_SHORT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

