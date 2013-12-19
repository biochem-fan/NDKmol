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

#ifndef RENDERABLE_INCLUDED
#define RENDERABLE_INCLUDED

#include <vector>
#include "Color.hpp"

class Renderable {
public:
	float *vertexBuffer, *colorBuffer, *vertexNormalBuffer;
	unsigned short *faceBuffer;
	int nFaces;
	float scalex, scaley, scalez;
	float posx, posy, posz;
	float rot, rotx, roty, rotz;
	bool vertexColors;
	Color objectColor;
	std::vector<Renderable*> children;
//	public Color objectColor = new Color(1, 0, 0, 1);

	Renderable();
	virtual void setMatrix();
	void drawChildren();
	virtual void render();
	~Renderable();
};

#endif
