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

#ifndef MATRIX_INCLUDED
#define MATRIX_INCLUDED

typedef struct Mat16 {
	float m[16];
} Mat16;

Mat16 matrixFrustum(double left, double right, double bottom, double top, double near, double far);
Mat16 multiplyMatrix(Mat16 &a, Mat16 &b);
Mat16 scaleMatrix(double x, double y, double z);
Mat16 translationMatrix(double x, double y, double z);
Mat16 rotationMatrix(double angle, double ax, double ay, double az);
Mat16 identityMatrix();

//TODO: we might need inverseMatrix

#endif
