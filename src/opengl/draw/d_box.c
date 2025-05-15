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
* @file d_box.c
* @short Functions to prepare the OpenGL rendering for the box(es) \n
         Functions to prepare the OpenGL rendering for the slab(s) \n
         Functions to prepare the OpenGL rendering for the volume(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_box.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering for the box(es)
 - The functions to prepare the OpenGL rendering for the slab(s)
 - The functions to prepare the OpenGL rendering for the volume(s)

*
* List of functions:

  int create_box_lists (int b_step);

  double draw_cuboid (gboolean draw, int SHADID, int shadnum, mat4_t rot, vec3_t cpos, double paral[3][3], ColRGBA col, double slab_alpha);

  gboolean are_identical_vec3 (vec3_t va, vec3_t vb);
  gboolean not_in_already (vec3_t a, vec3_t b, float * vertices);
  gboolean not_in_corners (vec3_t a, float * vertices);

  void setup_extra_box_vertices (vec3_t a, vec3_t b, int id, float * c_vert, float * s_vert);
  void setup_box_vertices (vec3_t ax, vec3_t bx, int id, float * c_vert, float * s_vert);
  void prepare_box_vertices (void (*c_func)(vec3_t, vec3_t, int, float *, float *), float * verts, float * serts, int id);
  void prepare_cuboid (vec3_t position, int id);
  void create_light_lists ();
  void cuboid_slab (mat4_t rot);
  void cylinder_slab (mat4_t rot);
  void spherical_slab ();
  void create_slab_lists (project * this_proj);
  void create_volumes_lists ();

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
extern void create_slab_info (project * this_proj);
extern void process_selected_atom (project * this_proj, glwin * view, int id, int ac, int se, int pi);
extern ColRGBA pcol;

int BOX_BUFF_SIZE;

/*!
  \fn gboolean are_identical_vec3 (vec3_t va, vec3_t vb)

  \brief are these 2 vectors indentical

  \param va 1st vector
  \param vb 2nd vector
*/
gboolean are_identical_vec3 (vec3_t va, vec3_t vb)
{

  if (va.x == vb.x && va.y == vb.y && va.z == vb.z)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn gboolean not_in_already (vec3_t a, vec3_t b, float * vertices)

  \brief is this box edge vector already saved ?

  \param a 1st set of coordinates
  \param b 2nd set of coordinates
  \param vertices the data buffer to check
*/
gboolean not_in_already (vec3_t a, vec3_t b, float * vertices)
{
  int i, j, k;
  vec3_t tma, tmb;
  for (i=0; i<nbs/2; i=i+2)
  {
    j = i*BOX_BUFF_SIZE;
    k = j+BOX_BUFF_SIZE;
    tma = vec3(vertices[j], vertices[j+1], vertices[j+2]);
    tmb = vec3(vertices[k], vertices[k+1], vertices[k+2]);
    if (are_identical_vec3(a, tma) && are_identical_vec3(b, tmb))
    {
      return FALSE;
    }
    else if (are_identical_vec3(b, tma) && are_identical_vec3(a, tmb))
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*!
  \fn gboolean not_in_corners (vec3_t a, float * vertices)

  \brief is this box cornder already saved ?

  \param a the coordinates
  \param vertices the data buffer to check
*/
gboolean not_in_corners (vec3_t a, float * vertices)
{
  int i, j;
  vec3_t tma;
  for (i=0; i<nbl; i++)
  {
    j = i*8;
    tma = vec3(vertices[j], vertices[j+1], vertices[j+2]);
    if (are_identical_vec3(a, tma))
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*!
  \fn void setup_extra_box_vertices (vec3_t a, vec3_t b, float * c_vert, float * s_vert)

  \brief prepare the extra cell(s) OpenGL rendering

  \param a 1st point coordinates
  \param b 2nd point coordinates
  \param c_vert OpenGL cylinder/line data buffer to fill
  \param s_vert OpenGL sphere data buffer to fill, or NULL
*/
void setup_extra_box_vertices (vec3_t a, vec3_t b, float * c_vert, float * s_vert)
{
  float j;
  int p, q, r;
  vec3_t shift;
  vec3_t t_a, t_b;
  for (j=-1.0; j<2.0; j=j+2.0)
  {
    for (p=0; p<plot -> extra_cell[0]+1; p++)
    {
      for (q=0; q<plot -> extra_cell[1]+1; q++)
      {
        for (r=0; r<plot -> extra_cell[2]+1; r++)
        {
          if (p > 0 || q > 0 || r > 0)
          {
            shift.x = p*box_gl -> vect[0][0]+q*box_gl -> vect[1][0]+r*box_gl -> vect[2][0];
            shift.y = p*box_gl -> vect[0][1]+q*box_gl -> vect[1][1]+r*box_gl -> vect[2][1];
            shift.z = p*box_gl -> vect[0][2]+q*box_gl -> vect[1][2]+r*box_gl -> vect[2][2];

            t_a = v3_add (v3_muls (a, j), shift);
            t_b = v3_add (v3_muls (b, j), shift);
            if (not_in_already (t_a, t_b, c_vert))
            {
              if (plot -> box_axis[BOX] == WIREFRAME)
              {
                setup_line_vertice (c_vert, t_a, pcol, 0.5);
                setup_line_vertice (c_vert, t_b, pcol, 0.5);
              }
              else
              {
                if (not_in_corners(t_a, s_vert)) setup_sphere_vertice (s_vert, t_a, pcol, plot -> box_axis_rad[BOX], pcol.alpha*0.5);
                if (not_in_corners(t_b, s_vert)) setup_sphere_vertice (s_vert, t_b, pcol, plot -> box_axis_rad[BOX], pcol.alpha*0.5);
                setup_cylinder_vertice (c_vert, t_a, t_b, pcol, plot -> box_axis_rad[BOX], 0.5, 0.0);
              }
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void setup_box_vertices (vec3_t ax, vec3_t bx, float * c_vert, float * s_vert)

  \brief prepare the unit cell OpenGL rendering

  \param ax 1st point coordinates
  \param bx 2nd point coordinates
  \param c_vert OpenGL cylinder/line data buffer to fill
  \param s_vert OpenGL sphere data buffer to fill, or NULL
*/
void setup_box_vertices (vec3_t ax, vec3_t bx, float * c_vert, float * s_vert)
{
  float j;
  vec3_t a, b;
  /*vec3_t cell[3];
  int i;
  for (i=0; i<3; i++)
  {
    cell[i] = vec3(box_gl -> vect[i][0], box_gl -> vect[i][1], box_gl -> vect[i][2]);
    cell[i] = v3_divs (cell[i], 2.0);
  }
  vec3_t lattice = v3_add(v3_add(cell[0], cell[1]), cell[2]);*/
  for (j=-1.0; j<2.0; j=j+2.0)
  {
    /*a = v3_sub (v3_muls (ax, j), lattice);
    b = v3_sub (v3_muls (bx, j), lattice);*/
    a = v3_muls (ax, j);
    b = v3_muls (bx, j);
    if (plot -> box_axis[BOX] == WIREFRAME)
    {
      setup_line_vertice (c_vert, a, pcol, 1.0);
      setup_line_vertice (c_vert, b, pcol, 1.0);
    }
    else
    {
      if (not_in_corners(a, s_vert)) setup_sphere_vertice (s_vert, a, pcol, plot -> box_axis_rad[BOX], pcol.alpha*1.0);
      if (not_in_corners(b, s_vert)) setup_sphere_vertice (s_vert, b, pcol, plot -> box_axis_rad[BOX], pcol.alpha*1.0);
      setup_cylinder_vertice (c_vert, a, b, pcol, plot -> box_axis_rad[BOX], 1.0, 0.0);
    }
  }
}

/*!
  \fn void prepare_box_vertices (void (*c_func)(vec3_t, vec3_t, float *, float *), float * verts, float * serts)

  \brief prepare a box OpenGL rendering

  \param c_func the function to use a render
  \param verts OpenGL cylinder/line data buffer to fill
  \param serts OpenGL sphere data buffer to fill, or NULL
*/
void prepare_box_vertices (void (*c_func)(vec3_t, vec3_t, float *, float *), float * verts, float * serts)
{
  int i;
  vec3_t pa, pb;

  for (i=-1; i<2; i=i+2)
  {
    pa.x = (- box_gl -> vect[0][0] - box_gl -> vect[1][0] + i * box_gl -> vect[2][0]) / 2.0;
    pa.y = (- box_gl -> vect[0][1] - box_gl -> vect[1][1] + i * box_gl -> vect[2][1]) / 2.0;
    pa.z = (- box_gl -> vect[0][2] - box_gl -> vect[1][2] + i * box_gl -> vect[2][2]) / 2.0;
    pb.x = (box_gl -> vect[0][0] - box_gl -> vect[1][0] + i * box_gl -> vect[2][0]) / 2.0;
    pb.y = (box_gl -> vect[0][1] - box_gl -> vect[1][1] + i * box_gl -> vect[2][1]) / 2.0;
    pb.z = (box_gl -> vect[0][2] - box_gl -> vect[1][2] + i * box_gl -> vect[2][2]) / 2.0;
    (* c_func)(pa, pb, verts, serts);
    pa.x = (i * box_gl -> vect[0][0] - box_gl -> vect[1][0] + box_gl -> vect[2][0]) / 2.0;
    pa.y = (i * box_gl -> vect[0][1] - box_gl -> vect[1][1] + box_gl -> vect[2][1]) / 2.0;
    pa.z = (i * box_gl -> vect[0][2] - box_gl -> vect[1][2] + box_gl -> vect[2][2]) / 2.0;
    pb.x = (i * box_gl -> vect[0][0] - box_gl -> vect[1][0] - box_gl -> vect[2][0]) / 2.0;
    pb.y = (i * box_gl -> vect[0][1] - box_gl -> vect[1][1] - box_gl -> vect[2][1]) / 2.0;
    pb.z = (i * box_gl -> vect[0][2] - box_gl -> vect[1][2] - box_gl -> vect[2][2]) / 2.0;
    (* c_func)(pa, pb, verts, serts);
    pa.x = (i * box_gl -> vect[0][0] + box_gl -> vect[1][0] + box_gl -> vect[2][0]) / 2.0;
    pa.y = (i * box_gl -> vect[0][1] + box_gl -> vect[1][1] + box_gl -> vect[2][1]) / 2.0;
    pa.z = (i * box_gl -> vect[0][2] + box_gl -> vect[1][2] + box_gl -> vect[2][2]) / 2.0;
    pb.x = (i * box_gl -> vect[0][0] - box_gl -> vect[1][0] + box_gl -> vect[2][0]) / 2.0;
    pb.y = (i * box_gl -> vect[0][1] - box_gl -> vect[1][1] + box_gl -> vect[2][1]) / 2.0;
    pb.z = (i * box_gl -> vect[0][2] - box_gl -> vect[1][2] + box_gl -> vect[2][2]) / 2.0;
    (* c_func)(pa, pb, verts, serts);
  }
}

/*!
  \fn int create_box_lists (int b_step)

  \brief prepare box OpenGL rendering

  \param b_step the step number if NPT, 0 otherwise
*/
int create_box_lists (int b_step)
{
  int vertex = 8;
  object_3d * box_a, * box_b;

  cleaning_shaders (wingl, MDBOX);
  wingl -> create_shaders[MDBOX] = FALSE;

  if (plot -> box_axis[BOX] == NONE) return 0;

  int shaders;
  vertex *= (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);

  if (plot -> box_axis[BOX] == WIREFRAME)
  {
    shaders = 1;
    BOX_BUFF_SIZE = LINE_BUFF_SIZE;
    box_b = g_malloc0 (sizeof*box_b);
    box_b -> vert_buffer_size = LINE_BUFF_SIZE;
    box_b -> num_vertices = vertex*3;
    box_b -> vertices = allocfloat (box_b -> vert_buffer_size*box_b -> num_vertices);
  }
  else
  {
    shaders = 2;
    BOX_BUFF_SIZE = CYLI_BUFF_SIZE;
    // Spheres at corners
    box_a = draw_sphere (plot -> quality);
    box_a -> num_instances = 3*vertex*3/2;
    box_a -> inst_buffer_size = ATOM_BUFF_SIZE;
    box_a -> instances = allocfloat (box_a -> num_instances*ATOM_BUFF_SIZE);
    // Cylinders
    box_b = draw_cylinder (plot -> quality, 1.0, 1.0);
    box_b -> num_instances = 3*vertex;
    box_b -> inst_buffer_size = CYLI_BUFF_SIZE;
    box_b -> instances = allocfloat (box_b -> num_instances*CYLI_BUFF_SIZE);

  }
  wingl -> ogl_glsl[MDBOX][b_step] = g_malloc0 (shaders*sizeof*wingl -> ogl_glsl[MDBOX][b_step]);

  nbs = nbl = 0;
  pcol = plot -> box_color;

  prepare_box_vertices (setup_box_vertices,
                        (plot -> box_axis[BOX] == WIREFRAME) ? box_b -> vertices: box_b -> instances,
                        (plot -> box_axis[BOX] == WIREFRAME) ? NULL : box_a -> instances);
  if (plot -> extra_cell[0] > 0 || plot -> extra_cell[1] > 0 || plot -> extra_cell[2] > 0)
  {
    prepare_box_vertices (setup_extra_box_vertices, (plot -> box_axis[BOX] == WIREFRAME) ? box_b -> vertices: box_b -> instances,
                                                    (plot -> box_axis[BOX] == WIREFRAME) ? NULL : box_a -> instances);
  }
  if (plot -> box_axis[BOX] == WIREFRAME)
  {
    wingl -> ogl_glsl[MDBOX][b_step][0] = init_shader_program (MDBOX, GLSL_LINES, line_vertex, NULL, line_color, GL_LINES, 2, 1, FALSE, box_b);
    wingl -> ogl_glsl[MDBOX][b_step][0] -> line_width = plot -> box_axis_line[BOX];
  }
  else
  {
    // Sphere at corners
    wingl -> ogl_glsl[MDBOX][b_step][0] = init_shader_program (MDBOX, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, TRUE, box_a);
    g_free (box_a);
    // Cylinders
    wingl -> ogl_glsl[MDBOX][b_step][1] = init_shader_program (MDBOX, GLSL_CYLINDERS, cylinder_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, TRUE, box_b);
  }
  g_free (box_b);

  return shaders;
}

/*vec3_t center;

/ *
*  void setup_cuboid_vertices (vec3_t a, vec3_t b, int id)

  \brief NOT USED !

  \param a
  \param b
  \param id
* /
void setup_cuboid_vertices (vec3_t a, vec3_t b, int id)
{
  float j;
  for (j=-1.0; j<2.0; j=j+2.0)
  {
    box_axis_vertices (wingl -> d_vertices[LIGHT][id], -1, 0.0, v3_muls (v3_add(a,center), j));
    box_axis_vertices (wingl -> d_vertices[LIGHT][id], -1, 0.0, v3_muls (v3_add(b,center), j));
  }
}*/

GLfloat cuboid_vertices[] = {
        // Positions          // Normals
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    };

/*!
  \fn void prepare_cuboid (vec3_t position, int id)

  \brief OpenGL 3D light object rendering

  \param position light position
  \param id shader number
*/
void prepare_cuboid (vec3_t position, int id)
{
  float lgt = 1.0;
  object_3d * light = g_malloc0 (sizeof*light);
  light -> vert_buffer_size = POLY_BUFF_SIZE;
  light -> num_vertices = 36;
  light -> vertices = allocfloat (light -> vert_buffer_size*light -> num_vertices);
  int i, j, k, l;
  l = 0;
  for (i=0; i<36; i++)
  {
    j = 10*l;
    k = 6*l;
    light -> vertices[j] = lgt*cuboid_vertices[k]*0.5 + position.x;
    light -> vertices[j+1] = lgt*cuboid_vertices[k+1]*0.5 + position.y;
    light -> vertices[j+2] = lgt*cuboid_vertices[k+2]*0.5 + position.z;
    light -> vertices[j+3] = cuboid_vertices[k+3];
    light -> vertices[j+4] = cuboid_vertices[k+4];
    light -> vertices[j+5] = cuboid_vertices[k+5];
    light -> vertices[j+6] = 1.0;
    light -> vertices[j+7] = 1.0;
    light -> vertices[j+8] = 1.0;
    light -> vertices[j+9] = 1.0;
    l ++;
  }
  wingl -> ogl_glsl[LIGHT][0][id] = init_shader_program (LIGHT, GLSL_POLYEDRA, full_vertex, NULL, full_color, GL_TRIANGLES, 3, 1, FALSE, light);
  g_free (light);
}

/*!
  \fn void create_light_lists ()

  \brief prepare light(s) OpenGL rendering
*/
void create_light_lists ()
{
  int i, j;
  j = 0;
  cleaning_shaders (wingl, LIGHT);
  for (i=0; i<plot -> lights; i++)
  {
    if (plot -> l_ght[i].show) j++;
  }
  wingl -> n_shaders[LIGHT][0] = j;
  if (plot -> light_loc != NULL)
  {
    g_free (plot -> light_loc);
    plot -> light_loc = NULL;
  }

  if (j > 0)
  {
    wingl -> ogl_glsl[LIGHT][0] = g_malloc0 (wingl -> n_shaders[LIGHT][0]*sizeof*wingl -> ogl_glsl[LIGHT][0]);
    plot -> light_loc = allocint (j);
    j = 0;
    for (i=0; i<plot -> lights; i++)
    {
      if (plot -> l_ght[i].show)
      {
        prepare_cuboid (plot -> l_ght[i].position, j);
        if (plot -> l_ght[i].type > 0 && plot -> l_ght[i].fix == 0) plot -> light_loc[j] = 1;
        j ++;
      }
    }
  }
  wingl -> create_shaders[LIGHT] = FALSE;
}

vec3_t get_normal (vec3_t v1, vec3_t v2, vec3_t v3)
{
  vec3_t edge_a = v3_sub(v3, v1);
  vec3_t edge_b = v3_sub(v2, v1);
  return v3_norm (v3_cross(edge_a, edge_b));
}

/*!
  \fn double draw_cuboid (gboolean draw, int SHADID, int shadnum, mat4_t rot, vec3_t cpos, double paral[3][3], ColRGBA col, double slab_alpha)

  \brief draw cuboid vertices

  \param draw draw or not (1/0)
  \param SHADID shader id
  \param shadnum shader number
  \param rot rotation matrix
  \param cpos position of center of slab
  \param paral cell parameters
  \param col slab color
  \param slab_alpha slab opacity
*/
double draw_cuboid (gboolean draw, int SHADID, int shadnum, mat4_t rot, vec3_t cpos, double paral[3][3], ColRGBA col, double slab_alpha)
{
  int i, j, k, l, m, n, o;
  vec3_t pos;
  double cvol;
  double shift[3];
  cvol =  (paral[0][1]*paral[1][2]-paral[0][2]*paral[1][1])*paral[2][0];
  cvol += (paral[0][2]*paral[1][0]-paral[0][0]*paral[1][2])*paral[2][1];
  cvol += (paral[0][0]*paral[1][1]-paral[0][1]*paral[1][0])*paral[2][2];
  cvol = fabs(cvol);
  if (draw)
  {
    object_3d * slab = g_malloc0 (sizeof*slab);
    slab -> vert_buffer_size = POLY_BUFF_SIZE;
    slab -> num_vertices = 36*(plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
    slab -> vertices = allocfloat (slab -> vert_buffer_size*slab -> num_vertices);
    for (i=0; i<36; i++)
    {
      j = i*POLY_BUFF_SIZE;
      k = 6*i;
      for (l=0; l<3; l++)
      {
        slab -> vertices[j+l] += 0.5*(cuboid_vertices[k]*paral[0][l] + cuboid_vertices[k+1]*paral[1][l] + cuboid_vertices[k+2]*paral[2][l]);
      }
      pos = vec3 (slab -> vertices[j], slab -> vertices[j+1], slab -> vertices[j+2]);
      pos = m4_mul_pos (rot, pos);
      slab -> vertices[j]   = pos.x + cpos.x;
      slab -> vertices[j+1] = pos.y + cpos.y;
      slab -> vertices[j+2] = pos.z + cpos.z;
    }
    n = 0;
    for (i=0; i<plot -> extra_cell[0]+1; i++)
    {
      for (j=0; j<plot -> extra_cell[1]+1; j++)
      {
        for (k=0; k<plot -> extra_cell[2]+1; k++)
        {
          for (l=0; l<3; l++) shift[l] = i*box_gl -> vect[0][l] + j*box_gl -> vect[1][l] + k*box_gl -> vect[2][l];
          for (l=0; l<36; l++)
          {
            m = POLY_BUFF_SIZE*n;
            for (o=0; o<3; o++)
            {
              slab -> vertices[m+o] = slab -> vertices[l*POLY_BUFF_SIZE+o] + shift[o];
              if (SHADID == SLABS)
              {
                slab -> vertices[m+6+o] = (o) ? 1.0 : 0.0;
              }
              else
              {
                if (! o) slab -> vertices[m+6+o] = col.red;
                if (o == 1) slab -> vertices[m+6+o] = col.green;
                if (o == 2) slab -> vertices[m+6+o] = col.blue;
              }
            }
            pos = vec3 (cuboid_vertices[6*l+3], cuboid_vertices[6*l+4], cuboid_vertices[6*l+5]);
            pos = m4_mul_pos (rot, pos);
            slab -> vertices[m+3] = pos.x;
            slab -> vertices[m+4] = pos.y;
            slab -> vertices[m+5] = pos.z;
            slab -> vertices[m+9] = slab_alpha;
            n ++;
          }
        }
      }
    }
    wingl -> ogl_glsl[SHADID][(SHADID == SLABS) ? 0 : step][shadnum] = init_shader_program (SHADID, GLSL_POLYEDRA, full_vertex, NULL, full_color, GL_TRIANGLES, 3, 1, TRUE, slab);
    g_free (slab);
  }
  return cvol;
}

/*!
  \fn void cuboid_slab (mat4_t rot)

  \brief prepare cuboid slab OpenGL rendering

  \param rot rotation matrix
*/
void cuboid_slab (mat4_t rot)
{
  int i, j, k, l, m;
  double ang[3], cang[3], sang[3];
  double paral[3][3];
  double tmp;
  for (i=0; i<3; i++)
  {
    if (wingl -> cell_win -> cparam[i+15] == 90.0)
    {
      ang[i] = pi/2.0;
      sang[i] = 1.0;
      cang[i] = 0.0;
    }
    else
    {
      ang[i] = wingl -> cell_win -> cparam[i+15]*pi/180.0;
      sang[i] = sin(ang[i]);
      cang[i] = cos(ang[i]);
    }
  }
  paral[0][0] = wingl -> cell_win -> cparam[9];
  paral[0][1] = 0.0;
  paral[0][2] = 0.0;
  paral[1][0] = wingl -> cell_win -> cparam[10] * cang[2];
  paral[1][1] = wingl -> cell_win -> cparam[10] * sang[2];
  paral[1][2] = 0.0;
  paral[2][0] = wingl -> cell_win -> cparam[11] * cang[1];
  tmp = (cang[0] - cang[1]*cang[2]) / sang[2];
  paral[2][1] = wingl -> cell_win -> cparam[11] * tmp;
  paral[2][2] = wingl -> cell_win -> cparam[11] * sqrt(sang[1]*sang[1] - tmp*tmp);
  ColRGBA null; // Not used
  wingl -> cell_win -> slab_vol = draw_cuboid (! wingl -> cell_win -> cut_this_slab, SLABS, 0, rot,
                                               vec3(wingl -> cell_win -> cparam[6], wingl -> cell_win -> cparam[7], wingl -> cell_win -> cparam[8]),
                                               paral, null, wingl -> cell_win -> slab_alpha);

  vec3_t at, atc;
  vec3_t ps[8];
  vec3_t pn[6];
  vec3_t cat = vec3(wingl -> cell_win -> cparam[6], wingl -> cell_win -> cparam[7], wingl -> cell_win -> cparam[8]);
  l = 0;
  float pmax[3];

  for (i=-1; i<2; i+=2)
  {
    for (j=-1; j<2; j+=2)
    {
      for (k=-1; k<2; k+=2)
      {
        ps[l].x = 0.5*(i*paral[0][0] + j*paral[1][0] + k*paral[2][0]);
        ps[l].y = 0.5*(i*paral[0][1] + j*paral[1][1] + k*paral[2][1]);
        ps[l].z = 0.5*(i*paral[0][2] + j*paral[1][2] + k*paral[2][2]);
        ps[l] = m4_mul_pos (rot, ps[l]);
        ps[l]= v3_add (ps[l], cat);
        if (l == 0)
        {
          pmax[0] = ps[l].x;
          pmax[1] = ps[l].y;
          pmax[2] = ps[l].z;
        }
        else
        {
          pmax[0] = max(pmax[0],ps[l].x);
          pmax[1] = max(pmax[1],ps[l].y);
          pmax[2] = max(pmax[2],ps[l].z);
        }
        l ++;
      }
    }
  }

  pn[0] = get_normal (ps[1], ps[0], ps[2]);
  pn[1] = get_normal (ps[1], ps[0], ps[4]);
  pn[2] = get_normal (ps[2], ps[0], ps[4]);
  pn[3] = get_normal (ps[3], ps[7], ps[5]);
  pn[4] = get_normal (ps[3], ps[7], ps[6]);
  pn[5] = get_normal (ps[5], ps[7], ps[6]);

  ps[0] = v3_sub (ps[0], cat);
  ps[7] = v3_sub (ps[7], cat);

  for (i=0; i<proj_gl -> nspec; i++) wingl -> cell_win -> slab_lot[i] = 0;
  wingl -> cell_win -> slab_atoms = 0;
  float val, vbl;
  atom slab_center;
  distance at_slab;
  slab_center.x = cat.x;
  slab_center.y = cat.y;
  slab_center.z = cat.z;
  for (i=0; i<proj_gl->natomes; i++)
  {
    at_slab = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[0][i], & slab_center);
    at = vec3(slab_center.x+at_slab.x, slab_center.y+at_slab.y, slab_center.z+at_slab.z);
    if (at.x <= pmax[0] && at.y <= pmax[1] && at.z <= pmax[2])
    {
      m = 0;
      atc = v3_sub (at, cat);
      for (j=0; j<6; j++)
      {
        k = (j < 3) ? 0 : 7;
        val = v3_dot (ps[k], pn[j]);
        vbl = v3_dot (atc, pn[j]);
        if (fabs(vbl) < fabs(val))
        {
          m ++;
        }
        else
        {
          break;
        }
      }
      if (m == 6 && (! at_slab.pbc || wingl -> cell_win -> slab_pbc))
      {
        wingl -> cell_win -> slab_atoms ++;
        l = proj_gl -> atoms[0][i].sp;
        wingl -> cell_win -> slab_lot[l] ++;
        if (wingl -> cell_win -> cut_this_slab) process_selected_atom (proj_gl, wingl, i, 0, 0, 0);
      }
    }
  }
}

/*!
  \fn void cylinder_slab (mat4_t rot)

  \brief prepare cylinder slab OpenGL rendering

  \param rot rotation matrix
*/
void cylinder_slab (mat4_t rot)
{
  int i, j, k;
  vec3_t pos_a = vec3(-wingl -> cell_win -> cparam[12]/2.0, 0.0, 0.0);
  vec3_t pos_b = vec3( wingl -> cell_win -> cparam[12]/2.0, 0.0, 0.0);
  pos_a = m4_mul_pos (rot, pos_a);
  pos_b = m4_mul_pos (rot, pos_b);
  vec3_t cat = vec3 (wingl -> cell_win -> cparam[6], wingl -> cell_win -> cparam[7], wingl -> cell_win -> cparam[8]);
  pos_a = v3_add (pos_a, cat);
  pos_b = v3_add (pos_b, cat);
  vec3_t axis = v3_sub (pos_b, pos_a);
  if (! wingl -> cell_win -> cut_this_slab)
  {
    object_3d * slab = g_malloc0 (sizeof*slab);
    object_3d * slab_cap = g_malloc0 (sizeof*slab_cap);
    slab = draw_cylinder (30, 1.0, 1.0);
    slab -> num_instances = (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
    slab -> inst_buffer_size = CYLI_BUFF_SIZE;
    slab -> instances = allocfloat (slab -> num_instances*CYLI_BUFF_SIZE);
    slab_cap = draw_cylinder_cap (50, 1.0, FALSE);
    slab_cap -> num_instances = 2 * slab -> num_instances;
    slab_cap -> inst_buffer_size = CAPS_BUFF_SIZE;
    slab_cap -> instances = allocfloat (slab_cap -> num_instances*CAPS_BUFF_SIZE);
    ColRGBA col;
    col.red = 0.0;
    col.blue = 1.0;
    col.green = 1.0;
    col.alpha = wingl -> cell_win -> slab_alpha;
    vec3_t shift;
    for (i=0; i<plot -> extra_cell[0]+1; i++)
    {
      for (j=0; j<plot -> extra_cell[1]+1; j++)
      {
        for (k=0; k<plot -> extra_cell[2]+1; k++)
        {
          shift.x = i*box_gl -> vect[0][0]+j*box_gl -> vect[1][0]+k*box_gl -> vect[2][0];
          shift.y = i*box_gl -> vect[0][1]+j*box_gl -> vect[1][1]+k*box_gl -> vect[2][1];
          shift.z = i*box_gl -> vect[0][2]+j*box_gl -> vect[1][2]+k*box_gl -> vect[2][2];
          pos_a = v3_add (pos_a, shift);
          pos_b = v3_add (pos_b, shift);
          setup_cylinder_vertice (slab -> instances, pos_a, pos_b, col, wingl -> cell_win -> cparam[13], 1.0, 0.0);
          pos_a = v3_sub (pos_a, shift);
          pos_b = v3_sub (pos_b, shift);
        }
      }
    }
    wingl -> ogl_glsl[SLABS][0][0] = init_shader_program (SLABS, GLSL_CYLINDERS, cylinder_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 6, 1, TRUE, slab);
    g_free (slab);
    nbs = 0;
    for (i=0; i<plot -> extra_cell[0]+1; i++)
    {
      for (j=0; j<plot -> extra_cell[1]+1; j++)
      {
        for (k=0; k<plot -> extra_cell[2]+1; k++)
        {
          shift.x = i*box_gl -> vect[0][0]+j*box_gl -> vect[1][0]+k*box_gl -> vect[2][0];
          shift.y = i*box_gl -> vect[0][1]+j*box_gl -> vect[1][1]+k*box_gl -> vect[2][1];
          shift.z = i*box_gl -> vect[0][2]+j*box_gl -> vect[1][2]+k*box_gl -> vect[2][2];
          pos_a = v3_add (pos_a, shift);
          pos_b = v3_add (pos_b, shift);
          setup_cap_vertice (slab_cap -> instances, pos_a, pos_b, col, wingl -> cell_win -> cparam[13], 1.0);
          setup_cap_vertice (slab_cap -> instances, pos_b, pos_a, col, wingl -> cell_win -> cparam[13], 1.0);
          pos_a = v3_sub (pos_a, shift);
          pos_b = v3_sub (pos_b, shift);
        }
      }
    }
    wingl -> ogl_glsl[SLABS][0][1] = init_shader_program (SLABS, GLSL_CAPS, cap_vertex, NULL, full_color, GL_TRIANGLE_FAN, 5, 1, TRUE, slab_cap);
    g_free (slab_cap);
  }
  wingl -> cell_win -> slab_vol = pi*pow(wingl -> cell_win -> cparam[13], 2)*wingl -> cell_win -> cparam[12];
  for (i=0; i<proj_gl -> nspec; i++) wingl -> cell_win -> slab_lot[i] = 0;
  wingl -> cell_win -> slab_atoms = 0;
  vec3_t atc, patc;
  atom slab_center;
  distance at_slab;
  slab_center.x = cat.x;
  slab_center.y = cat.y;
  slab_center.z = cat.z;
  for (i=0; i<proj_gl->natomes; i++)
  {
    atc = vec3(proj_gl -> atoms[0][i].x, proj_gl -> atoms[0][i].y, proj_gl -> atoms[0][i].z);
    at_slab = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[0][i], & slab_center);
    if (wingl -> cell_win -> slab_pbc || ! at_slab.pbc)
    {
      atc = vec3(at_slab.x, at_slab.y, at_slab.z);
      patc = v3_proj (atc, axis);
      if (v3_length(patc) <= wingl -> cell_win -> cparam[12]/2.0 && v3_length(v3_sub(patc,atc)) <= wingl -> cell_win -> cparam[13])
      {
        wingl -> cell_win -> slab_atoms ++;
        j = proj_gl -> atoms[0][i].sp;
        wingl -> cell_win -> slab_lot[j] ++;
        if (wingl -> cell_win -> cut_this_slab) process_selected_atom (proj_gl, wingl, i, 0, 0, 0);
      }
    }
  }
}

/*!
  \fn void spherical_slab ()

  \brief prepare spherical slab OpenGL rendering
*/
void spherical_slab ()
{
  int i, j, k, l;
  vec3_t pos;
  if (! wingl -> cell_win -> cut_this_slab)
  {
    object_3d * slab = g_malloc0 (sizeof*slab);
    slab = draw_sphere (50);
    slab -> inst_buffer_size = ATOM_BUFF_SIZE;
    slab -> num_instances = (plot -> extra_cell[0]+1)*(plot -> extra_cell[1]+1)*(plot -> extra_cell[2]+1);
    slab -> instances = allocfloat (slab -> num_instances*ATOM_BUFF_SIZE);
    ColRGBA col;
    col.red = 0.0;
    col.blue = 1.0;
    col.green = 1.0;
    col.alpha = wingl -> cell_win -> slab_alpha;
    double shift[3];
    for (i=0; i<plot -> extra_cell[0]+1; i++)
    {
      for (j=0; j<plot -> extra_cell[1]+1; j++)
      {
        for (k=0; k<plot -> extra_cell[2]+1; k++)
        {
          for (l=0; l<3; l++) shift[l] = i*box_gl -> vect[0][l] + j*box_gl -> vect[1][l] + k*box_gl -> vect[2][l] + wingl -> cell_win -> cparam[l+6];
          pos = vec3(shift[0], shift[1], shift[2]);
          setup_sphere_vertice (slab -> instances, pos, col, wingl -> cell_win -> cparam[14], 1.0);
        }
      }
    }
    wingl -> ogl_glsl[SLABS][0][0] = init_shader_program (SLABS, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, TRUE, slab);
    g_free (slab);
  }
  wingl -> cell_win -> slab_vol = (4.0*pi/3.0)*(pow(wingl -> cell_win -> cparam[14], 3));
  vec3_t cat = vec3 (wingl -> cell_win -> cparam[6], wingl -> cell_win -> cparam[7], wingl -> cell_win -> cparam[8]);
  for (i=0; i<proj_gl -> nspec; i++) wingl -> cell_win -> slab_lot[i] = 0;
  wingl -> cell_win -> slab_atoms = 0;
  atom slab_center;
  distance at_slab;
  slab_center.x = cat.x;
  slab_center.y = cat.y;
  slab_center.z = cat.z;
  for (i=0; i<proj_gl->natomes; i++)
  {
    at_slab = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[0][i], & slab_center);
    if (wingl -> cell_win -> slab_pbc || ! at_slab.pbc)
    {
      if (at_slab.length <= wingl -> cell_win -> cparam[14])
      {
        wingl -> cell_win -> slab_atoms ++;
        j = proj_gl -> atoms[0][i].sp;
        wingl -> cell_win -> slab_lot[j] ++;
        if (wingl -> cell_win -> cut_this_slab) process_selected_atom (proj_gl, wingl, i, 0, 0, 0);
      }
    }
  }
}

/*!
  \fn void create_slab_lists (project * this_proj)

  \brief prepare slab(s) OpenGL rendering

  \param this_proj the target project
*/
void create_slab_lists (project * this_proj)
{
  wingl = this_proj -> modelgl;
  proj_gl = this_proj;
  cell_gl = & proj_gl -> cell;
  plot = wingl -> anim -> last -> img;
  step = plot -> step;
  box_gl = (cell_gl -> npt) ? & cell_gl -> box[step] : & cell_gl -> box[0];
  cleaning_shaders (wingl, SLABS);
  if (wingl -> cell_win && (wingl -> cell_win -> slab_show || wingl -> cell_win -> cut_this_slab))
  {
    if (! wingl -> cell_win -> cut_this_slab)
    {
      wingl -> n_shaders[SLABS][0] = 1;
      if (wingl -> cell_win -> slab_type == 1) wingl -> n_shaders[SLABS][0] ++;
      wingl -> ogl_glsl[SLABS][0] = g_malloc0 (wingl -> n_shaders[SLABS][0]*sizeof*wingl -> ogl_glsl[SLABS][0]);
      nbs = nbl = 0;
    }
    mat4_t rot = m4_rotation_xyz (wingl -> cell_win -> cparam[18], wingl -> cell_win -> cparam[19], wingl -> cell_win -> cparam[20]);
    switch (wingl -> cell_win -> slab_type)
    {
      case 0:
        cuboid_slab (rot);
        break;
      case 1:
        cylinder_slab (rot);
        break;
      case 2:
        spherical_slab ();
        break;
    }
    if (! wingl -> cell_win -> cut_this_slab) create_slab_info (proj_gl);
  }
  wingl -> create_shaders[SLABS] = FALSE;
}

/*!
  \fn void create_volumes_lists ()

  \brief prepare volume(s) OpenGL rendering
*/
void create_volumes_lists ()
{
  cleaning_shaders (wingl, VOLMS);
  int i, j, k, l, m;
  wingl -> n_shaders[VOLMS][step] = 0;
  for (i=0; i<FILLED_STYLES; i++) if (plot -> show_vol[i]) wingl -> n_shaders[VOLMS][step] ++;
  if (wingl -> adv_bonding[0])
  {
    for (i=0; i<FILLED_STYLES; i++)
    {
      if (plot -> fm_show_vol[0][i])
      {
        for (j=0; j<proj_gl -> coord -> totcoord[2]; j++) if (plot -> fm_show_vol[0][i][j]) wingl -> n_shaders[VOLMS][step] ++;
      }
      if (plot -> fm_show_vol[1][i])
      {
        for (j=0; j<proj_gl -> coord -> totcoord[3]; j++) if (plot -> fm_show_vol[1][i][j]) wingl -> n_shaders[VOLMS][step] += proj_gl -> modelfc -> mols[step][j].multiplicity;
      }
    }
  }
  if (wingl -> n_shaders[VOLMS][step])
  {
    wingl -> ogl_glsl[VOLMS][step] = g_malloc0 (wingl -> n_shaders[VOLMS][step]*sizeof*wingl -> ogl_glsl[VOLMS][step]);
    mat4_t rot;
    vec3_t bx;
    double paral[3][3];
    m = 0;
    if (wingl -> adv_bonding[1])
    {
      for (i=0; i<FILLED_STYLES; i++)
      {
        if (plot -> fm_show_vol[1][i])
        {
          for (j=0; j<proj_gl -> coord -> totcoord[3]; j++)
          {
            if (plot -> fm_show_vol[1][i][j])
            {
              for (k=0; k<proj_gl -> modelfc -> mols[step][j].multiplicity; k++)
              {
                l = proj_gl -> modelfc -> mols[step][j].fragments[k];
                rot = m4_rotation_anti_xyz (wingl -> frag_box[i][step][l][3], wingl -> frag_box[i][step][l][4], wingl -> frag_box[i][step][l][5]);
                paral[0][0] = wingl -> frag_box[i][step][l][0];
                paral[0][1] = 0.0;
                paral[0][2] = 0.0;
                paral[1][0] = 0.0;
                paral[1][1] = wingl -> frag_box[i][step][l][1];
                paral[1][2] = 0.0;
                paral[2][0] = 0.0;
                paral[2][1] = 0.0;
                paral[2][2] = wingl -> frag_box[i][step][l][2];
                bx = m4_mul_coord (rot, vec3(wingl -> frag_box[i][step][l][6], wingl -> frag_box[i][step][l][7], wingl -> frag_box[i][step][l][8]));
                draw_cuboid (TRUE, VOLMS, m, rot, bx, paral, plot -> fm_vol_col[1][i][j], plot -> fm_vol_col[1][i][j].alpha);
                m ++;
              }
            }
          }
        }
      }
    }
    if (wingl -> adv_bonding[0])
    {
      for (i=0; i<FILLED_STYLES; i++)
      {
        if (plot -> fm_show_vol[0][i])
        {
          for (j=0; j<proj_gl -> coord -> totcoord[2]; j++)
          {
            if (plot -> fm_show_vol[0][i][j])
            {
              rot = m4_rotation_anti_xyz (wingl -> frag_box[i][step][j][3], wingl -> frag_box[i][step][j][4], wingl -> frag_box[i][step][j][5]);
              paral[0][0] = wingl -> frag_box[i][step][j][0];
              paral[0][1] = 0.0;
              paral[0][2] = 0.0;
              paral[1][0] = 0.0;
              paral[1][1] = wingl -> frag_box[i][step][j][1];
              paral[1][2] = 0.0;
              paral[2][0] = 0.0;
              paral[2][1] = 0.0;
              paral[2][2] = wingl -> frag_box[i][step][j][2];
              bx = m4_mul_coord (rot, vec3(wingl -> frag_box[i][step][j][6], wingl -> frag_box[i][step][j][7], wingl -> frag_box[i][step][j][8]));
              draw_cuboid (TRUE, VOLMS, m, rot, bx, paral, plot -> fm_vol_col[0][i][j], plot -> fm_vol_col[0][i][j].alpha);
              m ++;
            }
          }
        }
      }
    }
    for (i=0; i<FILLED_STYLES; i++)
    {
      if (plot -> show_vol[i])
      {
        rot = m4_rotation_anti_xyz (wingl -> volume_box[i][step][3], wingl -> volume_box[i][step][4], wingl -> volume_box[i][step][5]);
        paral[0][0] = wingl -> volume_box[i][step][0];
        paral[0][1] = 0.0;
        paral[0][2] = 0.0;
        paral[1][0] = 0.0;
        paral[1][1] = wingl -> volume_box[i][step][1];
        paral[1][2] = 0.0;
        paral[2][0] = 0.0;
        paral[2][1] = 0.0;
        paral[2][2] = wingl -> volume_box[i][step][2];
        bx = m4_mul_coord (rot, vec3(wingl -> volume_box[i][step][6], wingl -> volume_box[i][step][7], wingl -> volume_box[i][step][8]));
        draw_cuboid (TRUE, VOLMS, m, rot, bx, paral, plot -> vol_col[i], plot -> vol_col[i].alpha);
        m ++;
      }
    }
  }
  wingl -> create_shaders[VOLMS] = FALSE;
}
