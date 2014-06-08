/*  NDKmol - Molecular Viewer on Android NDK

     (C) Copyright 2011 - 2014, biochem_fan

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

/*
 
 This code is based on Three.js's implementation of Marching Cubes by
  alteredq http://alteredqualia.com/
   http://github.com/mrdoob/three.js/blob/master/examples/js/MarchingCubes.js
    , which is port of greggman's ThreeD version of marching cubes to Three.js
   http://webglsamples.googlecode.com/hg/blob/blob.html
 
 Then @biochem_fan modified it into Marching Squares as implemented in
 Cuemol http://www.cuemol.org/en/ .
 
 Original license of Three.js is as follows.
 
 The MIT License
 
 Copyright (C) 2010-2014 three.js authors
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include <cstdio>
#include "MarchingSquares.hpp"
#include "GLES.hpp"
#include "Debug.hpp"

void MarchingSquares::prepareVBO() {
// FIXME: meanwhile, we do not use VBO since mesh will be updated often
	return;
}

void MarchingSquares::setMatrix() {
#ifdef OPENGL_ES1
	//	__android_log_print(ANDROID_LOG_DEBUG, "NdkView","MatRenderable rendering %d/%d", i, lim);
	glMultMatrixf(matrix.m);
#else
	currentModelViewMatrix = multiplyMatrix(currentModelViewMatrix, matrix);
	glUniformMatrix4fv(shaderModelViewMatrix, 1, GL_FALSE, currentModelViewMatrix.m);
#endif
}

void MarchingSquares::render() {
	if (nPoints == 0) return;
		
	glPushMatrix();
	setMatrix();
	
	glLineWidth(width);

#ifdef OPENGL_ES1
	glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
		
	glVertexPointer(3,GL_FLOAT, 0, vertexBuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
#else
	glDisableVertexAttribArray(shaderVertexNormal); // disable feeding from an array
	glVertexAttrib4f(shaderVertexNormal, 0, 0, 0, 0); // instead use this value
		
	glDisableVertexAttribArray(shaderVertexColor);
	glVertexAttrib4f(shaderVertexColor, objectColor.r, objectColor.g, objectColor.b, objectColor.a);
			
	glEnableVertexAttribArray(shaderVertexPosition);
	glVertexAttribPointer(shaderVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
#endif

	glDrawArrays(GL_LINES, 0, nPoints / 3);
			
#ifdef OPENGL_ES1
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_LIGHTING);
#else
	glDisableVertexAttribArray(shaderVertexColor);
#endif
	glPopMatrix();
}
	
MarchingSquares::MarchingSquares(CCP4file *mapfile) {
	nc = mapfile->NCRS[1]; nr = mapfile->NCRS[2]; ns = mapfile->NCRS[3];
	maxvert = 1000000; // 1M vertices = 3 * 4  MB
	
	size = mapfile->NCRS[1];
	size2 = size * mapfile->NCRS[2];
	size3 = size2 * mapfile->NCRS[3];
		
	yd = size;
	zd = size2;
	field = mapfile->map;
	
	nPoints = 0;
	objectColor.r = 0.2; objectColor.g = 0.2; objectColor.b = 1.0; objectColor.a = 1.0;
	width = 1;
	matrix = mapfile->getMatrix(false);
	
	vertexBuffer = (float*)malloc(sizeof(float) * maxvert);
}
	
Mat16 MarchingSquares::getMatrix() {
	return matrix;
}

void MarchingSquares::build(int cc, int cr, int cs, int radius, float isol) {
	MarchingSquares::isol = isol;
	int q, x, y, z, y_offset, z_offset;
	
	int xlo = cc - radius, xhi = cc + radius,
        ylo = cr - radius, yhi = cr + radius,
        zlo = cs - radius, zhi = cs + radius;
	if (xlo < 0) xlo = 0;
	if (ylo < 0) ylo = 0;
	if (zlo < 0) zlo = 0;
	if (xhi < xlo) xhi = xlo;
	if (yhi < ylo) yhi = ylo;
	if (zhi < zlo) zhi = zlo;
	if (xhi > nc - 2) xhi = nc - 2;
	if (yhi > nr - 2) yhi = nr - 2;
	if (zhi > ns - 2) zhi = ns - 2;
	DEBUG_PRINTF("Mesh Range: (%d, %d, %d)-(%d, %d, %d)\n", xlo, ylo, zlo, xhi, yhi, zhi);
	
	nPoints = 0;
	
	for (z = zlo; z <= zhi ; z++) {
		z_offset = size2 * z;
		for (y = ylo; y <= yhi; y ++) {
            y_offset = z_offset + size * y;
            for (x = xlo; x <= xhi; x ++) {
				if (nPoints - 12 > maxvert * 3) return;
				q = y_offset + x;
				polygonizeXY(x, y, z, q);
				polygonizeYZ(x, y, z, q);
				polygonizeXZ(x, y, z, q);
            }
		}
	}
	DEBUG_PRINTF("Marching Squares: nPoints = %d\n", nPoints);
	// we don't need to fill 0s.
}

void MarchingSquares::VIntX(int offset, float x, float y, float z, float valp1, float valp2) {
	pout[offset] = x + (isol - valp1) / (valp2 - valp1);
	pout[offset + 1] = y;
	pout[offset + 2] = z;
}

void MarchingSquares::VIntY(int offset, float x, float y, float z, float valp1, float valp2) {
	pout[offset] = x;
	pout[offset + 1] = y + (isol - valp1) / (valp2 - valp1);
	pout[offset + 2] = z;
}

void MarchingSquares::VIntZ(int offset, float x, float y, float z, float valp1, float valp2) {
	pout[offset] = x;
	pout[offset + 1] = y;
	pout[offset + 2] = z + (isol - valp1) / (valp2 - valp1);
}

void MarchingSquares::polygonizeYZ(int fx, int fy, int fz, int q) {
	int qz = q + zd, qy = q + yd, qyz = qz + yd;
	
	float field0 = field[q], field1 = field[qz], field2 = field[qy], field3 = field[qyz];
	
	bool s0 = ((field1 - isol) * (field0 - isol) > 0),
		 s1 = ((field3 - isol) * (field1 - isol) > 0),
         s2 = ((field2 - isol) * (field3 - isol) > 0),
	     s3 = ((field0 - isol) * (field2 - isol) > 0);
	int fz2 = fz + 1, fy2 = fy + 1;
		
	if (s0  && s1 && s2  && s3) return;
	if (!s0 && !s1 && !s2  && !s3) {
		VIntZ(0, fx, fy, fz, field0, field1); // s0
		VIntZ(3, fx, fy2, fz, field2, field3); // s2
		VIntY(6, fx, fy, fz2, field1, field3); // s1
		VIntY(9, fx, fy, fz, field0, field2); // s3
		for (int i = 0; i < 12; i++) vertexBuffer[nPoints++] = pout[i];
		return;
	}
	
	if (!s0 && s1 && !s2 && s3) {
		VIntZ(0, fx, fy, fz, field0, field1); // s0
		VIntZ(3, fx, fy2, fz, field2, field3); // s2
	} else if (s0 && !s1 && s2 && !s3) {
		VIntY(0, fx, fy, fz2, field1, field3); // s1
		VIntY(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && s1 && !s2 && !s3) {
		VIntZ(0, fx, fy2, fz, field2, field3); // s2
		VIntY(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && !s1 && !s2 && s3) {
		VIntY(0, fx, fy, fz2, field1, field3); // s1
		VIntZ(3, fx, fy2, fz, field2, field3); // s2
	} else if (!s0 && !s1 && s2 && s3) {
		VIntZ(0, fx, fy, fz, field0, field1); // s0
		VIntY(3, fx, fy, fz2, field1, field3); // s1
	} else if (!s0 && s1 && s2 && !s3) {
		VIntZ(0, fx, fy, fz, field0, field1); // s0
		VIntY(3, fx, fy, fz, field0, field2); // s3
	}
	
	for (int i = 0; i < 6; i++) vertexBuffer[nPoints++] = pout[i];
}

void MarchingSquares::polygonizeXY(int fx, int fy, int fz, int q) {
	int qx = q + 1, qy = q + yd, qxy = qx + yd;
	
	float field0 = field[q], field1 = field[qx], field2 = field[qy], field3 = field[qxy];
	
	bool s0 = ((field1 - isol) * (field0 - isol) > 0),
	s1 = ((field3 - isol) * (field1 - isol) > 0),
	s2 = ((field2 - isol) * (field3 - isol) > 0),
	s3 = ((field0 - isol) * (field2 - isol) > 0);
	int fx2 = fx + 1, fy2 = fy + 1;
	
	if (s0  && s1 && s2  && s3) return;
	if (!s0 && !s1 && !s2  && !s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntX(3, fx, fy2, fz, field2, field3); // s2
		VIntY(6, fx2, fy, fz, field1, field3); // s1
		VIntY(9, fx, fy, fz, field0, field2); // s3
		for (int i = 0; i < 12; i++) vertexBuffer[nPoints++] = pout[i];
		return;
	}
	
	if (!s0 && s1 && !s2 && s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntX(3, fx, fy2, fz, field2, field3); // s2
	} else if (s0 && !s1 && s2 && !s3) {
		VIntY(0, fx2, fy, fz, field1, field3); // s1
		VIntY(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && s1 && !s2 && !s3) {
		VIntX(0, fx, fy2, fz, field2, field3); // s2
		VIntY(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && !s1 && !s2 && s3) {
		VIntY(0, fx2, fy, fz, field1, field3); // s1
		VIntX(3, fx, fy2, fz, field2, field3); // s2
	} else if (!s0 && !s1 && s2 && s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntY(3, fx2, fy, fz, field1, field3); // s1
	} else if (!s0 && s1 && s2 && !s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntY(3, fx, fy, fz, field0, field2); // s3
	}
	
	for (int i = 0; i < 6; i++) vertexBuffer[nPoints++] = pout[i];
}

void MarchingSquares::polygonizeXZ(int fx, int fy, int fz, int q) {
	int qx = q + 1, qz = q + zd, qxz = qx + zd;
	
	float field0 = field[q], field1 = field[qx], field2 = field[qz], field3 = field[qxz];
	
	bool s0 = ((field1 - isol) * (field0 - isol) > 0),
	s1 = ((field3 - isol) * (field1 - isol) > 0),
	s2 = ((field2 - isol) * (field3 - isol) > 0),
	s3 = ((field0 - isol) * (field2 - isol) > 0);
	int fx2 = fx + 1, fz2 = fz + 1;
	
	if (s0  && s1 && s2  && s3) return;
	if (!s0 && !s1 && !s2  && !s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntX(3, fx, fy, fz2, field2, field3); // s2
		VIntZ(6, fx2, fy, fz, field1, field3); // s1
		VIntZ(9, fx, fy, fz, field0, field2); // s3
		for (int i = 0; i < 12; i++) vertexBuffer[nPoints++] = pout[i];
		return;
	}
	
	if (!s0 && s1 && !s2 && s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntX(3, fx, fy, fz2, field2, field3); // s2
	} else if (s0 && !s1 && s2 && !s3) {
		VIntZ(0, fx2, fy, fz, field1, field3); // s1
		VIntZ(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && s1 && !s2 && !s3) {
		VIntX(0, fx, fy, fz2, field2, field3); // s2
		VIntZ(3, fx, fy, fz, field0, field2); // s3
	} else if (s0 && !s1 && !s2 && s3) {
		VIntZ(0, fx2, fy, fz, field1, field3); // s1
		VIntX(3, fx, fy, fz2, field2, field3); // s2
	} else if (!s0 && !s1 && s2 && s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntZ(3, fx2, fy, fz, field1, field3); // s1
	} else if (!s0 && s1 && s2 && !s3) {
		VIntX(0, fx, fy, fz, field0, field1); // s0
		VIntZ(3, fx, fy, fz, field0, field2); // s3
	}
	
	for (int i = 0; i < 6; i++) vertexBuffer[nPoints++] = pout[i];
}


