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

#ifndef MARCHING_SQUARES_INCLUDED
#define MARCHING_SQUARES_INCLUDED

#include <string>
#include "Renderable.hpp"
#include "MTZreader.hpp"

// FIXME: should inherit Line?
class MarchingSquares: public Renderable {
private:
	int xd, zd, yd, size, size2, size3, maxvert, nPoints;;
	int nc, nr, ns;
	float isol;
	float pout[12]; // 4 vertices * XYZ
	Mat16 matrix;
	
	void VIntX(int offset, float x, float y, float z, float valp1, float valp2);
	void VIntY(int offset, float x, float y, float z, float valp1, float valp2);
	void VIntZ(int offset, float x, float y, float z, float valp1, float valp2);
	void polygonizeYZ(int fx, int fy, int fz, int q);
	void polygonizeXY(int fx, int fy, int fz, int q);
	void polygonizeXZ(int fx, int fy, int fz, int q);
	
	void prepareVBO();
	virtual void setMatrix();

public:
	MarchingSquares(MTZfile *mtzfile);
	void build(int cc, int cr, int cs, int radius, float isol);
	void render();
	float *field;
	float width; // line width
	Mat16 getMatrix();
};

#endif
