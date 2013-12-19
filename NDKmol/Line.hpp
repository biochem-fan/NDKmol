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

#ifndef LINE_INCLUDED
#define LINE_INCLUDED

#include "Renderable.hpp"
#include "Vector3.hpp"
#include "Geometry.hpp"
#include <vector>

class Line: public Renderable {
public:
	float width;
	bool discrete;
	int nPoints;

	Line(std::vector<Vector3> &points);
	Line(std::vector<Vector3> &points, std::vector<Color> &colors);
	Line(float*, int);
	Line();
	virtual void render();
};


#endif
