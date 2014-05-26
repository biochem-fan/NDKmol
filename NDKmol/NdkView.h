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

#ifndef NDKVIEW_INCLUDED
#define NDKVIEW_INCLUDED

#include "Matrix.hpp"

// Representations
// IMPORTANT TODO: When SettingViewController.mm is changed, these constants must be modified!

#define MAINCHAIN_THINRIBBON 1
#define MAINCHAIN_TRACE 2
#define MAINCHAIN_STRAND 3
#define MAINCHAIN_TUBE 4
#define MAINCHAIN_NONE 6
#define MAINCHAIN_THICKRIBBON 0
#define MAINCHAIN_BONDS 5

#define HETATM_SPHERE 0
#define HETATM_STICK 1
#define HETATM_LINE 2
#define HETATM_NONE 3

#define BASE_LINE 0
#define BASE_POLYGON 1
#define BASE_NONE 2

#define SYMOP_NONE 0
#define SYMOP_BIOMT 1
#define SYMOP_PACKING 2

#define COLOR_CHAINBOW 0
#define COLOR_CHAIN 1
#define COLOR_SS 2
#define COLOR_POLAR 3
#define COLOR_B_FACTOR 4

typedef struct {
	float objX, objY, objZ; // centering
	float prevObjX, prevObjY, prevObjZ; // used by updateMesh
	float ax, ay, az, rot; // rotation axis and angle
	float cameraZ, slabNear, slabFar;
	
	Mat16 projectionMatrix, rotationGroupMatrix, modelGroupMatrix;
} SceneInfo;

extern void nativeGLResize (int width, int height);
extern void nativeSetScene(float objX, float objY, float objZ, float ax, float ay, float az, float rot,
                           float cameraZ, float slabNear, float slabFar);
extern void nativeGLRender();
extern void nativeLoadProtein(const char* filename);
extern void nativeLoadMTZ(const char* filename);
extern void nativeLoadSDF(const char* filename);
extern void nativeUpdateMap(bool force);
extern void nativeAdjustZoom(float *, float *, float *, float *, float *, float *, bool);
extern void buildScene(int proteinMode, int hetatmMode, int symmetryMode, int colorMode, bool showSidechain, bool showUnitcell,
				int nucleicAcidMode, bool showSolvents, bool resetView, bool doNotSmoothen, bool symopHetatms);
extern void nativeGLInit();

#endif