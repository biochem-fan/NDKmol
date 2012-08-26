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

package jp.sfjp.webglmol.NDKmol;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLSurfaceView;
import android.opengl.GLU;

public class NdkView implements GLSurfaceView.Renderer  {
	public float objX, objY, objZ;
	public float cameraZ, slabNear, slabFar, FOV = 20, maxD; // FIXME: FOV changing is not supported on JNI side.
	public Quaternion rotationQ;
	public boolean isMoving = false;
	public int width, height;

	// See View.hpp for these constants
	public int proteinMode = 0;
	public int hetatmMode = 2;
	public int nucleicAcidMode = 0;
	public boolean showSidechain = false;
	public boolean showUnitcell = false;
	public boolean showSolvents = false;
	public boolean doNotSmoothen = false;
	public boolean symopHetatms = true;
	public int symmetryMode = 0;
	public int colorMode = 0;
	public boolean fogEnabled = false;
	
	private static native void nativeGLInit();
	private static native void nativeGLResize(int w, int h);
	private static native void nativeGLRender();
	private static native void nativeLoadProtein(String path);
	private static native void nativeLoadSDF(String path);
	private static native void buildScene(int proteinMode, int hetatmMode, int symmetryMode, int colorMode, boolean showSidechain, 
			boolean showUnitcell, int nucleicAcidMode, boolean showSolvents, 
			boolean doNotSmoothen, boolean symopHetatms);
	public static native float[] nativeAdjustZoom(int symmetryMode);
	
	public NdkView() {
		resetCamera();
	} 
	
	public void resetCamera() {
		float [] parms = nativeAdjustZoom(symmetryMode);
		objX = parms[0]; objY = parms[1]; objZ = parms[2];
		cameraZ = parms[3]; slabNear = parms[4]; slabFar = parms[5];
		maxD = parms[6];
		rotationQ = new Quaternion(1, 0, 0, 0);
	}
	
	public void prepareScene() {
		buildScene(proteinMode, hetatmMode, symmetryMode, colorMode, showSidechain, showUnitcell, 
				nucleicAcidMode, showSolvents, doNotSmoothen, symopHetatms);
	}

	public void onDrawFrame(GL10 gl) {
		gl.glClear(GL10.GL_COLOR_BUFFER_BIT|GL10.GL_DEPTH_BUFFER_BIT);
		
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();
		float cameraNear = -cameraZ + slabNear;
		if (cameraNear < 1) cameraNear = 1;
		float cameraFar = -cameraZ + slabFar;
		if (cameraNear + 1 > cameraFar) cameraFar = cameraNear + 1;
		GLU.gluPerspective(gl, FOV, (float) width / height, cameraNear, cameraFar);
		if (fogEnabled) {
			gl.glEnable(GL10.GL_FOG);
			gl.glFogf(GL10.GL_FOG_MODE, GL10.GL_LINEAR); // EXP, EXP2 is not supported?
			gl.glFogfv(GL10.GL_FOG_COLOR, new float[] {0, 0, 0, 1}, 0);
			gl.glFogf(GL10.GL_FOG_DENSITY, 0.3f);
			//		gl.glHint(GL10.GL_FOG_HINT, GL10.GL_DONT_CARE);
			gl.glFogf(GL10.GL_FOG_START, cameraNear * 0.3f + cameraFar * 0.7f);
			gl.glFogf(GL10.GL_FOG_END, cameraFar);
		} else {
			gl.glDisable(GL10.GL_FOG);
		}
		
		gl.glMatrixMode(GL10.GL_MODELVIEW);
		gl.glLoadIdentity();
		gl.glTranslatef(0, 0, cameraZ);
		
		Vector3 axis = rotationQ.getAxis();
		gl.glRotatef(180 * rotationQ.getAngle() / (float)Math.PI, axis.x, axis.y, axis.z);
		gl.glTranslatef(objX, objY, objZ);
		
		nativeGLRender();
		
		if (fogEnabled) gl.glDisable(GL10.GL_FOG);
	}
 
	public void onSurfaceChanged(GL10 gl, int w, int h) {
		width = w; height = h;
		gl.glViewport(0, 0, width, height);
		gl.glMatrixMode(GL10.GL_PROJECTION);
		gl.glLoadIdentity();
		GLU.gluPerspective(gl, 20, (float)width / height, 1, 800);
		
		nativeGLResize(w, h);
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig arg1) {
		nativeGLInit();
		gl.glDisable(GL10.GL_FOG);
	}
	
	public void loadPDB(String path) {
		nativeLoadProtein(path);	
		prepareScene();
		resetCamera();
	}
	
	public void loadSDF(String path) {
		nativeLoadSDF(path);	
		prepareScene();
		resetCamera();
	}
}
