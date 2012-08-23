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

#include "ChemDatabase.hpp"

bool ChemDatabase::ready = false;
std::map<std::string, unsigned int> ChemDatabase::ElementColors;
std::map<std::string, float> ChemDatabase::vdwRadii;

Color ChemDatabase::getColor(std::string elem) {
	if (!ready) prepare();

	std::map<std::string, unsigned int>::iterator it = ElementColors.find(elem);
	if (it == ElementColors.end()) {
//		__android_log_print(ANDROID_LOG_DEBUG,"getColor", "selecting default for atom %s", elem.c_str());
		return Color(defaultColor);
	}
	return Color(it->second);
}

float ChemDatabase::getVdwRadius(std::string elem) {
	if (!ready) prepare();

	std::map<std::string, float>::iterator it = vdwRadii.find(elem);
	if (it == vdwRadii.end()) {
		return defaultRadius;
	}
	return it->second;
}

void ChemDatabase::prepare() {
	ElementColors.insert(std::make_pair("H", 0xCCCCCCU));
	ElementColors.insert(std::make_pair("C", 0xAAAAAAU));
	ElementColors.insert(std::make_pair("O", 0xCC0000U));
	ElementColors.insert(std::make_pair("N", 0x0000CCU));
	ElementColors.insert(std::make_pair("S", 0xCCCC00U));
	ElementColors.insert(std::make_pair("P", 0x6622CCU));
	ElementColors.insert(std::make_pair("F", 0x00CC00U));
	ElementColors.insert(std::make_pair("CL", 0x00CC00U));
	ElementColors.insert(std::make_pair("BR", 0x882200U));
	ElementColors.insert(std::make_pair("FE", 0xCC6600U));
	ElementColors.insert(std::make_pair("CA", 0x8888AAU));

	// Reference: A. Bondi, J. Phys. Chem., 1964, 68, 441.
	vdwRadii.insert(std::make_pair("H", 1.2f));
	vdwRadii.insert(std::make_pair("LI", 1.82f));
	vdwRadii.insert(std::make_pair("NA", 2.27f));
	vdwRadii.insert(std::make_pair("K", 2.75f));
	vdwRadii.insert(std::make_pair("C", 1.7f));
	vdwRadii.insert(std::make_pair("N", 1.55f));
	vdwRadii.insert(std::make_pair("O", 1.52f));
	vdwRadii.insert(std::make_pair("F", 1.47f));
	vdwRadii.insert(std::make_pair("P", 1.80f));
	vdwRadii.insert(std::make_pair("S", 1.80f));
	vdwRadii.insert(std::make_pair("CL", 1.75f));
	vdwRadii.insert(std::make_pair("BR", 1.85f));
	vdwRadii.insert(std::make_pair("SE", 1.90f));
	vdwRadii.insert(std::make_pair("ZN", 1.39f));
	vdwRadii.insert(std::make_pair("CU", 1.4f));
	vdwRadii.insert(std::make_pair("NI", 1.63f));
}
