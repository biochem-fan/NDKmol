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

#ifndef VECTOR3_INCLUDED
#define VECTOR3_INCLUDED

typedef struct Mat16 {
	float m[16];
} Mat16;

class Vector3 {
public:
	float x, y, z;
	Vector3();
	Vector3(float x, float y, float z);
	void set(float x, float y, float z);
	static float dot(Vector3 &p, Vector3 &q);
	static Vector3 cross(Vector3 &p, Vector3 &q);
	static double norm(float x, float y, float z);
	Vector3& normalize();
	Vector3& multiplyScalar(float s);
	Vector3& applyMat16(Mat16 m);
};

#endif
