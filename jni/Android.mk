LOCAL_PATH := $(call my-dir)/../NDKmol

include $(CLEAR_VARS)

LOCAL_MODULE    := Ndkmol
LOCAL_SRC_FILES := Ndkmol.cpp Atom.cpp NdkView.cpp Renderable.cpp CylinderGeometry.cpp \
	 SphereGeometry.cpp VBOSphere.cpp Color.cpp Geometry.cpp Vector3.cpp Line.cpp SmoothCurve.cpp \
	 RibbonStrip.cpp PDBReader.cpp Protein.cpp ChemDatabase.cpp MatRenderable.cpp VBOCylinder.cpp \
	 SDFReader.cpp SmoothTube.cpp GLES.cpp Matrix.cpp#PLYGeometry.cpp 

# When building for Android 1.6 (android-4), we must use -lGLESv1_CM instead 
LOCAL_LDLIBS := -llog -lGLESv2

include $(BUILD_SHARED_LIBRARY)