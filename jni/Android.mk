LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Ndkmol
LOCAL_SRC_FILES := Ndkmol.cpp Atom.cpp NdkView.cpp Renderable.cpp CylinderGeometry.cpp \
	 SphereGeometry.cpp VBOSphere.cpp Color.cpp Geometry.cpp Vector3.cpp Line.cpp SmoothCurve.cpp \
	 RibbonStrip.cpp PDBReader.cpp Protein.cpp ChemDatabase.cpp MatRenderable.cpp VBOCylinder.cpp \
	 SDFReader.cpp SmoothTube.cpp #PLYGeometry.cpp 

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)