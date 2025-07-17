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
* @file d_axis.c
* @short Functions to prepare the OpenGL rendering for the axis
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_axis.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering for the axis

*
* List of functions:

  int create_axis_lists ();

  void setup_arrow (float * vert, vec3_t a, vec3_t b, vec3_t c, vec3_t d, vec3_t e);
  void init_axis_param ();
  void prepare_axis_data (float * vert_a, float * vert_b, float * vert_c);

  ColRGBA color_axis (int id);

  mat4_t create_axis_matrices (int type);

*/

#include "global.h"
#include "glview.h"

extern object_3d * draw_sphere (int quality);
extern object_3d * draw_cylinder (int quality, float ra, float rb);
extern object_3d * draw_cylinder_cap (int quality, float rad, gboolean picked);
extern void setup_line_vertice (float * vertices, vec3_t pos, ColRGBA col, float alpha);
extern void setup_sphere_vertice (float * vertices, vec3_t pos, ColRGBA col, float rad, float alpha);
extern void setup_cylinder_vertice (float * vertices, vec3_t pos_a, vec3_t pos_b, ColRGBA col, float rad, float alpha, float delta);
extern void setup_cap_vertice (float * vertices, vec3_t pos_a, vec3_t pos_b, ColRGBA col, float rad, float alpha);
extern void setup_triangles (float * vertices, vec3_t sa, vec3_t sb, vec3_t sc);
extern float poly_alpha;
extern vec3_t centroid;

extern void clean_labels (int id);

extern ColRGBA pcol;
extern float extra;

float arrow_length;
float axis_size;
float axis_radius;
float arrow_base;
float label_pos;

/*!
  \fn ColRGBA color_axis (int id)

  \brief get axis color

  \param id axis x (0), y(1) or z(2)
*/
ColRGBA color_axis (int id)
{
  if (plot -> xyz -> color != NULL)
  {
    return plot -> xyz -> color[id];
  }
  else
  {
    ColRGBA col;
    col.red   = (id == 2) ? 1.0: 0.0;
    col.green = (id == 1) ? 1.0: 0.0;
    col.blue  = (id == 0) ? 1.0: 0.0;
    col.alpha = 1.0;
    return col;
  }
}

/*!
  \fn mat4_t create_axis_matrices (int type)

  \brief create axis OpenGL rendering matrices

  \param type axis type (standard: 0, atom edition viewer axis: 1)
*/
mat4_t create_axis_matrices (int type)
{
  GLfloat x, y, z;
  x = y = z = 0.0;
  float from_edge = 50.0;
  vec3_t win_coord;
  GLfloat sx = (GLfloat)wingl -> pixels[0];
  GLfloat sy = (GLfloat)wingl -> pixels[1];
  mat4_t axis_projection_matrix;
  mat4_t axis_model_matrix;
  mat4_t axis_view_matrix;
  mat4_t axis_model_view_matrix;
  if (plot -> xyz -> t_pos == CENTER)
  {
    if (plot -> rep == ORTHOGRAPHIC)
    {
      x = 0.5 * sx;
      y = 0.5 * sy;
    }
    axis_projection_matrix = wingl -> proj_model_matrix;
  }
  else
  {
    axis_projection_matrix = m4_ortho (plot -> gleft, plot -> gright, plot -> gbottom, plot -> gtop, -plot -> gfar, plot -> gfar);
    if (plot -> xyz -> t_pos == TOP_RIGHT)
    {
      x = sx - from_edge;
      y = from_edge;
    }
    else if (plot -> xyz -> t_pos == TOP_LEFT)
    {
      x = from_edge;
      y = from_edge;
    }
    else if (plot -> xyz -> t_pos == BOTTOM_RIGHT)
    {
      x = sx - from_edge;
      y = sy - from_edge;
    }
    else if (plot -> xyz -> t_pos == BOTTOM_LEFT)
    {
      x = from_edge;
      y = sy - from_edge;
    }
    else if (plot -> xyz -> t_pos == CUSTOM)
    {
      // Custom position
      x = plot -> xyz -> c_pos[0] * sx / 100.0;
      y = plot -> xyz -> c_pos[1] * sy / 100.0;
    }
  }
  win_coord = v3_un_project (vec3(x, y, z), wingl -> view_port, axis_projection_matrix);
  win_coord.z = 1.0;
  //print_axis_matrices (win_coord);
  if (type == 0)
  {
    axis_view_matrix = wingl -> view_matrix;
  }
  else
  {
    axis_view_matrix = wingl -> model_matrix;
  }
  axis_model_matrix           = m4_translation (win_coord);
  axis_model_view_matrix      = m4_mul (axis_model_matrix, axis_view_matrix);
  return m4_mul (axis_projection_matrix, axis_model_view_matrix);
}

/*!
  \fn void setup_arrow (float * vert, vec3_t a, vec3_t b, vec3_t c, vec3_t d, vec3_t e)

  \brief setup axis 3D arrow rendering data

  \param vert the OpenGL buffer data to fill
  \param a position vector a
  \param b position vector b
  \param c position vector c
  \param d position vector d
  \param e position vector e
*/
void setup_arrow (float * vert, vec3_t a, vec3_t b, vec3_t c, vec3_t d, vec3_t e)
{
  setup_triangles (vert, a, b, c);
  setup_triangles (vert, a, c, d);
  setup_triangles (vert, a, d, e);
  setup_triangles (vert, a, e, b);
  setup_triangles (vert, b, c, d);
  setup_triangles (vert, b, d, e);
}

/*!
  \fn void init_axis_param ()

  \brief initialize axis rendering parameters
*/
void init_axis_param ()
{
  arrow_length = 0.5;
  axis_size = plot -> xyz -> length;
  axis_radius = plot -> xyz -> rad;
  arrow_base = 0.1;
  label_pos = 0.2;
  if (plot -> rep == PERSPECTIVE && plot -> xyz -> t_pos != CENTER)
  {
    arrow_base /= (plot -> p_depth / plot -> gnear);
    arrow_length /= (plot -> p_depth / plot -> gnear);
    axis_radius /= (plot -> p_depth / plot -> gnear);
    axis_size /= (plot -> p_depth / plot -> gnear);
    label_pos /= (plot -> p_depth / plot -> gnear);
  }
}

/*!
  \fn void prepare_axis_data (float * vert_a, float * vert_b, float * vert_c)

  \brief prepare axis OpenGL rendering data buffer

  \param vert_a OpenGL buffer data to fill
  \param vert_b OpenGL buffer data to fill
  \param vert_c OpenGL buffer data to fill
*/
void prepare_axis_data (float * vert_a, float * vert_b, float * vert_c)
{
  init_axis_param ();
  poly_alpha = 1.0;

  vec3_t a, b, c, d, e;
  float sa;

  nbs = nbl = nba = 0;
  int i;
  for (i=0; i<3; i++)
  {
    pcol = color_axis (i);
    a = vec3(0.0, 0.0, 0.0);
    b = vec3((i==0)? axis_size-arrow_length : 0.0, (i==1)? axis_size-arrow_length : 0.0, (i==2)? axis_size-arrow_length : 0.0);
    if (plot -> xyz -> axis == WIREFRAME)
    {
      setup_line_vertice (vert_a, a, pcol, 1.0);
      setup_line_vertice (vert_a, b, pcol, 1.0);
      a = vec3((i==0)? axis_size : 0.0, (i==1)? axis_size : 0.0, (i==2)? axis_size : 0.0);
      sa = (i == 2) ? -1.0 : 1.0;
      b = vec3((i==0)? axis_size-arrow_length : -arrow_base, (i==1)? axis_size-arrow_length : -sa*arrow_base, (i==2)? axis_size-arrow_length : arrow_base);
      c = vec3((i==0)? axis_size-arrow_length : -arrow_base, (i==1)? axis_size-arrow_length : -arrow_base, (i==2)? axis_size-arrow_length : -arrow_base);
      d = vec3((i==0)? axis_size-arrow_length :  arrow_base, (i==1)? axis_size-arrow_length : sa*arrow_base, (i==2)? axis_size-arrow_length : -arrow_base);
      e = vec3((i==0)? axis_size-arrow_length :  arrow_base, (i==1)? axis_size-arrow_length : arrow_base, (i==2)? axis_size-arrow_length : arrow_base);
      centroid = v3_add (a, v3_add(b, v3_add(c, v3_add(d, e))));
      centroid = v3_divs (centroid, 5.0);
      setup_arrow (vert_b, a, b, c, d, e);
    }
    else
    {
      setup_cylinder_vertice (vert_a, a, b, pcol, axis_radius, 1.0, 0.0);
      c = vec3((i==0)? axis_size : 0.0, (i==1)? axis_size : 0.0, (i==2)? axis_size : 0.0);
      nbs --;
      setup_cylinder_vertice (vert_b, c, b, pcol, axis_radius+arrow_base, 1.0, 0.0);
      nbs --;
      setup_cap_vertice (vert_c, c, b, pcol, axis_radius+arrow_base, 1.0);
    }
  }
}

/*!
  \fn int create_axis_lists ()

  \brief prepare axis OpenGL rendering
*/
int create_axis_lists ()
{
  vec3_t pos;
  float shift[3]={0.0, 0.0, 0.0};
  int nshaders = 0;
  object_3d * axis_a, * axis_b, * axis_c, * axis_d;

  cleaning_shaders (wingl, MAXIS);
  wingl -> create_shaders[MAXIS] = FALSE;
  if (plot -> xyz -> axis == NONE) return nshaders;

  if (plot -> xyz -> axis == WIREFRAME)
  {
    axis_a = g_malloc0 (sizeof*axis_a);
    axis_a -> vert_buffer_size = LINE_BUFF_SIZE;
    axis_a -> num_vertices = 3*2;
    axis_a -> vertices = allocfloat (axis_a -> vert_buffer_size*axis_a -> num_vertices);
    axis_b = g_malloc0 (sizeof*axis_b);
    axis_b -> vert_buffer_size = POLY_BUFF_SIZE;
    axis_b -> num_vertices = 3*6*9;
    axis_b -> vertices = allocfloat (axis_b -> vert_buffer_size*axis_b -> num_vertices);
  }
  else
  {
    axis_a = draw_cylinder (plot -> quality, 1.0, 1.0);
    axis_a -> num_instances = 3;
    axis_a -> inst_buffer_size = CYLI_BUFF_SIZE;
    axis_a -> instances = allocfloat (axis_a -> num_instances*CYLI_BUFF_SIZE);
    axis_b = draw_cylinder (plot -> quality, 0.0, 1.0);
    axis_b -> num_instances = 3;
    axis_b -> inst_buffer_size = CYLI_BUFF_SIZE;
    axis_b -> instances = allocfloat (axis_b -> num_instances*CYLI_BUFF_SIZE);
    axis_c = draw_cylinder_cap (plot -> quality, 1.0, FALSE);
    axis_c -> num_instances = 3;
    axis_c -> inst_buffer_size = CAPS_BUFF_SIZE;
    axis_c -> instances = allocfloat (CAPS_BUFF_SIZE*axis_c -> num_instances);
    axis_d = draw_sphere (plot -> quality);
    axis_d -> num_instances = 1;
    axis_d -> inst_buffer_size = ATOM_BUFF_SIZE;
    axis_d -> instances = allocfloat (ATOM_BUFF_SIZE);
  }
  nshaders = (plot -> xyz -> axis == WIREFRAME) ? 2 : 4;
  prepare_axis_data ((plot -> xyz -> axis == WIREFRAME) ? axis_a -> vertices :  axis_a -> instances,
                     (plot -> xyz -> axis == WIREFRAME) ? axis_b -> vertices :  axis_b -> instances,
                     (plot -> xyz -> axis == WIREFRAME) ? NULL :  axis_c -> instances);
  clean_labels (2);
  if (plot -> xyz -> labels)
  {
    int i;
    for (i=0; i<3; i++)
    {
      pos = vec3 ((i==0) ? axis_size+label_pos : 0.0, (i==1) ? axis_size+label_pos : 0.0, (i==2) ? axis_size+label_pos : 0.0);
      prepare_string (plot -> xyz -> title[i], 2, color_axis (i), pos, shift, NULL, NULL, NULL);
    }
    nshaders += (plot -> labels[2].render+1) * (plot -> labels[2].list -> last -> id + 1);
    wingl -> ogl_glsl[MAXIS][0] = g_malloc0 (nshaders*sizeof*wingl -> ogl_glsl[MAXIS][0]);
    render_all_strings (MAXIS, 2);
  }
  else
  {
    wingl -> ogl_glsl[MAXIS][0] = g_malloc0 (nshaders*sizeof*wingl -> ogl_glsl[MAXIS][0]);
  }
  if (plot -> xyz -> axis == WIREFRAME)
  {
    // Lines
    wingl -> ogl_glsl[MAXIS][0][0] = init_shader_program (MAXIS, GLSL_LINES, line_vertex, NULL, line_color, GL_LINES, 2, 1, FALSE, axis_a);
    wingl -> ogl_glsl[MAXIS][0][0] -> line_width = plot -> xyz -> line;
    // Arrows
    wingl -> ogl_glsl[MAXIS][0][1] = init_shader_program (MAXIS, GLSL_POLYEDRA, full_vertex, NULL, full_color, GL_TRIANGLES, 3, 1, TRUE, axis_b);
  }
  else
  {
    // Yellow sphere at (0.0, 0.0, 0.0)
    pcol.red = pcol.green = 1.0;
    pcol.blue = 0.0;
    pcol.alpha = 1.0;
    setup_sphere_vertice (axis_d -> instances, vec3(0.0,0.0,0.0), pcol, axis_radius, 1.0);
    wingl -> ogl_glsl[MAXIS][0][0] = init_shader_program (MAXIS, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, TRUE, axis_d);
    // Cylinders
    wingl -> ogl_glsl[MAXIS][0][1] = init_shader_program (MAXIS, GLSL_CYLINDERS, cylinder_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, TRUE, axis_a);
    // Cones
    wingl -> ogl_glsl[MAXIS][0][2] = init_shader_program (MAXIS, GLSL_CYLINDERS, cone_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, TRUE, axis_b);
    // Cones Caps
    wingl -> ogl_glsl[MAXIS][0][3] = init_shader_program (MAXIS, GLSL_CAPS, cap_vertex, NULL, full_color, GL_TRIANGLE_FAN, 5, 1, TRUE, axis_c);
    g_free (axis_c);
    g_free (axis_d);
  }
  g_free (axis_a);
  g_free (axis_b);
  return nshaders;
}
