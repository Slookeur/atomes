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
* @file d_rings.c
* @short Functions to prepare the OpenGL rendering of rings polyhedra
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_rings.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering of rings polyhedra

*
* List of functions:

  int prepare_rings_gl (float * vertices, int se, int ge, int ta, int id, gboolean go);

  void create_ring_lists ();

*/

#include "global.h"
#include "glview.h"
#include "color_box.h"

extern ColRGBA pcol;
extern int nbs, nbl, nba;
extern void setup_tetrahedron (float * vertices, GLfloat ** xyz);
extern void setup_polyhedron (float * vertices, GLfloat ** xyz, int s);
extern void get_centroid (GLfloat ** xyz, int id);
extern void check_triangles (int s, GLfloat ** xyz);

/*!
  \fn int prepare_rings_gl (float * vertices, int se, int ge, int ta, int id, gboolean go)

  \brief prepare a ring polyhedra OpenGL rendering

  \param vertices the ring atomic positions buffer to fill, if any
  \param se type of ring
  \param ge the coord id for this size of ring
  \param ta the size of the ring
  \param id the ring id number
  \param go render the ring or not (1/0)
*/
int prepare_rings_gl (float * vertices, int se, int ge, int ta, int id, gboolean go)
{
  int i, j, k, l, m;
  gboolean clones;
  gboolean add_poly;
  gboolean old_pbc;
  GLfloat *** xyz;
  distance d;
  atom at, bt;

  xyz = alloctfloat (ta, ta, 3);
  j = -1;

  clones = FALSE;
  j = wingl -> all_rings[se][step][ta-1][id][0];
  l = 0;
  xyz[0][l][0] = proj_gl -> atoms[step][j].x;
  xyz[0][l][1] = proj_gl -> atoms[step][j].y;
  xyz[0][l][2] = proj_gl -> atoms[step][j].z;
  for (i=1; i < ta; i++)
  {
    j = wingl -> all_rings[se][step][ta-1][id][i];
    at = proj_gl -> atoms[step][j];
    bt.x = xyz[0][i-1][0];
    bt.y = xyz[0][i-1][1];
    bt.z = xyz[0][i-1][2];
    d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & at, & bt);
    if (d.pbc) clones = TRUE;
    xyz[0][i][0] = xyz[0][i-1][0] + d.x;
    xyz[0][i][1] = xyz[0][i-1][1] + d.y;
    xyz[0][i][2] = xyz[0][i-1][2] + d.z;
  }
  l = ta;
  m = 0;
  if (clones)
  {
    old_pbc = cell_gl -> pbc;
    cell_gl -> pbc = FALSE;
    add_poly = TRUE;
    while (add_poly)
    {
      for (i=0; i<ta; i++)
      {
        j = wingl -> all_rings[se][step][ta-1][id][i];
        at = proj_gl -> atoms[step][j];
        add_poly = TRUE;
        for (k=0; k<m+1; k++)
        {
          bt.x = xyz[k][i][0];
          bt.y = xyz[k][i][1];
          bt.z = xyz[k][i][2];
          d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & at, & bt);
          if (d.length < 0.01)
          {
            add_poly = FALSE;
            break;
          }
        }
        if (add_poly) break;
      }
      if (add_poly)
      {
        m ++;
        bt.x = xyz[0][i][0];
        bt.y = xyz[0][i][1];
        bt.z = xyz[0][i][2];
        d = distance_3d (cell_gl, (cell_gl -> npt) ? step : 0, & at, & bt);
        for (j=0; j<ta; j++)
        {
          xyz[m][j][0] = xyz[0][j][0] + d.x;
          xyz[m][j][1] = xyz[0][j][1] + d.y;
          xyz[m][j][2] = xyz[0][j][2] + d.z;
        }
      }
    }
    cell_gl -> pbc = old_pbc;
  }
  if (! go) return m+1;

  if (plot -> draw_clones || ! clones || plot -> cloned_poly)
  {
    for (i=0; i<m+1; i++)
    {
      // Set color
      pcol = plot -> spcolor[4+se][0][ge];
      get_centroid (xyz[i], l);
      if (l == 4)
      {
        setup_tetrahedron (vertices, xyz[i]);
      }
      else
      {
        check_triangles (l, xyz[i]);
        setup_polyhedron (vertices, xyz[i], l);
      }
    }
  }
  g_free (xyz);
  xyz = NULL;
  return 0;
}

/*!
  \fn void create_ring_lists ()

  \brief prepare ring(s) polyhedra OpenGL rendering
*/
void create_ring_lists ()
{
  // The order to draw the polyhedra could be based on the alpha channel
  // from the most transparent to the less transparent
  // However a better way is to used the neighbor list, if 3 atoms are linked thru bonds
  // and that all 3 of them are involved in polyhedra then the one at
  // the center is to be drawn first ... yet to be implemented
  int i, j, k, l, m, n;
#ifdef DEBUG
  g_debug ("Ring LIST");
#endif
  cleaning_shaders (wingl, RINGS);
  if (wingl -> rings)
  {
    int rtot = 0;
    int * nrings[5];
    for (i=0; i < 5; i++)
    {
      nrings[i] = g_malloc0 (coord_gl -> totcoord[i+4]*sizeof*nrings[i]);
      for (j=0; j < coord_gl -> totcoord[i+4]; j++)
      {
        // k is the size of the ring in total number of atoms:
        k = coord_gl -> geolist[i+4][0][j];
        l = 0;
        if (plot -> show_poly[i+4][j])
        {
          // Show all rings
          for (m = 0; m < wingl -> num_rings[i][step][k-1]; m++)
          {
            l += prepare_rings_gl (NULL, i, j, k, m, FALSE);
          }
        }
        else if (! in_movie_encoding || plot -> i_rings[i] == NULL)
        {
          // Show selected rings
          for (m=0; m < wingl -> num_rings[i][step][k-1]; m++)
          {
            if (wingl -> show_rpoly[i][step][k-1][m])
            {
              l += prepare_rings_gl (NULL, i, j, k, m, FALSE);
            }
          }
        }
        // m is the number of summit of the polyhedra
        // +1 if only a 3 atom size ring to include a centroid
        m = (k == 3) ? k+1: k;
        // Then we need the max number of triangles for all these polyedron
        nrings[i][j] = l*(m*(m-1)*(m-2)/6);
        rtot += nrings[i][j]*3;
      }
      if (in_movie_encoding && plot -> i_rings[i] != NULL)
      {
        for (l=0; l<plot -> i_rings[i][0][0]; l++)
        {
          j = plot -> i_rings[i][l+1][0];
          k = coord_gl -> geolist[i+4][0][j];
          m = prepare_rings_gl (NULL, i, j, k, plot -> i_rings[i][l+1][1], FALSE);
          if (m)
          {
            n = (k == 3) ? k+1: k;
            nrings[i][j] += (n*(n-1)*(n-2)/6);
            rtot += (n*(n-1)*(n-2)/6)*3;
          }
        }
      }
    }
    if (rtot > 0)
    {
      wingl -> ogl_glsl[RINGS][step] = g_malloc0 (sizeof*wingl -> ogl_glsl[RINGS][step]);
      wingl -> n_shaders[RINGS][step] = 1;
      object_3d * rings = g_malloc0 (sizeof*rings);
      rings -> vert_buffer_size = POLY_BUFF_SIZE;
      rings -> num_vertices = rtot * (plot -> abc -> extra_cell[0]+1)*(plot -> abc -> extra_cell[1]+1)*(plot -> abc -> extra_cell[2]+1);
      rings -> vertices = allocfloat (rings -> vert_buffer_size*rings -> num_vertices);
      nba = 0;
      for (i=0; i < 5; i++)
      {
        for (j=0; j < coord_gl -> totcoord[i+4]; j++)
        {
          if (nrings[i][j])
          {
            k = coord_gl -> geolist[i+4][0][j];
            if (plot -> show_poly[i+4][j])
            {
              for (l=0; l < wingl -> num_rings[i][step][k-1]; l++)
              {
                prepare_rings_gl (rings -> vertices, i, j, k, l, TRUE);
              }
            }
            else if (! in_movie_encoding || plot -> i_rings[i] == NULL)
            {
              for (l=0; l < wingl -> num_rings[i][step][k-1]; l++)
              {
                if (wingl -> show_rpoly[i][step][k-1][l])
                {
                  prepare_rings_gl (rings -> vertices, i, j, k, l, TRUE);
                }
              }
            }
          }
        }
        if (in_movie_encoding && plot -> i_rings[i] != NULL)
        {
          for (l=0; l<plot -> i_rings[i][0][0]; l++)
          {
            j = plot -> i_rings[i][l+1][0];
            k = coord_gl -> geolist[i+4][0][j];
            prepare_rings_gl (rings -> vertices, i, j, k, plot -> i_rings[i][l+1][1], TRUE);
          }
        }
      }
      wingl -> ogl_glsl[RINGS][step][0] = init_shader_program (RINGS, GLSL_POLYEDRA, full_vertex, NULL, full_color, GL_TRIANGLES, 3, 1, TRUE, rings);
      g_free (rings);
    }
  }
  wingl -> create_shaders[RINGS] = FALSE;
}

