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

#include "PDBReader.hpp"
#include "SDFReader.hpp"
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <iostream>

SDFReader::SDFReader() {
	protein = new Protein();
	atoms = protein->atoms;
}

Protein* SDFReader::parseSDF(std::string filename) {
	std::ifstream ifs(filename.c_str(), std::ios::in);

	std::string line;
	std::vector<std::string> lines;

	while (getline(ifs, line)) {
		lines.push_back(line);
	}
	ifs.close();

	protein->sdffile = true;
	if (lines.size() < 4) return NULL;

	int atomCount = safeParseInt(lines[3], 0, 3);
	if (atomCount <= 0) return NULL;

	int bondCount = safeParseInt(lines[3], 3, 3);
	int offset = 4;
	if (lines.size() < 4 + atomCount + bondCount) return NULL;

	for (int i = 1; i <= atomCount; i++) {
		line = lines[offset++];
		Atom *atom = atoms + i;
		atom->valid = true;
		atom->serial = i;
		atom->x = safeParseFloat(line, 0, 10);
		atom->y = safeParseFloat(line, 10, 10);
		atom->z = safeParseFloat(line, 20, 10);
		atom->hetflag = true;
		atom->atom = atom->elem = safeParseString(line, 31, 3);
	}
	for (int i = 1; i <= bondCount; i++) {
		line = lines[offset++];
		int from = safeParseInt(line, 0, 3);
		int to = safeParseInt(line, 3, 3);
		int order = safeParseInt(line, 6, 3);
		atoms[from].bonds[to] = order;
		atoms[to].bonds[from] = order;
	}

	return protein;
}

