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

Mat16 inverseMatrix(Mat16 &m) {
    Mat16 ret;
    float *mat = m.m;
    
    // inverse(M) = t(adj(M)) / det(M)
    
    // M:matrix([a0,a4,a8,a12],[a1,a5,a9,a13], [a2,a6,a10,a14],[a3,a7,a11,a15]);
    double a0 = mat[0], a1 = mat[1], a2 = mat[2], a3 = mat[3],
           a4 = mat[4], a5 = mat[5], a6 = mat[6], a7 = mat[7],
           a8 = mat[8], a9 = mat[9], a10 = mat[10], a11 = mat[11],
           a12 = mat[12], a13 = mat[13], a14 = mat[14], a15 = mat[15];
    
    // stardisp: true;
    // transpose(adjoint(M));
    ret.m[0] = - (a15*a6 - a14*a7)*a9 + a13*(a11*a6 - a10*a7) + (a10*a15 - a11*a14)*a5;
    ret.m[1] = (a15*a2 - a14*a3)*a9 - a13*(a11*a2 - a10*a3)- a1*(a10*a15 - a11*a14);
    ret.m[2] =  a13*(a2*a7 - a3*a6) + a1*(a15*a6 - a14*a7)- (a15*a2 - a14*a3)*a5;
    ret.m[3] = - (a2*a7 - a3*a6)*a9 - a1*(a11*a6 - a10*a7) + (a11*a2 - a10*a3)*a5;
    ret.m[4] = (a15*a6 - a14*a7)*a8 - a12*(a11*a6 - a10*a7) - (a10*a15 - a11*a14)*a4;
    ret.m[5] = - (a15*a2 - a14*a3)*a8 + a12*(a11*a2 - a10*a3) + a0*(a10*a15 - a11*a14);
    ret.m[6] = - a12*(a2*a7 - a3*a6) - a0*(a15*a6 - a14*a7) + (a15*a2 - a14*a3)*a4;
    ret.m[7] = (a2*a7 - a3*a6)*a8 + a0*(a11*a6 - a10*a7) - (a11*a2 - a10*a3)*a4;
    ret.m[8] = a12*(a11*a5 - a7*a9) + a4*(a15*a9 - a11*a13) - (a15*a5 - a13*a7)*a8;
    ret.m[9] = - a12*(a1*a11 - a3*a9) - a0*(a15*a9 - a11*a13) + (a1*a15 - a13*a3)*a8;
    ret.m[10] = a0*(a15*a5 - a13*a7) + a12*(a1*a7 - a3*a5) - (a1*a15 - a13*a3)*a4;
    ret.m[11] = - a0*(a11*a5 - a7*a9) + a4*(a1*a11 - a3*a9) - (a1*a7 - a3*a5)*a8;
    ret.m[12] = - a12*(a10*a5 - a6*a9) - a4*(a14*a9 - a10*a13) + (a14*a5 - a13*a6)*a8;
    ret.m[13] = a12*(a1*a10 - a2*a9) + a0*(a14*a9 - a10*a13) - (a1*a14 - a13*a2)*a8;
    ret.m[14] = - a0*(a14*a5 - a13*a6) - a12*(a1*a6 - a2*a5) + (a1*a14 - a13*a2)*a4;
    ret.m[15] = a0*(a10*a5 - a6*a9) - a4*(a1*a10 - a2*a9) + (a1*a6 - a2*a5)*a8;
    
    double det =  (a0 * ret.m[0] + a4 * ret.m[1] + a8 * ret.m[2] + a12 * ret.m[3]);
    if (det != 0) {
        double det_inv = 1 / det;
        for (int i = 0; i < 12; i++) ret.m[i] *= det_inv;
    } else {
        printf("Error while inverting matrix.\n");
        ret = identityMatrix();
    }
    
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

// t(1 / M) = 1 / t(M)
Mat16 transposedInverseMatrix(Mat16 &m) {
    Mat16 ret;
    float *mat = m.m;
    
    // inverse(M) = t(adj(M)) / det(M)
    
    // M:matrix([a0,a4,a8,a12],[a1,a5,a9,a13], [a2,a6,a10,a14],[a3,a7,a11,a15]);
    double a0 = mat[0], a1 = mat[4], a2 = mat[8], a3 = mat[12],
    a4 = mat[1], a5 = mat[5], a6 = mat[9], a7 = mat[13],
    a8 = mat[2], a9 = mat[6], a10 = mat[10], a11 = mat[14],
    a12 = mat[3], a13 = mat[7], a14 = mat[11], a15 = mat[15];
    
    // stardisp: true;
    // transpose(adjoint(M));
    ret.m[0] = - (a15*a6 - a14*a7)*a9 + a13*(a11*a6 - a10*a7) + (a10*a15 - a11*a14)*a5;
    ret.m[1] = (a15*a2 - a14*a3)*a9 - a13*(a11*a2 - a10*a3)- a1*(a10*a15 - a11*a14);
    ret.m[2] =  a13*(a2*a7 - a3*a6) + a1*(a15*a6 - a14*a7)- (a15*a2 - a14*a3)*a5;
    ret.m[3] = - (a2*a7 - a3*a6)*a9 - a1*(a11*a6 - a10*a7) + (a11*a2 - a10*a3)*a5;
    ret.m[4] = (a15*a6 - a14*a7)*a8 - a12*(a11*a6 - a10*a7) - (a10*a15 - a11*a14)*a4;
    ret.m[5] = - (a15*a2 - a14*a3)*a8 + a12*(a11*a2 - a10*a3) + a0*(a10*a15 - a11*a14);
    ret.m[6] = - a12*(a2*a7 - a3*a6) - a0*(a15*a6 - a14*a7) + (a15*a2 - a14*a3)*a4;
    ret.m[7] = (a2*a7 - a3*a6)*a8 + a0*(a11*a6 - a10*a7) - (a11*a2 - a10*a3)*a4;
    ret.m[8] = a12*(a11*a5 - a7*a9) + a4*(a15*a9 - a11*a13) - (a15*a5 - a13*a7)*a8;
    ret.m[9] = - a12*(a1*a11 - a3*a9) - a0*(a15*a9 - a11*a13) + (a1*a15 - a13*a3)*a8;
    ret.m[10] = a0*(a15*a5 - a13*a7) + a12*(a1*a7 - a3*a5) - (a1*a15 - a13*a3)*a4;
    ret.m[11] = - a0*(a11*a5 - a7*a9) + a4*(a1*a11 - a3*a9) - (a1*a7 - a3*a5)*a8;
    ret.m[12] = - a12*(a10*a5 - a6*a9) - a4*(a14*a9 - a10*a13) + (a14*a5 - a13*a6)*a8;
    ret.m[13] = a12*(a1*a10 - a2*a9) + a0*(a14*a9 - a10*a13) - (a1*a14 - a13*a2)*a8;
    ret.m[14] = - a0*(a14*a5 - a13*a6) - a12*(a1*a6 - a2*a5) + (a1*a14 - a13*a2)*a4;
    ret.m[15] = a0*(a10*a5 - a6*a9) - a4*(a1*a10 - a2*a9) + (a1*a6 - a2*a5)*a8;
    
    double det =  (a0 * ret.m[0] + a4 * ret.m[1] + a8 * ret.m[2] + a12 * ret.m[3]);
    if (det != 0) {
        double det_inv = 1 / det;
        for (int i = 0; i < 12; i++) ret.m[i] *= det_inv;
    } else {
        printf("Error while inverting matrix.\n");
        ret = identityMatrix();
    }
    
    return ret;
}