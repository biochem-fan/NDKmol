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

#include "Color.hpp"

Color::Color(float _r, float _g, float _b, float _a) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

void Color::setRGB(float _r, float _g, float _b, float _a) {
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

Color::Color() {
	r = 1;
	g = 0;
	b = 0;
	a = 0;
}

Color::Color(unsigned int c) {
	r = (float)((c >> 16) & 255) / 255;
	g = (float)((c >> 8) & 255) / 255;
	b = (float)(c & 255) / 255;
	a = 1;
//	__android_log_print(ANDROID_LOG_DEBUG,"Color","Color %#hx converted to %f, %f, %f", c, r, g, b);
}

std::string Color::toString() {
		//return "(" + r + ", " + g + ", " + b + ")";
	return ""; // FIXME: implement
}

void Color::setHSV(float h, float s, float v) {
	if (v == 0) return;

	int mod = (int)(h * 6);
	float f = ( h * 6 ) - mod;
	float p = v * ( 1 - s );
	float q = v * ( 1 - ( s * f ));
	float t = v * ( 1 - ( s * ( 1 - f )));

	switch (mod) {
	case 1: r = q; g = v; b = p; break;
	case 2: r = p; g = v; b = t; break;
	case 3: r = p; g = q; b = v; break;
	case 4: r = t; g = p; b = v; break;
	case 5: r = v; g = p; b = q; break;
	case 6: // fall through
	case 0: r = v; g = t; b = p; break;
	}
}
