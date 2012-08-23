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

#include "MatRenderable.hpp"
#include <GLES/gl.h>
#include <android/log.h>

MatRenderable::MatRenderable() : Renderable() {
}

MatRenderable::~MatRenderable() {
	for (int i = 0, lim = matrices.size(); i < lim; i++) {
		delete[] matrices[i];
	}
}

void MatRenderable::addMatrix(Mat16 &mat) {
	float *matrix = new float[16];

	matrix[0] = mat.m[0];
	matrix[4] = mat.m[1];
	matrix[8] = mat.m[2];
	matrix[12] = mat.m[3];
	matrix[1] = mat.m[4];
	matrix[5] = mat.m[5];
	matrix[9] = mat.m[6];
	matrix[13] = mat.m[7];
	matrix[2] = mat.m[8];
	matrix[6] = mat.m[9];
	matrix[10] = mat.m[10];
	matrix[14] = mat.m[11];
	matrix[3] = mat.m[12];
	matrix[7] = mat.m[13];
	matrix[11] = mat.m[14];
	matrix[15] = mat.m[15];

	matrices.push_back(matrix);
}

void MatRenderable::render() {
	glPushMatrix();
	setMatrix();

	for (int i = 0, lim = matrices.size(); i < lim; i++) {
		glPushMatrix(); // FIXME: crash!
		glMultMatrixf(matrices[i]);
		drawChildren();
		glPopMatrix();
	}

	glPopMatrix();
}
