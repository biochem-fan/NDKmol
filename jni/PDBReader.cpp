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
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <fstream>
#include <iostream>

PDBReader::PDBReader() {
	protein = new Protein();
	atoms = protein->atoms;
	compactMode = false;
}

void PDBReader::defineCell() {
	if (protein->a == 0) return;

	protein->ax = protein->a;
	protein->ay = 0;
	protein->az = 0;
	protein->bx = (float)(protein->b * std::cos(M_PI / 180.0 * protein->gamma));
	protein->by = (float)(protein->b * std::sin(M_PI / 180.0 * protein->gamma));
	protein->bz = 0;
	protein->cx = (float)(protein->c * std::cos(M_PI / 180.0 * protein->beta));
	protein->cy = (float)(protein->c * (std::cos(M_PI / 180.0 * protein->alpha) -
			std::cos(M_PI / 180.0 * protein->gamma)
	* std::cos(M_PI / 180.0 * protein->beta)
	/ std::sin(M_PI / 180.0 * protein->gamma)));
	protein->cz = (float)(std::sqrt(protein->c * protein->c * std::sin(M_PI / 180.0 * protein->beta)
	* std::sin(M_PI / 180.0 * protein->beta) - protein->cy * protein->cy));
}

std::string safeParseString(std::string &s, int from, int len) {
	std::string ret = "";

	int i = from;
	int lim = std::min((int)s.length(), from + len);

	for (; i < lim; i++) if (s[i] != ' ') break;
	for (i; i < lim; i++) {
		if (s[i] == ' ') break;
		ret += s[i];
	}

	return ret;
}

int safeParseInt(std::string &s, int from, int len) {
	return atoi(safeParseString(s, from, len).c_str());
}

float safeParseFloat(std::string &s, int from, int len) {
	return atof(safeParseString(s, from, len).c_str());
}

void PDBReader::parseOneLine(std::string line) {
	if (line.length() < 6) return;

	std::string recordName = safeParseString(line, 0, 6);

	if (recordName == "ATOM" || recordName == "HETATM") {
		std::string atomName = safeParseString(line, 12, 4);

		if (compactMode) {
			if (atomName != "CA" && atomName != "O" && atomName != "P" && recordName != "HETATM") {
				return;
			}
		}

		int serial = safeParseInt(line, 6, 5);
		std::string altLoc = safeParseString(line, 16, 1);
		if (altLoc != "" && altLoc != "A") return;
		Atom *atom = atoms + serial;
		atom->serial = serial;
		atom->atom = atomName;
		atom->resn = safeParseString(line, 17, 3);
		atom->chain = safeParseString(line, 21, 1);
		atom->resi = safeParseInt(line, 22, 5);
		atom->x = safeParseFloat(line, 30, 8);
		atom->y = safeParseFloat(line, 38, 8);
		atom->z = safeParseFloat(line, 46, 8);
		atom->b = safeParseFloat(line, 60, 8);
		atom->elem = safeParseString(line, 76, 2);
		atom->valid = true;
		if (atom->elem.length() == 0) atom->elem = atom->atom;
		if (recordName == "HETATM") atom->hetflag = true;
		else atom->hetflag = false;

//		__android_log_print(ANDROID_LOG_DEBUG,"PDBReader","parsed atom #%d %s", atom->serial, atom->atom.c_str());
	} else if (recordName == "SHEET") {
		RangeInfo ri;

		ri.chain = safeParseString(line, 21, 1)[0];
		ri.start = safeParseInt(line, 22, 4);
		ri.end = safeParseInt(line, 33, 4);
		sheets.push_back(ri);

	} else if (recordName == "HELIX") {
		RangeInfo ri;

		ri.chain = safeParseString(line, 19, 1)[0];
		ri.start = safeParseInt(line, 21, 4);
		ri.end = safeParseInt(line, 33, 4);

		helices.push_back(ri);

	} else if (recordName == "CONECT") {
		int from = safeParseInt(line, 6, 5);
		int to1 = safeParseInt(line, 11, 5);
		int to2 = safeParseInt(line, 16, 5);
		int to3 = safeParseInt(line, 21, 5);
		int to4 = safeParseInt(line, 26, 5);

		if (to1 != 0) atoms[from].bonds[to1] = 1; // As atom serial starts from 0, this is valid.
		if (to2 != 0) atoms[from].bonds[to2] = 1;
		if (to3 != 0) atoms[from].bonds[to3] = 1;
		if (to4 != 0) atoms[from].bonds[to4] = 1;

	} else if (recordName == "CRYST1") {
		protein->a = safeParseFloat(line, 6, 9);
		protein->b = safeParseFloat(line, 15, 9);
		protein->c = safeParseFloat(line, 24, 9);
		protein->alpha = safeParseFloat(line, 33, 7);
		protein->beta = safeParseFloat(line, 40, 7);
		protein->gamma = safeParseFloat(line, 47, 7);
		protein->spacegroup = line.substr(55, 66); // FIXME: chomp trailing spaces
		defineCell();
	}  else if (recordName == "REMARK") {
		int type = safeParseInt(line, 7, 3);
		std::string subtype = safeParseString(line, 13, 5);

		if (type == 290 && subtype == "SMTRY") {
			int n = safeParseInt(line, 18, 1);
			int m = safeParseInt(line, 21, 2);
			Mat16 mat = protein->symmetryMatrices[m];
			mat.m[4 * n - 4] = safeParseFloat(line, 24, 9);
			mat.m[4 * n - 3] = safeParseFloat(line, 34, 9);
			mat.m[4 * n - 2] = safeParseFloat(line, 44, 9);
			mat.m[4 * n - 1] = safeParseFloat(line, 54, 10);
			mat.m[12] = mat.m[13] = mat.m[14] = 0; mat.m[15] = 1; // PDB doesn't list this row
			protein->symmetryMatrices[m] = mat;
		} else if (type == 350 && safeParseString(line, 11, 11) == "BIOMOLECULE") {
			currentBiomol = safeParseInt(line, 23, 3);
			if (currentBiomol == 1) {
				protein->biomtMatrices.clear(); protein->biomtChains = "";
			}
		} else if (type == 350 && currentBiomol == 1 && subtype == "BIOMT") {
			int n = safeParseInt(line, 18, 1);
			int m = safeParseInt(line, 21, 2);
			Mat16 mat = protein->biomtMatrices[m];
			mat.m[4 * n - 4] = safeParseFloat(line, 24, 9);
			mat.m[4 * n - 3] = safeParseFloat(line, 34, 9);
			mat.m[4 * n - 2] = safeParseFloat(line, 44, 9);
			mat.m[4 * n - 1] = safeParseFloat(line, 54, 10);
			mat.m[12] = mat.m[13] = mat.m[14] = 0; mat.m[15] = 1; // PDB doesn't list this row
			protein->biomtMatrices[m] = mat; // map::insert will not replace existing values
		} else if (type == 350 && currentBiomol == 1 && safeParseString(line, 34, 6) == "CHAINS") {
			protein->biomtChains += line.substr(41, 100); // to the end of the line
		}
	}
}

void PDBReader::parse2ndPass() {
//	__android_log_print(ANDROID_LOG_DEBUG,"PDBReader", "%d sheets, %d helices", sheets.size(), helices.size());

	for (int i = 0; i < 100001; i++) {
		Atom *atom = atoms + i;
		if (!atom->valid) continue;

		bool found = false;
		for (int j = 0, lim = sheets.size(); j < lim && !found; j++) {
			RangeInfo ri = sheets[j];
			if (atom->resi < ri.start) continue;
			if (atom->resi > ri.end) continue;
			if (atom->chain[0] != ri.chain) continue;
			atom->ss = "s";
//			__android_log_print(ANDROID_LOG_DEBUG,"PDBReader", "atom #%d assigned to sheet", i);
			found = true;
		}
		for (int j = 0, lim = helices.size(); j < lim && !found; j++) {
			RangeInfo ri = helices[j];
			if (atom->resi < ri.start) continue;
			if (atom->resi > ri.end) continue;
			if (atom->chain[0] != ri.chain) continue;
			atom->ss = "h";
//			__android_log_print(ANDROID_LOG_DEBUG,"PDBReader", "atom #%d assigned to helix", i);
			found = true;
		}
	}
}

Protein* PDBReader::parsePDB(std::string filename) {
	std::ifstream ifs(filename.c_str(), std::ios::in);

	//FIXME: error handling
	std::string line;
	while (getline(ifs, line)) {
//		__android_log_print(ANDROID_LOG_DEBUG,"PDBReader","parsing %s", line.c_str());
		parseOneLine(line);
	}
	ifs.close();

	parse2ndPass();
	return protein;
}

