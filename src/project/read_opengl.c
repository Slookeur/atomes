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
* @file read_opengl.c
* @short Functions to read the OpenGL window data in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_opengl.c'
*
* Contains:
*

 - The functions to read the OpenGL window data in the atomes project file format

*
* List of functions:

  int read_atom_a (FILE * fp, project * this_proj, int s, int a);
  int read_atom_b (FILE * fp, project * this_proj, int s, int a);
  int read_rings_chains_data (FILE * fp, glwin * view, int type, int rid, int size, int steps);
  int read_this_image_label (FILE * fp, screen_label * label);
  int read_this_box (FILE * fp, box * abc);
  int read_this_axis (FILE * fp, axis * xyz);
  int read_opengl_image (FILE * fp, project * this_proj, image * img, int sid);

*/

#include "global.h"
#include "project.h"
#include "glview.h"
#include "initcoord.h"
#include "preferences.h"

extern gboolean old_la_bo_ax_gr;

/*!
  \fn int read_atom_a (FILE * fp, project * this_proj, int s, int a)

  \brief read atom properties from file (a)

  \param fp the file pointer
  \param this_proj the target project
  \param s the MD step
  \param a the atom number
*/
int read_atom_a (FILE * fp, project * this_proj, int s, int a)
{
  if (fread (& this_proj -> atoms[s][a].id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& this_proj -> atoms[s][a].sp, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& this_proj -> atoms[s][a].x, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& this_proj -> atoms[s][a].y, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& this_proj -> atoms[s][a].z, sizeof(double), 1, fp) != 1) return ERROR_RW;
  //g_debug ("Reading:: step= %d, at= %d, sp[%d]= %d, x[%d]= %f, y[%d]= %f, z[%d]= %f",
  //         s, a+1, a, this_proj -> atoms[s][a].sp, a, this_proj -> atoms[s][a].x, a, this_proj -> atoms[s][a].y, a, this_proj -> atoms[s][a].z);
  return OK;
}

/*!
  \fn int read_atom_b (FILE * fp, project * this_proj, int s, int a)

  \brief read atom properties from file (b)

  \param fp the file pointer
  \param this_proj the target project
  \param s the MD step
  \param a the atom number
*/
int read_atom_b (FILE * fp, project * this_proj, int s, int a)
{
  if (fread (this_proj -> atoms[s][a].show, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fread (this_proj -> atoms[s][a].label, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fread (& this_proj -> atoms[s][a].style, sizeof(int), 1, fp) != 1) return ERROR_RW;
  int i, j, k, l, m;
  int * rings_ij;
  if (this_proj -> modelgl -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (this_proj -> modelgl -> ring_max[i])
      {
        this_proj -> atoms[s][a].rings[i] = g_malloc0 (this_proj -> rsparam[i][1]*sizeof*this_proj -> atoms[s][a].rings[i]);
        for (j=0; j<this_proj -> rsparam[i][1]; j++)
        {
          rings_ij = allocint(this_proj -> modelgl -> num_rings[i][s][j]);
          m = 0;
          for (k=0; k<this_proj -> modelgl -> num_rings[i][s][j]; k++)
          {
            for (l=0; l<j+1; l++)
            {
              if (this_proj -> modelgl -> all_rings[i][s][j][k][l] == a)
              {
                rings_ij[m] = k;
                m ++;
                break;
              }
            }
          }
          this_proj -> atoms[s][a].rings[i][j] = allocint(m+1);
          this_proj -> atoms[s][a].rings[i][j][0] = m;
          for (k=0; k<m; k++) this_proj -> atoms[s][a].rings[i][j][k+1] = rings_ij[k];
          g_free (rings_ij);
        }
      }
    }
  }
  if (this_proj -> modelgl -> chains)
  {
    if (this_proj -> modelgl -> chain_max)
    {
      this_proj -> atoms[s][a].chain = g_malloc0 (this_proj -> csparam[5]*sizeof*this_proj -> atoms[s][a].chain);
      for (j=0; j<this_proj -> csparam[5]; j++)
      {
        rings_ij = allocint(this_proj -> modelgl -> num_chains[s][j]);
        m = 0;
        for (k=0; k<this_proj -> modelgl -> num_chains[s][j]; k++)
        {
          for (l=0; l<j; l++)
          {
            if (this_proj -> modelgl -> all_chains[s][j][k][l] == a)
            {
              m ++;
              rings_ij[m] = k;
              break;
            }
          }
        }
        this_proj -> atoms[s][a].chain[j] = allocint(m + 1);
        this_proj -> atoms[s][a].chain[j][0] = m;
        for (k=0; k<m; k++) this_proj -> atoms[s][a].chain[j][k+1] = rings_ij[k];
        g_free (rings_ij);
      }
    }
  }
  return OK;
}

/*!
  \fn int read_rings_chains_data (FILE * fp, glwin * view, int type, int rid, int size, int steps)

  \brief read rings and chains statistics data from file

  \param fp the file pointer
  \param view the glwin to store the data
  \param type Rings (0) or chains (1)
  \param rid the ring id or 0
  \param size the size of the ring or chain
  \param steps the number of MD steps
*/
int read_rings_chains_data (FILE * fp, glwin * view, int type, int rid, int size, int steps)
{
  int i, j, k;
  int * tmpcoo, * tmpcoord;
  if (! type)
  {
    if (fread (& view -> ring_max[rid], sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (view -> ring_max[rid])
    {
      view -> num_rings[rid] = allocdint (steps, size);
      view -> show_rpoly[rid] = g_malloc0 (steps*sizeof*view -> show_rpoly[rid]);
      view -> all_rings[rid] = g_malloc0 (steps*sizeof*view -> all_rings[rid]);
      tmpcoo = allocint(size);
      for (i=0; i<steps; i++)
      {
        if (fread (view -> num_rings[rid][i], sizeof(int), size, fp) != size) return ERROR_RW;
        view -> all_rings[rid][i] = g_malloc0 (size*sizeof*view -> all_rings[rid][i]);
        view -> show_rpoly[rid][i] = g_malloc0 (size*sizeof*view -> show_rpoly[rid][i]);
        for (j=0; j<size; j++)
        {
          tmpcoo[j] += view -> num_rings[rid][i][j];
          if (view -> num_rings[rid][i][j])
          {
            view -> all_rings[rid][i][j] = allocdint (view -> num_rings[rid][i][j], j+1);
            view -> show_rpoly[rid][i][j] = allocbool (view -> num_rings[rid][i][j]);
            if (fread (view -> show_rpoly[rid][i][j], sizeof(int), view -> num_rings[rid][i][j], fp) != view -> num_rings[rid][i][j]) return ERROR_RW;
            for (k=0; k<view -> num_rings[rid][i][j]; k++)
            {
              if (fread (view -> all_rings[rid][i][j][k], sizeof(int), j+1, fp) != j+1) return ERROR_RW;
            }
          }
        }
      }
      i = 0;
      for (j=0; j<size; j++)
      {
        if (tmpcoo[j]) i++;
      }

      tmpcoord = allocint(i);
      i = 0;
      for (j=0; j<size; j++)
      {
        if (tmpcoo[j])
        {
          tmpcoord[i] = j+1;
          i ++;
        }
      }
      j = 4 + rid;
      gboolean * show_rings = duplicate_bool(i, view -> anim -> last -> img -> show_coord[j]);
      init_opengl_coords (4+rid, i, 1);
      k = 0;
      init_menurings_ (& j, & rid, & i, tmpcoord, & k);
      g_free (tmpcoo);
      g_free (tmpcoord);
      view -> anim -> last -> img -> show_coord[j] = duplicate_bool(i, show_rings);
      g_free (show_rings);
    }
  }
  else
  {
    if (fread (& view -> chain_max, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (view -> chain_max)
    {
      view -> num_chains = allocdint (steps, size);
      view -> all_chains = g_malloc0 (steps*sizeof*view -> all_rings[rid]);
      tmpcoo = allocint(size);
      for (i=0; i<steps; i++)
      {
        if (fread (view -> num_chains[i], sizeof(int), size, fp) != size) return ERROR_RW;
        view -> all_chains[i] = g_malloc0 (size*sizeof*view -> all_chains[i]);
        for (j=0; j<size; j++)
        {
          tmpcoo[j] += view -> num_chains[i][j];
          if (view -> num_chains[i][j])
          {
            view -> all_chains[i][j] = allocdint (view -> num_chains[i][j], j+1);
            for (k=0; k<view -> num_chains[i][j]; k++)
            {
              if (fread (view -> all_chains[i][j][k], sizeof(int), j+1, fp) != j+1) return ERROR_RW;
            }
          }
        }
      }
      i = 0;
      for (j=0; j<size; j++)
      {
        if (tmpcoo[j]) i++;
      }
      tmpcoord = allocint(i);
      i = 0;
      for (j=0; j<size; j++)
      {
        if (tmpcoo[j])
        {
          tmpcoord[i] = j+1;
          i ++;
        }
      }
      j = 9;
      k = 0;
      gboolean * show_chains = duplicate_bool(i, view -> anim -> last -> img -> show_coord[j]);
      init_opengl_coords (j, i, 1);
      init_menurings_ (& j, & k , & i, tmpcoord, & k);
      g_free (tmpcoo);
      g_free (tmpcoord);
      view -> anim -> last -> img -> show_coord[j] = duplicate_bool(i, show_chains);
      g_free (show_chains);
    }
  }
  return OK;
}

/*!
  \fn int read_this_image_label (FILE * fp, screen_label label)

  \brief read image label from file

  \param fp the file pointer
  \param label the target label
*/
int read_this_image_label (FILE * fp, screen_label * label)
{
  if (fread (& label -> position, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& label -> render, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& label -> scale, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& label -> shift, sizeof(double), 3, fp) != 3) return ERROR_RW;
  if (fread (& label -> n_colors, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (label -> n_colors)
  {
    label -> color = g_malloc (label -> n_colors*sizeof*label -> color);
    if (fread (label -> color, sizeof(ColRGBA), label -> n_colors, fp) != label -> n_colors) return ERROR_RW;
  }
  else if (label -> color)
  {
    g_free (label -> color);
    label -> color = NULL;
  }
  label -> font = read_this_string (fp);
  if (label -> font == NULL) return ERROR_RW;
  return OK;
}

/*!
  \fn int read_this_box (FILE * fp, box * abc)

  \brief read OpenGL image box properties to file

  \param fp the file pointer
  \param box the target box data
*/
int read_this_box (FILE * fp, box * abc)
{
  if (fread (& abc -> box, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& abc -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& abc -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& abc -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  if (fread (abc -> extra_cell, sizeof(int), 3, fp) != 3) return ERROR_RW;
  return OK;
}

/*!
  \fn int read_this_axis (FILE * fp, axis * xyz)

  \brief read OpenGL image axis properties to file

  \param fp the file pointer
  \param axis the target axis data
*/
int read_this_axis (FILE * fp, axis * xyz)
{
  if (fread (& xyz -> axis, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& xyz -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& xyz -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (& xyz  -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  if (fread (& xyz -> t_pos, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& xyz -> length, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fread (xyz -> c_pos, sizeof(double), 3, fp) != 3) return ERROR_RW;
  gboolean val;
  int i;
  if (fread (& val, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (val)
  {
    xyz -> color = g_malloc (3*sizeof*xyz -> color);
    for (i=0; i<3; i++)
    {
      if (fread (& xyz -> color[i], sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    }
  }
  if (fread (& xyz -> labels, sizeof(int), 1, fp) != 1) return ERROR_RW;
  for (i=0; i<3; i++)
  {
    xyz -> title[i] = read_this_string (fp);
    if (xyz -> title[i] == NULL) return ERROR_RW;
  }
  return OK;
}


/*!
  \fn int read_opengl_image (FILE * fp, project * this_proj, image * img, int sid)

  \brief read OpenGL image properties from file

  \param fp the file pointer
  \param this_proj the target project
  \param img the latest image to store the data
  \param sid the number of chemical species
*/
int read_opengl_image (FILE * fp, project * this_proj, image * img, int sid)
{
  int i, j, k, l, m, n;
  gboolean val;

  duplicate_background_data (img -> back, & default_background);
  if (! old_la_bo_ax_gr)
  {
    if (fread (& img -> back -> gradient, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (img -> back -> gradient)
    {
      if (fread (& img -> back -> direction, sizeof(int), 1, fp) != 1) return ERROR_RW;
      if (fread (& img -> back -> position, sizeof(float), 1, fp) != 1) return ERROR_RW;
      if (fread (img -> back -> gradient_color, sizeof(ColRGBA), 2, fp) != 2) return ERROR_RW;
    }
    else
    {
      if (fread (& img -> back -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    }
  }
  else
  {
    img -> back -> gradient = 0;
    img -> back -> position = 0.5;
    if (fread (& img -> back -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  }
  if (fread (img -> color_map, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (img -> color_map[0] > ATOM_MAPS-1)
  {
    img -> color_map[0] -= 10;
    if (fread (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
    this_proj -> modelgl -> custom_map = allocate_color_map (j, this_proj);
    this_proj -> modelgl -> custom_map -> points = j;
    if (fread (& this_proj -> modelgl -> custom_map -> cmax, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_proj -> modelgl -> custom_map -> cmin, sizeof(int), 1, fp) != 1) return ERROR_RW;
    this_proj -> modelgl -> custom_map -> positions = allocfloat (j);
    this_proj -> modelgl -> custom_map -> values = g_malloc (j*sizeof*this_proj -> modelgl -> custom_map -> values);
    if (fread (this_proj -> modelgl -> custom_map -> positions, sizeof(float), j, fp) != j) return ERROR_RW;
    if (fread (this_proj -> modelgl -> custom_map -> values, sizeof(ColRGBA), j, fp) != j) return ERROR_RW;
    j = this_proj -> steps*this_proj -> natomes;
    for (i=0; i<this_proj -> steps; i++)
    {
      if (fread (this_proj -> modelgl -> custom_map -> data[i], sizeof(float), this_proj -> natomes, fp) != this_proj -> natomes) return ERROR_RW;
    }
    setup_custom_color_map (NULL, this_proj, FALSE);
  }

  if (fread (& img -> cloned_poly, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;

  if (fread (img -> at_color, sizeof(ColRGBA), sid*2, fp) != sid*2) return ERROR_RW;
  if (fread (img -> sphererad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;
  if (fread (img -> pointrad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;
  if (fread (img -> atomicrad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;

  for (i=0; i<sid*2; i++)
  {
    if (fread (img -> bondrad[i], sizeof(double), 2*sid, fp) != 2*sid) return ERROR_RW;
    if (fread (img -> linerad[i], sizeof(double), 2*sid, fp) != 2*sid) return ERROR_RW;
  }
  if (fread (img -> radall, sizeof(double), 2, fp) != 2) return ERROR_RW;
  if (fread (& img -> draw_clones, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (old_la_bo_ax_gr)
  {
    for (i=0; i<5; i++)
    {
      if (fread (& img -> labels[i].position, sizeof(int), 1, fp) != 1) return ERROR_RW;
    }
    for (i=0; i<5; i++)
    {
      if (fread (& img -> labels[i].render, sizeof(int), 1, fp) != 1) return ERROR_RW;
    }
    for (i=0; i<5; i++)
    {
      if (fread (& img -> labels[i].scale, sizeof(int), 1, fp) != 1) return ERROR_RW;
    }
    if (fread (img -> acl_format, sizeof(int), 2, fp) != 2) return ERROR_RW;
    for (i=0; i<5; i++)
    {
      if (fread (img -> labels[i].shift, sizeof(double), 3, fp) != 3) return ERROR_RW;
      if (fread (& val, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
      if (val)
      {
        if (i < 2)
        {
          j = 2*sid;
        }
        else if (i == 2)
        {
          j = 3;
        }
        else
        {
          j = 1;
        }
        img -> labels[i].n_colors = j;
        img -> labels[i].color = g_malloc0 (j*sizeof*img -> labels[i].color);
        for (k=0; k<j; k++)
        {
          if (fread (& img -> labels[i].color[k], sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
        }
      }
      else
      {
        img -> labels[i].n_colors = 0;
        if (img -> labels[i].color)
        {
          g_free (img -> labels[i].color);
          img -> labels[i].color = NULL;
        }
      }
      img -> labels[i].font = read_this_string (fp);
      if (img -> labels[i].font == NULL) return ERROR_RW;
    }

    if (fread (& img -> mtilt[0], sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    img -> mtilt[1] = img -> mtilt[0];
    if (fread (& img -> mpattern[0], sizeof(int), 1, fp) != 1) return ERROR_RW;
    img -> mpattern[1] = img -> mpattern[0];
    if (fread (& img -> mfactor[0], sizeof(int), 1, fp) != 1) return ERROR_RW;
    img -> mfactor[1] = img -> mfactor[0];
    if (fread (& img -> mwidth[0], sizeof(double), 1, fp) != 1) return ERROR_RW;
    img -> mwidth[1] = img -> mwidth[0];
  }
  else
  {
    for (i=0; i<5; i++)
    {
      if (read_this_image_label (fp, & img -> labels[i])) return ERROR_RW;
    }
    if (fread (img -> acl_format, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (img -> mtilt, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fread (img -> mpattern, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (img -> mfactor, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (img -> mwidth, sizeof(double), 2, fp) != 2) return ERROR_RW;

  }
  if (fread (& img -> m_is_pressed, sizeof(double), 1, fp) != 1) return ERROR_RW;

  // Model box and axis
  if (old_la_bo_ax_gr)
  {
    if (fread (& img -> abc -> box, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> xyz -> axis, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> abc -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> xyz -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> abc -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> xyz -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> abc -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (fread (img -> abc -> extra_cell, sizeof(int), 3, fp) != 3) return ERROR_RW;
    // Axis
    if (fread (& img -> xyz -> t_pos, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> xyz -> length, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (img -> xyz -> c_pos, sizeof(double), 3, fp) != 3) return ERROR_RW;

    if (fread (& val, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (val)
    {
      img -> xyz -> color = g_malloc (3*sizeof*img -> xyz -> color);
      for (i=0; i<3; i++)
      {
        if (fread (& img -> xyz -> color[i], sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
      }
    }
    if (fread (& img -> xyz -> labels, sizeof(int), 1, fp) != 1) return ERROR_RW;
    for (i=0; i<3; i++)
    {
      img -> xyz -> title[i] = read_this_string (fp);
      if (img -> xyz -> title[i] == NULL) return ERROR_RW;
    }
  }
  else
  {
    // Model box
    if (read_this_box (fp, img -> abc)) return ERROR_RW;
    // Axis
    if (read_this_axis (fp, img -> xyz)) return ERROR_RW;
  }
  // OpenGL
  if (fread (& img -> p_depth, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gnear, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gfar, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gleft, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gright, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gtop, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> gbottom, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> rotation_quaternion, sizeof(vec4_t), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> rotation_mode, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> zoom, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fread (img -> c_shift, sizeof(GLdouble), 2, fp) != 2) return ERROR_RW;
  if (fread (& img -> style, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> quality, sizeof(GLint), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> render, sizeof(GLint), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> l_ghtning.lights, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (img -> l_ghtning.spot != NULL)
  {
    g_free (img -> l_ghtning.spot);
    img -> l_ghtning.spot = NULL;
  }
  img -> l_ghtning.spot = g_malloc0 (img -> l_ghtning.lights*sizeof*img -> l_ghtning.spot);
  for (i=0; i<img -> l_ghtning.lights; i++)
  {
    if (fread (& img -> l_ghtning.spot[i].type, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].fix, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].show, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].position, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].direction, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].intensity, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].attenuation, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fread (& img -> l_ghtning.spot[i].spot_data, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
  }
  if (fread (& img -> m_terial.predefine, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> m_terial.albedo, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
  if (fread (img -> m_terial.param, sizeof(GLfloat), 6, fp) != 6) return ERROR_RW;
  if (fread (& img -> f_g.mode, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> f_g.based, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> f_g.density, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> f_g.depth, sizeof(float), 2, fp) != 2) return ERROR_RW;
  if (fread (& img -> f_g.color, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;

  if (fread (& img -> filled_type, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> step, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& img -> rep, sizeof(int), 1, fp) != 1) return ERROR_RW;

  for (i=0; i<4; i++)
  {
    if (i < 2)
    {
      for (j=0; j<sid; j++)
      {
        if (fread (img -> spcolor[i][j], sizeof(ColRGBA), this_proj -> coord -> ntg[i][j], fp) != this_proj -> coord -> ntg[i][j]) return ERROR_RW;
      }
    }
    else
    {
      if (fread (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
      if (j)
      {
        if (j != this_proj -> coord -> totcoord[i])
        {
          g_warning ("READING OPENGL:: this should not happen !\n i= %d, totcoord[i]= %d, j= %d", i, this_proj -> coord -> totcoord[i], j);
          return ERROR_RW;
        }
        if (fread (img -> spcolor[i][0], sizeof(ColRGBA), this_proj -> coord -> totcoord[i], fp) != this_proj -> coord -> totcoord[i]) return ERROR_RW;
      }
    }
  }
  active_glwin = this_proj -> modelgl;
  active_image = this_proj -> modelgl -> anim -> last -> img;
  active_coord = this_proj -> coord;
  if (fread (& this_proj -> modelgl -> rings, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (this_proj -> modelgl -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (read_rings_chains_data (fp, this_proj -> modelgl, 0, i, this_proj -> rsparam[i][1], this_proj -> steps) != OK) return ERROR_RINGS;
    }
  }
  if (fread (& this_proj -> modelgl -> chains, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (this_proj -> modelgl -> chains)
  {
     if (read_rings_chains_data (fp, this_proj -> modelgl, 1, 0, this_proj -> csparam[5], this_proj -> steps) != OK) return ERROR_CHAINS;
  }

  if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (i)
  {
    this_proj -> modelgl -> create_shaders[VOLMS] = TRUE;
    this_proj -> modelgl -> volumes = TRUE;
    for (i=0; i<FILLED_STYLES; i++)
    {
      this_proj -> modelgl -> atoms_volume[i] = allocdouble (this_proj -> steps);
      this_proj -> modelgl -> atoms_ppvolume[i] = allocdouble (this_proj -> steps);
      if (fread (this_proj -> modelgl -> atoms_volume[i], sizeof(double), this_proj -> steps, fp) != this_proj -> steps) return ERROR_RW;
      if (fread (this_proj -> modelgl -> atoms_ppvolume[i], sizeof(double), this_proj -> steps, fp) != this_proj -> steps) return ERROR_RW;
      this_proj -> modelgl -> volume_box[i] = allocddouble (this_proj -> steps, 9);
      for (j=0; j<this_proj -> steps; j++)
      {
        if (fread (this_proj -> modelgl -> volume_box[i][j], sizeof(double), 9, fp) != 9) return ERROR_RW;
      }
    }
    if (fread (this_proj -> modelgl -> comp_vol, sizeof(gboolean), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (fread (active_image -> show_vol, sizeof(gboolean), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (fread (active_image -> vol_col, sizeof(ColRGBA), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (i)
    {
      for (j=0; j<FILLED_STYLES; j++)
      {
        if (fread (& k, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (k)
        {
          this_proj -> modelgl -> frag_mol_volume[0][j] = allocddouble (this_proj -> steps, i);
          this_proj -> modelgl -> frag_mol_ppvolume[0][j] = allocddouble (this_proj -> steps, i);
          this_proj -> modelgl -> fm_comp_vol[0][j] = allocdbool (this_proj -> steps, i);
          active_image -> fm_show_vol[0][j] = allocbool (i);
          this_proj -> modelgl -> frag_box[j] = alloctdouble (this_proj -> steps, i, 9);
          for (l=0; l<k; l++)
          {
            if (fread (& m, sizeof(int), 1, fp) != 1) return ERROR_RW;
            if (fread (& n, sizeof(int), 1, fp) != 1) return ERROR_RW;
            if (fread (& this_proj -> modelgl -> frag_mol_ppvolume[0][j][m][n], sizeof(double), 1, fp) != 1) return ERROR_RW;
            if (fread (this_proj -> modelgl -> frag_box[j][m][n], sizeof(double), 9, fp) != 9) return ERROR_RW;
            this_proj -> modelgl -> fm_comp_vol[0][j][m][n] = TRUE;
          }
          active_image -> fm_vol_col[0][j] = g_malloc0 (i*sizeof*active_image -> fm_vol_col[0][j]);
          if (fread (active_image -> fm_show_vol[0][j], sizeof(gboolean), i, fp) != i) return ERROR_RW;
          if (fread (active_image -> fm_vol_col[0][j], sizeof(ColRGBA), i, fp) != i) return ERROR_RW;
        }
      }
      if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
      if (i)
      {
        for (j=0; j<FILLED_STYLES; j++)
        {
          if (fread (& k, sizeof(int), 1, fp) != 1) return ERROR_RW;
          if (k)
          {
            this_proj -> modelgl -> frag_mol_volume[1][j] = allocddouble (this_proj -> steps, i);
            this_proj -> modelgl -> frag_mol_ppvolume[1][j] = allocddouble (this_proj -> steps, i);
            this_proj -> modelgl -> fm_comp_vol[1][j] = allocdbool (this_proj -> steps, i);
            active_image -> fm_show_vol[1][j] = allocbool (i);
            for (l=0; l<k; l++)
            {
              if (fread (& m, sizeof(int), 1, fp) != 1) return ERROR_RW;
              if (fread (& n, sizeof(int), 1, fp) != 1) return ERROR_RW;
              if (fread (& this_proj -> modelgl -> frag_mol_ppvolume[1][j][m][n], sizeof(double), 1, fp) != 1) return ERROR_RW;
              this_proj -> modelgl -> fm_comp_vol[1][j][m][n] = TRUE;
            }
            if (fread (active_image -> fm_show_vol[1][j], sizeof(gboolean), i, fp) != i) return ERROR_RW;
            active_image -> fm_vol_col[1][j] = g_malloc0 (i*sizeof*active_image -> fm_vol_col[1][j]);
            if (fread (active_image -> fm_vol_col[1][j], sizeof(ColRGBA), i, fp) != i) return ERROR_RW;
          }
        }
      }
    }
  }

  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j< this_proj -> natomes; j++)
    {
      if (read_atom_b (fp, this_proj, i, j) != OK) return ERROR_ATOM_B;
    }
  }
  // Finally selection lists, bonds, angles and dihedrals
  for (i=0; i<2; i++)
  {
    if (fread (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (j)
    {
      for (k=0; k<j; k++)
      {
        if (fread (& l, sizeof(int), 1, fp) != 1) return ERROR_RW;
        process_selected_atom (this_proj, this_proj -> modelgl, l, 0, 0, i);
      }
      update_all_selections (this_proj -> modelgl, i);
      if (img -> selected[i] -> selected >= 2 && img -> selected[i] -> selected <= 20)
      {
        j = num_bonds (img -> selected[i] -> selected);
        if (fread (img -> selected[i] -> selected_bonds, sizeof(int), j, fp) != j) return ERROR_RW;
        if (img -> selected[i] -> selected >= 3)
        {
          j = num_angles (img -> selected[i] -> selected);
          if (fread (img -> selected[i] -> selected_angles, sizeof(int), j, fp) != j) return ERROR_RW;
          if (img -> selected[i] -> selected >= 4 && img -> selected[i] -> selected <= 10)
          {
            j = num_dihedrals (img -> selected[i] -> selected);
            if (fread (img -> selected[i] -> selected_dihedrals, sizeof(int), j, fp) != j) return ERROR_RW;
          }
        }
      }
    }
  }
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, 2);
#ifdef GTK3
  // GTK3 Menu Action To Check
  for (i=0; i<2; i++)
  {
    for (j=0; j<sid; j++)
    {
      if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_spec[i][j]))
      {
        if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_spec[i][j]) != img -> show_atom[i][j])
        {
          gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_spec[i][j], img -> show_atom[i][j]);
        }
      }
    }
  }
  for (i=0; i<10; i++)
  {
    if (this_proj -> modelgl -> ogl_poly[0][i] != NULL)
    {
      for (j=0; j<this_proj -> coord -> totcoord[i]; j++)
      {
        if (i < 2 || (i > 3 && i < 9))
        {
          if (this_proj -> modelgl -> ogl_poly[0][i][j] != NULL)
          {
            if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_poly[0][i][j]))
            {
              if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_poly[0][i][j]) != img -> show_poly[i][j])
              {
                gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_poly[0][i][j], img -> show_poly[i][j]);
              }
            }
          }
        }
      }
    }
    if (this_proj -> modelgl -> ogl_geom[0][i] != NULL)
    {
      for (j=0; j<this_proj -> coord -> totcoord[i]; j++)
      {
        if (this_proj -> modelgl -> ogl_geom[0][i][j] != NULL)
        {
          if (GTK_IS_WIDGET(this_proj -> modelgl -> ogl_geom[0][i][j]))
          {
            if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][i][j]) != img -> show_coord[i][j])
            {
              gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_geom[0][i][j], img -> show_coord[i][j]);
            }
          }
        }
      }
    }
  }
  show_the_widgets (this_proj -> modelgl -> ogl_coord[0]);
  update_all_menus (this_proj -> modelgl, this_proj -> natomes);
#endif
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, 2);
#ifdef GTK4
  for (i=0; i<2; i++)
  {
    for (j=0; j<sid; j++)
    {
      if (! img -> show_atom[i][j])
      {
        show_hide_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[i][j]);
      }
    }
  }
  for (i=0; i<10; i++)
  {
    for (j=0; j<this_proj -> coord -> totcoord[i]; j++)
    {
      if (i < 2 || (i > 3 && i < 9))
      {
        if (img -> show_poly[i][j])
        {
          show_hide_poly (NULL, NULL, & this_proj -> modelgl -> gcid[i][j][i]);
        }
      }
    }
    for (j=0; j<this_proj -> coord -> totcoord[i]; j++)
    {
      if (! img -> show_coord[i][j])
      {
        show_hide_coord (NULL, NULL, & this_proj -> modelgl -> gcid[i][j][i]);
      }
    }
  }
  update_menu_bar (this_proj -> modelgl);
#endif
  return OK;
}
