# NOTE: it's only for GLUT (desktop) version of ndkmol
project(NDKmol C CXX)
cmake_minimum_required(VERSION 2.6)

option(USE_ES2 "Use OpenGL ES 2" OFF)
if(UNIX AND NOT APPLE)
 option(USE_X11_ICON "Use icon in X11 Window Manager" ON)
endif()
#if(APPLE)
# option(MACOSX_BUNDLE "Make OSX app bundle" OFF)
#endif()

if ("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xMSVC") # avoiding dereferencing MSVC
 add_definitions(-D_USE_MATH_DEFINES)  # needed for M_PI
endif()

include(CheckCXXCompilerFlag)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(ZLIB REQUIRED)
if (DEFINED USE_X11_ICON AND USE_X11_ICON)
  find_package(X11 REQUIRED)
  if (NOT X11_Xpm_LIB)
    message(FATAL_ERROR "libXpm not found. On Linux you need a package named "
                        "libxpm-dev or libXpm-devel or similarly.")
  endif()
  add_definitions(-DUSE_X11_ICON)
  set(EXTRA_LIBS ${X11_Xpm_LIB})
endif()

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIRS} ${X11_INCLUDE_DIR})
include_directories("${PROJECT_SOURCE_DIR}")

# FindGLUT.cmake adds libXmu and libXi even if it's not needed
if (NOT GLUT_Xmu_LIBRARY)
  set(GLUT_Xmu_LIBRARY "")
endif()
if (NOT GLUT_Xi_LIBRARY)
  set(GLUT_Xi_LIBRARY "")
endif()
set(GLUT_LIBRARIES ${GLUT_glut_LIBRARY} ${GLUT_Xmu_LIBRARY} ${GLUT_Xi_LIBRARY}
                   ${GLUT_cocoa_LIBRARY})

# show warnings by default
check_cxx_compiler_flag(-Wextra HAS_WEXTRA_FLAG)
if (HAS_WEXTRA_FLAG)
  set(EXTRA_CXX_FLAGS "-Wall -Wextra" CACHE STRING "")
endif()
set(EXTRA_CXX_FLAGS ${EXTRA_CXX_FLAGS} CACHE STRING "Flags for compiler" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_FLAGS}")

# set_property(TARGET ndkmol PROPERTY CXX_STANDARD 11) # only in cmake 3.1+
check_cxx_compiler_flag(-std=c++11 HAS_STDCXX11_FLAG)
if (HAS_STDCXX11_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if (NOT USE_ES2)
  add_definitions(-DOPENGL_ES1)
endif()

add_definitions("-DINITIAL_PDB=\"${CMAKE_INSTALL_PREFIX}/share/ndkmol/initial.pdb\"")

if(WIN32)
  set(RCFILE GLUT/ndkmol.rc)
endif()

add_executable(ndkmol WIN32
NDKmol/Atom.cpp              NDKmol/PDBReader.cpp
NDKmol/CCP4Reader.cpp        NDKmol/Protein.cpp
NDKmol/ChemDatabase.cpp      NDKmol/Quaternion.cpp
NDKmol/Color.cpp             NDKmol/Renderable.cpp
NDKmol/CylinderGeometry.cpp  NDKmol/RibbonStrip.cpp
NDKmol/Debug.cpp             NDKmol/SDFReader.cpp
NDKmol/Geometry.cpp          NDKmol/SmoothCurve.cpp
NDKmol/GLES.cpp              NDKmol/SmoothTube.cpp
NDKmol/Line.cpp              NDKmol/SphereGeometry.cpp
NDKmol/MarchingSquares.cpp   NDKmol/VBOCylinder.cpp
NDKmol/MatRenderable.cpp     NDKmol/VBOSphere.cpp
NDKmol/Matrix.cpp            NDKmol/Vector3.cpp
NDKmol/NdkView.cpp           #NDKmol/VolumeRenderer.cpp
GLUT/NDKmol.cpp
${RCFILE}
res/raw/initial.pdb
)

# We could make an app bundle, but would it be useful?
#if (APPLE AND MACOSX_BUNDLE)
#  set_target_properties(ndkmol PROPERTIES
#                        MACOSX_BUNDLE TRUE
#                        MACOSX_BUNDLE_BUNDLE_NAME "NDKmol")
#  set(MACOSX_BUNDLE_ICON_FILE ndkmol.icns)
#  set_source_files_properties(res/raw/initial.pdb PROPERTIES
#                              MACOSX_PACKAGE_LOCATION Resources)
#endif()

target_link_libraries(ndkmol ${EXTRA_LIBS} ${GLUT_LIBRARIES}
                             ${OPENGL_LIBRARIES} ${ZLIB_LIBRARIES})

# check if the program starts at least
add_custom_target(check COMMAND ndkmol "--version")

install(TARGETS ndkmol DESTINATION bin)
install(FILES res/raw/initial.pdb DESTINATION share/ndkmol/)
