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

#include "VBOSphere.hpp"
#include "SphereGeometry.hpp"
#include <GLES/gl.h>
#include <android/log.h>

int VBOSphere::faceVBO = -1, VBOSphere::vertexVBO = -1, VBOSphere::vertexNormalVBO = -1, VBOSphere::faceCount = -1;

VBOSphere::VBOSphere() {}

VBOSphere::VBOSphere(float x, float y, float z, float radius, Color c) {
	scalex = scaley = scalez = radius;
	posx = x; posy = y; posz = z;
	objectColor = c;
}

void VBOSphere::prepareVBO() {
	GLuint vbo[3];
	glGenBuffers(3, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, SphereGeometry::nVertices * 3 * 4, SphereGeometry::getVertexBuffer(), GL_STATIC_DRAW);
	vertexVBO = vbo[0];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, SphereGeometry::nVertices * 3 * 4, SphereGeometry::getVertexNormalBuffer(), GL_STATIC_DRAW);
	vertexNormalVBO = vbo[1];

	faceCount = SphereGeometry::nFaces;
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, faceCount * 3 * 2, SphereGeometry::getFaceBuffer(), GL_STATIC_DRAW);
	faceVBO = vbo[2];

	__android_log_print(ANDROID_LOG_DEBUG,"VBOSphere", "prepared VBOs: vertex %d normal %d face %d", vertexVBO, vertexNormalVBO, faceVBO);

	// unbind -- IMPORTANT! otherwise, GL will crash!
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VBOSphere::render() {
	glPushMatrix();
	setMatrix();

	glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexNormalVBO);
	glNormalPointer(GL_FLOAT, 0, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceVBO);
	glDrawElements(GL_TRIANGLES, VBOSphere::faceCount * 3, GL_UNSIGNED_SHORT, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopMatrix();
}

