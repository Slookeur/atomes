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
* @file glwin.h
* @short Variable declarations related the OpenGL window \n
         Data structure declarations related the OpenGL window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'glwin.h'
*
* Contains:

 - Variable declarations related the OpenGL window
 - Data structure declarations related the OpenGL window

*/

#ifndef GLWIN_H_
#define GLWIN_H_

#include "ogl_shading.h"

#ifndef GL_COLOR_SUM_EXT
# define GL_COLOR_SUM_EXT 0x8458
#endif

#ifndef GL_LIGHT_MODEL_COLOR_CONTROL_EXT
# define GL_LIGHT_MODEL_COLOR_CONTROL_EXT 0x81F8
#endif

#ifndef GL_SEPARATE_SPECULAR_COLOR_EXT
# define GL_SEPARATE_SPECULAR_COLOR_EXT 0x81FA
#endif

#ifndef GL_TEXTURE_RECTANGLE_ARB
# define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#define BOX 0
#define AXIS 1

#define OGL_RENDERS 3
#define OGL_BOX 8
#define OGL_AXIS 13
#define OGL_REPS 2
#define ATOM_MAPS 7
#define POLY_MAPS 7
#define OGL_COORDS 5
#define OGL_RINGS 12
#define NUM_COLORS 64
#define NINPUTS 6
#define NSELECTION 7

#define NGLOBAL_SHADERS 15

/*! \enum object_types */
enum object_types
{
  FROM_DATA    = -8, /*!< -8 */
  FROM_SPEC    = -7, /*!< -7 */
  FROM_MOL     = -6, /*!< -6 */
  FROM_FRAG    = -5, /*!< -5 */
  FROM_COORD_P = -4, /*!< -4 */
  FROM_COORD_T = -3, /*!< -3 */
  FROM_LIBRARY = -2, /*!< -2 */
  FROM_PROJECT = -1  /*!< -1 */
};

/*! \enum shaders

  \brief The different types of shaders in the atomes program
*/
enum shaders {
  ATOMS =  0,  /*!< 0 */
  BONDS =  1,  /*!< 1 */
  SELEC =  2,  /*!< 2 */
  POLYS =  3,  /*!< 3 */
  MDBOX =  4,  /*!< 4 */
  MAXIS =  5,  /*!< 5 */
  ARROW =  6,  /*!< 6 */
  RINGS =  7,  /*!< 7 */
  PICKS =  8,  /*!< 8 */
  LABEL =  9,  /*!< 9 */
  MEASU = 10, /*!< 10 */
  LIGHT = 11, /*!< 11 */
  SLABS = 12, /*!< 12 */
  VOLMS = 13, /*!< 13 */
  BACKG = 14  /*!< 14 */
};

#define FILLED_STYLES 4
#define OGL_STYLES 6

extern char * text_styles[OGL_STYLES];
extern char * text_filled[FILLED_STYLES];

/*! \typedef angle */
typedef struct angle angle;
struct angle
{
  double angle;   /*!< the angle value */
  gboolean pbc;   /*!< are PBC used ? 1 = yes, 0 = no */
};

/*! \typedef distance */
typedef struct distance distance;
struct distance
{
  double length;  /*!< the distance value */
  double x;       /*!< x vector component */
  double y;       /*!< y vector component */
  double z;       /*!< z vector component */
  gboolean pbc;   /*!< are PBC used ? 1 = yes, 0 = no */
};

typedef struct atom_data atom_data;
struct atom_data
{
  gboolean show[2];
  gboolean label[2];
  gboolean pick[2];
  gboolean cloned;
  int style;
};

typedef struct spinner spinner;
struct spinner
{
  GtkWidget * win;
  GtkWidget * right;
  GtkWidget * left;
  GtkWidget * up;
  GtkWidget * down;
  GtkWidget * stop;
};

typedef struct sequencer sequencer;
struct sequencer
{
  GtkWidget * win;
  GtkWidget * first;
  GtkWidget * last;
  GtkWidget * prev;
  GtkWidget * next;
  GtkWidget * jump;
  GtkWidget * play;
  GtkWidget * stop;
  GtkWidget * loop;
  GtkWidget * fast;
  GtkWidget * slow;
};

typedef struct recorder recorder;
struct recorder
{
  GtkWidget * win;
  GtkWidget * rec;
  GtkWidget * stop;
};

/*! \typedef Light

  \brief Light parameters for OpenGL rendering
*/
typedef struct Light Light;
struct Light
{
  int type;           /*!< Light type: 0 = directional, 1 = point, 2 = spot light */
  int fix;            /*!< Light fix: 0 = fix for the view, 1 = fix for the molecule */
  int show;           /*!< Show (1) or hide light (0) */
  vec3_t position;    /*!< Light position */
  vec3_t direction;   /*!< Light direction for directional and spot lights */
  vec3_t intensity;   /*!< Light colors */
  vec3_t attenuation; /*!< Constant, linear and quadratic attenuation */
  vec3_t spot_data;   /*!< Angle, inner and outer spot light cutoff */
};

/*! \typedef Lightning

  \brief Lightning data for OpenGL rendering
*/
typedef struct Lightning Lightning;
struct Lightning
{
  int lights;         /*!< The number of lights */
  Light * spot;       /*!< The lights */
};

/*! \typedef Material

  \brief Material parameters for OpenGL rendering
*/
typedef struct Material Material;
struct Material
{
  int predefine;       /*!< Cutsom (0) or template (1) */
  vec3_t albedo;       /*!< Albedo components */
  // 0 = lightning model
  // 2 = metallic
  // 3 = roughness
  // 4 = back lightning
  // 5 = gamma
  // 6 = opacity
  GLfloat param[6];    /*!< Material parameters \n
                         0 = lightning model, \n
                         1 = metallic, \n
                         2 = roughness, \n
                         3 = back lightning, \n
                         4 = gamma, \n
                         5 = opacity*/
};

/*! \typedef Fog

  \brief Fog parameters for OpenGL rendering
*/
typedef struct Fog Fog;
struct Fog
{
  int mode;         /*!< Fog mode in: none, linear (LINEAR), exponential (EXP) and exponential squared (EXP2) */
  int based;        /*!< Fog type: plane based (0) or range based (1) */
  float density;    /*!< Fog density */
  float depth[2];   /*!< Fog depth: 0 = start value, 1 = end value */
  vec3_t color;     /*!< Fog color */
};

/*! \typedef screen_string

  \brief OpenGL string rendering
*/
typedef struct screen_string screen_string;
struct screen_string
{
  int id;                         /*!< The string unique ID */
  int type;                       /*!< The type of string (3= labels, 4 = bonds, 5= angles) */
  char * word;                    /*!< The text to be displayed */
  ColRGBA col;                    /*!< Color of the string */
  float shift[4];                 /*!< The shifts (if any) on x, y, z, then visibility */
  int num_instances;              /*!< The number of instances for that string */
  float * instances;              /*!< The list of instances for that string */
  screen_string * prev;
  screen_string * last;
};

typedef struct screen_label screen_label;
struct screen_label
{
  int position;
  int render;
  int scale;
  gchar * font;
  int n_colors;
  ColRGBA * color;
  double shift[3];
  screen_string * list;
};

typedef struct atom_in_selection atom_in_selection;
struct atom_in_selection
{
  int id;
  int sp;
  atom_in_selection * prev;
  atom_in_selection * next;
};

typedef struct atom_selection atom_selection;
struct atom_selection
{
  int selected;
  int * selected_bonds;
  ColRGBA * bond_colors;
  int * selected_angles;
  ColRGBA * angle_colors;
  int * selected_dihedrals;
  ColRGBA * dihedral_colors;
  atom_in_selection * first;
  atom_in_selection * last;
};

/*! \typedef background

  \brief a background data structure
*/
typedef struct background background;
struct background
{
  int gradient;   /*!< 0 = none \n
                       1 = linear \n
                       2 = circular */
  int direction;  /*!< If linear: \n
                         0 = right to left \n
                         1 = top to bottom \n
                         2 = bottom right to top left \n
                         3 = top right to bottom left \n
                         4 = center \n
                       If circular: \n
                         0 = right to left \n
                         1 = left to right \n
                         2 = top to bottom \n
                         3 = bottom to top \n
                         4 = bottom right to top left \n
                         5 = bottom left to top right \n
                         6 = top right to bottom left \n
                         7 = top left to bottom right \n
                         8 = center  */
  float position; /*!< Equal mixed position */
  ColRGBA color;
  ColRGBA gradient_color[2];
};

/*! \typedef axis

  \brief axis layout data structure
*/
typedef struct axis axis;
struct axis
{
  int axis;              /*!< 0 = NONE (hide), 1 = wireframe, 4 = cylinders */
  double line;           /*!< Width for wireframe */
  double rad;            /*!< Radius for cylinders */
  double length;         /*!< Axis length */
  int t_pos;             /*!< Axis template position */
  GLfloat c_pos[3];      /*!< Axis custom positions */
  int labels;            /*!< Show / hide axis labels */
  gchar * title[3];      /*!< Axis titles */
  ColRGBA * color;       /*!< Associated color */
};

/*! \typedef box

  \brief box layout data structure
*/
typedef struct box box;
struct box
{
  int box;             /*!< 0 = NONE (hide), 1 = wireframe, 4 = cylinders */
  double line;         /*!< Width for wireframe */
  double rad;          /*!< Radius for cylinders */
  ColRGBA color;       /*!< Associated color */
  int extra_cell[3];   /*!< Extra cells (if any) on x, y and z */
};

/*! \typedef image

  \brief a structure to describe the visual content of the OpenGL rendering
*/
typedef struct image image;
struct image
{
  background * back;                            /*!< Background data structure */
  axis * xyz;                                   /*!< Axis layout data structure */
  box * abc;                                    /*!< Box layout data structure */

  // Color maps for atoms [0] and polyhedra [1]
  int color_map[2];                             /*!< Color maps, 0= atoms, 1 = polyhedra */

  gboolean * show_atom[2];                      /*!< Daw or not the atom(s), 0 = atoms, 1 = clones */
  gboolean * show_label[2];                     /*!< Draw or not the label(s), 0 = atoms, 1 = clones */
  gboolean * show_poly[9];                      /*!< Draw or not the polyhedra */
  gboolean * show_coord[10];                    /*!< Draw or not the coordination(s) */
  // Atoms
  ColRGBA * at_color;                           /*!< Atomic species color(s) */
  double * sphererad;                           /*!< Sphere radii (spheres or ball and stick used as style) */
  double * pointrad;                            /*!< Point size (points or wireframe used as style) */
  double * atomicrad;                           /*!< Sphere radii (spacefilled used as style) */

  // Bonds
  double ** bondrad;                            /*!< Cylinder radii (ball and stick used as style) */
  double ** linerad;                            /*!< Line width (wireframe used as style)  */
  double radall[2];                             /*!< Cylinder radii (cylinders used as style) */

  // Clones
  gboolean draw_clones;                         /*!< Draw clones: 0 = no, 1 = yes */
  gboolean cloned_poly;                         /*!< Draw cloned ployhedra: 0 = no, 1 = yes */

  // Labels
  // 0 = atoms, 1 = clones, 2 = axis, 3 = measures, 4 = measure edition
  screen_label labels[5];                       /*!< Labels: \n
                                                    0 = atom(s), \n
                                                    1 = clone(s), \n
                                                    2 = axis, \n
                                                    3 = measure(s), \n
                                                    4 = measure(s) in edition mode */
  // 0 = Element name, 1 = Atomic symbol, 2 = Atomic symbol + ID number, 3 = ID number
  int acl_format[2];                            /*!< Label format for the atom(s) and clone(s): \n
                                                    0 = element name, \n
                                                    1 = atomic symbol, \n
                                                    2 = atomic symbol + ID number (default), \n
                                                    3 = ID number */
  // In the following: 0 = measure in analysis mode, 1 = measure in edition mode
  gboolean mtilt[2];                             /*!< Measure tilt, if any */
  int mpattern[2];                               /*!< Measure line pattern */
  int mfactor[2];                                /*!< Measure  */
  double mwidth[2];                              /*!< Measure line width */

  int m_is_pressed;                             /*!< is the key m pressed ? */
  // Atom selection: 0 = normal mode, 1 = edition mode
  atom_selection * selected[2];          /*!< atom(s) selection lists \n
                                                    0 = analysis mode, \n
                                                    1 = edition mode */
  // Colors for atom selections
  ColRGBA sel_color[2];

 // Coordination(s)
  ColRGBA ** spcolor[10];                       /*!<  Coordination sphere colors */

  GLdouble p_depth;                             /*!< Camera depth */
  GLdouble m_depth;                             /*!< Maximum camera depth */
  GLdouble c_angle[2];                          /*!< Camera angle: pitch and heading */
  GLdouble c_shift[2];                          /*!< Camera position: x and y */
  GLdouble gnear;                               /*!< Near plane position */
  GLdouble gfar;                                /*!< Far plane position */
  GLdouble gleft;                               /*!< Left plane position */
  GLdouble gright;                              /*!< Right plane position  */
  GLdouble gtop;                                /*!< Top plane position  */
  GLdouble gbottom;                             /*!< Bottom plane position  */
  GLdouble zoom;                                /*!< Zoom factor */

  vec4_t rotation_quaternion;                   /*!< Rotation quaternion */

  // Only for recording
  int ** i_rings[5];                            /*!< Ring(s) visual information, temporary buffer for movie encoding */
  atom_data * at_data;                          /*!< Atom visual information, temporary buffer for movie encoding */

  // Volumes
  gboolean show_vol[FILLED_STYLES];             /*!< Show (1) or hide (0) overall molecular volumes, calculated using: \n
                                                   0 = covalent radii, \n
                                                   1 = ionic radii, \n
                                                   2 = van Der Waals radii, \n
                                                   3 = in crystal radii */
  ColRGBA vol_col[FILLED_STYLES];               /*!< Overall molecular volume colors, calculated using: \n
                                                   0 = covalent radii, \n
                                                   1 = ionic radii, \n
                                                   2 = van Der Waals radii, \n
                                                   3 = in crystal radii */
  gboolean * fm_show_vol[2][FILLED_STYLES];     /*!< Show (1) or hide (0) isolated fragment(s) and molecule(s) volumes, calculated using: \n
                                                   0 = covalent radii, \n
                                                   1 = ionic radii, \n
                                                   2 = van Der Waals radii, \n
                                                   3 = in crystal radii */
  ColRGBA * fm_vol_col[2][FILLED_STYLES];       /*!< Isolated fragment(s) and molecule(s) volume colors: \n
                                                   0 = covalent radii, \n
                                                   1 = ionic radii, \n
                                                   2 = van Der Waals radii, \n
                                                   3 = in crystal radii */

  int rotation_mode;                            /*!< Not used anymore, should be removed */

  int style;                                    /*!< Default style, in: \n
                                                    0 = ball and stick, \n
                                                    1 = wireframe, \n
                                                    2 = spacefilled, \n
                                                    2 = spheres, \n
                                                    3 = cylinders, \n
                                                    4 = dots */
  int filled_type;                              /*!< Spacefilled type, in: \n
                                                    0 = covalent radii, \n
                                                    1 = ionic radii, \n
                                                    2 = van Der Waals radii, \n
                                                    3 = in crystal radii */
  GLint quality;                                /*!< Quality of the rendering */
  GLint render;                                 /*!< OpenGL render type, in \n
                                                    0 = filled (default), \n
                                                    1 = lines, \n
                                                    2 = points */
  int * light_loc;                              /*!< Lights locations (only used when drawing light spots), in \n
                                                    0 = ambient light, not in the model, \n
                                                    1 = spot or directional light, in the model */
  Lightning l_ghtning;                          /*!< Lightning description */
  Material m_terial;                            /*!< Material description, if any */
  Fog f_g;                                      /*!< Fog description, if any*/

  int step;                                     /*!< The MD step, in case of trajectory */
  int rep;                                      /*!< Representation: 0 = orthographic, 1 = perspective */
  int id;                                       /*!< Image ID */
};

/*! \typedef snapshot */
typedef struct snapshot snapshot;
struct snapshot
{
  image * img;               /*!< The image structure for this snapshot */
  snapshot * prev;
  snapshot * next;
};

/*! \typedef animation */
typedef struct animation animation;
struct animation
{
  int frames;                /*!< Number of frames or snapshots */
  snapshot * first;
  snapshot * last;
};

/*! \typedef colormap */
typedef struct colormap colormap;
struct colormap
{
  float ** data;              /*!< Data to use as custom color map, value per MD step x atom */
  int points;                 /*!< Number of color(s) to build the custom color map */
  ColRGBA ** colors;          /*!< Color(s) to use to build the custom color map */
  float * positions;          /*!< Point positions, in the overall value range */
  float cmin;                 /*!< Minimum value */
  float cmax;                 /*!< Maximum value */
  ColRGBA * values;           /*!< The atom(s) colors calculated using the custom color map information */
};

/*! \typedef atom_search

  \brief a data structure to search for atom(s) and edit the model
*/
typedef struct atom_search atom_search;
struct atom_search
{
  int proj;                        /*!< Target project */
  int action;                      /*!< Action to be performed */
  GtkWidget * atom_tree;
  GtkWidget * mode_box;
  GtkWidget * object_box;
  GtkWidget * filter_box;
  GtkWidget * atom_box;
  GtkWidget * id_box;
  GtkWidget * big_box;
  GtkTreeStore * atom_model;
  GtkListStore * obj_model;
  GtkTreePath * path;
  GtkWidget * info[2];
  GtkWidget * preview[2];
  // 0 = Unselected
  // 1 = Selected
  // 2 = All
  int status;                      /*!< Atom(s) status for the search, in: \n
                                        0 = unselected, \n
                                        1 = selected, \n
                                        2 = all */
  // O = Normal
  // 1 = Random
  int mode;                        /*!< Search mode, in: \n
                                        0 = normal, \n
                                        1 = random */
  // 0 = Atoms
  // 1 = Groups
  int object;                      /*!< Search object, in: \n
                                        0 = isolated atom(s), \n
                                        1 = group of atoms */
  int filter;                      /*!< Search filter, in: \n
                                        0 = chemical species, \n
                                        1 = total coordination(s), \n
                                        2 = partial coordination(s), \n
                                        3 = fragment(s), \n
                                        4 = molecule(s) */
  int spec;                        /*!< Target chemical species, if any */
  int search_digit;
  int spec_to_add;
  int num_to_add;
  int in_selection;
  GtkWidget * entry_a, * entry_b;
  GtkWidget * but_a, * but_b;
  GtkWidget * img_a, * img_b;
  // Passivating = for all atoms of that type but not as a group
  gboolean passivating;
  gboolean was_selected;
  gboolean recompute_bonding;
  int set_for_all;
  int int_b;
  int * lab;
  int * pick;
  int todo_size;
  int * todo;
  tint pointer[6];
};

/*! \typedef atomic_object

  \brief a data structure to describe an object to work on. \n
  the object can be one or more: \n
    - atom(s) \n
    - chemical species \n
    - coordination sphere(s) (total or partial) \n
    - fragment(s) \n
    - molecule(s) \n
    - selection(s) of atom(s) from any project in the workspace
*/
typedef struct atomic_object atomic_object;
struct atomic_object
{
  int id;                       /*!< Object ID */
  int origin;                   /*!< Origin project for the object */
  float dim;                    /*!< Maximum size in x, y or z for the object */
  gchar * name;                 /*!< Name of the object */
  int type;                     /*!< Type of object */
  int atoms;                    /*!< Number of atom(s) */
  int species;                  /*!< Number of chemical species */
  int * old_z;                  /*!< Temporary buffer to preserve the atomic numbers */
  struct atom * at_list;        /*!< List of atom(s) in the object */
  int ifcl;                     /*!< Number of clone(s), if any */
  int * bcid;                   /*!< Cloned bonds ID */
  double occ;                   /*!< Occupancy (for crystal building purposes) */
  double * baryc;               /*!< Position barycenter for the object */
  int bonds;                    /*!< Number of chemical bonds */
  int ** ibonds;                /*!< List of bonds */
  coord_info * coord;           /*!< coordination information */
  atomic_object * prev;
  atomic_object * next;
};

/*! \typedef wyckoff_position

  \brief a structure to describe a Wyckoff position in crystallography
*/
typedef struct wyckoff_position wyckoff_position;
struct wyckoff_position
{
  int multi;           /*!< Multiplicity */
  gchar * let;         /*!< Letter */
  gchar * site;        /*!< Symmetry */
  gchar *** pos;       /*!< Coordinate(s) */
};

/*! \typedef spg_setting

  \brief a structure to describe the settings of a space group in crystallography
*/
typedef struct spg_setting spg_setting;
struct spg_setting
{
  int origin;          /*!< Origin */
  gchar * name;        /*!< Name of the setting */
  gchar * pos[3];      /*!< Lattice orientation modification(s) */
  int nump;            /*!< Number of Wyckoff position modification(s) */
  gchar *** points;    /*!< Wyckoff position modification(s) */
};

/*! \typedef space_group

  \brief a structure to described a space group in crystallography
*/
typedef struct space_group space_group;
struct space_group
{
  int id;                         /*!< ID number, in [1-230] */
  gchar * name;                   /*!< Name */
  gchar * hms;                    /*!< Hermann-Mauguin symbol */
  gchar * bravais;                /*!< Type of Bravais lattice */
  gchar * setting;                /*!< Available setting(s), if any*/
  int sid;                        /*!< Selected setting, if any */
  int nums;                       /*!< Number of possible setting(s) */
  spg_setting * settings;         /*!< Space group settings */
  int numw;                       /*!< Number of Wyckoff position(s) */
  wyckoff_position * wyckoff;     /*!< Wyckoff position(s) */
  mat4_t coord_origin;            /*!< Origin of the atomic coordinates */
  mat4_t wyck_origin;             /*!< Origin of the Wyckoff positions */
};

/*! \typedef box_info

  \brief model box information
*/
typedef struct box_info box_info;
struct box_info
{
  double param[2][3];     /*!< Box parameters: \n
                               0: a, b, c \n
                               1: alpha, beta, gamma */
  double vect[3][3];      /*!< Box vectors */
  double rvect[3][3];     /*!< Reciprocal vectors */
  mat4_t frac_to_cart;    /*!< Fractional to Cartesian matrix */
  mat4_t cart_to_frac;    /*!< Cartesian to fractional matrix */
  double vol;             /*!< Volume */
  double dens;            /*!< Density */
};

/*! \typedef cell_info

  \brief Description of the periodicity
*/
typedef struct cell_info cell_info;
struct cell_info
{
  box_info * box;         /*!< Model box description \n
                               In the case of NPT calculation as many boxes as MD steps are described */
  int cextra[3];          /*!< Extra boxes (if any), on x, y and z*/
  int pbc;                /*!< Apply PBC */
  int frac;               /*!< Are the initial coordinates fractional ? */
  int ltype;              /*!< Lattice type, in \n
                               0 = isolated, \n
                               1 = a,b,c + angles, \n
                               2 = vectors */
  double volume;          /*!< Volume, average if NPT */
  double density;         /*!< Density, average if NPT */
  gboolean npt;           /*!< NPT trajectory (0 = no, 1 = yes) */
  gboolean has_a_box;     /*!< Is there a model box ?  (0 = no, 1 = yes) */
  gboolean crystal;       /*!< Is this a crystal ?  (0 = no, 1 = yes) */
  space_group * sp_group; /*!< Space group in the case of a crystal */
};

typedef struct builder_edition builder_edition;
struct builder_edition
{
  GtkWidget * win;
  GtkWidget * bh_box;
  GtkWidget * bv_box[2];
  GtkWidget * cs_combo;
  GtkWidget * bl_box;
  GtkWidget * ltc_box;
  GtkWidget * ltc_cons;
  GtkWidget * bv_img;
  GtkWidget * bl_combo;
  GtkWidget * sg_box;
  GtkWidget * sg_combo;
  GtkWidget * sg_but;
  GtkWidget * so_box;
  GtkWidget * so_combo;
  GtkWidget * so_info;
  GtkWidget * la_combo;
  GtkWidget * lattice_grid;
  GtkWidget * lattice_box;
  GtkWidget * bentry[2][3];
  GtkWidget * ventry[3][3];
  GtkWidget * add_combo;
  GtkWidget * pbut;
  cell_info cell;
  int occupancy;
  int rounding;
  gboolean overlapping;
  gboolean wrap;
  gboolean clones;
  int extrac[3];
  tint pointers[3][3];
};

typedef struct atom_edition atom_edition;
struct atom_edition
{
  GtkWidget * win;
  GtkWidget * vbox;
  GtkWidget * notebook;
  GtkWidget * atom_combo[5];
  GtkWidget * axis_combo[2];
  GtkWidget * axis_but[2];
  GtkWidget * edit_entry[6];
  GtkWidget * edit_scale[6];
  GtkWidget * at_expand[3];
  GtkWidget * edition_but[2];
  GtkTreePath ** replace_nodes[2];
  gboolean visible;
  int active;
  int axis[2];
  gboolean show_axis[2];
  gboolean rebuilt[2];
  gboolean old_axis;
  float new_param[3][2][6];
  float old_param[3][2][6];
  int repeat_move;
  float * msd;
  float * msd_all;
  // 0 = REPLACE
  // 1 = INSERT
  // 2 = CBUILD
  // 3 = PASSIVATE
  atomic_object * to_be_inserted[4];
  atomic_object * to_be_moved[2];
  int adv_bonding[2];
  coord_info * coord;
  int add_spec;
  double * new_z;
};

typedef struct cell_edition cell_edition;
struct cell_edition
{
  GtkWidget * win;
  GtkWidget * notebook;
  GtkWidget * put_in_box;
  GtkWidget * passivate;
  GtkWidget * superbut;
  GtkWidget * shift_box[2];
  GtkWidget * ax_cell[3];
  GtkWidget * edit_entry[21];
  GtkWidget * edit_scale[21];
  GtkWidget * density_box;
  GtkWidget * density;
  gboolean slab_show;
  gboolean slab_pbc;
  gboolean cut_this_slab;
  gboolean slab_passivate;
  int slab_type;
  int slab_act;
  int slab_out;
  double slab_vol;
  float slab_alpha;
  int slab_atoms;
  int * slab_lot;
  GtkWidget * slab_opts;
  GtkWidget * slab_hbox[3];
  GtkWidget * slab_param[3];
  GtkWidget * slab_box[6];
  GtkWidget * slab_info_box;
  GtkWidget * slab_info;
  gboolean homo_density;
  double cparam[21];
  double initbox[3];
  dint slab_pointer[5];
};

typedef struct opengl_edition opengl_edition;
struct opengl_edition
{
  GtkWidget * win;
  GtkWidget * templates;
  GtkWidget * param_mat;
  GtkWidget * m_entry[5];
  GtkWidget * m_scale[5];
  GtkWidget * lights;
  GtkWidget * lights_box;

  GtkWidget * basic[2];
  GtkWidget * base_ogl[2][5];
  GtkWidget * entogl[5][3];

  GtkWidget * advanced_light_box;
  GtkWidget * light_type;
  GtkWidget * light_type_box;
  GtkWidget * light_fix;
  GtkWidget * light_show;
  GtkWidget * light_b_entry[2];
  GtkWidget * light_entry[6];
  GtkWidget * light_b_coord[2];

  GtkWidget * dens_box;
  GtkWidget * depth_box;
  GtkWidget * fog_range[3];
  GtkWidget * param_fog;

  int proj;
  dint pointer[6];
  tint pos_pointer[5][3];
};

typedef struct coord_edition coord_edition;
struct coord_edition
{
  GtkWidget * win;
  GtkWidget * notebook;
  GtkTreeStore * rings_model[5];
  GtkTreeStore * chains_model;
  GtkListStore * frag_mol_model[2];
  int rst[5], rsz[5], ri[5];
  GtkWidget * rilab[5][3];
  int cst, csz, ch;
  GtkWidget * chlab[3];
};

typedef struct measures measures;
struct measures
{
  GtkWidget * win;
  GtkWidget * label;
  GtkWidget * selection_tree[3];
};

typedef struct volumes volumes;
struct volumes
{
  GtkWidget * win;
  // All model
  GtkWidget * compb[FILLED_STYLES];
  GtkWidget * hboxv[FILLED_STYLES];
  GtkWidget * hbvol[FILLED_STYLES];
  GtkWidget * lab_vol[FILLED_STYLES];
  double angp;
  // Fragments and molecules
  int sid[2];
  GtkWidget ** fm_compb[2][FILLED_STYLES];
  GtkWidget ** fm_hboxv[2][FILLED_STYLES];
  GtkWidget ** fm_hbvol[2][FILLED_STYLES];
  GtkWidget ** fm_lab_vol[2][FILLED_STYLES];
  GtkWidget * fm_vbox[2];
  GtkWidget * fm_vvbox[2];
  int ngeov[2];
  int * geov_id[2];
};

typedef struct model_edition model_edition;
struct model_edition
{
  GtkWidget * win;
  GtkWidget * notebook;
};

typedef struct box_edition box_edition;
struct box_edition
{
  GtkWidget * win;
  GtkWidget * show_hide;
  GtkWidget * styles;
  GtkWidget * width_box;
  GtkWidget * width;
  GtkWidget * radius_box;
  GtkWidget * radius;
  GtkWidget * box_data;
};

typedef struct axis_edition axis_edition;
struct axis_edition
{
  GtkWidget * win;
  GtkWidget * show_hide;
  GtkWidget * styles;
  GtkWidget * width_box;
  GtkWidget * width;
  GtkWidget * radius_box;
  GtkWidget * radius;
  GtkWidget * length;
  GtkWidget * axis_data;
  GtkWidget * templates;
  GtkWidget * axis_position_box;
  GtkWidget * axis_label_box[2];
  GtkWidget * axis_color_title[3];
};

typedef struct rep_edition rep_edition;
struct rep_edition
{
  GtkWidget * win;
  GtkWidget * camera_widg[7];
};

typedef struct gradient_edition gradient_edition;
struct gradient_edition
{
  GtkWidget * win;
  GtkWidget * g_box;
  GtkWidget * dir;
  GtkWidget * d_box[2];
  GtkWidget * color_box[2];
  GtkWidget * p_box;
  GtkWidget * p_scale;
  GtkWidget * col_but;
  GtkWidget * grad_but[2];
};

/*! \typedef glwin

  \brief OpenGL window widget structure */
typedef struct glwin glwin;
struct glwin
{
  gboolean init;                                  /*!< Was rendering initialized (0 = no, 1 = yes) */
  int proj;                                       /*!< Target project */
  // The entire OpenGL window
  GtkWidget * win;
  // The menu bar
  GtkWidget * menu_box;
  GtkWidget * menu_bar;
#ifdef GTK3
  // OpenGL menus
  GtkWidget * ogl_styles[OGL_STYLES];
  GtkWidget * filled_styles[FILLED_STYLES];
  GtkWidget * color_styles[ATOM_MAPS+POLY_MAPS];
  GtkWidget * ogl_render[OGL_RENDERS];
  GtkWidget * ogl_rep[OGL_REPS];
  GtkWidget * ogl_quality;
  GtkWidget ** ogl_box_axis[2];
  GtkWidget * ogl_box[8];
  GtkWidget * ogl_mouse[3];
  GtkWidget * ogl_atoms[8];
  GtkWidget ** ogl_spec[2];
  GtkWidget ** ogl_lab[2];
  GtkWidget * ogl_bonds[14];
  GtkWidget * ogl_clones[6];
  GtkWidget * ogl_coord[OGL_COORDS];
  GtkWidget * ogl_rings[OGL_RINGS];
  GtkWidget * ogl_chains[2];
  GtkWidget ** ogl_geom[2][10];
  GtkWidget ** ogl_poly[2][9];
  GtkWidget ** oglmv[2][10];                 // Coordination sphere show/hide menus
  GtkWidget ** oglmc[2][9];                  // Coordination sphere color menus
  GtkWidget ** oglmpv[2][9];                 // Polyhedra view/hide menus
  GtkWidget * ogl_mode[2+2*NINPUTS];
  GtkWidget * ogl_smode[NSELECTION];
  GtkWidget * ogl_anim[2];
#endif
  GtkWidget * rbuild[2];
  GtkWidget * cbuilder;

  // Matrices
  vec3_t model_position;
  vec4_t view_port;
  mat4_t projection_matrix;
  mat4_t model_matrix;
  mat4_t view_matrix;
  mat4_t un_view_matrix;
  mat4_t model_view_matrix;
  mat4_t view_model_matrix;
  mat4_t proj_model_matrix;
  mat4_t proj_model_view_matrix;
  mat4_t proj_view_matrix;
  mat4_t normal_matrix;
  mat4_t axis_proj_model_view_matrix;
  mat4_t label_projection_matrix;

  gboolean create_shaders[NGLOBAL_SHADERS];
  glsl_program *** ogl_glsl[NGLOBAL_SHADERS];
  int * n_shaders[NGLOBAL_SHADERS];
  opengl_edition * opengl_win;
  model_edition * model_win[2];
  builder_edition * builder_win;
  box_edition * box_win;
  axis_edition * axis_win;
  rep_edition * rep_win;
  gradient_edition * gradient_win;

  // OpenGL plot
  GtkWidget * plot;
#ifndef GTKGLAREA
  GLXContext glcontext;
#endif
  int pixels[2];

  int mouseX;
  int mouseY;
  int mouseButton;
  int mouseAction;
  int mouseStatus;
  int nth_copy;
  gboolean to_pick;

  // Action mode
  // 0 = analyze (normal), 1 = edit structure
  int mode;                                  /*!< Mouse mode, in: \n
                                                  0 = analyze (default), \n
                                                  1 = edition (default for new project) */
  // Selection mode
  int selection_mode;                        /*!< Mouse selection mode, in: \n
                                                  0 = atom / bond, \n
                                                  1 = total coordination sphere, \n
                                                  2 = partial coordination sphere, \n
                                                  3 = fragment, \n
                                                  4 = molecule, \n
                                                  5 = single fragment, \n
                                                  6 = single molecule, \n
                                                  7 = selection in edition mode */

  // For temporary backup purposes only:
  atom_selection * tmp_sel[2];
  int * stored_labels[2];

  int other_status;
  coord_edition * coord_win;
  int cmap[ATOM_MAPS];
  atom_edition * atom_win;
  double ** saved_coord[3];
  cell_edition * cell_win;
  // 0 = atoms
  // 1 = clones
  // 2 = MOVE
  // 3 = REPLACE
  // 4 = REMOVE
  // 5 = INSERT
  // 6 = RANMOVE
  // 7 = CBUILDER
  // 8 = PASSIVATE
  atom_search * search_widg[9];
  vec3_t baryc[3];
  vec3_t insert_coords;
  // 0 = Status
  // 1 = Translation / Rotation
  // 3 = x_t, y_t, z_t, x_r, y_r, z_r
  float edition_param[3][2][6]; // 0-2 translation, 3-5 rotation

  // Color pointers, these are used to deal with the
  // Callbacks for the OpenGL window menus, and are used
  // for atoms / box / axis ...
  tint * colorp[64];                        /*!< Color pointers \n
                                               these are used to deal with the Callbacks for the OpenGL window, \n
                                               and are used for atoms / box / axis ... */

  colormap * custom_map;                    /*!< User defined color map, if any */

  int allbonds[2];                          /*!< Total number of chemical bond(s): \n
                                                 0 = normal bond(s), \n
                                                 1 = cloned bonds */
  int ** bonds;                             /*!< Number of bond(s) by MD step, then 0/1=normal/cloned */
  int **** bondid;                          /*!< Atoms ID in bonds, by MD step, then 0/1=norm/clones, then bond ID */

  // Clones
  vec3_t ** clones;                         /*!< List of cloned atomic coordiantes */

  gboolean prepare_motion;
  gboolean was_moved;
  // Rebuild trigger switch on edition
  // [0] = on move:
  //    [0] atom_win active: 0/1 to turn off/on
  //    [1] atom_win inactive: 0/1 to turn off/on
  // [1] = on copy:
  //    [0] atom_win active: 0/1 to turn off/on
  //    [1] atom_win inactive: 0/1 to turn off/on
  gboolean rebuild[2][2];                   /*!< Rebuild trigger switch on edition: \n
                                                 [0] = on move: \n
                                                     [0] atom_win active: 0/1 to turn off/on \n
                                                     [1] atom_win inactive: 0/1 to turn off/on \n
                                                  [1] = on copy: \n
                                                     [0] atom_win active: 0/1 to turn off/on \n
                                                     [1] atom_win inactive: 0/1 to turn off/on */
  gboolean bonding;
  gboolean adv_bonding[2];                   /*!< Is advanced bonding information available, \n
                                                  0 = Fragments, \n
                                                  1 = Molecules */
  qint ** gcid[10];                          /*!< Geom colors pointers */
  gboolean rings;
  int ring_max[5];                           /*!< The largest ring size */
  int ** num_rings[5];                       /*!< The number of rings (search type, step, ring size) */
  int **** all_rings[5];                     /*!< The ring(s) of atoms */
  gboolean *** show_rpoly[5];                /*!< Show polyhedra of selected rings */
  gboolean chains;
  int chain_max;                             /*!< The largest chain size */
  int ** num_chains;                         /*!< The number of chains (chain size, step) */
  int **** all_chains;                       /*!< The chain(s) of atoms */

  // Volumes data
  gboolean volumes;
  double * atoms_volume[FILLED_STYLES];
  double * atoms_ppvolume[FILLED_STYLES];
  // [9] : a,b,c,alpha,beta,gama,p.x,p.y,p.z
  double ** volume_box[FILLED_STYLES];
  gboolean comp_vol[FILLED_STYLES];
  double ** frag_mol_volume[2][FILLED_STYLES];
  double ** frag_mol_ppvolume[2][FILLED_STYLES];
  double *** frag_box[FILLED_STYLES];
  gboolean ** fm_comp_vol[2][FILLED_STYLES];

  int labelled;
  int picked;

  // Color picking
  int to_be_picked;                          /*!< Total number of objects that can be picked */
  int atoms_to_be_picked;                    /*!< Number of atoms that can be picked */
  int clones_to_be_picked;                   /*!< Number of clones that can be picked */
  int bonds_to_be_picked;                    /*!< Number of bonds that can be picked (do not include clones) */
  int * color_to_pick;                       /*!< The different colors that can be picked */

  // Spinner, player
  sequencer * player;
  gboolean play;
  gboolean stop;
  gboolean loop;
  int speed;
  spinner * spiner;
  gboolean spin[4];
  int spin_speed[4];
  recorder * rec;
  gboolean record;

  int frames;
  animation * anim;

  double cshift[3];
  gboolean wrapped;

  measures * measure_win;
  volumes * volume_win;

  float zoom_factor;
  GLdouble p_moy;
  gboolean fullscreen;

  int action_id;
  GSimpleActionGroup * action_group;
  GtkWidget * shortcuts;
};

#endif
