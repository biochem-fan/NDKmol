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


#include "Quaternion.h"

Quaternion Quaternion::multiply(Quaternion p, Quaternion q) {
	Quaternion ret;
	
	ret.x = p.x * q.w + p.y * q.z - p.z * q.y + p.w * q.x; // 1
	ret.y = -p.x * q.z + p.y * q.w + p.z * q.x + p.w * q.y; // 2
	ret.z = p.x * q.y - p.y * q.x + p.z * q.w + p.w * q.z; //3
	ret.w = -p.x * q.x - p.y * q.y - p.z * q.z + p.w * q.w; // 0
//	if (ret.w > 1) ret.w = 1;
//	if (ret.w < -1) ret.w = -1;
	
	return ret;
}

Quaternion Quaternion::clone() {
	Quaternion q(x, y, z, w);
	return q;
}

Quaternion Quaternion::invert() {
	x *= -1;
	y *= -1;
	z *= -1;
	return *this;
}

Quaternion Quaternion::normalize() {
	float r = (float)sqrt(x * x + y * y + z * z + w * w);
	x /= r;
	y /= r;
	z /= r;
	w /= r;
	return *this;
}

float Quaternion::getAngle() {
	return (float)acos(w) * 2;
}

void Quaternion::rotateVector(float vecx, float vecy, float vecz, float *xout, float *yout, float *zout) {
	Quaternion vec(vecx, vecy, vecz, 0);
	Quaternion inverted = clone().invert();
	Quaternion tmp = multiply(inverted, vec);
	Quaternion q = multiply(tmp, clone());
	
	*xout = q.x; *yout = q.y; *zout = q.z;
}

void Quaternion::getAxis(float *xout, float *yout, float *zout) {
	float angle = getAngle();
	float tmp = (float)sin(angle / 2);
	
	if (angle < 0.001 && angle > -0.001) {
		*xout = 1; *yout = 0; *zout = 0;
	} else {
	*xout = x / tmp;
	*yout = y / tmp;
	*zout = z / tmp;
	}
}

Quaternion::Quaternion() {
	x = 0; y = 0; z = 0; w = 1;
}

//public String toString() {
//	return "(" + x + ", " + y + ", " + z + ", " + w + ")";
//}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) {
	x = _x; y = _y; z = _z; w = _w;
}
