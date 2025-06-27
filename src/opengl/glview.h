/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2025 by CNRS and University of Strasbourg */

/*!
* @file glview.h
* @short Variable declarations related to the OpenGL window \n
         Function declarations related to the OpenGL window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'glview.h'
*
* Contains:

 - Variable declarations related to the OpenGL window
 - Function declaration related to the OpenGL window

*/

#ifndef GL_VIEW_H_
#define GL_VIEW_H_

/*! \def QUALITY
  \brief Default OpenGL quality (number of polygons)
*/
#define QUALITY 30
/*! \def DEFAULT_STYLE
  \brief Default OpenGL style: ball and stick
*/
#define DEFAULT_STYLE 1

#define DEFAULT_SIZE 1

/*! \def DEFAULT_LIGHTNING
  \brief Default OpenGL material ligthning
*/
#define DEFAULT_LIGHTNING          1.00

/*! \def DEFAULT_METALLIC
  \brief Default OpenGL material metallic
*/
#define DEFAULT_METALLIC           0.35

/*! \def DEFAULT_ROUGHNESS
  \brief Default OpenGL material roughness
*/
#define DEFAULT_ROUGHNESS          0.15

/*! \def DEFAULT_AMBIANT_OCCLUSION
  \brief Default OpenGL material ambiant occlusion
*/
#define DEFAULT_AMBIANT_OCCLUSION  1.00

/*! \def DEFAULT_GAMMA_CORRECTION
  \brief Default OpenGL material gamma correction
*/
#define DEFAULT_GAMMA_CORRECTION   1.50

/*! \def DEFAULT_OPACITY
  \brief Default OpenGL opacity
*/
#define DEFAULT_OPACITY            1.00

/*! \def TEMPLATES
  \brief Number of material templates
*/
#define TEMPLATES 7

extern gchar * material_template[TEMPLATES];
extern GLfloat template_parameters[TEMPLATES][5];

// default light data
/*! \def MAX_LIGHTS
  \brief Maximum number of light sources
*/
#define MAX_LIGHTS 10

/*! \def DEFAULT_INTENSITY
  \brief Default light intensity
*/
#define DEFAULT_INTENSITY 10.0

/*! \def NEAR_PLANE
  \brief Default value for the OpenGL near plane
*/
#define NEAR_PLANE 1.0

/*! \def FAR_PLANE
  \brief Default value for the OpenGL far plane
*/
#define FAR_PLANE 10.0

/*! \def CAMERA_ANGLE_X
  \brief Default value for the OpenGL camera pitch in °
*/
#define CAMERA_ANGLE_X 5.0    // Pitch in degree

/*! \def CAMERA_ANGLE_Y
  \brief Default value for the OpenGL camera heading in °
*/
#define CAMERA_ANGLE_Y -25.0  // Heading in degree

/*! \def CAMERA_ANGLE_Z
  \brief Default value for the OpenGL camera toll in °
*/
#define CAMERA_ANGLE_Z 0.0

/*! \def ZOOM
  \brief Default value for the OpenGL zoom
*/
#define ZOOM 1.5

/*! \def ZOOM_FACTOR
  \brief Default value for the OpenGL zoom factor
*/
#define ZOOM_FACTOR 0.05

/*! \def ZOOM_MAX
  \brief Maximum value for the OpenGL zoom factor
*/
#define ZOOM_MAX 0.001

/*! \def REFRESH
  \brief Refresh time for spinning the OpenGL window in milliseconds
*/
#define REFRESH 10

/*! \def MAX_IN_SELECTION
  \brief Maximum number of atoms in selection to display measure information
*/
#define MAX_IN_SELECTION 21
/*! \def COORD_MAX_MENU
  \brief Maximum number of fragments or molecules to build the related menu items
*/
#define COORD_MAX_MENU 20

/*! \enum radii */
enum radii {
  VDW = 0, /*!< 0 */
  INC = 1, /*!< 1 */
  COV = 2, /*!< 2 */
  ION = 3  /*!< 3 */
};

/*! \enum representation */
enum representation {
  ORTHOGRAPHIC = 0, /*!< 0 */
  PERSPECTIVE  = 1 /*!< 1 */
};

/*! \enum modes */
enum modes {
  ANALYZE   = 0, /*!< 0 */
  EDITION   = 1, /*!< 1 */
  MOTION    = 2, /*!< 2 */
  DL_POLY   = 3, /*!< 3 */
  LAAMPS    = 4, /*!< 4 */
  CPMD_AI   = 5, /*!< 5 */
  CP2K_AI   = 6, /*!< 6 */
  CP2K_QMMM = 7, /*!< 7 */
  CP2K_MM   = 8  /*!< 8 */
};

#define NUM_STYLES 7

/*! \enum styles */
enum styles {
  NONE           = -1, /*!< -1 */
  BALL_AND_STICK =  0, /*!< 0 */
  WIREFRAME      =  1, /*!< 1 */
  SPACEFILL      =  2, /*!< 2 */
  SPHERES        =  3, /*!< 3 */
  CYLINDERS      =  4, /*!< 4 */
  PUNT           =  5  /*!< 5 */
};

/*! \enum render */
enum render {
  FILL = 0, /*!< 0 */
  LINE = 1, /*!< 1 */
  PTS  = 2  /*!< 2 */
};

/*! \enum action */
enum action {
  ROTATE      = 0, /*!< 0 */
  MOVE        = 1, /*!< 1 */
  SELECT      = 2, /*!< 2 */
  ZOOM_IN_OUT = 3, /*!< 3 */
  SPIN        = 4  /*!< 4 */
};

 /*! \enum mouse_status */
enum mouse_status {
  CLICKED  = 0, /*!< 0 */
  RELEASED = 1  /*!< 1 */
};

/*! \enum axisposition */
enum axisposition {
  TOP_RIGHT    = 0, /*!< 0 */
  TOP_LEFT     = 1, /*!< 1 */
  BOTTOM_RIGHT = 2, /*!< 2 */
  BOTTOM_LEFT  = 3, /*!< 3 */
  CENTER       = 4, /*!< 4 */
  CUSTOM       = 5  /*!< 5 */
};

/*! \enum labels */
enum labels {
  ELEMENT_NAME   = 0, /*!< 0 */
  SYMBOL         = 1, /*!< 1 */
  SYMBOL_AND_NUM = 2, /*!< 2 */
  NUM            = 3, /*!< 3 */
  ID_IN_MOLECULE = 4  /*!< 4 */
};

/*! \enum actions */
enum actions {
  DISPL   = 2, /*!< 2 */
  REPLACE = 3, /*!< 3 */
  REMOVE  = 4, /*!< 4 */
  INSERT  = 5, /*!< 5 */
  RANMOVE = 6  /*!< 6 */
};

/*! \enum texts */
enum texts {
  SIMPLE_TEXT = 0, /*!< 0 */
  BETTER_TEXT = 1  /*!< 1 */
};

/*enum material {
  AMBIENT = 0,
  DIFFUSE = 1,
  SPECULAR = 2,
  EMISSION = 3,
  OPACITY = 4
};*/

extern GLfloat ** allocdGLfloat (int xal, int yal);
extern GLfloat initlights[4][4];
extern gboolean pick;
extern glwin * wingl;
extern project * proj_gl;
extern coord_info * coord_gl;
extern box_info * box_gl;
extern cell_info * cell_gl;
extern int proj_sp;
extern int proj_at;
extern image * plot;
extern int qual;
extern int acolorm;
extern int pcolorm;
extern int step;
extern int gColorID[3];
extern int field_object;
extern GLenum ogl_texture;

extern gboolean create_bond (int ac, int bid, int ba, int bb, int sel, double length);
extern void draw_cylinder_bond (atom a, atom b, int bid, int ci, int bi);
extern void draw_cylinder_bond_to_pick (atom a, atom b, int bid);
extern void draw_wireframe_bond (atom a, atom b, int ci, int bi);
extern gboolean create_atom (GLUquadricObj * quadric, int aid, int ac, int sp, int sel);

extern void draw_vertices (int id);
extern void draw_atom_to_pick (atom at);
extern void draw_atom (atom at, double al);
extern void draw_rings_gl (int se, int ge, int ta, int id);
extern void draw_box ();
extern void prepare_cuboid (vec3_t position, int id);
extern void prepare_axis ();
extern void draw (glwin * view);
extern void render_all_strings (int glsl, int id);
extern void prepare_string (char * text, int id, ColRGBA col, vec3_t pos, float lshift[3],
                            atom * at, atom * bt, atom * ct);

ColRGBA init_color (int id, int numid);
ColRGBA set_default_color (int z);
extern void sort (int dim, int * tab);
extern vec3_t get_insertion_coordinates (glwin * view);
void setup_bonds (glwin * view);
void update (glwin * view);
void center_this_molecule (glwin * view);
G_MODULE_EXPORT void to_center_this_molecule (GtkWidget * widg, gpointer data);
#ifdef GTK3
G_MODULE_EXPORT gboolean on_lib_pressed (GtkWidget * widg, GdkEvent * event, gpointer data);
G_MODULE_EXPORT gboolean on_glwin_button_event (GtkWidget * widg, GdkEvent * event, gpointer data);
G_MODULE_EXPORT gboolean on_motion (GtkWidget * widg, GdkEvent * event, gpointer data);
G_MODULE_EXPORT gboolean on_scrolled (GtkWidget * widg, GdkEvent * event, gpointer data);
#else
G_MODULE_EXPORT void on_lib_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
G_MODULE_EXPORT void on_lib_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
G_MODULE_EXPORT void on_glwin_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
G_MODULE_EXPORT void on_glwin_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
G_MODULE_EXPORT void on_glwin_pointer_motion (GtkEventControllerMotion * motion, gdouble x, gdouble y, gpointer data);
G_MODULE_EXPORT gboolean on_glwin_pointer_scoll (GtkEventControllerScroll * event, gdouble dx, gdouble dy, gpointer data);
#endif
void reshape (glwin * view, int width, int height, gboolean use_ratio);
void zoom (glwin * view, int delta);

void init_opengl ();
void init_camera (project * this_proj, gboolean get_depth);

#ifdef GTKGLAREA
  G_MODULE_EXPORT void on_realize (GtkGLArea * area, gpointer data);
  G_MODULE_EXPORT gboolean on_expose (GtkGLArea * area, GdkGLContext * context, gpointer data);
#else
  G_MODULE_EXPORT void on_realize (GtkWidget * area, gpointer data);
  G_MODULE_EXPORT gboolean on_expose (GtkWidget * widg, cairo_t * cr, gpointer data);
#endif

extern void debug_image (image img, int i);

extern distance distance_2d (atom * at, atom * bt);
extern distance distance_3d (cell_info * cell, int mdstep, atom * at, atom * bt);
extern angle angle_2d (atom * at, atom * bt, atom * ct);
extern angle angle_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct);
extern angle dihedral_3d (cell_info * cell, int mdstep, atom * at, atom * bt, atom * ct, atom * dt);

extern atom * duplicate_atom (atom * at);
extern void at_shift (atom * at, float * shift);
extern void at_unshift (atom * at, float * shift);
extern int check_label_numbers (project * this_proj, int types);

extern mat4_t create_axis_matrices (int type);
extern mat4_t create_label_matrices ();

extern int nbs, nbl, nba;
extern void re_create_all_md_shaders (glwin * view);
extern void re_create_md_shaders (int nshaders, int shaders[nshaders], project * this_proj);
extern void cleaning_shaders (glwin * view, int shader);
extern void init_default_shaders (glwin * view);
extern void init_shaders(glwin * view);

extern glsl_program * init_shader_program (int object, int object_id,
                                           const GLchar * vertex, const GLchar * geometry, const GLchar * fragment,
                                           GLenum type_of_vertices, int narray, int nunif, gboolean lightning, object_3d * obj);

extern void update_selection_list (atom_selection * at_list, atom * at, gboolean add);
extern void update_all_selections (glwin * view, int pi);
extern void save_all_selections (glwin * view, int pi);

extern void process_selected_atom (project * this_proj, glwin * view, int id, int ac, int se, int pi);

extern void update_selection_tree (glwin * view, int pi, int id);
extern void update_all_menus (glwin * view, int nats);

extern gboolean is_coord_in_menu (int id, project * this_proj);

extern atom_search * free_this_search_data (atom_search * this_search);
#ifdef GTK3
extern G_MODULE_EXPORT void show_hide_clones (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_rep (GtkWidget * widg, gpointer data);
#else
extern G_MODULE_EXPORT void show_hide_clones (GSimpleAction * action, GVariant * state, gpointer data);
#endif

extern void popup_selection (glwin * view, double ptx, double pty, int se, int pe, int ai, int bi, int ac);
extern void popup_main_menu (glwin * view, double ptx, double pty);

typedef struct gl_pop_info gl_pop_info;
struct gl_pop_info
{
  int action;
  double x, y;
  int pts[5];
};

extern gl_pop_info to_pop;

#endif
