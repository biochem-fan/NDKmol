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
#include "NdkView.hpp"
#include <GLES/gl.h>
#include <android/log.h>
#include <cmath>
#include <vector>
#include <set>
#include <string>
#include <map>
#include "Color.hpp"
#include "PDBReader.hpp"
#include "SDFReader.hpp"
#include "SmoothCurve.hpp"
#include "SmoothTube.hpp"
#include "RibbonStrip.hpp"
#include "Line.hpp"
#include "Geometry.hpp"
//#include "PLYGeometry.hpp"
#include "Renderable.hpp"
#include "MatRenderable.hpp"
#include "VBOSphere.hpp"
#include "VBOCylinder.hpp"
#include "ChemDatabase.hpp"
#include "Protein.hpp"
#include "View.hpp"

#define DIV 1

Atom *atoms = NULL;
Protein *protein = NULL;
Renderable *scene = NULL;

float sphereRadius = 1.5f;
float cylinderRadius = 0.2f;
float lineWidth = 0.5f;
float curveWidth = 2.0f;
float thickness = 0.4f;

float* getExtent(std::vector<int> &atomlist);
std::vector<int> getAll();
std::vector<int> getHetatms(std::vector<int> &atomlist);
std::vector<int> removeSolvents(std::vector<int> &atomlist);
std::vector<int> getResiduesById(std::vector<int> &atomlist, std::set<int> &resi);
std::vector<int> getSideChain(std::vector<int> &atomlist);
std::vector<int> getChain(std::vector<int> &atomlist, std::string &chain);
std::vector<int> getNonbonded(std::vector<int> &atomlist);
void colorByAtom(std::vector<int> &atomlist, std::map<std::string, unsigned int> &colors);
void colorByStructure(std::vector<int> &atomlist, Color helixColor, Color sheetColor);
void colorByChain(std::vector<int> &atomlist);
void drawCartoon(Renderable &scene, std::vector<int> &atomlist, int div, bool doNotSmoothen, float thickness);
void drawStrand(Renderable &scene, std::vector<int> &atomlist, int num, int div, bool fill, bool doNotSmoothen, float thickness);
void colorChainbow(std::vector<int> &atomlist);
void colorByBFactor(std::vector<int> &atomlist);
void colorByPolarity(std::vector<int> &atomlist, Color polar, Color nonPolar);
void colorByResidue(std::vector<int> &atomlist, std::map<std::string, Color> colorMap);
void drawMainchainCurve(Renderable &scene, std::vector<int> &atomlist, float curveWidth, std::string atomName);
void drawMainchainTube(Renderable &scene, std::vector<int> &atomlist, std::string atomName);
bool isIdentity(Mat16 mat);
void drawBondsAsStick(Renderable &scene, std::vector<int> &atomlist, float bondR, float atomR);
void drawBondsAsLine(Renderable &scene, std::vector<int> &atomlist, float lineWidth);
void drawAtomsAsVdWSphere(Renderable &scene, std::vector<int> &atomlist);
void drawAtomsAsStar(Renderable &scene, std::vector<int> &atomlist, float delta);
void drawSymmetryMates(Renderable &scene, Renderable *asu, std::map<int, Mat16> biomtMatrices);
void drawSymmetryMatesWithTranslation(Renderable &scene, Renderable *asu, std::map<int, Mat16> matrices);
void drawUnitcell(Renderable &scene, float width);
void drawNucleicAcidAsLine(Renderable &scene, std::vector<int> &atomlist);
void drawNucleicAcidCartoon(Renderable &scene, std::vector<int> &atomlist, int div, float thickness);
void drawNucleicAcidStrand(Renderable &scene, std::vector<int> &atomlist, int num, int div, bool fill, float thickness);
void drawNucleicAcidLadder(Renderable &scene, std::vector<int> &atomlist);

// onSurfaceChanged
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeGLResize
(JNIEnv *env, jclass clasz, jint width, jint height) {
	__android_log_print(ANDROID_LOG_DEBUG, "NdkView", "resize");

	VBOSphere::prepareVBO();
	VBOCylinder::prepareVBO();
}

// onDrawFrame
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeGLRender
(JNIEnv *env, jclass clasz) {
	if (scene != NULL) scene->render();
}

// loadProtein
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeLoadProtein
(JNIEnv *env, jclass clasz, jstring path) {
	const char *filename = env->GetStringUTFChars(path, NULL);
	__android_log_print(ANDROID_LOG_DEBUG, "NdkView","opening %s", filename);
	PDBReader pdb;
	if (scene) {
		delete scene;
		scene = NULL;
	}
	if (protein) {
		delete protein;
		protein = NULL;
	}
	protein = pdb.parsePDB(filename);
	atoms = protein->atoms;

	env->ReleaseStringUTFChars(path, filename);
}

// loadSDF
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeLoadSDF
(JNIEnv *env, jclass clasz, jstring path) {
	const char *filename = env->GetStringUTFChars(path, NULL);
	__android_log_print(ANDROID_LOG_DEBUG, "NdkView","opening SDFFile %s", filename);
	SDFReader sdf;
	if (scene) {
		delete scene;
		scene = NULL;
	}
	if (protein) {
		delete protein;
		protein = NULL;
	}
	protein = sdf.parseSDF(filename);
	atoms = protein->atoms;

	env->ReleaseStringUTFChars(path, filename);
}

JNIEXPORT jfloatArray JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeAdjustZoom
(JNIEnv *env, jclass clasz, jint symmetryMode) {
	// FIXME: What happens when protein is not loaded.
	jfloatArray ret = env->NewFloatArray(7);
	if (protein == NULL) return ret;

	jboolean dummy;
	jfloat *array = env->GetFloatArrayElements(ret, &dummy);

	int nBiomt = protein->biomtMatrices.size();
	std::vector<int> all = getAll();
	if (symmetryMode ==  SYMOP_BIOMT && nBiomt > 0) all = getChain(all, protein->biomtChains);
	float *extent = getExtent(all);
	float slabNear, slabFar, cameraZ, objX, objY, objZ, FOV = 20;

	objX = (extent[0] + extent[3]) / 2;
	objY = (extent[1] + extent[4]) / 2;
	objZ = (extent[2] + extent[5]) / 2;

	if (symmetryMode ==  SYMOP_BIOMT && nBiomt > 0) {
		Mat16 averagedMat = {};
		for (std::map<int, Mat16>::iterator it = protein->biomtMatrices.begin(); it != protein->biomtMatrices.end(); ++it) {
			Mat16 mat = it->second;
			for (int i = 0; i < 16; i++) averagedMat.m[i] += mat.m[i];
		}
		for (int i = 0; i < 16; i++) averagedMat.m[i] /= nBiomt;

		Vector3 center(objX, objY, objZ);
		center.applyMat16(averagedMat);
		objX = center.x; objY = center.y; objZ = center.z;
	}
	objX *= -1; objY *= -1; objZ *= -1;

	float x = extent[3] - extent[0], y = extent[4] - extent[1], z = extent[5] - extent[2];
	float maxD = (float)std::sqrt(x * x + y * y + z * z);
	if (maxD < 25) maxD = 25;

	slabNear = -maxD / 1.9f;
	slabFar = maxD / 3;
	cameraZ = -(float)(maxD * 0.5 / std::tan(M_PI / 180.0 * FOV / 2));

	*(array++) = objX; *(array++) = objY; *(array++) = objZ;
	*(array++) = cameraZ; *(array++) = slabNear; *(array++) = slabFar;
	*(array++) = maxD;
	delete [] extent;
	env->ReleaseFloatArrayElements(ret, array, 0);
	return ret;
}
/*
#include "CommonPara.h"
#include "ParsePDB.h"
#include "ProteinSurface.h"
 */

// prepareScene
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_buildScene
(JNIEnv *env, jclass clasz, jint proteinMode, jint hetatmMode, jint symmetryMode, jint colorMode,
		jboolean showSidechain, jboolean showUnitcell, jint nucleicAcidMode, jboolean showSolvents,
		jboolean doNotSmoothen, jboolean symopHetatms) {
	if (scene != NULL) {
		delete scene;
		scene = NULL;
	}
	if (protein == NULL) return;

	scene = new Renderable();
	Renderable *asu = new Renderable();

	std::vector<int> all = getAll();
	if (symmetryMode == SYMOP_BIOMT && protein->biomtChains.length() > 0) all = getChain(all, protein->biomtChains);
	std::vector<int> allHet = getHetatms(all);
	std::vector<int> hetatm = removeSolvents(allHet);
	std::map<std::string, unsigned int> m;
	colorByAtom(all, m);

	switch (colorMode) {
		case COLOR_CHAINBOW:
			colorChainbow(all);
			break;
		case COLOR_CHAIN:
			colorByChain(all);
			break;
		case COLOR_SS:
			colorByStructure(all, Color(0xCC00CC), Color(0x00CCCC));
			break;
		case COLOR_POLAR:
			colorByPolarity(all, Color(0xcc0000), Color(0xcccccc));
			break;
		case COLOR_B_FACTOR:
			colorByBFactor(all);
			break;
	}

	switch (proteinMode) {
		case MAINCHAIN_THICKRIBBON:
			drawCartoon(*asu, all, 5, doNotSmoothen, thickness);
			drawNucleicAcidCartoon(*asu, all, 5, thickness);
			break;
		case MAINCHAIN_THINRIBBON:
			drawCartoon(*asu, all, 5, doNotSmoothen, -1);
			drawNucleicAcidCartoon(*asu, all, 5, -1);
			break;
		case MAINCHAIN_TRACE:
			drawMainchainCurve(*asu, all, curveWidth, "CA");
			drawMainchainCurve(*asu, all, curveWidth, "O3'");
			break;
		case MAINCHAIN_STRAND:
			drawStrand(*asu, all, 5, 5, false, doNotSmoothen, -1);
			drawNucleicAcidStrand(*asu, all, 5, 5, false, -1);
			break;
		case MAINCHAIN_TUBE:
			drawMainchainTube(*asu, all, "CA");
			drawMainchainTube(*asu, all, "O3'");
			break;
		case MAINCHAIN_BONDS:
			drawBondsAsLine(*asu, all, lineWidth);
			break;
	}

	switch (nucleicAcidMode) {
		case BASE_LINE:
			drawNucleicAcidAsLine(*asu, all);
			break;
		case BASE_POLYGON:
			drawNucleicAcidLadder(*asu, all);
			break;
	}

	if (showSidechain) {
		std::vector<int> sidechains = getSideChain(all);
		drawBondsAsLine(*asu, sidechains, lineWidth);
	}

	Renderable *target = (symopHetatms) ? asu : scene;
	switch (hetatmMode) {
		case HETATM_SPHERE:
			drawAtomsAsVdWSphere(*target, hetatm);
			break;
		case HETATM_STICK:
			drawBondsAsStick(*target, hetatm, cylinderRadius, cylinderRadius);
			break;
		case HETATM_LINE:
			drawBondsAsLine(*target, hetatm, lineWidth * 4);
	}

	if (showSolvents) {
		std::vector<int> nonBonded = getNonbonded(allHet);
		drawAtomsAsStar(*target, nonBonded, 0.3f);
	}

	if (showUnitcell) {
		drawUnitcell(*scene, lineWidth);
	}

	if (symmetryMode == SYMOP_BIOMT && protein->biomtMatrices.size() > 0) {
		drawSymmetryMates(*scene, asu, protein->biomtMatrices);
	} else if (symmetryMode == SYMOP_PACKING && protein->symmetryMatrices.size() > 0) {
		drawSymmetryMatesWithTranslation(*scene, asu, protein->symmetryMatrices);
	} else { // No SYMOP, No BIOMT defined, or NMR structure
		scene->children.push_back(asu);
	}
}

// onSurfaceCreated
JNIEXPORT void JNICALL Java_jp_sfjp_webglmol_NDKmol_NdkView_nativeGLInit
(JNIEnv *env, jclass clasz) {
	__android_log_print(ANDROID_LOG_DEBUG,"NdkView","init");

	VBOSphere::prepareVBO(); // FIXME: Why is it necessary here when resize is also called?
	VBOCylinder::prepareVBO();

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	//	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//		glEnable(GL_LINE_SMOOTH); // FIXME: Check if this is working.
	glLightModelx(GL_LIGHT_MODEL_TWO_SIDE, 1); // double sided
	glEnable(GL_COLOR_MATERIAL); // glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE) ?
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_DITHER);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float f1[] = {0.4f, 0.4f, 0.4f, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, f1);
	float f2[] = {0, 0, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, f2);
	float f3[] = {0.8f, 0.8f, 0.8f, 1};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, f3);
	glEnable(GL_LIGHT1);
	//		glLightfv(GL_LIGHT0, GL_AMBIENT, Geometry.getFloatBuffer(new float[] {0.4f, 0.4f, 0.4f, 1}));
	float f4[] = {0, 0, -1, 0};
	glLightfv(GL_LIGHT1, GL_POSITION, f4);
	float f5[] = {0.1f, 0.1f, 0.1f, 1};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, f3);
	glLightfv(GL_LIGHT0, GL_SPECULAR, f5);
	glLightfv(GL_LIGHT1, GL_SPECULAR, f5);
	//	glPointParameterfv(GL11.GL_POINT_DISTANCE_ATTENUATION, {0, 0, 1}));

	/*
	glEnable(GL_FOG);
	glFogf(GL_FOG_MODE, GL_LINEAR); // EXP, EXP2 is not supported?
	float f6[] = {0, 0, 0, 1};
	glFogfv(GL_FOG_COLOR, f6);
	glFogf(GL_FOG_DENSITY, 0.3f);
	//		gl.glHint(GL10.GL_FOG_HINT, GL10.GL_DONT_CARE); */
}

float* getExtent(std::vector<int> &atomlist) {
	float minx = 9999, miny = 9999, minz = 9999;
	float maxx = -9999, maxy = -9999, maxz = -9999;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->x < minx) minx = atom->x;
		if (atom->x > maxx) maxx = atom->x;
		if (atom->y < miny) miny = atom->y;
		if (atom->y > maxy) maxy = atom->y;
		if (atom->z < minz) minz = atom->z;
		if (atom->z > maxz) maxz = atom->z;
	}

	float *ret = new float[6];
	ret[0] = minx; ret[1] = miny; ret[2] = minz; ret[3] = maxx; ret[4] = maxy; ret[5] = maxz;

	return ret;
}

std::vector<int> getAll() { // TODO: should return reference?
	std::vector<int> ret;
	for (int i = 0; i < 100001; i++) {
		if (atoms[i].valid) ret.push_back(i);
	}
	return ret;
}

std::vector<int> getHetatms(std::vector<int> &atomlist) {
	std::vector<int> ret;
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->hetflag)	ret.push_back(atom->serial);
	}
	return ret;
}

std::vector<int> removeSolvents(std::vector<int> &atomlist) {
	std::vector<int> ret;
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->resn !="HOH") ret.push_back(atom->serial);
	}
	return ret;
}

std::vector<int> getResiduesById(std::vector<int> &atomlist, std::set<int> &resi) {
	std::vector<int> ret;
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (resi.count(atom->resi) > 0) ret.push_back(atom->serial);
	}
	return ret;
}

std::vector<int> getNonbonded(std::vector<int> &atomlist) {
	std::vector<int> ret;
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->hetflag && atom->bonds.size() == 0) ret.push_back(atom->serial);
	}
	return ret;
}

std::vector<int> getSideChain(std::vector<int> &atomlist) {
	std::vector<int> ret;
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->hetflag) continue;
		if (atom->atom == "O" || atom->atom == "C" || (atom->atom == "N" && atom->resn != "PRO")) continue;
		ret.push_back(atom->serial);
	}

	return ret;
}

std::vector<int> getChain(std::vector<int> &atomlist, std::string &chain) {
	std::vector<int> ret;
	std::set<std::string> chains;

	// FIXME: This assumes chain name is 'char'. It is valid for PDB but
	//  then we should have used 'char' for atom->chain.
	for (int i = 0, lim = chain.length(); i < lim; i++)
		chains.insert(chain.substr(i, 1));

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (chains.find(atom->chain) != chains.end()) ret.push_back(atom->serial);
	}
	return ret;
}

void colorByAtom(std::vector<int> &atomlist, std::map<std::string, unsigned int> &colors) {
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		std::map<std::string, unsigned int>::iterator it = colors.find(atom->elem);
		if (it != colors.end()) {
			atom->color = it->second;
		} else {
			atom->color = ChemDatabase::getColor(atom->elem);
		}
		//		__android_log_print(ANDROID_LOG_DEBUG,"CbyA","%d %s colored %f %f %f", atom->serial, atom->elem.c_str(), atom->color.r, atom->color.g, atom->color.b);
	}
}

void colorByStructure(std::vector<int> &atomlist, Color helixColor, Color sheetColor) {
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->atom != "CA" || atom->hetflag) continue;
		if (atom->ss[0] == 's') atom->color = sheetColor;
		else if (atom->ss[0] == 'h') atom->color = helixColor;
	}
}

void colorByChain(std::vector<int> &atomlist) {
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->atom != "CA" || atom->hetflag) continue;
		atom->color.setHSV((float)(atom->chain[0] * 5 % 17) / 17, 1, 0.9f);
	}
}

void colorByResidue(std::vector<int> &atomlist, std::map<std::string, Color> colorMap) {
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (colorMap.count(atom->resn) > 0) {
			atom->color = colorMap[atom->resn];
		}
	}
}

void colorChainbow(std::vector<int> &atomlist) {
	int cnt = 0;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->atom != "CA" || atom->hetflag) continue;
		cnt++;
	}

	int total = cnt;
	cnt = 0;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if (atom->atom != "CA" || atom->hetflag) continue;

		atom->color.setHSV((float)240 / 360 * (total - cnt) / total, 1, 0.9f);
		cnt++;
	}
}

void colorByBFactor(std::vector<int> &atomlist) {
	float minB = 1000, maxB = -1000;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if (atom->atom == "CA" || atom->atom == "O3'") {
			if (minB > atom->b) minB = atom->b;
			if (maxB < atom->b) maxB = atom->b;
		}
	}
	float mid = (maxB + minB) / 2, range = (maxB - minB) / 2;
	if (range < 0.01f && range > -0.01f) return;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if (atom->atom == "CA" || atom->atom == "O3'") {
			if (atom->b < mid) {
				atom->color.setHSV(0.667f, (mid - atom->b) / range, 1);
			} else {
				atom->color.setHSV(0, (atom->b - mid) / range, 1);
			}
		}
	}
}

void colorByPolarity(std::vector<int> &atomlist, Color polar, Color nonPolar) {
	std::string polarResidues[] = {"ARG", "HIS", "LYS", "ASP", "GLU", "SER", "THR", "ASN", "GLN", "CYS"}; // 10
	std::string nonPolarResidues[] = {"GLY", "PRO", "ALA", "VAL", "LEU", "ILE", "MET", "PHE", "TYR", "TRP"}; // 10

	std::map<std::string, Color> colorMap;
	for (int i = 0; i < 10; i++) {
		colorMap[polarResidues[i]] = polar;
		colorMap[nonPolarResidues[i]] = nonPolar;
	}
	colorByResidue(atomlist, colorMap);
}

void drawAtomsAsStar(Renderable &scene, std::vector<int> &atomlist, float delta) {
	std::vector<Vector3> points;
	std::vector<Color> colors;
	float pointsBase[] = {delta, 0, 0, -delta, 0, 0, 0, delta, 0, 0, -delta, 0, 0, 0, delta, 0, 0, -delta};

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		int offset = 0;
		float x = atom->x, y = atom->y, z = atom->z;
		for (int j = 0; j < 6; j++) {
			points.push_back(Vector3(x + pointsBase[offset++], y + pointsBase[offset++], z + pointsBase[offset++]));
			colors.push_back(atom->color);
		}
	}
	Line *line = new Line(points, colors);
	line->discrete = true;
	line->width = 1;
	scene.children.push_back(line);
}

void drawMainchainCurve(Renderable &scene, std::vector<int> &atomlist, float curveWidth, std::string atomName) {
	std::vector<Vector3> points;
	std::vector<Color> colors;

	std::string currentChain = "A";
	int currentResi = -1;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if ((atom->atom == atomName) && !atom->hetflag) {
			if (currentChain != atom->chain || currentResi + 1 != atom->resi) {
				scene.children.push_back(new SmoothCurve(points, colors, curveWidth, DIV));
				points.clear();
				colors.clear();
			}
			points.push_back(Vector3(atom->x, atom->y, atom->z));
			colors.push_back(atom->color);
			currentChain = atom->chain;
			currentResi = atom->resi;
		}
	}
	scene.children.push_back(new SmoothCurve(points, colors, curveWidth * 1.5, DIV));
}

void drawMainchainTube(Renderable &scene, std::vector<int> &atomlist, std::string atomName) {
	std::vector<Vector3> points;
	std::vector<Color> colors;
	std::vector<float> radii;

	std::string currentChain = "A";
	int currentResi = -1;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		if ((atom->atom == atomName) && !atom->hetflag) {
			if (currentChain != atom->chain || currentResi + 1 != atom->resi) {
				scene.children.push_back(new SmoothTube(points, colors, radii));
				points.clear();
				colors.clear();
				radii.clear();
			}
			radii.push_back((atom->b > 0) ? atom->b / 100 : 0.3);
			points.push_back(Vector3(atom->x, atom->y, atom->z));
			colors.push_back(atom->color);
			currentChain = atom->chain;
			currentResi = atom->resi;
		}
	}
	scene.children.push_back(new SmoothTube(points, colors, radii));
}

void drawNucleicAcidLadderSub(std::vector<Vector3> &vertices, std::vector<Vector3> &normals, std::vector<unsigned short> &faces, std::vector<Color> &colors, Color &color, Vector3 *atoms) {
	//      color.r *= 0.9; color.g *= 0.9; color.b *= 0.9;
	if (atoms[0].x != -9999 && atoms[1].x != -9999 && atoms[2].x != -9999 &&
		atoms[3].x != -9999 && atoms[4].x != -9999 && atoms[5].x != -9999) {
		Vector3 tmp1(atoms[1].x - atoms[0].x, atoms[1].y - atoms[0].y, atoms[1].z - atoms[0].z);
		Vector3 tmp2(atoms[2].x - atoms[0].x, atoms[2].y - atoms[0].y, atoms[2].z - atoms[0].z);
		Vector3 normal = Vector3::cross(tmp1, tmp2).normalize();

		unsigned short baseFaceId = (unsigned short)vertices.size();
		for (int i = 0; i <= 5; i++) {
			vertices.push_back(atoms[i]);
			normals.push_back(normal);
			colors.push_back(color);
		}

		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 1)); faces.push_back((unsigned short) (baseFaceId + 2));
		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 2)); faces.push_back((unsigned short) (baseFaceId + 3));
		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 3)); faces.push_back((unsigned short) (baseFaceId + 4));
		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 4)); faces.push_back((unsigned short) (baseFaceId + 5));
	}
	if (atoms[4].x != -9999 && atoms[3].x != -9999 && atoms[6].x != -9999 &&
		atoms[7].x != -9999 && atoms[8].x != -9999) {
		Vector3 tmp1(atoms[4].x - atoms[3].x, atoms[4].y - atoms[3].y, atoms[4].z - atoms[3].z);
		Vector3 tmp2(atoms[6].x - atoms[3].x, atoms[6].y - atoms[3].y, atoms[6].z - atoms[3].z);
		Vector3 normal = Vector3::cross(tmp1, tmp2).normalize();

		unsigned short baseFaceId = (unsigned short)vertices.size();
		vertices.push_back(atoms[4]);
		vertices.push_back(atoms[3]);
		vertices.push_back(atoms[6]);
		vertices.push_back(atoms[7]);
		vertices.push_back(atoms[8]);
		for (int i = 0; i <= 4; i++) {
			colors.push_back(color);
			normals.push_back(normal);
		}

		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 1)); faces.push_back((unsigned short) (baseFaceId + 2));
		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 2)); faces.push_back((unsigned short) (baseFaceId + 3));
		faces.push_back(baseFaceId); faces.push_back((unsigned short) (baseFaceId + 3)); faces.push_back((unsigned short) (baseFaceId + 4));
	}
}

void drawNucleicAcidLadder(Renderable &scene, std::vector<int> &atomlist) {
	std::vector<Vector3> vertices, normals;
	std::vector<unsigned short> faces;
	std::vector<Color> colors;
	Color color;
	Vector3 currentComponent[9]; for (int j = 0; j <= 8; j++) {currentComponent[j].x = -9999;}

	std::string baseAtoms[] = {"N1", "C2", "N3", "C4", "C5", "C6", "N9", "C8", "N7"};
	std::string currentChain = "";
	int currentResi = -1;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if (atom->resi != currentResi || atom->chain != currentChain) {
			drawNucleicAcidLadderSub(vertices, normals, faces, colors, color, currentComponent);
			for (int j = 0; j <= 8; j++) {currentComponent[j].x = -9999;}
		}
		int pos = -1;
		for (int j = 0; j < 8; j++) {
			if (baseAtoms[j] == atom->atom) {
				pos = j;
				break;
			}
		}
		if (pos != -1) {
			currentComponent[pos].x = atom->x;
			currentComponent[pos].y = atom->y;
			currentComponent[pos].z = atom->z;
		}
		if (atom->atom == "O3'") color = atom->color;
		currentResi = atom->resi; currentChain = atom->chain;
	}
	drawNucleicAcidLadderSub(vertices, normals, faces, colors, color, currentComponent);

	Renderable *renderable = new Renderable();
	renderable->colorBuffer = colorVectorToFloatArray(colors, 1);
	renderable->vertexColors = true;
	renderable->faceBuffer = vectorToShortArray(faces);
	renderable->vertexBuffer = vectorToFloatArray(vertices);
	renderable->vertexNormalBuffer = vectorToFloatArray(normals);
	renderable->nFaces = faces.size();
	scene.children.push_back(renderable);
}

void drawBondsAsLineSub(std::vector<Vector3> &points, std::vector<Color> &colors, Atom *atom1, Atom *atom2, int order) {
	Vector3 midpoint((atom1->x + atom2->x) / 2, (atom1->y + atom2->y) / 2, (atom1->z + atom2->z) / 2);
	float dot = 0, dx = 0, dy = 0, dz = 0;
	if (order > 1) { // Find the bond plane. TODO: Find inner side of a ring
		Vector3 axis(atom1->x - atom2->x, atom1->y - atom2->y, atom1->z - atom2->z);
		Atom *found = NULL;
		for (int i = 0, lim = atom1->bonds.size(); i < lim && found != NULL; i++) {
			Atom *atom = atoms + atom1->bonds[i]; if (!atom->valid) continue;
			if (atom->serial != atom2->serial && atom->elem != "H") found = atom;
		}
		for (int i = 0, lim = atom2->bonds.size(); i < lim && found != NULL; i++) {
			Atom *atom = atoms + atom2->bonds[i]; if (!atom->valid) continue;
			if (atom->serial != atom1->serial && atom->elem != "H") found = atom;
		}
		if (found != NULL) {
			Vector3 tmp(atom1->x - found->x, atom1->y - found->y, atom1->z - found->z);
			dot = Vector3::dot(tmp, axis);
			dx = tmp.x - axis.x * dot; dy = tmp.y - axis.y * dot; dz = tmp.z - axis.z * dot;
		}
		if (found == NULL || std::abs(dot - 1) < 0.001 || std::abs(dot + 1) < 0.001) {
			if (axis.x < 0.01 && axis.y < 0.01) {
				dx = 0; dy = - axis.z; dz = axis.y;
			} else {
				dx = - axis.y; dy = axis.x; dz = 0;
			}
		}
		float norm = (float)std::sqrt(dx * dx + dy * dy + dz * dz);
		dx /= norm; dy /= norm; dz /= norm;
		dx *= 0.15; dy *= 0.15; dz *= 0.15;
	}

	Color c = atom1->color;
	float ax = atom1->x, ay = atom1->y, az = atom1->z;
	points.push_back(Vector3(ax, ay, az));
	colors.push_back(c);
	points.push_back(midpoint);
	colors.push_back(c);
	if (order > 1) {
		points.push_back(Vector3(ax + dx, ay + dy, az + dz));
		colors.push_back(c);
		points.push_back(Vector3(midpoint.x + dx, midpoint.y + dy, midpoint.z + dz));
		colors.push_back(c);
	}
	if (order > 2) {
		points.push_back(Vector3(ax + dx * 2, ay + dy * 2, az + dz * 2));
		colors.push_back(c);
		points.push_back(Vector3(midpoint.x + dx * 2, midpoint.y + dy * 2, midpoint.z + dz * 2));
		colors.push_back(c);
	}

	ax = atom2->x; ay = atom2->y; az = atom2->z;
	c = atom2->color;
	points.push_back(Vector3(ax, ay, az));
	colors.push_back(c);
	points.push_back(midpoint);
	colors.push_back(c);
	if (order > 1) {
		points.push_back(Vector3(ax + dx, ay + dy, az + dz));
		colors.push_back(c);
		points.push_back(Vector3(midpoint.x + dx, midpoint.y + dy, midpoint.z + dz));
		colors.push_back(c);
	}
	if (order > 2) {
		points.push_back(Vector3(ax + dx * 2, ay + dy * 2, az + dz * 2));
		colors.push_back(c);
		points.push_back(Vector3(midpoint.x + dx * 2, midpoint.y + dy * 2, midpoint.z + dz * 2));
		colors.push_back(c);
	}
}

void drawNucleicAcidAsLine(Renderable &scene, std::vector<int> &atomlist) {
	std::vector<Vector3> points;
	std::vector<Color> colors;
	std::string currentChain = "";
	int currentResi = -1;
	Atom *start = NULL, *end = NULL;

	for (int i = 1, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if (atom->resi != currentResi || atom->chain !=currentChain) {
			if (start != NULL && end != NULL) {
				points.push_back(Vector3(start->x, start->y, start->z));
				colors.push_back(start->color);
				points.push_back(Vector3(end->x, end->y, end->z));
				colors.push_back(start->color);
			}
			start = NULL; end = NULL;
		}
		if (atom->atom == "O3'") start = atom;
		if (atom->resn == "A" || atom->resn == "G" || atom->resn == "DA" || atom->resn == "DG") {
			if (atom->atom == "N1") end = atom; //  N1(AG), N3(CTU)
		} else if (atom->atom == "N3") {
			end = atom;
		}
		currentResi = atom->resi; currentChain = atom->chain;
	}
	if (start != NULL && end != NULL) {
		points.push_back(Vector3(start->x, start->y, start->z));
		colors.push_back(start->color);
		points.push_back(Vector3(end->x, end->y, end->z));
		colors.push_back(start->color);
	}
	Line *line = new Line(points, colors);
	line->vertexColors = true;
	line->discrete = true;
	scene.children.push_back(line);
}

void drawNucleicAcidCartoon(Renderable &scene, std::vector<int> &atomlist, int div, float thickness) {
	drawNucleicAcidStrand(scene, atomlist, 2, div, true, thickness);
}

void drawNucleicAcidStrand(Renderable &scene, std::vector<int> &atomlist, int num, int div, bool fill, float thickness) {
	std::vector<Vector3> *points = new std::vector<Vector3>[num];
	std::vector<Color> colors;
	std::string currentChain = "";
	int currentResi = -1;
	Vector3 currentO3(-9999, 0, 0);
	Vector3 prevOO(-9999, 0, 0);

	for (int i = 1, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if ((atom->atom == "O3'" || atom->atom == "OP2") && !atom->hetflag) {
			if (atom->atom == "O3'") {
				if (atom->chain != currentChain || currentResi + 1 != atom->resi) {
					if (currentO3.x != -9999) {
						for (int j = 0; j < num; j++) {
							float delta = -1 + 2.0f / (num - 1) * j;
							points[j].push_back(Vector3(currentO3.x + prevOO.x * delta,
														currentO3.y + prevOO.y * delta, currentO3.z + prevOO.z * delta));
						}
					}
					if (fill) scene.children.push_back(new RibbonStrip(points[0], points[1], colors, thickness));
					for (int j = 0; thickness < 0 && j < num; j++)
						scene.children.push_back(new SmoothCurve(points[j], colors, 1.0f, div));
					for (int k = 0; k < num; k++) points[k].clear();
					colors.clear();
					prevOO.x = -9999; prevOO.y = 0; prevOO.z = 0;
				}
				currentO3.x = atom->x; currentO3.y = atom->y; currentO3.z = atom->z;
				currentChain = atom->chain;
				currentResi = atom->resi;
				colors.push_back(atom->color);
			} else { // OP2
				if (currentO3.x == -9999) {
					prevOO.x = -9999;
					continue;
				}
				Vector3 O(atom->x - currentO3.x, atom->y - currentO3.y, atom->z - currentO3.z);
				O.normalize().multiplyScalar(0.8f);
				if (prevOO.x != -9999 && Vector3::dot(O, prevOO) < 0) {
					O.multiplyScalar(-1);
				}
				prevOO = O;
				for (int j = 0; j < num; j++) {
					float delta = -1 + 2.0f / (num - 1) * j;
					points[j].push_back(Vector3(currentO3.x + prevOO.x * delta,
												currentO3.y + prevOO.y * delta, currentO3.z + prevOO.z * delta));
				}
				currentO3.x = -9999;
			}
		}
	}
	if (currentO3.x != -9999) {
		for (int j = 0; j < num; j++) {
			float delta = -1 + 2.0f / (num - 1) * j;
			points[j].push_back(Vector3(currentO3.x + prevOO.x * delta,
										currentO3.y + prevOO.y * delta, currentO3.z + prevOO.z * delta));
		}
	}
	if (fill) scene.children.push_back(new RibbonStrip(points[0], points[1], colors, thickness));
	for (int j = 0; thickness < 0 && j < num; j++)
		scene.children.push_back(new SmoothCurve(points[j], colors, 1.0f, div));

	delete [] points;
}

void drawCartoon(Renderable &scene, std::vector<int> &atomlist, int div, bool doNotSmoothen, float thickness) {
	drawStrand(scene, atomlist, 2, div, true, doNotSmoothen, thickness);
}

void drawStrand(Renderable &scene, std::vector<int> &atomlist, int num, int div, bool fill, bool doNotSmoothen, float thickness) {
	std::vector<Vector3> *points = new std::vector<Vector3>[num];
	std::vector<Color> colors;
	std::vector <bool> smoothen;
	std::string currentChain = "", ss = "";
	int currentResi = -1;
	Vector3 prevCO(-9999, 0, 0), currentCA(0, 0, 0);

	for (int i = 1, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid || atom->hetflag) continue;

		if ((atom->atom == "O" || atom->atom == "CA") && !atom->hetflag) {
			if (atom->atom == "CA") {
				if (currentChain != atom->chain || currentResi + 1 != atom->resi) {
					if (fill) scene.children.push_back(new RibbonStrip(points[0], points[1], smoothen, colors, thickness));
					for (int j = 0; thickness < 0 && j < num; j++)
						scene.children.push_back(new SmoothCurve(points[j], smoothen, colors, 1.0f, div));
					for (int k = 0; k < num; k++) points[k].clear();
					colors.clear();
					smoothen.clear();
					prevCO.x = -9999; ss = "";
				}
				currentCA.x = atom->x; currentCA.y = atom->y; currentCA.z = atom->z;
				currentChain = atom->chain;
				currentResi = atom->resi;
				ss = atom->ss;
				colors.push_back(atom->color);
			} else { // O
				Vector3 O = Vector3(atom->x, atom->y, atom->z);
				O.x -= currentCA.x; O.y -= currentCA.y; O.z -= currentCA.z;
				O.normalize(); // can be omitted for performance
				O.multiplyScalar((ss == "c") ? 0.3 : 1.3);
				if (prevCO.x != -9999 && Vector3::dot(O, prevCO) < 0) {
					O.multiplyScalar(-1);
				}
				prevCO = O;
				for (int j = 0; j < num; j++) {
					float delta = -1 + 2.0f / (num - 1) * j;
					points[j].push_back(Vector3(currentCA.x + prevCO.x * delta,
												currentCA.y + prevCO.y * delta, currentCA.z + prevCO.z * delta));
				}
				smoothen.push_back(!doNotSmoothen && ss == "s");
			}
		}
	}
	if (fill) scene.children.push_back(new RibbonStrip(points[0], points[1], smoothen, colors, thickness));
	for (int j = 0; thickness < 0 && j < num; j++)
		scene.children.push_back(new SmoothCurve(points[j], smoothen, colors, 1.0f, div));

	delete[] points;
}

bool isIdentity(Mat16 mat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j && std::abs(mat.m[i * 4 + j] - 1) > 0.001) return false;
			if (i != j && std::abs(mat.m[i * 4 + j]) > 0.001) return false;
		}
	}
	return true;
}


void drawBondsAsStick(Renderable &scene, std::vector<int> &atomlist, float bondR, float atomR) {
	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom1 = atoms + atomlist[i];
		if (!atom1->valid) continue;

		for (int j = i + 1; j < i + 40 && j < lim; j++) { // TODO: Check if 40 is sufficient for DNA and HETATMs.
			Atom *atom2 = atoms + atomlist[j];
			if (!atom2->valid) continue;
			if (atom1->isConnected(*atom2) == 0) continue;

			Renderable *cylinder = new VBOCylinder(atom1->x, atom1->y, atom1->z,
												   (atom1->x + atom2->x) / 2, (atom1->y + atom2->y) / 2, (atom1->z + atom2->z) / 2, bondR, atom1->color);
			scene.children.push_back(cylinder);

			cylinder = new VBOCylinder((atom1->x + atom2->x) / 2, (atom1->y + atom2->y) / 2, (atom1->z + atom2->z) / 2,
									   atom2->x, atom2->y, atom2->z, bondR, atom2->color);
			scene.children.push_back(cylinder);
		}
		Renderable *sphere = new VBOSphere(atom1->x, atom1->y, atom1->z, atomR, atom1->color);
		scene.children.push_back(sphere);
	}
}

void drawBondsAsLine(Renderable &scene, std::vector<int> &atomlist, float lineWidth) {
	std::vector<Vector3> points;
	std::vector<Color> colors;

	for (int i = 0, ilim = atomlist.size(); i < ilim; i++) {
		Atom *atom1 = atoms + atomlist[i];
		if (!atom1->valid) continue;
		for (int j = i + 1; j < i + 20 && j < ilim; j++) {
			Atom *atom2 = atoms + atomlist[j];
			if (!atom2->valid) continue;
			int order = atom1->isConnected(*atom2);
			if (order == 0) continue;
			drawBondsAsLineSub(points, colors, atom1, atom2, order);
		}

		for (std::map<int, int>::iterator it = atom1->bonds.begin(), itlim = atom1->bonds.end();
			 it != itlim; ++it) {
			int j = it->first, order = it->second;
			if (order < 1) continue;
			if (j < i + 20) continue; // be conservative. drawing lines twice doesn't harm us.
			bool found = false;
			int k = 0;
			for (int klim = atomlist.size(); k < klim; k++) { // FIXME: This is too slow!
				if (atomlist[k] == j) {
					found = true;
					break;
				}
			}
			if (!found) continue;
			Atom *atom2 = atoms + j;
			if (!atom2->valid) continue;
			if (k < i) continue; // important. otherwise, double bonds look like triple bonds!
			drawBondsAsLineSub(points, colors, atom1, atom2, order);
		}
	}

	Line *line = new Line(points, colors);
	line->width = lineWidth;
	line->discrete = true;
	scene.children.push_back(line);
}

void drawAtomsAsVdWSphere(Renderable &scene, std::vector<int> &atomlist) {
	std::vector<Vector3> points;
	std::vector<Color> colors;
	std::vector<float> radii;

	for (int i = 0, lim = atomlist.size(); i < lim; i++) {
		Atom *atom = atoms + atomlist[i];
		if (!atom->valid) continue;

		Renderable *sphere = new VBOSphere(atom->x, atom->y, atom->z, ChemDatabase::getVdwRadius(atom->elem), atom->color);
		scene.children.push_back(sphere);
		//		points.push_back(new Vector3(atom->x, atom->y, atom->z));
		//		colors.push_back(atom->color);
		//		radii.push_back(ChemDatabase.getVdwRadius(atom->elem));
	}
	//	Renderable sphere = new VBOSpheres(points, colors, radii);
	//	scene.children.push_back(sphere);
}

void drawSymmetryMates(Renderable &scene, Renderable *asu, std::map<int, Mat16> biomtMatrices) {
	if (biomtMatrices.size() == 0) return;

	MatRenderable *symmetryMate = new MatRenderable();
	symmetryMate->children.push_back(asu);
	scene.children.push_back(symmetryMate);

	for (std::map<int, Mat16>::iterator it = biomtMatrices.begin(); it != biomtMatrices.end(); ++it) {
		Mat16 mat = it->second;
//		if (isIdentity(mat)) continue;

		symmetryMate->addMatrix(mat);
	}
}

void drawSymmetryMatesWithTranslation(Renderable &scene, Renderable *asu, std::map<int, Mat16> matrices) {
	MatRenderable *symmetryMate = new MatRenderable();
	symmetryMate->children.push_back(asu);
	scene.children.push_back(symmetryMate);

	for (std::map<int, Mat16>::iterator it = matrices.begin(); it != matrices.end(); ++it) {
		Mat16 mat = it->second;

		for (int a = -1; a <= 0; a++) {
			for (int b = -1; b <= 0; b++) {
				for (int c = -1; c <= 0; c++) {
					Mat16 translated = mat;
					translated.m[3] += protein->ax * a + protein->bx * b + protein->cx * c;
					translated.m[7] += protein->ay * a + protein->by * b + protein->cy * c;
					translated.m[11] += protein->az * a + protein->bz * b + protein->cz * c;
//					if (isIdentity(translated)) continue;

					symmetryMate->addMatrix(translated);
				}
			}
		}
	}
}


void drawUnitcell(Renderable &scene, float width) {
	if (protein->a == 0) return;

	float vertices[][3] = {{0, 0, 0},
		{protein->ax, protein->ay, protein->az},
		{protein->bx, protein->by, protein->bz},
		{protein->ax + protein->bx, protein->ay + protein->by, protein->az + protein->bz},
		{protein->cx, protein->cy, protein->cz},
		{protein->cx + protein->ax, protein->cy + protein->ay,  protein->cz + protein->az},
		{protein->cx + protein->bx, protein->cy + protein->by, protein->cz + protein->bz},
		{protein->cx + protein->ax + protein->bx, protein->cy + protein->ay + protein->by, protein->cz + protein->az + protein->bz}};
	int edges[] = {0, 1, 0, 2, 1, 3, 2, 3, 4, 5, 4, 6, 5, 7, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

	float *points = new float[24 * 3];
	for (int i = 0; i < 24; i++) {
		points[i * 3] = vertices[edges[i]][0];
		points[i * 3 + 1] = vertices[edges[i]][1];
		points[i * 3 + 2] = vertices[edges[i]][2];
	}
	Line *line = new Line(points, 24);
	line->objectColor = Color(0.8f, 0.8f, 0.8f, 1);
	line->discrete = true;
	line->width = width;
	scene.children.push_back(line);
}
