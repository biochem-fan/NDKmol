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

#include "Line.hpp"
#include <GLES/gl.h>
#include <android/log.h>


Line::Line(std::vector<Vector3> &points) {
	vertexBuffer = vectorToFloatArray(points);
	nPoints = points.size();
	width = 2;
	discrete = false;
}

Line::Line(float *points, int _nPoints) {
	vertexBuffer = points;
	nPoints = _nPoints;
	width = 2;
	discrete = false;
}

Line::Line(std::vector<Vector3> &points, std::vector<Color> &colors) {
	nPoints = 0;
	if (points.size() > 0) {
		vertexBuffer =  vectorToFloatArray(points);
		colorBuffer = colorVectorToFloatArray(colors, 1);
		nPoints = points.size();
		vertexColors = true;
		width = 2;
		discrete = false;
	}
}

Line::Line() {
	nPoints = 0;
}

void Line::render() {
	glPushMatrix();
	setMatrix();

//	__android_log_print(ANDROID_LOG_DEBUG,"Line","rendering started. nPoints = %d", nPoints);
	if (nPoints > 0) {
		glDisable(GL_LIGHTING);
		glLineWidth(width);
		if (vertexColors && colorBuffer != NULL) {
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_FLOAT, 0, colorBuffer);
		} else {
			glColor4f(objectColor.r, objectColor.g, objectColor.b, objectColor.a);
		}

		glVertexPointer(3,GL_FLOAT, 0, vertexBuffer);
		glEnableClientState(GL_VERTEX_ARRAY);
		if (discrete) {
			glDrawArrays(GL_LINES, 0, nPoints);
		} else {
			glDrawArrays(GL_LINE_STRIP, 0, nPoints);
		}
		glDisableClientState(GL_VERTEX_ARRAY);

		if (vertexColors) {
			glDisableClientState(GL_COLOR_ARRAY);
		}
		glEnable(GL_LIGHTING);
	}
	glPopMatrix();
//	__android_log_print(ANDROID_LOG_DEBUG,"Line","rendering end");
}
