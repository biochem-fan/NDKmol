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

#include "Atom.h"
#include <cmath>

Atom::Atom() {
	ss = "c";
	color.setRGB(0.8f, 0.8f, 0.8f, 1);
	valid = false;
}

int Atom::isConnected(Atom &atom) {
	int order = bonds[atom.serial];
	if (order > 0) return order;

	float distSquared = (x - atom.x) * (x - atom.x) + (y - atom.y) * (y - atom.y) + (z - atom.z) * (z - atom.z);
	if (std::isnan(distSquared)) return 0;
	if (distSquared < 0.5) return 0;
	if (distSquared > 1.3 && (atom.elem == "H" || elem == "H")) return 0;
	if (distSquared < 3.42 && (atom.elem == "S" || elem == "S")) return 1;
	if (distSquared > 2.78) return 0;

	return true;
}
