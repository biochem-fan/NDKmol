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

#ifndef VBOSPHERE_INCLUDED
#define VBOSPHERE_INCLUDED

#include "Renderable.hpp"

class VBOSphere: public Renderable {
private:
	static int faceVBO, vertexVBO, vertexNormalVBO, faceCount;

public:
	VBOSphere();
	VBOSphere(float x, float y, float z, float radius, Color c);
	static void prepareVBO();
	virtual void render();
};

#endif
