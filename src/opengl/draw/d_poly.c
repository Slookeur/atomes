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
* @file d_poly.c
* @short Functions to prepare the OpenGL rendering of coordination polyhedra
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_poly.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering of coordination polyhedra

*
* List of functions:

  int is_atom_cloned (int at);

  gboolean is_inside (vec3_t p, float * mi, float * ma);
  gboolean is_in_triangle (vec3_t p, vec3_t a, vec3_t b, vec3_t c);
  gboolean check_it (int i, int j, int k, int l);

  void setup_summit (float * vertices, vec3_t s, vec3_t n);
  void setup_triangles (float * vertices, vec3_t sa, vec3_t sb, vec3_t sc);
  void setup_polyhedron (float * vertices, GLfloat ** xyz, int s);
  void setup_tetra (float * vertices, vec3_t a, vec3_t b, vec3_t c, vec3_t d);
  void setup_tetrahedron (float * vertices, GLfloat ** xyz);
  void get_centroid (GLfloat ** xyz, int id);
  void check_triangles (int s, GLfloat ** xyz);
  void prepare_poly_gl (float * vertices, atom at, int c);
  void create_poly_lists ();

  vec3_t get_triangle_normal (vec3_t v1, vec3_t v2, vec3_t v3);

*/

#include "global.h"
#include "glview.h"
#include "color_box.h"
#include "dlp_field.h"

gboolean * do_gl;
vec3_t centroid;
ColRGBA pcol;

extern int nbs, nbl, nba;

float the_sign;
float poly_alpha;

/*!
  \fn void setup_summit (float * vertices, vec3_t s, vec3_t n)

  \brief prepare the polygon summit to render

  \param vertices the OpenGL data buffer to fill
  \param s position vector
  \param n normal vector
*/
void setup_summit (float * vertices, vec3_t s, vec3_t n)
{
  int val = nba*POLY_BUFF_SIZE;
  vertices[val]   = s.x;
  vertices[val+1] = s.y;
  vertices[val+2] = s.z;
  vertices[val+3] = n.x;
  vertices[val+4] = n.y;
  vertices[val+5] = n.z;
  vertices[val+6] = pcol.red;
  vertices[val+7] = pcol.green;
  vertices[val+8] = pcol.blue;
  vertices[val+9] = pcol.alpha * poly_alpha;
  nba ++;
}

/*!
  \fn vec3_t get_triangle_normal (vec3_t v1, vec3_t v2, vec3_t v3)

  \brief compute triangle normal vector

  \param v1 1st summit
  \param v2 2nd summit
  \param v3 3rd summit
*/
vec3_t get_triangle_normal (vec3_t v1, vec3_t v2, vec3_t v3)
{
  vec3_t edge_a = v3_sub(v3, v1);
  vec3_t edge_b = v3_sub(v2, v1);
  vec3_t normal = v3_norm (v3_cross(edge_a, edge_b));
  vec3_t plane  = v3_add (v1, v3_add(v2, v3));
  plane = v3_divs (plane, 3.0);
  float sign = 1.0;
  if (v3_dot (v3_sub(plane,centroid), normal) < 0)
  {
    sign = -1.0;
  }
  return v3_muls (normal, sign);
}

/*!
  \fn void setup_triangles (float * vertices, vec3_t sa, vec3_t sb, vec3_t sc)

  \brief setup triangle vertices

  \param vertices
  \param sa 1st summit
  \param sb 2nd summit
  \param sc 3rd summit
*/
void setup_triangles (float * vertices, vec3_t sa, vec3_t sb, vec3_t sc)
{
  vec3_t normal = get_triangle_normal (sa, sb, sc);
  setup_summit (vertices, sa, normal);
  setup_summit (vertices, sb, normal);
  setup_summit (vertices, sc, normal);
}

/*vec3_t get_summit_normal (vec3_t va, vec3_t vb, vec3_t vc, vec3_t vd)
{
  vec3_t ta, tb, tc;
  ta = get_triangle_normal (va, vb, vc);
  tb = get_triangle_normal (va, vb, vd);
  tc = get_triangle_normal (va, vc, vd);
  return v3_norm(v3_add(ta, v3_add(tb, tc)));
}*/

/*!
  \fn void setup_polyhedron (float * vertices, GLfloat ** xyz, int s)

  \brief fill the OpenGL data buffer for a polyhedron to render

  \param vertices the OpenGL buffer data to fill
  \param xyz the summits coordinates
  \param s the number of summits
*/
void setup_polyhedron (float * vertices, GLfloat ** xyz, int s)
{
  int i, j, k, l, n, o, p, q, r;
  vec3_t a, b, c;
  float shift[3];
  poly_alpha = 1.0;
  for (n=0; n<plot -> abc -> extra_cell[0]+1;n++)
  {
    for (o=0; o<plot -> abc -> extra_cell[1]+1; o++)
    {
      for (p=0; p<plot -> abc -> extra_cell[2]+1; p++)
      {
        shift[0]=n*box_gl -> vect[0][0]+o*box_gl -> vect[1][0]+p*box_gl -> vect[2][0];
        shift[1]=n*box_gl -> vect[0][1]+o*box_gl -> vect[1][1]+p*box_gl -> vect[2][1];
        shift[2]=n*box_gl -> vect[0][2]+o*box_gl -> vect[1][2]+p*box_gl -> vect[2][2];
        for (q=0; q<s; q++)
        {
          for (r=0; r<3; r++) xyz[q][r] += shift[r];
        }

        l = 0;
        for (i=0; i<s-2; i++)
        {
          for (j=i+1; j<s-1; j++)
          {
            for (k=j+1; k<s; k++)
            {
              if (do_gl[l])
              {
                a = vec3 (xyz[i][0], xyz[i][1], xyz[i][2]);
                b = vec3 (xyz[j][0], xyz[j][1], xyz[j][2]);
                c = vec3 (xyz[k][0], xyz[k][1], xyz[k][2]);
                setup_triangles (vertices, a, b ,c);
              }
              l ++;
            }
          }
        }
        poly_alpha = 0.5;
        for (q=0; q<s; q++)
        {
          for (r=0; r<3; r++) xyz[q][r] -= shift[r];
        }
      }
    }
  }
}

/*!
  \fn void setup_tetra (float * vertices, vec3_t a, vec3_t b, vec3_t c, vec3_t d)

  \brief fill the OpenGL data buffer for a tetrahedra to render

  \param vertices the OpenGL buffer data to fill
  \param a 1st summit
  \param b 2nd summit
  \param c 3rd summit
  \param d 4th summit
*/
void setup_tetra (float * vertices, vec3_t a, vec3_t b, vec3_t c, vec3_t d)
{
  setup_triangles (vertices, a, b, c);
  setup_triangles (vertices, b, c, d);
  setup_triangles (vertices, a, c, d);
  setup_triangles (vertices, a, b, d);
}

/*!
  \fn void setup_tetrahedron (float * vertices, GLfloat ** xyz)

  \brief fill the OpenGL data buffer for a tetrahedra to render

  \param vertices the OpenGL buffer data to fill
  \param xyz the summits coordinates
*/
void setup_tetrahedron (float * vertices, GLfloat ** xyz)
{
  int n, o, p, q, r;
  float shift[3];
  poly_alpha = 1.0;
  for (n=0; n<plot -> abc -> extra_cell[0]+1;n++)
  {
    for (o=0; o<plot -> abc -> extra_cell[1]+1; o++)
    {
      for (p=0; p<plot -> abc -> extra_cell[2]+1; p++)
      {
        shift[0]=n*box_gl -> vect[0][0]+o*box_gl -> vect[1][0]+p*box_gl -> vect[2][0];
        shift[1]=n*box_gl -> vect[0][1]+o*box_gl -> vect[1][1]+p*box_gl -> vect[2][1];
        shift[2]=n*box_gl -> vect[0][2]+o*box_gl -> vect[1][2]+p*box_gl -> vect[2][2];
        for (q=0; q<4; q++)
        {
          for (r=0; r<3; r++) xyz[q][r] += shift[r];
        }
        setup_tetra (vertices, vec3(xyz[0][0], xyz[0][1], xyz[0][2]),
                               vec3(xyz[1][0], xyz[1][1], xyz[1][2]),
                               vec3(xyz[2][0], xyz[2][1], xyz[2][2]),
                               vec3(xyz[3][0], xyz[3][1], xyz[3][2]));
        poly_alpha = 0.5;
        for (q=0; q<4; q++)
        {
          for (r=0; r<3; r++) xyz[q][r] -= shift[r];
        }
      }
    }
  }
}

/*!
  \fn void get_centroid (GLfloat ** xyz, int id)

  \brief find the barycenter of a polyhedron

  \param xyz the OpenGL buffer data to fill
  \param id the number of summits
*/
void get_centroid (GLfloat ** xyz, int id)
{
  int i;
  centroid.x = centroid.y = centroid.z = 0.0;
  for (i=0; i<id; i++)
  {
    centroid.x += xyz[i][0];
    centroid.y += xyz[i][1];
    centroid.z += xyz[i][2];
  }
  centroid = v3_divs (centroid, id);
}

/*!
  \fn gboolean is_inside (vec3_t p, float * mi, float * ma)

  \brief is this point inside the polyhedron ?

  \param p the position to test
  \param mi the min values in the summits coordinates on each axis
  \param ma the max values in the summits coordinates on each axis
*/
gboolean is_inside (vec3_t p, float * mi, float * ma)
{
  if (p.x > mi[0] && p.x < ma[0])
  {
    if (p.y > mi[1] && p.y < ma[1])
    {
      if (p.z > mi[2] && p.z < ma[2])
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/*!
  \fn gboolean is_in_triangle (vec3_t p, vec3_t a, vec3_t b, vec3_t c)

  \brief is this point inside a triangle ?

  \param p the position to test
  \param a 1st summit of the triangle
  \param b 2nd summit of the triangle
  \param c 3rd summit of the triangle
*/
gboolean is_in_triangle (vec3_t p, vec3_t a, vec3_t b, vec3_t c)
{
  float area = 0.5 * v3_length(v3_cross(v3_sub(a, b), v3_sub(a, c)));
  float alpha = v3_length(v3_cross(v3_sub(p, b), v3_sub(p, c))) / (2.0*area);
  float beta = v3_length(v3_cross(v3_sub(p, c), v3_sub(p, a))) / (2.0*area);
  float gamma = 1 - alpha - beta;
  if ((alpha >= 0.0 && alpha <= 1.0) && (beta >= 0.0 && beta <= 1.0) && (gamma >= 0.0 && gamma <= 1.0))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn gboolean check_it (int i, int j, int k, int l)

  \brief test this atom id ?

  \param i 1st summit atom id
  \param j 2nd summit atom id
  \param k 3rd summit atom id
  \param l atom id to test
*/
gboolean check_it (int i, int j, int k, int l)
{
  if (l != i && l != j && l != k)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn void check_triangles (int s, GLfloat ** xyz)

  \brief check triangle intersection

  \param s number of summits for the polygon
  \param xyz the coordinates of the summits
*/
void check_triangles (int s, GLfloat ** xyz)
{
  int h, i, j, k, l, m, n;
  float d, r;
  float vd, v0;
  vec3_t vi, vj, vk, vl, vm;
  vec3_t u, v, w;
  vec3_t p, pt;
  float min_c[3], min_p[3];
  float max_c[3], max_p[3];

  i = s*(s-1)*(s-2)/6;
  do_gl = allocbool (i);

  for (i=0; i<3; i++)
  {
    min_c[i] = max_c[i] = xyz[0][i];
  }
  for (i=1; i<s; i++)
  {
    for (j=0; j<3; j++)
    {
      min_c[j] = min (min_c[j], xyz[i][j]);
      max_c[j] = max (max_c[j], xyz[i][j]);
    }
  }
  // We will build the polygon using triangles
  // to ensure that we need to draw a particular triangle
  // we need to check:
  // 1) that the planes to be selected do not contains (cover) any atom or summit,
  //    ie. that the projection of the atom coordinates onto the plane do not belong
  //    to the triangle defined by the 3 points.
  // 2) that the planes already selected and described by the 3 points
  // do not intersect with any other couple of edges inside the entire polygon

  // 1)
  /*h = -1;
  for (i=0; i<s-2; i++)
  {
    for (j=i+1; j<s-1; j++)
    {
      for (k=j+1; k<s; k++)
      {
        h ++;
        do_gl[h] = TRUE;
        if (j-i > 1 && k-j > 1)
        {


        // check the plane defined by the 3 points i, j and k
        vi = vec3(xyz[i][0], xyz[i][1], xyz[i][2]);
        vj = vec3(xyz[j][0], xyz[j][1], xyz[j][2]);
        vk = vec3(xyz[k][0], xyz[k][1], xyz[k][2]);
        u = v3_sub (vi, vj);
        v = v3_sub (vi, vk);
        p = v3_norm(v3_cross (u, v));
        for (l=0; l<s; l++)
        {
          if (check_it (i, j, k, l))
          {
            vl = vec3(xyz[l][0], xyz[l][1], xyz[l][2]);
            w = v3_sub (vl, vi);
            pt = v3_sub (vl, v3_muls (p, v3_dot (w, p)));
            if (is_in_triangle (pt, vi, vj, vk))
            {
              do_gl[h] = FALSE;
              break;
            }
          }
        }
        }
      }
    }
  }*/
  // 2)
  h = -1;
  for (i=0; i<s-2; i++)
  {
    for (j=i+1; j<s-1; j++)
    {
      for (k=j+1; k<s; k++)
      {
        h ++;
        do_gl[h] = TRUE;
        if (do_gl[h])
        {
          // check the plane defined by the 3 points i, j and k
          vi = vec3(xyz[i][0], xyz[i][1], xyz[i][2]);
          vj = vec3(xyz[j][0], xyz[j][1], xyz[j][2]);
          vk = vec3(xyz[k][0], xyz[k][1], xyz[k][2]);

          u = v3_sub (vi, vj);
          v = v3_sub (vi, vk);
          p = v3_cross (u, v);
          d = - v3_dot (p, vi);

          // Now lets look for the intersections with all the other couple of edges that are not i, j and k
          for (l=0; l<s-1; l++)
          {
            if (check_it (i, j, k, l))
            {
              vl = vec3(xyz[l][0], xyz[l][1], xyz[l][2]);
              for (m=l+1; m<s; m++)
              {
                if (check_it (i, j, k, m))
                {
                  vm = vec3(xyz[m][0], xyz[m][1], xyz[m][2]);
                  w = v3_sub (vl, vm);
                  vd = - (v3_dot (p, vm) + d);
                  v0 = v3_dot (p, w);
                  r = vd / v0;
                  if (r != 0.0)
                  {
                    // Now we know where the plane (i, j, k) and the ray (l, m) intersect
                    // We do not want to draw the plane (i, j, k) only if
                    // the intersection is located somewhere inside the polygon
                    pt = v3_add (v3_muls(w, r), vm);
                    if (is_inside (pt, min_c, max_c))
                    {
                      for (n=0; n<3; n++)
                      {
                        min_p[n] = min(xyz[l][n], xyz[m][n]);
                        max_p[n] = max(xyz[l][n], xyz[m][n]);
                      }
                      if (is_inside(pt, min_p, max_p))
                      {
                        //g_debug ("DISCARD:: i= %d, summit[%d]= %d, j= %d, summit[%d]= %d, k= %d, summit[%d]= %d",
                        //         i, i, summit[i], j, j, summit[j], k, k, summit[k]);
                        //g_debug ("BECAUSE OF:: l= %d, summit[%d]= %d, m= %d, summit[%d]= %d",
                        //                l, l, summit[l], m, m, summit[m]);
                        do_gl[h] = FALSE;
                        break;
                      }
                    }
                  }
                }
              }
              if (! do_gl[h]) break;
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void prepare_poly_gl (float * vertices, atom at, int c)

  \brief prepare the OpenGL rendering of a polyhedron

  \param vertices the OpenGL data buffer to fill
  \param at the atom origin of the polyhedron
  \param c the coordination (0= total, 1= partial)
*/
void prepare_poly_gl (float * vertices, atom at, int c)
{
  int j, k, l;
  gboolean clones;
  GLfloat ** xyz;
  distance d;
  xyz = allocdfloat (at.numv+1, 3);
  clones = FALSE;
  for (l=0; l < at.numv; l++)
  {
    j = at.vois[l];
    d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & at, & proj_gl -> atoms[step][j]);
    xyz[l][0] = at.x - d.x;
    xyz[l][1] = at.y - d.y;
    xyz[l][2] = at.z - d.z;
    if (d.pbc) clones = TRUE;
  }
  j = at.numv;
  if (j > 1)
  {
    if (j == 3)
    {
      xyz[j][0] = at.x;
      xyz[j][1] = at.y;
      xyz[j][2] = at.z;
    }
    if (plot -> draw_clones || ! clones || plot -> cloned_poly)
    {
      k = at.sp;
      // Set color
      if (pcolorm == 0)
      {
        pcol = plot -> at_color[k];
        l = at.coord[c];
        pcol.alpha = plot -> spcolor[c][k][l].alpha;
      }
      else if (pcolorm < 5)
      {
        l = at.coord[pcolorm - 1];
        if (pcolorm > 2)
        {
          k = 0;
        }
        pcol = plot -> spcolor[pcolorm - 1][k][l];
      }
      else if (pcolorm == 5)
      {
        field_molecule * fmol = get_active_field_molecule_from_model_id (proj_gl, at.id);
        if (fmol)
        {
          l = proj_gl -> atoms[0][at.id].fid;
          k = fmol -> mol -> natoms;
        }
        else
        {
          l = 0;
          k = 1;
        }
        pcol = init_color (l, k);
        pcol.alpha = 0.5;
      }
      else
      {
        pcol = wingl -> custom_map -> colors[step][at.id];
        l = at.coord[1];
        pcol.alpha = plot -> spcolor[1][k][l].alpha;
      }
      /*color[0] = pcol.red;
      color[1] = pcol.green;
      color[2] = pcol.blue;
      color[3] = pcol.alpha;*/
      switch (j)
      {
        case 3:
          // Tetrahedral unit
          // Tétraèdre, assemblage de triangles
          get_centroid (xyz, 4);
          setup_tetrahedron (vertices, xyz);
          break;
        default:
          get_centroid (xyz, j);
          check_triangles (j, xyz);
          setup_polyhedron (vertices, xyz, j);
          break;
      }
    }
  }
  g_free (xyz);
  xyz = NULL;
}

/*!
  \fn int is_atom_cloned (int at)

  \brief does this atom have clone(s) ?

  \param at the atom id to test
*/
int is_atom_cloned (int at)
{
  int i, j, k;
  i=0;
  for (j=0; j<proj_gl -> atoms[step][at].numv; j++)
  {
    k = proj_gl -> atoms[step][at].vois[j];
    if (distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & proj_gl -> atoms[step][at], & proj_gl -> atoms[step][k]).pbc) i ++;
  }
  return i;
}

/*!
  \fn void create_poly_lists ()

  \brief prepare coordination polyhedra(s) OpenGL rendering
*/
void create_poly_lists ()
{
  // The order to draw the polyhedra could be based on the alpha channel
  // from the most transparent to the less transparent
  // However a better way is to used the neighbor list, if 3 atoms are linked thru bonds
  // and that all 3 of them are involved in polyhedra then the one at
  // the center is to be drawn first ... yet to be implemented
  int h, i, j, k, l, m, n, o, p, q;
#ifdef DEBUG
  g_debug ("Poly LIST");
#endif
  cleaning_shaders (wingl, POLYS);
  if (wingl -> init)
  {
    h = 0;
    for (i=0; i<2; i++)
    {
      for (j=0; j<coord_gl -> totcoord[i]; j++)
      {
        if (plot -> show_poly[i])
        {
          if (plot -> show_poly[i][j])
          {
            h++;
          }
        }
      }
    }
    if (h)
    {
      int * npoly[2];
      int ptot = 0;
      for (i=0; i<2; i++)
      {
        npoly[i] = allocint (coord_gl -> totcoord[i]);
        for (j=0; j<coord_gl -> totcoord[i]; j++)
        {
          for (k=0; k < proj_at; k++)
          {
            l = 0;
            for (m=0; m<proj_gl -> atoms[step][k].sp; m++)
            {
              l += coord_gl -> ntg[i][m];
            }
            n = l + proj_gl -> atoms[step][k].coord[i];
            if (n == j && plot -> show_poly[i] && plot -> show_poly[i][n])
            {
              m = proj_gl -> atoms[step][k].coord[1];
              n = proj_gl -> atoms[step][k].sp;
              o = 0;
              for (p=0; p<proj_sp; p++)
              {
                 o += coord_gl -> partial_geo[n][m][p];
              }
              p = (plot -> draw_clones) ? 1 + is_atom_cloned (k) : 1;
              // q is the number of summit of the polyhedra
              // +1 if only a coord 3 to include the central atom
              q = (o == 3) ? o+1: o;
              // Then we need the max number of triangle for this polyedron
              npoly[i][j] += p*(q*(q-1)*(q-2)/6);
            }
          }
          ptot += npoly[i][j]*3;
        }
      }
      if (ptot > 0)
      {
        wingl -> ogl_glsl[POLYS][step] = g_malloc0 (sizeof*wingl -> ogl_glsl[POLYS][step]);
        wingl -> n_shaders[POLYS][step] = 1;
        object_3d * poly = g_malloc0 (sizeof*poly);
        poly -> vert_buffer_size = POLY_BUFF_SIZE;
        poly -> num_vertices = ptot * (plot -> abc -> extra_cell[0]+1)*(plot -> abc -> extra_cell[1]+1)*(plot -> abc -> extra_cell[2]+1);
        poly -> vertices = allocfloat (poly -> vert_buffer_size*poly -> num_vertices);
        nba = 0;
        for (i=0; i<2; i++)
        {
          for (j=0; j<coord_gl -> totcoord[i]; j++)
          {
            if (npoly[i][j] > 0)
            {
              for (m=0; m < proj_at; m++)
              {
                n = 0;
                for (o=0; o<proj_gl -> atoms[step][m].sp; o++)
                {
                  n += coord_gl -> ntg[i][o];
                }
                o = n + proj_gl -> atoms[step][m].coord[i];
                if (o == j && plot -> show_poly[i] && plot -> show_poly[i][o] &&  proj_gl -> atoms[step][m].numv > 1)
                {
                  prepare_poly_gl (poly -> vertices, proj_gl -> atoms[step][m], i);
                }
              }
            }
          }
          g_free (npoly[i]);
        }
        wingl -> ogl_glsl[POLYS][step][0] = init_shader_program (POLYS, GLSL_POLYEDRA, full_vertex, NULL, full_color, GL_TRIANGLES, 3, 1, TRUE, poly);
        g_free (poly);
      }
    }
  }
  wingl -> create_shaders[POLYS] = FALSE;
}
