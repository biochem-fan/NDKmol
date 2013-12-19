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

#include "RibbonStrip.hpp"
#include "Geometry.hpp"
#include <cmath>

RibbonStrip::RibbonStrip(std::vector<Vector3> &_points1, std::vector<Vector3> &_points2, std::vector<bool> &smoothen, std::vector<Color> &colors, float thickness): Renderable() {
	if (_points1.size() < 1) return; // §≥§Ï§¨§¢§Î§´§È°¢nFaces §ŒΩÈ¥¸≤Ω§ÚÀ∫§Ï§ §§!
	
	float *points1 = subdivide(_points1, div, smoothen);
	float *points2 = subdivide(_points2, div, smoothen);
	
	if (thickness > 0) {
		initMesh(points1, points2, colors, _points1.size(), thickness);
	} else {
		initMesh(points1, points2, colors, _points1.size());
	}

}


RibbonStrip::RibbonStrip(std::vector<Vector3> &_points1, std::vector<Vector3> &_points2, std::vector<Color> &colors, float thickness): Renderable() {
	if (_points1.size() < 1) return; // §≥§Ï§¨§¢§Î§´§È°¢nFaces §ŒΩÈ¥¸≤Ω§ÚÀ∫§Ï§ §§!

	float *points1 = subdivide(_points1, div);
	float *points2 = subdivide(_points2, div);

	if (thickness > 0) {
		initMesh(points1, points2, colors, _points1.size(), thickness);
	} else {
		initMesh(points1, points2, colors, _points1.size());
	}
}

// points1 and points2 will be deleted within this method
void RibbonStrip::initMesh(float *points1, float *points2, std::vector<Color> &colors, int num, float thickness) {
	vertexColors = true;
	int size = ((num - 1) * div + 1); // number of segments
	vertexBuffer = new float[(size * 8 + 4) * 3]; // 8 triangles / segment + cap (4 triangles)
	vertexNormalBuffer = new float[(size * 8 + 4) * 3];
	faceBuffer = new unsigned short[(size * 8 + 4) * 3];
	nFaces = ((size - 1) * 8) * 3; // FIX: We should cap ends (+4)
	
	float *vb = vertexBuffer;
	unsigned short *fb = faceBuffer;
	
	Vector3 toNext, toSide, axis; // TODO: Of course we can improve this!
	for (int i = 0; i < size; i++) {
		int vertexOffset = i * 3;
		*(vb++) = points1[vertexOffset]; // 0
		*(vb++) = points1[vertexOffset + 1];
		*(vb++) = points1[vertexOffset + 2];
		*(vb++) = points1[vertexOffset]; // 1
		*(vb++) = points1[vertexOffset + 1];
		*(vb++) = points1[vertexOffset + 2];
		*(vb++) = points2[vertexOffset]; // 2
		*(vb++) = points2[vertexOffset + 1];
		*(vb++) = points2[vertexOffset + 2];
		*(vb++) = points2[vertexOffset]; // 3
		*(vb++) = points2[vertexOffset + 1];
		*(vb++) = points2[vertexOffset + 2];
		
		if (i < size - 1) {
			toNext.x = points1[vertexOffset + 3] - points1[vertexOffset + 0];
			toNext.y = points1[vertexOffset + 4] - points1[vertexOffset + 1];
			toNext.z = points1[vertexOffset + 5] - points1[vertexOffset + 2];
			toSide.x = points2[vertexOffset + 0] - points1[vertexOffset + 0];
			toSide.y = points2[vertexOffset + 1] - points1[vertexOffset + 1];
			toSide.z = points2[vertexOffset + 2] - points1[vertexOffset + 2];
			axis = Vector3::cross(toSide, toNext).normalize().multiplyScalar(thickness);
		}
		*(vb++) = axis.x + points1[vertexOffset]; // 4
		*(vb++) = axis.y + points1[vertexOffset + 1];
		*(vb++) = axis.z + points1[vertexOffset + 2];
		*(vb++) = axis.x + points1[vertexOffset]; // 5
		*(vb++) = axis.y + points1[vertexOffset + 1];
		*(vb++) = axis.z + points1[vertexOffset + 2];
		*(vb++) = axis.x + points2[vertexOffset]; // 6
		*(vb++) = axis.y + points2[vertexOffset + 1];
		*(vb++) = axis.z + points2[vertexOffset + 2];
		*(vb++) = axis.x + points2[vertexOffset]; // 7
		*(vb++) = axis.y + points2[vertexOffset + 1];
		*(vb++) = axis.z + points2[vertexOffset + 2];
	}
	
	int faces[] = {0, 2, -6, 0, -6, -8, -4, -2, 6, -4, 6, 4, // top and bottom
					7, 3, -5, 7, -5, -1, -3, -7, 1, -3, 1, 5}; // sides
	for (int i = 1; i < size; i++) {
		int faceOffset = i * 8;
		for (int j = 0; j < 24; j++) {
			*(fb++) = (short)(faceOffset + faces[j]);
		}
	}
	
	float *vnb = vertexNormalBuffer;
	for (int i = 0; i < size; i++) { // TODO: We shouldn't calculate them again. Reuse vb.
		int vertexOffset = 3 * i;
		
		float ax = 0, ay = 0, az = 0; // for ribbon direction, take average
		if (i > 0) {
			ax += points1[vertexOffset] - points1[vertexOffset - 3];
			ay += points1[vertexOffset + 1] - points1[vertexOffset - 2];
			az += points1[vertexOffset + 2] - points1[vertexOffset - 1];
		}
		if (i < size - 1) {
			ax += points1[vertexOffset + 3] - points1[vertexOffset];
			ay += points1[vertexOffset + 4] - points1[vertexOffset + 1];
			az += points1[vertexOffset + 5] - points1[vertexOffset + 2];
		}
		float bx = points2[vertexOffset] - points1[vertexOffset];
		float by = points2[vertexOffset + 1] - points1[vertexOffset + 1];
		float bz = points2[vertexOffset + 2] - points1[vertexOffset + 2];
		
		float nx = ay * bz - az * by;
		float ny = az * bx - ax * bz;
		float nz = ax * by - ay * bx;
		
		float norm = (float)std::sqrt(nx * nx + ny * ny + nz * nz);
		nx /= norm; ny /= norm; nz /= norm; // TODO: Is this correct? Invert??
		
		norm = (float)std::sqrt(bx * bx + by * by + bz * bz);
		bx /= norm; by /= norm; bz /= norm;
		
		// 0 2 - top, 4 6 - bottom, 1 5 - side, 3 7 - side
		*(vnb++) = nx; // 0 top
		*(vnb++) = ny;
		*(vnb++) = nz;

		*(vnb++) = bx; // 1 side1
		*(vnb++) = by;
		*(vnb++) = bz;
		
		*(vnb++) = nx; // 2 top
		*(vnb++) = ny;
		*(vnb++) = nz;
		
		*(vnb++) = -bx; // 3 side2
		*(vnb++) = -by;
		*(vnb++) = -bz;
		
		*(vnb++) = -nx; // 4 bottom
		*(vnb++) = -ny;
		*(vnb++) = -nz;
		
		*(vnb++) = bx; // 5 side1
		*(vnb++) = by;
		*(vnb++) = bz;
		
		*(vnb++) = -nx; // 6 bottom
		*(vnb++) = -ny;
		*(vnb++) = -nz;
		
		*(vnb++) = -bx; // 7 side2
		*(vnb++) = -by;
		*(vnb++) = -bz;
		
	}
	
	colorBuffer = colorVectorToFloatArray(colors, div * 8);
	
	delete points1;
	delete points2;
}


// points1 and points2 will be deleted within this method
void RibbonStrip::initMesh(float *points1, float *points2, std::vector<Color> &colors, int num) {
	vertexColors = true;
	int size = ((num - 1) * div + 1); // number of segments
	
	vertexBuffer = new float[size * 2 * 3]; // 2 triangles / segment * 3 vertices / triangle
	vertexNormalBuffer = new float[size * 2 * 3];
	faceBuffer = new unsigned short[size * 2 * 3];
	nFaces = (size - 1) * 2 * 3;
	
	int vertexOffset = 0, faceOffset = 2;
	float *vb = vertexBuffer;
	unsigned short *fb = faceBuffer;
	
	for (int i = 0; i < size; i++) {
		*(vb++) = points1[vertexOffset]; // face 2 * i
		*(vb++) = points1[vertexOffset + 1];
		*(vb++) = points1[vertexOffset + 2];
		*(vb++) = points2[vertexOffset]; // face 2 * i + 1
		*(vb++) = points2[vertexOffset + 1];
		*(vb++) = points2[vertexOffset + 2];
		vertexOffset += 3;
		
		if (i == 0) continue;
		*(fb++) = (short)(faceOffset - 2);
		*(fb++) = (short)(faceOffset - 1);
		*(fb++) = (short)faceOffset;
		*(fb++) = (short)faceOffset;
		*(fb++) = (short)(faceOffset - 1);
		*(fb++) = (short)(faceOffset + 1);
		faceOffset += 2;
	}
	
	float *vnb = vertexNormalBuffer;
	for (int i = 0; i < size; i++) {
		vertexOffset = 3 * i;
		float ax = 0, ay = 0, az = 0; // for ribbon direction, take average
		if (i > 0) {
			ax += points1[vertexOffset] - points1[vertexOffset - 3];
			ay += points1[vertexOffset + 1] - points1[vertexOffset - 2];
			az += points1[vertexOffset + 2] - points1[vertexOffset - 1];
		}
		if (i < size - 1) {
			ax += points1[vertexOffset + 3] - points1[vertexOffset];
			ay += points1[vertexOffset + 4] - points1[vertexOffset + 1];
			az += points1[vertexOffset + 5] - points1[vertexOffset + 2];
		}
		float bx = points2[vertexOffset] - points1[vertexOffset];
		float by = points2[vertexOffset + 1] - points1[vertexOffset + 1];
		float bz = points2[vertexOffset + 2] - points1[vertexOffset + 2];
		
		float nx = ay * bz - az * by;
		float ny = az * bx - ax * bz;
		float nz = ax * by - ay * bx;
		
		float norm = (float)std::sqrt(nx * nx + ny * ny + nz * nz);
		nx /= norm; ny /= norm; nz /= norm;
		
		*(vnb++) = nx;
		*(vnb++) = ny;
		*(vnb++) = nz;
		*(vnb++) = nx;
		*(vnb++) = ny;
		*(vnb++) = nz;
	}
	
	colorBuffer = colorVectorToFloatArray(colors, div * 2);
	
	delete points1;
	delete points2;
}
