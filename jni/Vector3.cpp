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

#include "Vector3.hpp"
#include <cmath>

Vector3::Vector3() {
	x = y = z = 0;
}

Vector3::Vector3(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

void Vector3::set(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}

float Vector3::dot(Vector3 &p, Vector3 &q) {
	return p.x * q.x + p.y * q.y + p.z * q.z;
}

Vector3 Vector3::cross(Vector3 &p, Vector3 &q) {
	return Vector3(p.y * q.z - p.z * q.y, p.z * q.x - p.x * q.z, p.x * q.y - p.y * q.x);
}

double Vector3::norm(float x, float y, float z) {
	return std::sqrt(x * x + y * y + z * z);
}

Vector3& Vector3::normalize() {
	float n = (float)norm(x, y, z);
	x /= n;
	y /= n;
	z /= n;
	return *this;
}

Vector3& Vector3::multiplyScalar(float s) {
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::applyMat16(Mat16 mat) {
	float nx, ny, nz;
	
	nx = mat.m[0] * x + mat.m[1] * y + mat.m[2] * z + mat.m[3];
	ny = mat.m[4] * x + mat.m[5] * y + mat.m[6] * z + mat.m[7];
	nz = mat.m[8] * x + mat.m[9] * y + mat.m[10] * z + mat.m[11];

	x = nx; y = ny; z = nz;
	return *this;
}
