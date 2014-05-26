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
	private static native void nativeGLRender(float objX, float objY, float objZ, float ax, float ay, float az, float rot,
            float cameraZ, float slabNear, float slabFar);
	private static native void nativeLoadProtein(String path);
	private static native void nativeLoadSDF(String path);
	private static native void nativeLoadCCP4(String path);
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
		if (NDKmolActivity.GLES1) {
			if (fogEnabled) {
				float cameraNear = -cameraZ + slabNear;
				if (cameraNear < 1) cameraNear = 1;
				float cameraFar = -cameraZ + slabFar;
				if (cameraNear + 1 > cameraFar) cameraFar = cameraNear + 1;
				
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
		}
		
		Vector3 axis = rotationQ.getAxis();
		nativeGLRender(objX, objY, objZ, axis.x, axis.y, axis.z, rotationQ.getAngle(),
                cameraZ, slabNear, slabFar);
		
		if (NDKmolActivity.GLES1) {
			if (fogEnabled) gl.glDisable(GL10.GL_FOG);
		}
	}
 
	public void onSurfaceChanged(GL10 gl, int w, int h) {
		width = w; height = h;
		gl.glViewport(0, 0, width, height);
		
		nativeGLResize(w, h);
//		nativeGLInit(); // TODO: Do we need this? Do we need to re-register VBOs?
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig arg1) {
		nativeGLInit();
		
		if (NDKmolActivity.GLES1) {
			gl.glDisable(GL10.GL_FOG);
		}
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
	
	public void loadCCP4(String path) {
		nativeLoadCCP4(path);	
		prepareScene();
	}
}
