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

// How to compile for WebGL with Emscripten
// ln -s PDB_FILE_YOU_WANT_TO_TEST.pdb initial.pdb
// em++  -std=c++11 -I../NDKmol  -c ../NDKmol/*.cpp
// em++  -std=c++11 -I../NDKmol --preload-file initial.pdb -s TOTAL_MEMORY=100000000 -o NDKmol.html *.o NDKmol.cpp

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <Glut/glut.h>
#else
#include "NDKmol/GLES.hpp"
#include <GL/glut.h>
#endif
#ifdef _MSC_VER
#include "Shellapi.h"  // for ShellExecute
#endif

#ifdef USE_X11_ICON
#include <X11/xpm.h>
#include <GL/glx.h>
#include "icon.xpm"
#endif

#include "NDKmol/NdkView.h"
#include "NDKmol/Quaternion.h"
#include "NDKmol/Vector3.hpp"

// GLUT is apparently deprecated in OSX 10.9
#if defined(__APPLE__) && defined(OPENGL_DEPRECATED)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

class Protein;
extern Protein *protein; // global from NDKmol/NdkView.cpp
extern float mapRadius;
extern float mapIsoLevel;

static const char* version = "NDKmol 0.92.1";
static const char *help_url =
    "https://github.com/wojdyr/NDKmol/wiki/NDKmol-for-Desktop";

enum {
  kMenuProteinTrace,
  kMenuProteinThinRibbon,
  kMenuProteinThickRibbon,
  kMenuProteinStrand,
  kMenuProteinTube,
  kMenuProteinBonds,
  kMenuProteinNone,
  kMenuNuclAcidLine,
  kMenuNuclAcidPolygon,
  kMenuLigandSphere,
  kMenuLigandStick,
  kMenuLigandLine,
  kMenuLigandInvisible,
  kMenuColorRainbow,
  kMenuColorChain,
  kMenuColorSS,
  kMenuColorPolarity,
  kMenuColorBfactor,
  kMenuShowMonomer,
  kMenuShowBiological,
  kMenuShowCrystal,
  kMenuToggleSmoothSheets,
  kMenuToggleSideChains,
  kMenuToggleSolvents,
  kMenuToggleUnitCell,
  kMenuToggleHetatmMates,
  kMenuToggleFullScreen,
  kMenuToggleFog,
  kMenuToggleFPS,
  kMenuHelp
};

enum MouseFunction {
  kMouseInactive,
  kMouseRotate,
  kMousePan,
  kMouseZoomRotate, // zoom + rotation in the screen plane
  kMouseSlab,
  kMouseMapLevel
};

struct WindowState {
  // view
  Vector3 obj;
  float cameraZ, slab_near, slab_far;
  Quaternion rotationQ;
  bool use_fog;

  // fullscreen handling
  bool fullscreen;
  int normal_width, normal_height; // size of normal (not fullscreen) window

  // values stored when mouse button is pressed
  MouseFunction mouse;
  int start_x, start_y;
  Vector3 current_obj;
  float current_cameraZ;
  Quaternion currentQ;
  float current_isol;
  float current_slab_near;
  float current_slab_far;
  float current_radius;

  bool menu_in_use; // not used atm

  // parameters for buildScene()
  int protein_mode;
  int nucleic_acid_mode;
  int hetatm_mode;
  int symmetry_mode;
  int color_mode;
  bool show_sidechains;
  bool show_unitcell;
  bool show_solvents;
  bool smoothen;
  bool symop_hetatms;
  float isol; // for map
  float map_radius;

  // status-bar-like display
  char status_str[80];
  int status_timeout;

  // for calculating fps
  bool calculate_fps;
  int fps_frame;
  int fps_time_start;
};

static WindowState w;

static void init_state() {
  w.rotationQ.w = -1;
  w.use_fog = true;
  w.fullscreen = false;
  w.normal_width = 800;
  w.normal_height = 600;
  w.mouse = kMouseInactive;
  w.menu_in_use = false;
  w.protein_mode = MAINCHAIN_THICKRIBBON;
  w.nucleic_acid_mode = BASE_LINE;
  w.hetatm_mode = HETATM_STICK;
  w.symmetry_mode = SYMOP_BIOMT;
  w.color_mode = COLOR_CHAINBOW;
  w.show_sidechains = false;
  w.show_unitcell = false;
  w.show_solvents = false;
  w.smoothen = true;
  w.symop_hetatms = false;
  mapIsoLevel = w.isol = 0.5f;
  mapRadius = w.map_radius = 10.0f;
  w.status_str[0] = '\0';
}

static Vector3 operator+(const Vector3& a, const Vector3& b) {
  return Vector3(a.x+b.x, a.y+b.y, a.z+b.z);
}

static void status(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
#if defined(_WIN32) && !defined(__CYGWIN__)
  int n = _vsnprintf(w.status_str, sizeof(w.status_str), fmt, args);
#else
  int n = vsnprintf(w.status_str, sizeof(w.status_str), fmt, args);
#endif
  va_end(args);
  (void) n;
  //if (n >= (int) sizeof(w.status_str)) printf("[%d chars] ", n);
  puts(w.status_str);
  w.status_timeout = glutGet(GLUT_ELAPSED_TIME) + 3000;
  glutPostRedisplay();
}

// it doesn't restore the state, so it can be called only at the end
static void render_status_string() {
  if (w.status_str[0] == '\0')
    return;
  if (glutGet(GLUT_ELAPSED_TIME) > w.status_timeout) {
    w.status_str[0] = '\0';
    return;
  }
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_FOG);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  int width = glutGet(GLUT_WINDOW_WIDTH);
  glOrtho(0, width, 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4f(0.f, 0.f, 0.f, 0.5f);
  glRectf(0, 0, width, 18);
  glDisable(GL_BLEND);
  glRasterPos2i(5, 5);
  glColor3f(0.6f, 1.0f, 0.6f);
  glRasterPos2i(5, 5);
  for (const char* c = w.status_str; *c != '\0'; ++c)
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
  if (w.use_fog)
    glEnable(GL_FOG);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

static void set_window_icon() {
#ifdef USE_X11_ICON
  Display *display = glXGetCurrentDisplay();
  GLXDrawable drawable = glXGetCurrentDrawable();
  if (!display || !drawable)
    return;
  XWMHints* wm_hints = XAllocWMHints();
  if (!wm_hints)
    return;
  Pixmap pixmap;
  if (XpmCreatePixmapFromData(display, drawable, (char**) icon_xpm, &pixmap,
                              NULL, NULL) == XpmSuccess) {
    wm_hints->flags = IconPixmapHint;
    wm_hints->icon_pixmap = pixmap;
    XSetWMHints(display, drawable, wm_hints);
  }
  XFree(wm_hints);
#endif
}

static void render() {
  glClear(GL_COLOR_BUFFER_BIT);
  float ax, ay, az;
  w.rotationQ.getAxis(&ax, &ay, &az);
  nativeSetScene(w.obj.x, w.obj.y, w.obj.z, ax, ay, az,
                 w.rotationQ.getAngle(), w.cameraZ, w.slab_near, w.slab_far);
  nativeUpdateMap(false);
  nativeGLRender();
  render_status_string();
  glutSwapBuffers();

  if (w.calculate_fps) {
    ++w.fps_frame;
    int elapsed_time = glutGet(GLUT_ELAPSED_TIME); // time in ms
    int time_diff = elapsed_time - w.fps_time_start;
    if (time_diff > 500) {
      // FPS is for debugging only, it goes to stdout for simplicity
      printf("FPS: %.1f\n", 1000.0 * w.fps_frame / time_diff);
      fflush(stdout);
      w.fps_time_start = elapsed_time;
      w.fps_frame = 0;
    }
    glutPostRedisplay();
  }
}

static void rebuild_scene() {
  bool reset_view = false;
  buildScene(w.protein_mode, w.hetatm_mode, w.symmetry_mode, w.color_mode,
             w.show_sidechains, w.show_unitcell,
             w.nucleic_acid_mode, w.show_solvents,
             reset_view, !w.smoothen, w.symop_hetatms);
  glutPostRedisplay();
}

static void toggle_fps() {
  w.calculate_fps = !w.calculate_fps;
  if (w.calculate_fps) {
    w.fps_time_start = glutGet(GLUT_ELAPSED_TIME);
    w.fps_frame = 0;
  }
}

static void on_change_size(int w, int h) {
  glViewport(0, 0, w, h);
  nativeGLResize(w, h);
}

static void toggle_fullscreen() {
  w.fullscreen = !w.fullscreen;
  if (w.fullscreen) {
    w.normal_width = glutGet(GLUT_WINDOW_WIDTH);
    w.normal_height = glutGet(GLUT_WINDOW_HEIGHT);
    glutFullScreen();
  } else {
    glutReshapeWindow(w.normal_width, w.normal_height);
  }
}

static void show_help() {
#if defined(__linux__)
  system((std::string("xdg-open ") + help_url).c_str());
#elif defined(__APPLE__)
  system((std::string("open ") + help_url).c_str());
#elif defined (_MSC_VER)
  ShellExecute(NULL, "open", help_url, NULL, NULL, SW_SHOWNORMAL);
#endif
  status("See %s", help_url);
}

static void menu_handler(int option) {
  switch (option) {
    case kMenuProteinTrace:
      w.protein_mode = MAINCHAIN_TRACE;
      break;
    case kMenuProteinThinRibbon:
      w.protein_mode = MAINCHAIN_THINRIBBON;
      break;
    case kMenuProteinThickRibbon:
      w.protein_mode = MAINCHAIN_THICKRIBBON;
      break;
    case kMenuProteinStrand:
      w.protein_mode = MAINCHAIN_STRAND;
      break;
    case kMenuProteinTube:
      w.protein_mode = MAINCHAIN_TUBE;
      break;
    case kMenuProteinBonds:
      w.protein_mode = MAINCHAIN_BONDS;
      break;
    case kMenuProteinNone:
      w.protein_mode = MAINCHAIN_NONE;
      break;
    case kMenuNuclAcidLine:
      w.nucleic_acid_mode = (w.nucleic_acid_mode == BASE_LINE ? BASE_NONE
                                                              : BASE_LINE);
      break;
    case kMenuNuclAcidPolygon:
      w.nucleic_acid_mode = (w.nucleic_acid_mode == BASE_POLYGON ? BASE_NONE
                                                              : BASE_POLYGON);
      break;
    case kMenuLigandSphere:
      w.hetatm_mode = HETATM_SPHERE;
      break;
    case kMenuLigandStick:
      w.hetatm_mode = HETATM_STICK;
      break;
    case kMenuLigandLine:
      w.hetatm_mode = HETATM_LINE;
      break;
    case kMenuLigandInvisible:
      w.hetatm_mode = HETATM_NONE;
      break;
    case kMenuColorRainbow:
      w.color_mode = COLOR_CHAINBOW;
      break;
    case kMenuColorChain:
      w.color_mode = COLOR_CHAIN;
      break;
    case kMenuColorSS:
      w.color_mode = COLOR_SS;
      break;
    case kMenuColorPolarity:
      w.color_mode = COLOR_POLAR;
      break;
    case kMenuColorBfactor:
      w.color_mode = COLOR_B_FACTOR;
      break;
    case kMenuShowMonomer:
      w.symmetry_mode = SYMOP_NONE;
      break;
    case kMenuShowBiological:
      w.symmetry_mode = SYMOP_BIOMT;
      break;
    case kMenuShowCrystal:
      w.symmetry_mode = SYMOP_PACKING;
      break;
    case kMenuToggleSmoothSheets:
      w.smoothen = !w.smoothen;
      break;
    case kMenuToggleSideChains:
      w.show_sidechains = !w.show_sidechains;
      break;
    case kMenuToggleUnitCell:
      w.show_unitcell = !w.show_unitcell;
      break;
    case kMenuToggleSolvents:
      w.show_solvents = !w.show_solvents;
      break;
    case kMenuToggleHetatmMates:
      w.symop_hetatms = !w.symop_hetatms;
      break;
    case kMenuToggleFullScreen:
      toggle_fullscreen();
      return; // skip rebuild_scene()
    case kMenuToggleFog:
      w.use_fog = !w.use_fog;
      w.use_fog ? glEnable(GL_FOG) : glDisable(GL_FOG);
      glutPostRedisplay();
      return; // skip rebuild_scene()
    case kMenuToggleFPS:
      toggle_fps();
      return; // skip rebuild_scene()
    case kMenuHelp:
      show_help();
      return; // skip rebuild_scene()
  }
  rebuild_scene();
}

static Vector3 pan_by(float dx, float dy) {
  float f = -w.cameraZ * 0.0005f;
  Vector3 v(f * dx, -f * dy, 0);
  w.rotationQ.rotateVector(v.x, v.y, v.z, &v.x, &v.y, &v.z);
  return v;
}

static void on_key(unsigned char key, int /*x*/, int /*y*/) {
  switch(key) {
    case '+':
    case '=':
      w.cameraZ /= 1.2f;
      glutPostRedisplay();
      break;
    case '-':
    case '_':
      w.cameraZ *= 1.2f;
      glutPostRedisplay();
      break;
    case 'f': menu_handler(kMenuToggleFullScreen); break;
    case 's': menu_handler(kMenuToggleSmoothSheets); break;
    case 'd': menu_handler(kMenuToggleSideChains); break;
    case 'c': menu_handler(kMenuToggleUnitCell); break;
    case 'w': menu_handler(kMenuToggleSolvents); break;
    case 'm': menu_handler(kMenuToggleHetatmMates); break;
    case '1': menu_handler(kMenuColorRainbow); break;
    case '2': menu_handler(kMenuColorChain); break;
    case '3': menu_handler(kMenuColorSS); break;
    case '4': menu_handler(kMenuColorPolarity); break;
    case '5': menu_handler(kMenuColorBfactor); break;
    case '7': menu_handler(kMenuShowMonomer); break;
    case '8': menu_handler(kMenuShowBiological); break;
    case '9': menu_handler(kMenuShowCrystal); break;
    case 'b': menu_handler(kMenuNuclAcidPolygon); break;
    case 'n': menu_handler(kMenuNuclAcidLine); break;
    case 'y': menu_handler(kMenuLigandSphere); break;
    case 'u': menu_handler(kMenuLigandStick); break;
    case 'i': menu_handler(kMenuLigandLine); break;
    case 'o': menu_handler(kMenuLigandInvisible); break;
    case 'R': rebuild_scene(); break; // intended for debugging only
    case '?': show_help(); break;

    case 27: // ESC
    case 'q':
      exit(0);
      break;
    default:
      status("key '%c' does nothing.", key);
  }
}

static void on_special_key(int key, int /*x*/, int /*y*/) {
  switch (key) {
    case GLUT_KEY_F1: menu_handler(kMenuProteinTrace); break;
    case GLUT_KEY_F2: menu_handler(kMenuProteinThinRibbon); break;
    case GLUT_KEY_F3: menu_handler(kMenuProteinThickRibbon); break;
    case GLUT_KEY_F4: menu_handler(kMenuProteinStrand); break;
    case GLUT_KEY_F5: menu_handler(kMenuProteinTube); break;
    case GLUT_KEY_F6: menu_handler(kMenuProteinBonds); break;
    case GLUT_KEY_F7: menu_handler(kMenuProteinNone); break;
    case GLUT_KEY_F8: break;
    case GLUT_KEY_F9: break;
    case GLUT_KEY_F10: break;
    case GLUT_KEY_F11: toggle_fullscreen(); break;
    case GLUT_KEY_F12: break;

    case GLUT_KEY_LEFT:
      w.obj = w.obj + pan_by(-40, 0);
      rebuild_scene();
      break;
    case GLUT_KEY_RIGHT:
      w.obj = w.obj + pan_by(+40, 0);
      rebuild_scene();
      break;
    case GLUT_KEY_UP:
      w.obj = w.obj + pan_by(0, -40);
      rebuild_scene();
      break;
    case GLUT_KEY_DOWN:
      w.obj = w.obj + pan_by(0, +40);
      rebuild_scene();
      break;
    case GLUT_KEY_PAGE_UP: break;
    case GLUT_KEY_PAGE_DOWN: break;
    case GLUT_KEY_HOME: break;
    case GLUT_KEY_END: break;
    case GLUT_KEY_INSERT: break;
    default:
      break;
  }
}

static void on_mouse_move(int x, int y) {
  switch (w.mouse) {
    case kMouseZoomRotate: { // zooming + rotation like in JMol
      w.cameraZ = w.current_cameraZ + (y - w.start_y) * 0.5f;
      float rot = (x - w.start_x) * 0.0025f;
      Quaternion dq(0, 0, sin(rot), cos(rot));
      w.rotationQ = Quaternion::multiply(dq, w.currentQ);
      break;
    }
    case kMouseRotate: {
      float dx = (x - w.start_x) / (float)glutGet(GLUT_WINDOW_WIDTH);
      float dy = (y - w.start_y) / (float)glutGet(GLUT_WINDOW_HEIGHT);
      float r = sqrt(dx * dx + dy * dy);
      if (r == 0)
          return;
      float rs = sin(r * M_PI) / r;
      Quaternion dq(rs * dy, rs * dx, 0, cos(r * M_PI));
      w.rotationQ = Quaternion::multiply(dq, w.currentQ);
      break;
    }
    case kMousePan:
      w.obj = w.current_obj + pan_by(x - w.start_x, y - w.start_y);
      nativeUpdateMap(false);
      break;
    case kMouseSlab: {
      w.slab_near = w.current_slab_near + (y - w.start_y) * 0.2f;
      w.slab_far = w.current_slab_far + (x - w.start_x) * 0.2f;
      status("slab from %.1f to %.1f", w.slab_near, w.slab_far);
      break;
    }
    case kMouseMapLevel:
      mapIsoLevel = w.isol = w.current_isol + (y - w.start_y) * 0.002f;
      mapRadius = w.map_radius = w.current_radius + (x - w.start_x) * 0.05f;
      nativeUpdateMap(true);
      status("map contour level: %.2f, radius: %.1f", w.isol, w.map_radius);
      break;
    default:
      return;
  }
  glutPostRedisplay();
}

static MouseFunction mouse_function(int button, int modif) {
  if (button == GLUT_LEFT_BUTTON) {
    if (modif & GLUT_ACTIVE_SHIFT)
      return kMouseZoomRotate;
    else if (modif & GLUT_ACTIVE_CTRL)
      return kMouseSlab;
    else
      return kMouseRotate;
  } else if (button == GLUT_MIDDLE_BUTTON) {
    if (modif & GLUT_ACTIVE_SHIFT)
      return kMouseMapLevel;
    else
      return kMousePan;
  }
  return kMouseInactive;
}

static void on_mouse_button(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON || button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {
      w.mouse = mouse_function(button, glutGetModifiers());
      w.start_x = x;
      w.start_y = y;
      w.current_obj = w.obj;
      w.current_cameraZ = w.cameraZ;
      w.currentQ = w.rotationQ;
      w.current_isol = w.isol;
      w.current_slab_near = w.slab_near;
      w.current_slab_far = w.slab_far;
      w.current_radius = w.map_radius;
    } else {
      on_mouse_move(x, y);
      w.mouse = kMouseInactive;
    }
  } else if ((button == 3 || button == 4) && state == GLUT_DOWN) { // scroll
    float zoom_factor = 1.1f;
    if (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
      zoom_factor = 1.01f;
    if (button == 3)
      w.cameraZ /= zoom_factor;
    else // button == 4
      w.cameraZ *= zoom_factor;
    glutPostRedisplay();
  }
}

static void on_menu_status(int status, int /*x*/, int /*y*/) {
  w.menu_in_use = (status == GLUT_MENU_IN_USE);
}

static void create_menu() {
  int polymer_menu = glutCreateMenu(menu_handler);
  glutAddMenuEntry("mainchain trace [F1]", kMenuProteinTrace);
  glutAddMenuEntry("thin ribbon [F2]", kMenuProteinThinRibbon);
  glutAddMenuEntry("thick ribbon [F3]", kMenuProteinThickRibbon);
  glutAddMenuEntry("strand [F4]", kMenuProteinStrand);
  glutAddMenuEntry("B-factor tube [F5]", kMenuProteinTube);
  glutAddMenuEntry("all bonds [F6]", kMenuProteinBonds);
  glutAddMenuEntry("invisible [F7]", kMenuProteinNone);

  int ligands_menu = glutCreateMenu(menu_handler);
  glutAddMenuEntry("Sphere [y]", kMenuLigandSphere);
  glutAddMenuEntry("Stick [u]", kMenuLigandStick);
  glutAddMenuEntry("Line [i]", kMenuLigandLine);
  glutAddMenuEntry("Invisible [o]", kMenuLigandInvisible);

  int color_menu = glutCreateMenu(menu_handler);
  glutAddMenuEntry("Rainbow [1]", kMenuColorRainbow);
  glutAddMenuEntry("Chain [2]", kMenuColorChain);
  glutAddMenuEntry("Structure [3]", kMenuColorSS);
  glutAddMenuEntry("Polarity [4]", kMenuColorPolarity);
  glutAddMenuEntry("B-factor [5]", kMenuColorBfactor);

  int packing_menu = glutCreateMenu(menu_handler);
  glutAddMenuEntry("Monomer [7]", kMenuShowMonomer);
  glutAddMenuEntry("Biological Unit [8]", kMenuShowBiological);
  glutAddMenuEntry("Crystal Packing [9]", kMenuShowCrystal);

  int toggle_menu = glutCreateMenu(menu_handler);
  glutAddMenuEntry("Smooth Sheets [s]", kMenuToggleSmoothSheets);
  glutAddMenuEntry("Sidechains [d]", kMenuToggleSideChains);
  glutAddMenuEntry("Solvents [w]", kMenuToggleSolvents);
  glutAddMenuEntry("Unit Cell [c]", kMenuToggleUnitCell);
  glutAddMenuEntry("Nucleic Acid Polygons [b]", kMenuNuclAcidPolygon);
  glutAddMenuEntry("Nucleic Acid Lines [n]", kMenuNuclAcidLine);
  glutAddMenuEntry("HETATM symmetry mates [m]", kMenuToggleHetatmMates);
  glutAddMenuEntry("Full Screen [f]", kMenuToggleFullScreen);
  glutAddMenuEntry("Depth Cue", kMenuToggleFog);
  glutAddMenuEntry("Calculate FPS", kMenuToggleFPS);


  glutCreateMenu(menu_handler); // main menu
  glutAddSubMenu("Polymer as", polymer_menu);
  glutAddSubMenu("Ligands as", ligands_menu);
  glutAddSubMenu("Color by", color_menu);
  glutAddSubMenu("Show", packing_menu);
  glutAddSubMenu("Toggle", toggle_menu);
  glutAddMenuEntry("Help [?]", kMenuHelp);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// returns uppercased extension (if the last extension is GZ, returns EXT.GZ)
static std::string get_extension(const char* filename) {
  std::string ext;
  const char* dot = strrchr(filename, '.');
  if (dot == NULL)
    return ext;
  if (toupper(dot[1]) == 'G' && toupper(dot[2]) == 'Z' && dot[3] == '\0')
    for (const char *p = dot-1; p > filename; --p)
      if (*p == '.') {
        dot = p;
        break;
      }
  for (const char* x = dot+1; *x != '\0'; ++x)
    ext.append(1, toupper(*x));
  return ext;
}

static void open_file(const char* filename) {
  //FIXME: nativeLoad* functions don't check for errors
  std::string ext = get_extension(filename);
  if (ext == "PDB" || ext == "ENT")
    nativeLoadProtein(filename);
  else if (ext == "SDF" || ext == "MOL")
    nativeLoadSDF(filename);
  else if (ext == "CCP4" || ext == "CCP4.GZ" || ext == "MAP")
    nativeLoadCCP4(filename);
  else {
    status("Unrecognized filetype (%s) of: %s", ext.c_str(), filename);
    return;
  }
  glutSetWindowTitle(filename);
}

static bool same(const char* a, const char* b) { return strcmp(a, b) == 0; }

static void parse_options(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    const char* arg = argv[i];
    if (same(arg, "-V") || same(arg, "--version")) {
#ifdef OPENGL_ES1
      const char* flavor = "ES1";
#else
      const char* flavor = "ES2";
#endif
      printf("%s (%s)\n", version, flavor);
      exit(0);
    }
    if (same(arg, "-h") || same(arg, "--help")) {
      printf("Usage: ndkmol protein.pdb [elden.ccp4]\n");
      exit(0);
    }
  }
}

// portable but suboptimal
static bool file_exists(const std::string& path) {
  FILE *f = fopen(path.c_str(), "r");
  if (f != NULL)
    fclose(f);
  return f != NULL;
}

static std::string pdb_example_path(const char* argv0) {
  const char* pdb_name = "initial.pdb";
#ifdef INITIAL_PDB
  if (file_exists(INITIAL_PDB))
    return INITIAL_PDB;
#endif
  std::string dir = argv0;
  size_t last_sep = dir.find_last_of("/\\");
  if (last_sep == std::string::npos)
    dir.clear();
  else
    dir.erase(last_sep+1);
#ifdef __APPLE__
  if (file_exists(dir+"../Resources/"+pdb_name))
      return dir+"../Resources/"+pdb_name;
#endif
  if (file_exists(dir+pdb_name))
      return dir+pdb_name;
  if (file_exists(dir+"res/raw/" + pdb_name))
      return dir+"res/raw/" + pdb_name;
  return "";
}

int main(int argc, char **argv) {
  init_state();
  parse_options(argc, argv);
  glutInit(&argc, argv);
  glutInitWindowSize(w.normal_width, w.normal_height);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(version);
#ifdef _WIN32
  PrepareGlFunctions();
#endif
  glutDisplayFunc(render);
  glutReshapeFunc(on_change_size);
  glutKeyboardFunc(on_key);
  glutSpecialFunc(on_special_key);
  glutMouseFunc(on_mouse_button);
  glutMotionFunc(on_mouse_move);
  glutMenuStatusFunc(on_menu_status);
  set_window_icon();

  bool have_file = false;
  for (int i = 1; i < argc; ++i) {
    open_file(argv[i]);
    have_file = true;
  }
  if (!have_file)
    nativeLoadProtein(pdb_example_path(argv[0]).c_str());

  nativeAdjustZoom(&w.obj.x, &w.obj.y, &w.obj.z,
                   &w.cameraZ, &w.slab_near, &w.slab_far, false);
  rebuild_scene();
  nativeGLInit();

  create_menu();
  glutMainLoop();

  return 0;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  return main(__argc, __argv);
}
#endif

// vim: et:ts=2:sw=2
