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
* @file ogl_draw.c
* @short OpenGL window drawing funcions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'ogl_draw.c'
*
* Contains:
*

  - The OpenGL window drawing funcions

*
* List of functions:

  void print_matrices ();
  void setup_camera ();
  void unrotate_camera ();
  void duplicate_fog (Fog * new_fog, Fog * old_fog);
  void duplicate_material (Material * new_mat, Material * old_mat);
  void duplicate_material_and_lightning (image * new_img, image * old_img);
  void duplicate_screen_label (screen_label * new_lab, screen_label * old_lab);
  void add_image ();
  void at_shift (atom * at, float * shift);
  void at_unshift (atom * at, float * shift);
  void draw (glwin * view);

  screen_string * duplicate_screen_string (screen_string * old_s);
  atom * duplicate_atom (atom * at);

  image * duplicate_image (image * old_img);

*/

#include "global.h"
#include "glview.h"
#include "dlp_field.h"
#include "preferences.h"
#include <time.h>

extern ColRGBA init_color (int id, int numid);
extern Light * copy_light_sources (int dima, int dimb, Light * old_sp);
extern atom_selection * duplicate_ogl_selection (atom_selection * old_sel);
double x, y, z;
GLUquadricObj * quadric;
glwin * wingl;
project * proj_gl;
int proj_sp;
int proj_at;
coord_info * coord_gl;
box_info * box_gl;
cell_info * cell_gl;
image * plot;
int qual;
int acolorm;
int pcolorm;
int step;

extern int nbs, nbl;
extern void create_atom_lists (gboolean to_pick);
extern int create_bond_lists (gboolean to_pick);
extern int create_selection_lists ();
extern void create_poly_lists ();
extern void create_ring_lists ();
extern int create_box_lists (int b_step);
extern int create_axis_lists ();
extern int create_pick_lists ();
extern int create_label_lists ();
extern void create_measures_lists ();
extern void create_light_lists ();
extern void create_slab_lists (project * this_proj);
extern void create_volumes_lists ();
extern void create_background_lists ();

/*!
  \fn void print_matrices ()

  \brief print OpenGL matrices
*/
void print_matrices ()
{
  g_debug ("*** Print Matrices ***");
  g_debug ("\n");
  g_debug (":: MODEL_MATRIX");
  m4_print (wingl -> model_matrix);
  g_debug ("\n");
  g_debug (":: VIEW_MATRIX");
  m4_print (wingl -> view_matrix);
  g_debug ("\n");
  g_debug (":: MODEL_VIEW_MATRIX");
  m4_print (wingl -> model_view_matrix);
  g_debug ("\n");
  g_debug (":: NORMAL_MATRIX");
  m4_print (wingl -> normal_matrix);
  g_debug ("\n");
  g_debug (":: PROJECTION_MATRIX");
  m4_print (wingl -> projection_matrix);
  g_debug ("\n");
  g_debug (":: PROJECTION_MODEL_VIEW_MATRIX");
  m4_print (wingl -> proj_model_view_matrix);
}

/*!
  \fn void setup_camera ()

  \brief setup OpenGL camera
*/
void setup_camera ()
{
  wingl -> model_position         = vec3 (0.0, 0.0, -plot -> p_depth);
  wingl -> model_matrix           = m4_translation (wingl -> model_position);
  wingl -> proj_model_matrix      = m4_mul (wingl -> projection_matrix, wingl -> model_matrix);
  wingl -> view_matrix            = m4_quat_rotation (plot -> rotation_quaternion);
  vec4_t quat;
  quat = plot -> rotation_quaternion;
  quat.z = - quat.z;
  wingl -> un_view_matrix         = m4_quat_rotation (quat);
  wingl -> model_view_matrix      = m4_mul (wingl -> model_matrix, wingl -> view_matrix);
  wingl -> normal_matrix          = m4_transpose (m4_invert_affine(wingl -> model_matrix));
  wingl -> proj_model_view_matrix = m4_mul (wingl -> projection_matrix, wingl -> model_view_matrix);
  wingl -> proj_view_matrix       = m4_mul (wingl -> projection_matrix, wingl -> view_matrix);
  wingl -> view_model_matrix      = m4_mul (wingl -> view_matrix, wingl -> model_matrix);
  // print_matrices();
}

/*!
  \fn void unrotate_camera ()

  \brief unrotate OpenGL camera
*/
void unrotate_camera ()
{
  vec4_t quat;
  quat = plot -> rotation_quaternion;
  quat.z = - quat.z;
  wingl -> model_view_matrix = m4_mul (wingl -> model_view_matrix, m4_quat_rotation (quat));
}

/*!
  \fn screen_string * duplicate_screen_string (screen_string * old_s)

  \brief create a copy a screen_string data structure

  \param old_s the data structure to be copied
*/
screen_string * duplicate_screen_string (screen_string * old_s)
{
  screen_string * new_s = g_malloc0 (sizeof*new_s);
  new_s -> word = g_strdup_printf ("%s", old_s -> word);
  new_s -> col = old_s -> col;
  int i;
  for (i=0; i<3; i++) new_s -> shift[i] = old_s -> shift[i];
  new_s -> num_instances = old_s -> num_instances;
  new_s -> instances = duplicate_float (old_s -> num_instances*4, old_s -> instances);
  new_s -> prev = NULL;
  new_s -> last = NULL;
  return new_s;
}

/*!
  \fn void duplicate_material (Material * new_mat, Material * old_mat)

  \brief duplicate material data

  \param new_mat the new material data
  \param old_mat the old material data to be copied

*/
void duplicate_material (Material * new_mat, Material * old_mat)
{
  new_mat -> predefine = old_mat -> predefine;
  new_mat -> albedo = old_mat -> albedo;
  int i;
  for (i=0; i<6; i++) new_mat -> param[i] = old_mat -> param[i];
}

/*!
  \fn void duplicate_fog (Fog * new_fog, Fog * old_fog);

  \brief duplicate fog data

  \param new_fog the new fog data
  \param old_fog the old fog data to be copied

*/
void duplicate_fog (Fog * new_fog, Fog * old_fog)
{
  new_fog -> mode = old_fog -> mode;
  new_fog -> based = old_fog -> based;
  new_fog -> density = old_fog -> density;
  int i;
  for (i=0; i<2; i++) new_fog -> depth[i] = old_fog -> depth[i];
  new_fog -> color = old_fog -> color;
}

/*!
  \fn void duplicate_material_and_lightning (image * new_img, image * old_img)

  \brief copy the material and lightning parameters of an image data structure

  \param new_img the new image
  \param old_img the old image with the data to be copied
*/
void duplicate_material_and_lightning (image * new_img, image * old_img)
{
  new_img -> quality = old_img -> quality;
  new_img -> render = old_img -> render;
  duplicate_material (& new_img -> m_terial, & old_img -> m_terial);
  new_img -> l_ghtning.lights = old_img -> l_ghtning.lights;
  new_img -> l_ghtning.spot = copy_light_sources (old_img -> l_ghtning.lights, old_img -> l_ghtning.lights, old_img -> l_ghtning.spot);
  duplicate_fog (& new_img -> f_g, & old_img -> f_g);
}

/*!
  \fn void duplicate_screen_label (screen_label * new_lab, screen_label * old_lab)

  \brief copy screen_label data structure

  \param new_lab the new screen_label structure
  \param old_lab the old screen_label structure to be copied
*/
void duplicate_screen_label (screen_label * new_lab, screen_label * old_lab)
{
  screen_string * stmp_a, * stmp_b;

  new_lab -> position = old_lab -> position;
  new_lab -> render = old_lab -> render;
  new_lab -> scale = old_lab -> scale;
  new_lab -> font = g_strdup_printf ("%s", old_lab -> font);
  new_lab -> color = NULL;
  int i;
  for (i=0; i<3; i++) new_lab -> shift[i] = old_lab -> shift[i];
  new_lab -> n_colors = old_lab -> n_colors;
  if (old_lab -> color != NULL)
  {
    new_lab -> color = duplicate_color (new_lab -> n_colors, old_lab -> color);
  }
  new_lab -> list = NULL;
  if (old_lab -> list != NULL)
  {
    new_lab -> list = duplicate_screen_string (old_lab -> list);
    new_lab -> list -> last = duplicate_screen_string (old_lab -> list -> last);
    stmp_a = old_lab -> list -> last;
    stmp_b = new_lab -> list -> last;
    while (stmp_a -> prev != NULL)
    {
      stmp_b -> prev = duplicate_screen_string (stmp_a -> prev);
      stmp_b -> prev -> last = stmp_b -> last;
      stmp_a = stmp_a -> prev;
      stmp_b = stmp_b -> prev;
    }
  }
}

/*!
  \fn image * duplicate_image (image * old_img)

  \brief create a copy of an image data structure

  \param old_img the image to copy
*/
image * duplicate_image (image * old_img)
{
  int i, j, k, l, m;
  image * new_img = g_malloc0 (sizeof*new_img);

  // This line will copy all the stuff that is not dynamically allocated
  * new_img = * old_img;

  new_img -> back = g_malloc0(sizeof*new_img -> back);
  duplicate_background_data (new_img -> back, old_img -> back);
  j = proj_gl -> nspec;
  for (i=0; i<2; i++)
  {
    new_img -> color_map[i] = old_img -> color_map[i];
    new_img -> show_atom[i] = duplicate_bool(j, old_img -> show_atom[i]);
    new_img -> show_label[i] = duplicate_bool(j, old_img -> show_label[i]);
  }

  new_img -> sphererad = duplicate_double(2*j, old_img -> sphererad);
  new_img -> pointrad = duplicate_double(2*j, old_img -> pointrad);
  new_img -> atomicrad = duplicate_double(2*j, old_img -> atomicrad);
  new_img -> bondrad = g_malloc0 (2*j*sizeof*new_img -> bondrad);
  new_img -> linerad = g_malloc0 (2*j*sizeof*new_img -> linerad);
  new_img -> at_color = duplicate_color (2*j, old_img -> at_color);
  for (i=0; i<2*j; i++)
  {
    new_img -> bondrad[i] = duplicate_double(2*j, old_img -> bondrad[i]);
    new_img -> linerad[i] = duplicate_double(2*j, old_img -> linerad[i]);
  }

  for (i=0; i<9; i++)
  {
    new_img -> show_coord[i] = duplicate_bool(coord_gl -> totcoord[i], old_img -> show_coord[i]);
    if (i < 2 || i > 3) new_img -> show_poly[i] = duplicate_bool(coord_gl -> totcoord[i], old_img -> show_poly[i]);
    k = (i < 2) ? proj_gl -> nspec : 1;
    new_img -> spcolor[i] = g_malloc (k*sizeof*new_img -> spcolor[i]);
    for (j=0; j<k; j++)
    {
     new_img -> spcolor[i][j] = duplicate_color (coord_gl -> totcoord[i], old_img -> spcolor[i][j]);
    }
  }
  new_img -> at_data = g_malloc0 (proj_gl -> natomes*sizeof*new_img -> at_data);
  for (i=0; i<proj_gl -> natomes; i++)
  {
    new_img -> at_data[i].show[0] = proj_gl -> atoms[step][i].show[0];
    new_img -> at_data[i].show[1] = proj_gl -> atoms[step][i].show[1];
    new_img -> at_data[i].label[0] = proj_gl -> atoms[step][i].label[0];
    new_img -> at_data[i].label[1] = proj_gl -> atoms[step][i].label[1];
    new_img -> at_data[i].pick[0] = proj_gl -> atoms[step][i].pick[0];
    new_img -> at_data[i].cloned = proj_gl -> atoms[step][i].cloned;
    new_img -> at_data[i].style = proj_gl -> atoms[step][i].style;
  }

  for (i=0; i<3; i++)
  {
    new_img -> xyz -> title[i] = g_strdup_printf ("%s", old_img -> xyz -> title[i]);
  }
  new_img -> xyz -> color = NULL;
  if (old_img -> xyz -> color != NULL)
  {
    new_img -> xyz -> color = duplicate_color (3, old_img -> xyz -> color);
  }

  for (i=0; i<5; i++) duplicate_screen_label (& new_img -> labels[i], & old_img -> labels[i]);
  duplicate_material_and_lightning (new_img, old_img);

  // Atom selection
  for (i=0; i<2; i++) new_img -> selected[i] = duplicate_ogl_selection (old_img -> selected[i]);

  // Rings poly if any 'isolated'
  if (wingl -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (wingl -> ring_max[i])
      {
        m = 0;
        for (j=0; j< coord_gl -> totcoord[i+4]; j++)
        {
          k = coord_gl -> geolist[i+4][0][j];
          for (l=0; l<wingl -> num_rings[i][step][k-1]; l++)
          {
            if (wingl -> show_rpoly[i][step][k-1][l]) m++;
          }
        }
        if (m)
        {
         new_img -> i_rings[i] = allocdint (m+1, 2);
         new_img -> i_rings[i][0][0] = m;
          m = 1;
          for (j=0; j<coord_gl -> totcoord[i+4]; j++)
          {
            k = coord_gl -> geolist[i+4][0][j];
            for (l=0; l<wingl -> num_rings[i][step][k-1]; l++)
            {
              if (wingl -> show_rpoly[i][step][k-1][l])
              {
                new_img -> i_rings[i][m][0] = j;
                new_img -> i_rings[i][m][1] = l;
                m ++;
              }
            }
          }
        }
      }
    }
  }

  // Volumes
  if (wingl -> volumes)
  {
    for (i=0; i<2; i++)
    {
      for (j=0; j<FILLED_STYLES; j++)
      {
        new_img -> fm_show_vol[i][j] = duplicate_bool (coord_gl -> totcoord[2+i], old_img -> fm_show_vol[i][j]);
        for (k=0; k<coord_gl -> totcoord[2+i]; k++)
        {
          new_img -> fm_vol_col[i][j][k] = old_img ->  fm_vol_col[i][j][k];
        }
      }
    }
  }
  return new_img;
}

/*!
  \fn void add_image ()

  \brief add an image to the animation
*/
void add_image ()
{
  snapshot * nextsnap = g_malloc0 (sizeof*nextsnap);
  nextsnap -> img = duplicate_image (plot);
  nextsnap -> img -> id ++;

  // Now the pointers
  if (wingl -> anim -> frames == 0)
  {
    wingl -> anim -> first -> prev = NULL;
    wingl -> anim -> last = nextsnap;
    wingl -> anim -> last -> prev = NULL;
    wingl -> anim -> first = nextsnap;

  }
  else
  {
    wingl -> anim -> last -> next = nextsnap;
    nextsnap -> prev = wingl -> anim -> last;
    wingl -> anim -> last = wingl -> anim -> last -> next;
    wingl -> anim -> last -> img -> id = wingl -> anim -> frames;
  }
  wingl -> anim -> frames += 1;
}

extern void update_gl_pick_colors ();

/*!
  \fn atom * duplicate_atom (atom * at)

  \brief copy (partially) an atom data structure

  \param at the atom to copy
*/
atom * duplicate_atom (atom * at)
{
  atom * bt = g_malloc0 (sizeof*bt);
  bt -> x = at -> x;
  bt -> y = at -> y;
  bt -> z = at -> z;
  bt -> sp = at -> sp;
  bt -> id = at -> id;
  bt -> style = at -> style;
  bt -> cloned = at -> cloned;
  int i;
  for (i=0; i<2; i++)
  {
    bt -> show[i] = at -> show[i];
    bt -> pick[i] = at -> pick[i];
    bt -> label[i] = at -> label[i];
  }
  for (i=0; i<5; i++)
  {
    bt -> coord[i] = at -> coord[i];
  }
  bt -> numv = at -> numv;
  bt -> fid = at -> fid;
  bt -> faid = at -> faid;
  if (bt -> numv) bt -> vois = duplicate_int (bt -> numv, at -> vois);
  return bt;
}

/*!
  \fn void at_shift (atom * at, float * shift)

  \brief modify atomic coordinates to display image in cell replica

  \param at the atom
  \param shift the shift to apply
*/
void at_shift (atom * at, float * shift)
{
  at -> x += shift[0];
  at -> y += shift[1];
  at -> z += shift[2];
}

/*!
  \fn void at_unshift (atom * at, float * shift)

  \brief correct atomic coordinates modified to display image in cell replica

  \param at the atom
  \param shift the shift to correct
*/
void at_unshift (atom * at, float * shift)
{
  at -> x -= shift[0];
  at -> y -= shift[1];
  at -> z -= shift[2];
}

/*!
  \fn void draw (glwin * view)

  \brief main drawing subroutine for the OpenGL window

  \param view the target glwin
*/
void draw (glwin * view)
{
  wingl = view;
  proj_gl = get_project_by_id(view -> proj);
  proj_sp = proj_gl -> nspec;
  proj_at = proj_gl -> natomes;
  coord_gl = proj_gl -> coord;
  cell_gl = & proj_gl -> cell;
  plot = wingl -> anim -> last -> img;
  qual = plot -> quality-1;
  acolorm = plot -> color_map[0];
  pcolorm = plot -> color_map[1];
  step = plot -> step;
  int box_step = (cell_gl -> npt) ? step : 0;
  box_gl = & cell_gl -> box[box_step];

/* #ifdef DEBUG
  clock_gettime (CLOCK_MONOTONIC, & start_time);
  GLuint GPU_time;
  glGenQueries (1, & GPU_time);
  glBeginQuery (GL_TIME_ELAPSED, GPU_time);
// #endif */
  // First, if needed, we prepare the display lists
  if (proj_at)
  {
    if (wingl -> create_shaders[ATOMS] && wingl -> n_shaders[ATOMS][step] < 0) create_atom_lists (FALSE);
    if (wingl -> create_shaders[BONDS] && wingl -> n_shaders[BONDS][step] < 0) wingl -> n_shaders[BONDS][step] = create_bond_lists (FALSE);
    if (wingl -> create_shaders[SELEC] && wingl -> n_shaders[SELEC][step] < 0) wingl -> n_shaders[SELEC][step] = create_selection_lists ();
    if (wingl -> create_shaders[POLYS] && wingl -> n_shaders[POLYS][step] < 0) create_poly_lists ();
    if (wingl -> create_shaders[RINGS] && wingl -> n_shaders[RINGS][step] < 0) create_ring_lists ();
    if (wingl -> create_shaders[PICKS]) wingl -> n_shaders[PICKS][0] = create_pick_lists ();
    if (wingl -> create_shaders[SLABS]) create_slab_lists (proj_gl);
    if (wingl -> create_shaders[VOLMS] && wingl -> n_shaders[VOLMS][step] < 0) create_volumes_lists ();
    if (wingl -> create_shaders[LABEL]) wingl -> n_shaders[LABEL][0] = create_label_lists ();
    if (wingl -> create_shaders[MEASU]) create_measures_lists ();
  }
  else
  {
    int i;
    for (i=0; i<NGLOBAL_SHADERS; i++) cleaning_shaders (wingl, i);
    if (plot -> back -> gradient) wingl -> create_shaders[BACKG] = TRUE;
  }
  if (plot -> back -> gradient && wingl -> create_shaders[BACKG]) create_background_lists ();
  if (wingl -> create_shaders[MDBOX]) wingl -> n_shaders[MDBOX][box_step] = create_box_lists (box_step);
  if (wingl -> create_shaders[MAXIS]) wingl -> n_shaders[MAXIS][0] = create_axis_lists ();
  if (wingl -> create_shaders[LIGHT]) create_light_lists ();

  setup_camera ();
  // We draw normal scene or picking mode scene (only atoms or selection)

  if (wingl -> to_pick)
  {
    // Picking mode scene
    glDisable (GL_LIGHTING);
    glClearColor (plot -> back -> color.red,
                  plot -> back -> color.green,
                  plot -> back -> color.blue,
                  1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    draw_vertices (PICKS);

    glEnable (GL_LIGHTING);
  }
  else
  {
    // Normal mode scene
    glClearColor (plot -> back -> color.red,
                  plot -> back -> color.green,
                  plot -> back -> color.blue,
                  plot -> back -> color.alpha);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Gradient background
    draw_vertices (BACKG);

    // We want to draw the elements by reverse order
    // so that atoms will be last and and will appear on
    // top of bonds and so on

    // Box
    draw_vertices (MDBOX);

    // The bonds
    draw_vertices (BONDS);

    // Now the atoms
    draw_vertices (ATOMS);

    // The selected atoms/bonds
    draw_vertices (SELEC);

    int i = plot -> render;
    plot -> render = FILL;

    // Atom labels
    draw_vertices (LABEL);

    // Axis if centered
    if (view -> anim -> last -> img -> xyz -> t_pos == 4) draw_vertices (MAXIS);

    // Last the coordination polyhedra
    draw_vertices (POLYS);
    draw_vertices (RINGS);

    // Measures
    draw_vertices (MEASU);

    // Slab
    draw_vertices (SLABS);

    // Volumes
    draw_vertices (VOLMS);

    // Axis if not centered
    if (view -> anim -> last -> img -> xyz -> t_pos != 4) draw_vertices (MAXIS);

    // Lights
    draw_vertices (LIGHT);

    plot -> render = i;

    //draw_labels ();
    if (wingl -> record) add_image ();
  }

/* #ifdef DEBUG
  glEndQuery (GL_TIME_ELAPSED);
  GLint done = 0;
  while (! done)
  {
    glGetQueryObjectiv (GPU_time, GL_QUERY_RESULT_AVAILABLE, & done);
  }
  GLint GPU_res;
  glGetQueryObjectiv (GPU_time, GL_QUERY_RESULT, & GPU_res);
  g_print (":: TIME TO RENDER (GPU) = %f s\n", (double)GPU_res/CLOCKS_PER_SEC);
  clock_gettime (CLOCK_MONOTONIC, & stop_time);
  g_print (":: TIME TO RENDER (CPU) = %s\n", calculation_time(FALSE, get_calc_time (start_time, stop_time)));
// #endif */
}

