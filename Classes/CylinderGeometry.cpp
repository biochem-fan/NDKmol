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

#include "CylinderGeometry.hpp"
#include <cmath>

float *CylinderGeometry::vertexBuffer = NULL;
float *CylinderGeometry::vertexNormalBuffer = NULL;
short *CylinderGeometry::faceBuffer = NULL;
int CylinderGeometry::nVertices, CylinderGeometry::nFaces;

float *CylinderGeometry::getVertexBuffer() {
	if (!vertexBuffer) prepare(cylinderQuality);
	return vertexBuffer;
}

float *CylinderGeometry::getVertexNormalBuffer() {
	if (!vertexNormalBuffer) prepare(cylinderQuality);
	return vertexNormalBuffer;
}

short *CylinderGeometry::getFaceBuffer() {
	if (!faceBuffer) prepare(cylinderQuality);
	return faceBuffer;
}

void CylinderGeometry::prepare(int div) {
	nVertices = (div + 1) * 2;
	vertexBuffer = new float[nVertices * 3];
	vertexNormalBuffer = new float[nVertices * 3];

	float cos = (float)std::cos(2 * M_PI / div);
	float sin = (float)std::sin(2 * M_PI / div);

	vertexBuffer[0] = 1; vertexBuffer[1] = 0;vertexBuffer[2] = 0;
	vertexBuffer[3] = 1; vertexBuffer[4] = 0;vertexBuffer[5] = 1;
	vertexNormalBuffer[0] = 1; vertexNormalBuffer[1] = 0;vertexNormalBuffer[2] = 0;
	vertexNormalBuffer[3] = 1; vertexNormalBuffer[4] = 0;vertexNormalBuffer[5] = 1;

	int offset = 6;
	for (int i = 1; i <= div; i++) {
		float x = vertexBuffer[offset - 3];
		float y = vertexBuffer[offset - 2];
		float nx = x * cos - y * sin;
		float ny = x * sin + y * cos;
		vertexBuffer[offset] = vertexBuffer[offset + 3] = vertexNormalBuffer[offset] = vertexNormalBuffer[offset + 3] = nx;
		vertexBuffer[offset + 1] = vertexBuffer[offset + 4] = vertexNormalBuffer[offset + 1] = vertexNormalBuffer[offset + 4] = ny;
		vertexBuffer[offset + 2] = vertexNormalBuffer[offset + 2] = vertexNormalBuffer[offset + 5] = 0;
		vertexBuffer[offset + 5] = 1;
		offset += 6;
	}


	nFaces = div * 2;
	faceBuffer = new short[nFaces * 3];
	offset = 0;
	for (int i = 0; i < div; i++) {
		faceBuffer[6 * i] = (short) (offset);
		faceBuffer[6 * i + 1] = (short) (offset + 1);
		faceBuffer[6 * i + 2] = (short) (offset + 2);
		faceBuffer[6 * i + 3] = (short) (offset + 2);
		faceBuffer[6 * i + 4] = (short) (offset + 1);
		faceBuffer[6 * i + 5] = (short) (offset + 3);
		offset += 2;
	}
}
