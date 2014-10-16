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

#include "CCP4Reader.hpp"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <zlib.h>

CCP4file::CCP4file(std::string filename) {
	FILE *ccp4in = std::fopen(filename.c_str(), "rb");
	if (!ccp4in) return;
	
	unsigned char buf[300];
	std::fread(buf, 4, 56, ccp4in);
	if (parseHeader(buf)) {
		std:fseek(ccp4in, 256 * 4 + NSYMBT, SEEK_SET);
		map = (float*)malloc(NCRS[1] * NCRS[2] * NCRS[3] * sizeof(float));
		if (map) std::fread(map, sizeof(float), NCRS[1] * NCRS[2] * NCRS[3], ccp4in);
		fclose(ccp4in);
		return;
	} else if (buf[0] == 0x1f && buf[1] == 0x8b) { // gzipped map
		fclose(ccp4in);
		
		gzFile ccp4gz = gzopen(filename.c_str(), "rb");
		if (!ccp4gz) return;
		gzread(ccp4gz, buf, 56 * 4);
		if (parseHeader(buf)) {
			gzseek(ccp4gz, 256 * 4 + NSYMBT, SEEK_SET);
			map = (float*)malloc(NCRS[1] * NCRS[2] * NCRS[3] * sizeof(float));
			if (map) gzread(ccp4gz, map, NCRS[1] * NCRS[2] * NCRS[3] * sizeof(float));
		}
		gzclose(ccp4gz);
	}
}

bool CCP4file::parseHeader(unsigned char* buf) {
	if (buf[52 * 4] != 'M' || buf[52 * 4 + 1] != 'A' || buf[52 * 4 + 2] != 'P') return false;
	
	int *header_int = (int*)buf;
	float *header_float = (float*)buf;
	NCRS[1] = header_int[0];
	NCRS[2] = header_int[1];
	NCRS[3] = header_int[2];
	NSTART[1] = header_int[4];
	NSTART[2] = header_int[5];
	NSTART[3] = header_int[6];
	NXYZ[1] = header_int[7];
	NXYZ[2] = header_int[8];
	NXYZ[3] = header_int[9];
	a = header_float[10];
	b = header_float[11];
	c = header_float[12];
	alpha = header_float[13];
	beta = header_float[14];
	gamma = header_float[15];
	MAPCRS[1] = header_int[16];
	MAPCRS[2] = header_int[17];
	MAPCRS[3] = header_int[18];
	ISPG = header_int[22];
	NSYMBT = header_int[23];
	AMIN = header_float[19];
	AMAX = header_float[20];
	AMEAN = header_float[21];
	ARMS = header_float[54];
	
	printf("CCP4file: NCRS = (%d, %d, %d), NSTART = (%d, %d, %d), NXYZ = (%d, %d, %d)\n",
		   NCRS[1], NCRS[2], NCRS[3], NSTART[1], NSTART[2], NSTART[3], NXYZ[1], NXYZ[2], NXYZ[3]);
	printf("CCP4file: CELL %f %f %f %f %f %f\n", a, b, c, alpha, beta, gamma);
	printf("CCP4file: MIN %f MEAN %f MAX %f RMS %f\n", AMIN, AMEAN, AMAX, ARMS);
	
	return true;
}

Mat16 CCP4file::getMatrix(bool scale) {
	// TODO: Should share codes with PDBReader.cpp
	basis[1][0] = a;
	basis[1][1] = 0;
	basis[1][2] = 0;
	basis[2][0] = (float)(b * std::cos(M_PI / 180.0 * gamma));
	basis[2][1] = (float)(b * std::sin(M_PI / 180.0 * gamma));
	basis[2][2] = 0;
	basis[3][0] = (float)(c * std::cos(M_PI / 180.0 * beta));
	basis[3][1] = (float)(c * (std::cos(M_PI / 180.0 * alpha) -
					  std::cos(M_PI / 180.0 * gamma)
					* std::cos(M_PI / 180.0 * beta))
						  / std::sin(M_PI / 180.0 * gamma));
	basis[3][2] = (float)(std::sqrt(c * c * std::sin(M_PI / 180.0 * beta)
								 * std::sin(M_PI / 180.0 * beta) - basis[3][1] * basis[3][1]));
	
	Mat16 matrix;
	matrix.m[0] = basis[MAPCRS[1]][0] / NXYZ[MAPCRS[1]];
	matrix.m[1] = basis[MAPCRS[1]][1] / NXYZ[MAPCRS[1]];
	matrix.m[2] = basis[MAPCRS[1]][2] / NXYZ[MAPCRS[1]];
	matrix.m[3] = 0;
	matrix.m[4] = basis[MAPCRS[2]][0] / NXYZ[MAPCRS[2]];
	matrix.m[5] = basis[MAPCRS[2]][1] / NXYZ[MAPCRS[2]];
	matrix.m[6] = basis[MAPCRS[2]][2] / NXYZ[MAPCRS[2]];
	matrix.m[7] = 0;
	matrix.m[8] = basis[MAPCRS[3]][0] / NXYZ[MAPCRS[3]];
	matrix.m[9] = basis[MAPCRS[3]][1] / NXYZ[MAPCRS[3]];
	matrix.m[10] = basis[MAPCRS[3]][2] / NXYZ[MAPCRS[3]];
	matrix.m[11] = 0;
	matrix.m[12] = 0;
	matrix.m[13] = 0;
	matrix.m[14] = 0;
	matrix.m[15] = 1;
	
	Mat16 centering = translationMatrix(NSTART[1], NSTART[2], NSTART[3]);
	matrix = multiplyMatrix(matrix, centering);
	
	if (scale) {
		Mat16 scaling = scaleMatrix(NCRS[1], NCRS[2], NCRS[3]);
		matrix = multiplyMatrix(matrix, scaling);
	}
	
	return matrix;
	
	/* Note on MATRIX
	 
	 for MC
	 
	 mesh.matrix.set(basis[mapcrs[1]][0] / nxyz[mapcrs[1]], basis[mapcrs[2]][0] / nxyz[mapcrs[2]], basis[mapcrs[3]][0] / nxyz[mapcrs[3]], 0,
	 basis[mapcrs[1]][1] / nxyz[mapcrs[1]], basis[mapcrs[2]][1] / nxyz[mapcrs[2]], basis[mapcrs[3]][1] / nxyz[mapcrs[3]], 0,
	 basis[mapcrs[1]][2] / nxyz[mapcrs[1]],  basis[mapcrs[2]][2] / nxyz[mapcrs[2]], basis[mapcrs[3]][2] / nxyz[mapcrs[3]], 0,
	 0, 0, 0, 1);
	 mapTransMatrix = new THREE.Matrix4().makeTranslation(map_header.NCSTART, map_header.NRSTART, map_header.NSSTART);
	 mesh.matrix.multiply(mesh.matrix, mapTransMatrix);
	 
	 for Volume Rendering
	 //  grid (CRS) coordinate to orthogonal (XYZ) coordinate
	 matrix = new THREE.Matrix4(basis[mapcrs[1]][0] / nxyz[mapcrs[1]], basis[mapcrs[2]][0] / nxyz[mapcrs[2]], basis[mapcrs[3]][0] / nxyz[mapcrs[3]], 0,
	 basis[mapcrs[1]][1] / nxyz[mapcrs[1]], basis[mapcrs[2]][1] / nxyz[mapcrs[2]], basis[mapcrs[3]][1] / nxyz[mapcrs[3]], 0,
	 basis[mapcrs[1]][2] / nxyz[mapcrs[1]], basis[mapcrs[2]][2] / nxyz[mapcrs[2]], basis[mapcrs[3]][2] / nxyz[mapcrs[3]], 0,
	 0, 0, 0, 1);
	 
	 // Below, C, R, S axis is mapped to X, Y, Z axis in the pseudo-3D texture.
	 // We are working in grid coordinate.
	 mapTransMatrix = new THREE.Matrix4().makeTranslation(start[1], start[2], start[3]);
	 matrix.multiply(matrix, mapTransMatrix);
	 tmp = new THREE.Matrix4().makeScale(crs[1], crs[2], crs[3]);
	 matrix.multiply(matrix, tmp);
	 */
}

CCP4file::~CCP4file() {
	if (!map) {
		free(map);
	}
}
