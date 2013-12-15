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

#ifndef RIBBONSTRIP_INCLUDED
#define RIBBONSTRIP_INCLUDED

#include "Renderable.hpp"
#include "Geometry.hpp"
#include <vector>

class RibbonStrip: public Renderable {
public:
	static const int div = 5;
	RibbonStrip(std::vector<Vector3> &_points1, std::vector<Vector3> &_points2, std::vector<Color> &colors, float thickness);
	RibbonStrip(std::vector<Vector3> &_points1, std::vector<Vector3> &_points2, std::vector<bool> &smoothen, std::vector<Color> &colors, float thickness);
	
private:
	void initMesh(float *points1, float *points2, std::vector<Color> &colors, int num);
	void initMesh(float *points1, float *points2, std::vector<Color> &colors, int num, float thickness);
};

#endif
