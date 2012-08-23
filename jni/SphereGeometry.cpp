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

#include "SphereGeometry.hpp"
#include <cmath>

float *SphereGeometry::vertexBuffer = NULL;
float *SphereGeometry::vertexNormalBuffer = NULL;
unsigned short *SphereGeometry::faceBuffer = NULL;
int SphereGeometry::nVertices, SphereGeometry::nFaces;

float *SphereGeometry::getVertexBuffer() {
	if (!vertexBuffer) prepare(sphereQuality, sphereQuality);
	return vertexBuffer;
}

float *SphereGeometry::getVertexNormalBuffer() {
	if (!vertexNormalBuffer) prepare(sphereQuality, sphereQuality);
	return vertexNormalBuffer;
}

unsigned short *SphereGeometry::getFaceBuffer() {
	if (!faceBuffer) prepare(sphereQuality, sphereQuality);
	return faceBuffer;
}

void SphereGeometry::prepare(int div1, int div2) {
	nVertices = (div1 + 1) * (div2 + 1);
	vertexBuffer = new float[nVertices * 3];

	int offset = 0;
	for (int i = 0; i <= div1; i++) { // z
		float z = 1 - 2.0f * i / div1;
		float r = (float)std::sqrt(1 - z * z);
		for (int j = 0; j <= div2; j++) {
			float x = r * (float)std::cos(j * 2 * M_PI / div2);
			float y = r * (float)std::sin(j * 2 * M_PI / div2);
			vertexBuffer[offset] = x;
			vertexBuffer[offset + 1] = y;
			vertexBuffer[offset + 2] = z;
			offset += 3;
		}
	}

	nFaces = div1 * div2 * 2;
	faceBuffer = new unsigned short[nFaces * 3];
	offset = 0;
	for (int i = 0; i < div1; i++) { // z
		for (int j = 0; j < div2; j++) {
			faceBuffer[offset] = (short)((div2 + 1) * i + j);
			faceBuffer[offset + 1] = (short) ((div2 + 1) * i + j + 1);
			faceBuffer[offset + 2] = (short)((div2 + 1) * (i + 1) + j + 1);
			faceBuffer[offset + 3] = (short)((div2 + 1) * i + j);
			faceBuffer[offset + 4] = (short) ((div2 + 1) * (i + 1) + j + 1);
			faceBuffer[offset + 5] = (short)((div2 + 1) * (i + 1) + j);
			offset += 6;
		}
	}

	vertexNormalBuffer = vertexBuffer;
}
