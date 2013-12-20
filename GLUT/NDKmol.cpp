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

//  How to compile on Mac OS X
//  g++ -g -std=c++11 -c ../NDKmol/*.cpp
//  g++ -g -std=c++11 -framework GLUT -framework OpenGL -I../NDKmol -o NDKmol *.o NDKmol.cpp
//  ./NDKmol ../3V8T.pdb

#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <Glut/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include "GLES.hpp"
#include "NdkView.h"

void on_display();
void init(char* filename);
float objX, objY, objZ, cameraZ, slabNear, slabFar;

void on_display() {
  glClear(GL_COLOR_BUFFER_BIT);
  nativeGLRender(objX, objY, objZ, 0, 0, 1, 0, cameraZ, slabNear, slabFar);
  glutSwapBuffers();
}

void init(char* filename) {
  nativeLoadProtein(filename);
  printf("Load %s\n", filename);
  nativeGLResize(800, 600);

  nativeAdjustZoom(&objX, &objY, &objZ, &cameraZ, &slabNear, &slabFar, false);
  buildScene(MAINCHAIN_THICKRIBBON, HETATM_STICK, SYMOP_BIOMT, COLOR_CHAINBOW, false,
  	     false, BASE_LINE, false, false, false, false);
  //rotationQ.x = 0; rotationQ.y = 0; rotationQ.z = 0; rotationQ.w = -1;

  nativeGLInit();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(800, 600);

  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow("NDKmol");

  glutDisplayFunc(on_display);  

  init(argv[1]);
  glutMainLoop();

  return 0;
}
