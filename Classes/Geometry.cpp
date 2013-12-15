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

#include "Geometry.hpp"

float* vectorToFloatArray(std::vector<Vector3> &points) {
	float *ret = new float[points.size() * 3];
	for (int i = 0, lim = points.size(); i < lim; i++) {
		Vector3 v = points[i];
		ret[3 * i] = v.x;
		ret[3 * i + 1] = v.y;
		ret[3 * i + 2] = v.z;
	}
	return ret;
}

unsigned short* vectorToShortArray(std::vector<unsigned short> &faces) {
	unsigned short *ret = new unsigned short[faces.size()];
	for (int i = 0, lim = faces.size(); i < lim; i++) {
		ret[i] = faces[i];
	}
	return ret;
}

float* colorVectorToFloatArray(std::vector<Color> &colors, int duplicate) {
	float *ret = new float[colors.size() * duplicate * 4];
	int offset = 0;
	for (int i = 0, lim = colors.size(); i < lim; i++) {
		Color color = colors[i];
		for (int j = 0; j < duplicate; j++) {
			ret[offset] = color.r;
			ret[offset + 1] = color.g;
			ret[offset + 2] = color.b;
			ret[offset + 3] = color.a;
			offset += 4;
			
		}
	}
	return ret;
}

float* subdivide(std::vector<Vector3> &_points, int div, std::vector<bool> &smoothen) {
	std::vector<Vector3> points;
	points.push_back(_points[0]);
	for (int i = 1, lim = (int)_points.size() - 1; i < lim; i++) {
		if (!smoothen[i]) {
			points.push_back(_points[i]);
		} else {
			points.push_back(Vector3((_points[i].x + _points[i + 1].x) / 2,
							 (_points[i].y + _points[i + 1].y) / 2,
							 (_points[i].z + _points[i + 1].z) / 2));
		}
	}
	points.push_back(_points[_points.size() - 1]);
	
	return subdivide(points, div);
}

// Catmull-Rom subdivision
// number of returned elements = ((points.size() - 1) * div + 1) * 3.
float* subdivide(std::vector<Vector3> &points, int div) {
	float *ret = new float[((points.size() - 1) * div + 1) * 3];
	
	int offset = 0;
	for (int i = -1, size = points.size(); i <= size - 3; i++) {
		Vector3 p0 = points[(i == -1) ? 0 : i];
		Vector3 p1 = points[i + 1];
		Vector3 p2 = points[i + 2];
		Vector3 p3 = points[(i == size - 3) ? size - 1 : i + 3];
		
		float v0x = (p2.x - p0.x) / 2;
		float v0y = (p2.y - p0.y) / 2;
		float v0z = (p2.z - p0.z) / 2;
		float v1x = (p3.x - p1.x) / 2;
		float v1y = (p3.y - p1.y) / 2;
		float v1z = (p3.z - p1.z) / 2;
		
		for (int j = 0; j < div; j++) {
			float t = 1.0f / div * j;
			float x = p1.x + t * v0x
			+ t * t * (-3 * p1.x + 3 * p2.x - 2 * v0x - v1x)
			+ t * t * t * (2 * p1.x - 2 * p2.x + v0x + v1x);
			float y = p1.y + t * v0y
			+ t * t * (-3 * p1.y + 3 * p2.y - 2 * v0y - v1y)
			+ t * t * t * (2 * p1.y - 2 * p2.y + v0y + v1y);
			float z = p1.z + t * v0z
			+ t * t * (-3 * p1.z + 3 * p2.z - 2 * v0z - v1z)
			+ t * t * t * (2 * p1.z - 2 * p2.z + v0z + v1z);
			ret[offset] = x;
			ret[offset + 1] = y;
			ret[offset + 2] = z;
			offset += 3;
		}
	}
	Vector3 last = points[points.size() - 1];
	ret[offset] = last.x;
	ret[offset + 1] = last.y;
	ret[offset + 2] = last.z;
	return ret;
}
