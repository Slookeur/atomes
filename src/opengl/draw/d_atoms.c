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
* @file d_atoms.c
* @short nctions to prepare the OpenGL rendering atom(s) and clone(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_atoms.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering atom(s) and clone(s)

*
* List of functions:

  int sphere_vertices (int qual);
  int sphere_indices (int qual);
  int find_atom_vertices (gboolean to_pick);
  int find_clone_vertices (gboolean to_pick);

  float get_sphere_radius (int style, int sp, int ac, int sel);

  void setup_sphere_vertice (float * vertices, vec3_t pos, ColRGBA col, float rad, float alpha);
  void setup_this_atom (int style, gboolean to_pick, gboolean picked, atom * at, int ac, float * vert, float al);
  void setup_atom_vertices (int style, gboolean to_pick, float * vertices);
  void prepare_clone (int style, gboolean to_pick, int picked, atom at, atom bt, float x, float y, float z, float * vertices);
  void setup_clone_vertices (int style, gboolean to_pick, float * vertices);
  void atom_positions_colors_and_sizes (int style, gboolean to_pick, float * instances);
  void create_atom_lists (gboolean to_pick);

  ColRGBA get_atom_color (int i, int j, double al, int picked, gboolean to_pick);

  object_3d * draw_sphere (int quality);

*/

#include "global.h"
#include "glview.h"
#include "ogl_shading.h"
#include "dlp_field.h"
#include "preferences.h"

int atom_id;
int gColorID[3];
int all_styles[NUM_STYLES];

/*!
  \fn ColRGBA get_atom_color (int i, int j, double al, int picked, gboolean to_pick)

  \brief get atom (or clone) color

  \param i chemical species
  \param j the atom id
  \param al opacity (atom: 1.0, clone: 0.5)
  \param picked is the atom (clone) selected (1) or not (0)
  \param to_pick to pick (1) or to draw (0)
*/
ColRGBA get_atom_color (int i, int j, double al, int picked, gboolean to_pick)
{
  // i = atom spec, j = atom id (for coordination maps), al = alpha
  int s = i;
  int m;
  ColRGBA colo;
  if (to_pick)
  {
    gColorID[0]++;
    if(gColorID[0] > 255)
    {
      gColorID[0] = 0;
      gColorID[1]++;
      if(gColorID[1] > 255)
      {
        gColorID[1] = 0;
        gColorID[2]++;
      }
    }
    wingl -> color_to_pick[wingl -> to_be_picked] = gColorID[0] + 256*gColorID[1] + 256*256*gColorID[2];
    colo.red = gColorID[0]/255.0;
    colo.green = gColorID[1]/255.0;
    colo.blue = gColorID[2]/255.0;
    colo.alpha = 1.0;
    wingl -> to_be_picked ++;
  }
  else if (picked)
  {
    if (field_color)
    {
      colo = init_color (proj_gl -> atoms[step][j].coord[4], num_field_objects);
      colo.alpha = DEFAULT_OPACITY*0.75;
    }
    else
    {
      colo = plot -> sel_color[(picked == 1) ? 0 : 1];
    }
  }
  else
  {
    if (acolorm == 0)
    {
      colo.red = plot -> at_color[s].red;
      colo.green = plot -> at_color[s].green;
      colo.blue = plot -> at_color[s].blue;
      colo.alpha = al;// * plot -> m_terial.param[5];
    }
    else if (acolorm < 5)
    {
      if (acolorm < 3)
      {
        s -= proj_sp*(s/proj_sp);
      }
      else
      {
        s = 0;
      }
      int k = proj_gl -> atoms[step][j].coord[acolorm -1];
      colo.red = plot -> spcolor[acolorm - 1][s][k].red;
      colo.green = plot -> spcolor[acolorm - 1][s][k].green;
      colo.blue = plot -> spcolor[acolorm - 1][s][k].blue;
      colo.alpha = al;// * plot -> m_terial.param[5];
    }
    else if (acolorm == 5)
    {
      field_molecule * fmol = get_active_field_molecule_from_model_id (proj_gl, j);
      if (fmol)
      {
        s = proj_gl -> atoms[0][j].fid;
        m = fmol -> mol -> natoms;
      }
      else
      {
        s = 0;
        m = 1;
      }
      colo = init_color (s, m);
      colo.alpha = al;// * plot -> m_terial.param[5];
    }
    else if (acolorm == 6)
    {
      colo.red = wingl -> custom_map -> colors[step][j].red;
      colo.blue = wingl -> custom_map -> colors[step][j].blue;
      colo.green = wingl -> custom_map -> colors[step][j].green;
      colo.alpha = wingl -> custom_map -> colors[step][j].alpha;
    }
  }
  return colo;
}

int nbs, nbl, nba;

/*!
  \fn int sphere_vertices (int qual)

  \brief return the number of OpenGL vertices to render a sphere

  \param qual OpenGL quality
*/
int sphere_vertices (int qual)
{
  return qual * qual;
}

/*!
  \fn int sphere_indices (int qual)

  \brief return the number of OpenGL indices to render a sphere

  \param qual OpenGL quality
*/
int sphere_indices (int qual)
{
  return 2 * qual * (qual - 1);
}

/*!
  \fn object_3d * draw_sphere (int quality)

  \brief OpenGL 3D sphere object rendering

  \param quality OpenGL quality
*/
object_3d * draw_sphere (int quality)
{
  float theta, phi;
  float cos_phi, sin_phi;
  float xPos, yPos, zPos;
  int stack, slice;
  int x, y, z;

  object_3d * new_sphere = g_malloc0 (sizeof*new_sphere);
  new_sphere -> quality = quality;
  new_sphere -> num_vertices = sphere_vertices (quality);
  new_sphere -> vert_buffer_size = 3;
  new_sphere -> vertices = allocfloat (3*new_sphere -> num_vertices);
  new_sphere -> num_indices = sphere_indices (quality);
  new_sphere -> ind_buffer_size = 1;
  new_sphere -> indices = allocint (new_sphere -> num_indices);
  x = 0;
  for (stack=0; stack <= quality-1; stack++)
  {
    phi = pi * stack / (quality-1);
    cos_phi = cos(phi);
    sin_phi = sin(phi);
    for (slice=0; slice <= quality-1; slice++)
    {
      theta = 2.0 * pi * slice / (quality-1);
      xPos = cos(theta) * sin_phi;
      yPos = cos_phi;
      zPos = sin(theta) * sin_phi;
      new_sphere -> vertices[3*x]   = xPos;
      new_sphere -> vertices[3*x+1] = yPos;
      new_sphere -> vertices[3*x+2] = zPos;
      x ++;
    }
  }

  gboolean oddRow = FALSE;
  z = 0;
  for (y = 0; y < quality-1; y++)
  {
    if (!oddRow) // even rows: y == 0, y == 2; and so on
    {
      for (x = 0; x <= quality-1; x++)
      {
        new_sphere -> indices[z] = (y * quality + x);
        new_sphere -> indices[z+1]= ((y + 1) * quality + x);
        z += 2;
      }
    }
    else
    {
      for (x = quality-1; x >= 0; x--)
      {
        new_sphere -> indices[z] = ((y + 1) * quality + x);
        new_sphere -> indices[z+1] = (y * quality + x);
        z += 2;
      }
    }
    oddRow = !oddRow;
  }
  return new_sphere;
}

/*!
  \fn float get_sphere_radius (int style, int sp, int ac, int sel)

  \brief get an atom sphere radius

  \param style rendering style
  \param sp chemical species
  \param ac atom (0) or clone (1)
  \param sel is the atom selected (1) or not (0)
*/
float get_sphere_radius (int style, int sp, int ac, int sel)
{
  if (style == WIREFRAME || style == PUNT)
  {
    return plot -> pointrad[sp + ac*proj_sp] + sel*4.0;
  }
  else if (style == SPACEFILL)
  {
    return plot -> atomicrad[sp + ac*proj_sp] + sel*0.05;
  }
  else if (style == CYLINDERS)
  {
    return plot -> radall[ac] + sel*0.1;
  }
  else
  {
    return plot -> sphererad[sp + ac*proj_sp] + sel*0.05;
  }
}

/*!
  \fn void setup_sphere_vertice (float * vertices, vec3_t pos, ColRGBA col, float rad, float alpha)

  \brief fill the OpenGL data buffer for a atom (or clone) to render

  \param vertices the OpenGL buffer data to fill
  \param pos the position vector
  \param col the color
  \param rad the radius
  \param alpha the opacity (atom: 1.0, clone: 0.5)
*/
void setup_sphere_vertice (float * vertices, vec3_t pos, ColRGBA col, float rad, float alpha)
{
  int l;
  l = nbl*ATOM_BUFF_SIZE;
  vertices[l]   = pos.x;
  vertices[l+1] = pos.y;
  vertices[l+2] = pos.z;
  vertices[l+3] = rad;
  vertices[l+4] = col.red;
  vertices[l+5] = col.green;
  vertices[l+6] = col.blue;
  vertices[l+7] = col.alpha * alpha;
  nbl ++;
}

/*!
  \fn void setup_this_atom (int style, gboolean to_pick, gboolean picked, atom * at, int ac, float * vert, float al)

  \brief prepare the OpenGL rendering data of an atom / clone

  \param style rendering style
  \param to_pick to pick (1) or to draw (0)
  \param picked is the atom selected (1) or not (0)
  \param at the atom to render
  \param ac atom (0) or clone (1)
  \param vert the OpenGL buffer data to fill
  \param al the opacity (atom: 1.0, clone: 0.5)
*/
void setup_this_atom (int style, gboolean to_pick, gboolean picked, atom * at, int ac, float * vert, float al)
{
  int i, j, k;
  float alpha = 1.0;
  float shift[3];
  ColRGBA col = get_atom_color (at -> sp, at -> id, 1.0, picked, to_pick);
  if (at -> sp > proj_sp - 1) at -> sp -= proj_sp;
  float rad = get_sphere_radius ((style == NONE) ? plot -> style : style, at -> sp, ac, (picked) ? 1 : 0);
  for (i=0; i<plot -> abc -> extra_cell[0]+1;i++)
  {
    for (j=0; j<plot -> abc -> extra_cell[1]+1; j++)
    {
      for (k=0; k<plot -> abc -> extra_cell[2]+1; k++)
      {
        shift[0]=i*box_gl -> vect[0][0]+j*box_gl -> vect[1][0]+k*box_gl -> vect[2][0];
        shift[1]=i*box_gl -> vect[0][1]+j*box_gl -> vect[1][1]+k*box_gl -> vect[2][1];
        shift[2]=i*box_gl -> vect[0][2]+j*box_gl -> vect[1][2]+k*box_gl -> vect[2][2];
        at_shift (at, shift);
        setup_sphere_vertice (vert, vec3(at -> x, at -> y, at -> z), col, rad, (to_pick) ? 1.0 : alpha*al);
        at_unshift (at, shift);
        alpha = 0.5;
      }
    }
  }
}

/*!
  \fn int find_atom_vertices (gboolean to_pick)

  \brief find the number of atom(s) to render

  \param to_pick to pick (1) or to draw (0)
*/
int find_atom_vertices (gboolean to_pick)
{
  int i, j;
  j = 0;
  for (i=0; i<proj_at; i++)
  {
    if (in_movie_encoding && plot -> at_data != NULL)
    {
      if (plot -> at_data[i].show[0])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[plot -> at_data[i].style + 1] ++;
        }
        j ++;
      }
    }
    else
    {
      if (proj_gl -> atoms[step][i].show[0])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[proj_gl -> atoms[step][i].style + 1] ++;
        }
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn void setup_atom_vertices (int style, gboolean to_pick, float * vertices)

  \brief prepare an atom OpenGL rendering

  \param style the rendering style
  \param to_pick to pick (1) or to draw (0)
  \param vertices the OpenGL buffer data to fill
*/
void setup_atom_vertices (int style, gboolean to_pick, float * vertices)
{
  int i;
  atom * tmp_a;
  for (i=0; i<proj_at; i++)
  {
    tmp_a = duplicate_atom (& proj_gl -> atoms[step][i]);
    if (in_movie_encoding && plot -> at_data != NULL)
    {
      tmp_a -> show[0] = plot -> at_data[i].show[0];
      tmp_a -> style = plot -> at_data[i].style;
    }
    if (tmp_a -> show[0] && (tmp_a -> style == style || to_pick))
    {
      setup_this_atom (style, to_pick, 0, tmp_a, 0, vertices, 1.0);
    }
    g_free (tmp_a);
  }
}

/*!
  \fn void prepare_clone (int style, gboolean to_pick, int picked, atom at, atom bt, float x, float y, float z, float * vertices)

  \brief prepare the rendering data of a clone

  \param style the rendering style
  \param to_pick to pick (1) or to draw (0)
  \param picked is the clone selected (1) or not (0)
  \param at 1st atom of the cloned chemical bond
  \param bt 2nd atom of the cloned chemical bond
  \param x x position
  \param y y position
  \param z z position
  \param vertices the OpenGL buffer data to fill
*/
void prepare_clone (int style, gboolean to_pick, int picked, atom at, atom bt, float x, float y, float z, float * vertices)
{
  atom * tmp_a = duplicate_atom (& bt);
  tmp_a -> x += x;
  tmp_a -> y += y;
  tmp_a -> z += z;
  tmp_a -> id = at.id;
  tmp_a -> sp = at.sp + proj_sp;
  int sty = at.style;
  if (in_movie_encoding && plot -> at_data != NULL)
  {
    tmp_a -> show[1] = plot -> at_data[at.id].show[1];
    sty = plot -> at_data[at.id].style;
  }
  if (at.show[1] && (sty == style || to_pick))
  {
    setup_this_atom (style, to_pick, picked, tmp_a, 1, vertices, 0.5);
  }
  g_free (tmp_a);
}

/*!
  \fn int find_clone_vertices (gboolean to_pick)

  \brief find the number of clones to render

  \param to_pick to pick (1) or to draw (0)
*/
int find_clone_vertices (gboolean to_pick)
{
  int i, j, k, l;
  l = 0;
  for (i=0; i < wingl -> bonds[step][1]; i++)
  {
    j = wingl -> bondid[step][1][i][0];
    k = wingl -> bondid[step][1][i][1];
    if (in_movie_encoding && plot -> at_data != NULL)
    {
      if (plot -> at_data[j].show[1])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[plot -> at_data[j].style + 1] ++;
        }
        l++;
      }
      if (plot -> at_data[k].show[1])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[plot -> at_data[k].style + 1] ++;
        }
        l++;
      }
    }
    else
    {
      if (proj_gl -> atoms[step][j].show[1])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[proj_gl -> atoms[step][j].style + 1] ++;
        }
        l ++;
      }
      if (proj_gl -> atoms[step][k].show[1])
      {
        if (to_pick)
        {
          all_styles[0] ++;
        }
        else
        {
          all_styles[proj_gl -> atoms[step][k].style + 1] ++;
        }
        l ++;
      }
    }
  }
  return l;
}

/*!
  \fn void setup_clone_vertices (int style, gboolean to_pick, float * vertices)

  \brief find clone(s) position(s), color(s) and prepare the data for the OpenGL rendering

  \param style rendering style
  \param to_pick to pick (1) or to draw (0)
  \param vertices the OpenGL data buffer to fill
*/
void setup_clone_vertices (int style, gboolean to_pick, float * vertices)
{
  int i, j, k;
  for (i=0; i < wingl -> bonds[step][1]; i++)
  {
    j = wingl -> bondid[step][1][i][0];
    k = wingl -> bondid[step][1][i][1];
    prepare_clone (style, to_pick, 0,
                   proj_gl -> atoms[step][j],
                   proj_gl -> atoms[step][k],
                   wingl -> clones[step][i].x,
                   wingl -> clones[step][i].y,
                   wingl -> clones[step][i].z, vertices);
    prepare_clone (style, to_pick, 0,
                   proj_gl -> atoms[step][k],
                   proj_gl -> atoms[step][j],
                  -wingl -> clones[step][i].x,
                  -wingl -> clones[step][i].y,
                  -wingl -> clones[step][i].z, vertices);

  }
}

/*!
  \fn void atom_positions_colors_and_sizes (int style, gboolean to_pick, float * instances)

  \brief find atom(s) position(s), color(s) and prepare the data for the OpenGL rendering

  \param style rendering style
  \param to_pick to pick (1) or to draw (0)
  \param instances the OpenGL data buffer to fill
*/
void atom_positions_colors_and_sizes (int style, gboolean to_pick, float * instances)
{
  setup_atom_vertices (style, to_pick, instances);
  if (to_pick) wingl -> atoms_to_be_picked = wingl -> clones_to_be_picked = wingl -> to_be_picked;
  if (plot -> draw_clones)
  {
    setup_clone_vertices (style, to_pick, instances);
    if (to_pick) wingl -> clones_to_be_picked = wingl -> to_be_picked;
  }
}

/*!
  \fn void create_atom_lists (gboolean to_pick)

  \brief prepare atom(s) and clone(s) OpenGL rendering

  \param to_pick to pick (1) or to draw (0)
*/
void create_atom_lists (gboolean to_pick)
{
  int i, j, k;
  object_3d * atos;
  gboolean sphere = TRUE;

  if (! to_pick)
  {
    cleaning_shaders (wingl, ATOMS);
    wingl -> create_shaders[ATOMS] = FALSE;
  }

  for (i=0; i<NUM_STYLES; i++) all_styles[i] = 0;
  j = find_atom_vertices (to_pick);
  if (plot -> draw_clones) j += find_clone_vertices (to_pick);
#ifdef DEBUG
  g_debug ("Atom LIST:: to_pick= %s, Atom(s) to render= %d", (to_pick) ? "true" : "false", j);
#endif
  if (j > 0)
  {
    // Render atom(s)
    j = (plot -> abc -> extra_cell[0]+1)*(plot -> abc -> extra_cell[1]+1)*(plot -> abc -> extra_cell[2]+1);
    if (! to_pick)
    {
      wingl -> n_shaders[ATOMS][step] = 0;
      for (i=0; i<NUM_STYLES; i++) if (all_styles[i]) wingl -> n_shaders[ATOMS][step] ++;
      wingl -> ogl_glsl[ATOMS][step] = g_malloc0 (wingl -> n_shaders[ATOMS][step]*sizeof*wingl -> ogl_glsl[ATOMS][step]);
    }
    k = 0;
    for (i=0; i<NUM_STYLES; i++)
    {
      sphere = TRUE;
      if (all_styles[i] || to_pick)
      {
        if (! i)
        {
          if (plot -> style == WIREFRAME || plot -> style == PUNT) sphere = FALSE;
        }
        else
        {
          if (i-1 == WIREFRAME || i-1 == PUNT) sphere = FALSE;
        }
        if (sphere)
        {
          atos = draw_sphere (plot -> quality);
        }
        else
        {
          atos = g_malloc0 (sizeof*atos);
          atos -> vert_buffer_size = 3;
          atos -> num_vertices = 1;
          atos -> vertices = allocfloat (3);
          atos -> vertices[0] = atos -> vertices[1] = atos -> vertices[2] = 0.0;
        }
        atos -> num_instances = j*all_styles[i];
        atos -> inst_buffer_size = ATOM_BUFF_SIZE;
        atos -> instances = allocfloat (j*all_styles[i]*ATOM_BUFF_SIZE);
        nbl = 0;
        atom_positions_colors_and_sizes (i-1, to_pick, atos -> instances);
        if (! to_pick)
        {
          if (sphere)
          {
            wingl -> ogl_glsl[ATOMS][step][k] = init_shader_program (ATOMS, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, TRUE, atos);
          }
          else
          {
            wingl -> ogl_glsl[ATOMS][step][k] = init_shader_program (ATOMS, GLSL_POINTS, point_vertex, NULL, point_color, GL_POINTS, 4, 1, FALSE, atos);
          }
        }
        else
        {
          wingl -> ogl_glsl[PICKS][0][0] = init_shader_program (PICKS, GLSL_SPHERES, sphere_vertex, NULL, full_color, GL_TRIANGLE_STRIP, 4, 1, FALSE, atos);
        }
        g_free (atos);
        k ++;
      }
      if (to_pick) break;
    }
  }
}
