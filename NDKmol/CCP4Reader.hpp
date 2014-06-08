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

#ifndef CCP4READER_INCLUDED
#define CCP4READER_INCLUDED

#include <string>
#include "Matrix.hpp"

class CCP4file {
private:

public:
	CCP4file(std::string filename);
	~CCP4file();
	Mat16 getMatrix(bool scale);
	bool parseHeader(unsigned char* header);
	
	float* map = NULL;
	// These arrays are 1-indexed!!
	int NCRS[4], NSTART[4], NXYZ[4], MAPCRS[4], ISPG, NSYMBT;
	float a, b, c, alpha, beta, gamma, AMIN, AMAX, AMEAN, ARMS;
	float basis[4][3];
};

#endif
