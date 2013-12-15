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

#include "Matrix.hpp"
#include "stdio.h"
#include "math.h"

// In Open GL, matrix is stored in column-major
// 0 4 8  12
// 1 5 9  13
// 2 6 10 14
// 3 7 11 15
// (12, 13, 14) is translational component
// OpenGL ES does not accept transpose argument. (only GL_FALSE is valid)

// http://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
Mat16 matrixFrustum(double left, double right, double bottom, double top, double near, double far) {
    Mat16 ret = {};
//    printf("matrixFrustum: %f %f %f %f %f %f\n", left, right, bottom, top, near, far);
    ret.m[0] = 2 * near / (right - left);
    ret.m[5] = 2 * near / (top - bottom);
    ret.m[8] = (right + left) / (right - left);
    ret.m[9] = (top + bottom) / (top - bottom);
    ret.m[10] = - (far + near) / (far - near);
    ret.m[11] = -1;
    ret.m[14] = -2 * far * near / (far - near);
    
    return ret;
}

Mat16 identityMatrix() {
    Mat16 ret = {};
    ret.m[0] = ret.m[5] = ret.m[10] = ret.m[15] = 1;
    return ret;
}

// http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
Mat16 rotationMatrix(double angle, double ax, double ay, double az) {
    Mat16 ret = identityMatrix();
    
    double len = sqrt(ax * ax + ay * ay + az * az);
//    printf("rotationMatrix: rad=%f (%f ,%f ,%f) len=%f\n", angle, ax, ay, az, len);

    if (len == 0) return ret;
    ax /= len; ay /= len; az /= len;
    double c = cos(angle), s = sin(angle);
    ret.m[0] = ax * ax * (1 - c) + c;
    ret.m[1] = ax * ay * (1 - c) + az * s;
    ret.m[2] = ax * az * (1 - c) - ay * s;
    ret.m[4] = ax * ay * (1 - c) - az * s;
    ret.m[5] = ay * ay * (1 - c) + c;
    ret.m[6] = ay * az * (1 - c) + ax * s;
    ret.m[8] = ax * az * (1 - c) + ay * s;
    ret.m[9] = ay * az * (1 - c) - ax * s;
    ret.m[10] = az * az * (1 - c) + c;
    
    return ret;
}

Mat16 scaleMatrix(double x, double y, double z) {
    Mat16 ret = identityMatrix();
    ret.m[0] = x;
    ret.m[5] = y;
    ret.m[10] = z;
    return ret;
}

Mat16 translationMatrix(double x, double y, double z) {
    Mat16 ret = identityMatrix();
    ret.m[12] = x;
    ret.m[13] = y;
    ret.m[14] = z;
    return ret;
}

Mat16 multiplyMatrix(Mat16 &a, Mat16 &b) {
    Mat16 ret = {};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            double tmp = 0;
            for (int k = 0; k < 4; k++) {
                tmp += a.m[i + k * 4] * b.m[k + j * 4];
            }
            ret.m[i + j * 4] = tmp;
        }
    }
    
    return ret;
}