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
* @file save_opengl.c
* @short Functions to save OpenGL information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_opengl.c'
*
* Contains:
*

 - The functions to save OpenGL information in the atomes project file format

*
* List of functions:

  int save_atom_a (FILE * fp, project * this_proj, int s, int a);
  int save_atom_b (FILE * fp, project * this_proj, int s, int a);
  int save_rings_chains_data (FILE * fp, int type, int size, int steps, int data_max, int ** num_data, gboolean *** show, int **** all_data);
  int write_this_image_label (FILE * fp, screen_label label);
  int write_this_box (FILE * fp, box * abc);
  int write_this_axis (FILE * fp, axis * xyz);
  int save_opengl_image (FILE * fp, project * this_proj, image * img, int sid);

*/

#include "global.h"
#include "project.h"
#include "glwin.h"

/*!
  \fn int save_atom_a (FILE * fp, project * this_proj, int s, int a)

  \brief save atom data to file (a)

  \param fp the file pointer
  \param this_proj the target project
  \param s the MD step
  \param a the atom number
*/
int save_atom_a (FILE * fp, project * this_proj, int s, int a)
{
  if (fwrite (& this_proj -> atoms[s][a].id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].sp, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].x, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].y, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].z, sizeof(double), 1, fp) != 1) return ERROR_RW;
  //g_debug ("Saving:: step= %d, at= %d, sp[%d]= %d, x[%d]= %f, y[%d]= %f, z[%d]= %f",
  //         s, a+1, a, this_proj -> atoms[s][a].sp, a, this_proj -> atoms[s][a].x, a, this_proj -> atoms[s][a].y, a, this_proj -> atoms[s][a].z);
  return OK;
}

/*!
  \fn int save_atom_b (FILE * fp, project * this_proj, int s, int a)

  \brief save atom data to file (b)

  \param fp the file pointer
  \param this_proj the target project
  \param s the MD step
  \param a the atom number
*/
int save_atom_b (FILE * fp, project * this_proj, int s, int a)
{
  if (fwrite (this_proj -> atoms[s][a].show, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fwrite (this_proj -> atoms[s][a].label, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].style, sizeof(int), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_rings_chains_data (FILE * fp, int type, int size, int steps, int data_max, int ** num_data, gboolean *** show, int **** all_data)

  \brief saving rings and chains statistics data to file

  \param fp the file pointer
  \param type Rings (0) or chains (1)
  \param size the number of data to save
  \param steps the MD step
  \param data_max the maximum size of
  \param num_data Id of the objects to save
  \param show Display information pointer
  \param all_data Atoms id of the objects to save
*/
int save_rings_chains_data (FILE * fp, int type, int size, int steps, int data_max, int ** num_data, gboolean *** show, int **** all_data)
{
  int i, j, k;
  if (fwrite (& data_max, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (data_max)
  {
    for (i=0; i<steps; i++)
    {
      if (fwrite (num_data[i], sizeof(int), size, fp) != size) return ERROR_RW;
      for (j=0; j<size; j++)
      {
        if (num_data[i][j])
        {
          //g_debug ("type= %d, steps= %d, j= %d, num_data[i][j]= %d", type, step, j, num_data[i][j]);
          if (! type) if (fwrite (show[i][j], sizeof(int), num_data[i][j], fp) != num_data[i][j]) return ERROR_RW;
          for (k=0; k<num_data[i][j]; k++)
          {
            if (fwrite (all_data[i][j][k], sizeof(int), j+1, fp) != j+1) return ERROR_RW;
          }
        }
      }
    }
  }
  return OK;
}

/*!
  \fn int write_this_image_label (FILE * fp, screen_label label)

  \brief save image label to file

  \param fp the file pointer
  \param label the target label
*/
int write_this_image_label (FILE * fp, screen_label label)
{
  int i;
  if (fwrite (& label.position, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& label.render, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& label.scale, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (label.shift, sizeof(double), 3, fp) != 3) return ERROR_RW;
  if (fwrite (& label.n_colors, sizeof(int), 1, fp) != 1) return ERROR_RW;
  for (i=0; i<label.n_colors; i++)
  {
    if (fwrite (& label.color[i], sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  }
  if (save_this_string (fp, label.font) != OK) return ERROR_RW;
  return OK;
}

/*!
  \fn int write_this_box (FILE * fp, box * abc)

  \brief save OpenGL image box properties to file

  \param fp the file pointer
  \param box the target box data
*/
int write_this_box (FILE * fp, box * abc)
{
  if (fwrite (& abc -> box, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& abc -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& abc -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& abc -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  if (fwrite (abc -> extra_cell, sizeof(int), 3, fp) != 3) return ERROR_RW;
  return OK;
}

/*!
  \fn int write_this_axis (FILE * fp, axis * xyz)

  \brief save OpenGL image axis properties to file

  \param fp the file pointer
  \param axis the target axis data
*/
int write_this_axis (FILE * fp, axis * xyz)
{
  if (fwrite (& xyz -> axis, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& xyz -> rad, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& xyz -> line, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& xyz  -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& xyz -> t_pos, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& xyz -> length, sizeof(double), 1, fp) != 1) return ERROR_RW;
  if (fwrite (xyz -> c_pos, sizeof(double), 3, fp) != 3) return ERROR_RW;
  gboolean val;
  int i;
  if (xyz -> color != NULL)
  {
    val = TRUE;
  }
  else
  {
    val = FALSE;
  }
  if (fwrite (& val, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (val)
  {
    for (i=0; i<3; i++)
    {
      if (fwrite (& xyz -> color[i], sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    }
  }
  if (fwrite (& xyz -> labels, sizeof(int), 1, fp) != 1) return ERROR_RW;
  for (i=0; i<3; i++)
  {
    if (save_this_string (fp, xyz -> title[i]) != OK) return ERROR_RW;
  }
  return OK;
}

/*!
  \fn int save_opengl_image (FILE * fp, project * this_proj, image * img, int sid)

  \brief save OpenGL image properties to file

  \param fp the file pointer
  \param this_proj the target project
  \param img the image that contains the data
  \param sid the number of chemical species
*/
int save_opengl_image (FILE * fp, project * this_proj, image * img, int sid)
{
  int i, j, k, l;

  if (fwrite (& img -> back -> gradient, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (img -> back -> gradient)
  {
    if (fwrite (& img -> back -> direction, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> back -> position, sizeof(float), 1, fp) != 1) return ERROR_RW;
    if (fwrite (img -> back -> gradient_color, sizeof(ColRGBA), 2, fp) != 2) return ERROR_RW;
  }
  else
  {
    if (fwrite (& img -> back -> color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
  }
  if (this_proj -> modelgl -> custom_map != NULL) img -> color_map[0] += 10;
  if (fwrite (img -> color_map, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (this_proj -> modelgl -> custom_map != NULL)
  {
    img -> color_map[0] -= 10;
    colormap * map = this_proj -> modelgl -> custom_map;
    if (fwrite (& map -> points, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& map -> cmax, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& map -> cmin, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (map -> positions, sizeof(float), map -> points, fp) != map -> points) return ERROR_RW;
    if (fwrite (map -> values, sizeof(ColRGBA), map -> points, fp) != map -> points) return ERROR_RW;
    for (i=0; i<this_proj -> steps; i++)
    {
      if (fwrite (map -> data[i], sizeof(float), this_proj -> natomes, fp) != this_proj -> natomes) return ERROR_RW;
    }
  }

  if (fwrite (& img -> cloned_poly, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (img -> at_color, sizeof(ColRGBA), sid*2, fp) != sid*2) return ERROR_RW;
  if (fwrite (img -> sphererad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;
  if (fwrite (img -> pointrad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;
  if (fwrite (img -> atomicrad, sizeof(double), sid*2, fp) != sid*2) return ERROR_RW;

  for (i=0; i<sid*2; i++)
  {
    if (fwrite (img -> bondrad[i], sizeof(double), 2*sid, fp) != 2*sid) return ERROR_RW;
    if (fwrite (img -> linerad[i], sizeof(double), 2*sid, fp) != 2*sid) return ERROR_RW;
  }
  if (fwrite (img -> radall, sizeof(double), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& img -> draw_clones, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;

  for (i=0; i<5; i++)
  {
    if (write_this_image_label(fp, img -> labels[i])) return ERROR_RW;
  }
  if (fwrite (img -> acl_format, sizeof(int), 2, fp) != 2) return ERROR_RW;
  // Measures
  if (fwrite (img -> mtilt, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
  if (fwrite (img -> mpattern, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (fwrite (img -> mfactor, sizeof(int), 2, fp) != 2) return ERROR_RW;
  if (fwrite (img -> mwidth, sizeof(double), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& img -> m_is_pressed, sizeof(double), 1, fp) != 1) return ERROR_RW;

  // Model box
  if (write_this_box (fp, img -> abc)) return ERROR_RW;
  // Axis
  if (write_this_axis (fp, img -> xyz)) return ERROR_RW;

  // OpenGL
  if (fwrite (& img -> p_depth, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gnear, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gfar, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gleft, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gright, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gtop, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> gbottom, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> rotation_quaternion, sizeof(vec4_t), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> rotation_mode, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> zoom, sizeof(GLdouble), 1, fp) != 1) return ERROR_RW;
  if (fwrite (img -> c_shift, sizeof(GLdouble), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& img -> style, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> quality, sizeof(GLint), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> render, sizeof(GLint), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> l_ghtning.lights, sizeof(int), 1, fp) != 1) return ERROR_RW;
  for (i=0; i<img -> l_ghtning.lights; i++)
  {
    if (fwrite (& img -> l_ghtning.spot[i].type, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].fix, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].show, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].position, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].direction, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].intensity, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].attenuation, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& img -> l_ghtning.spot[i].spot_data, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
  }
  if (fwrite (& img -> m_terial.predefine, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> m_terial.albedo, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;
  if (fwrite (img -> m_terial.param, sizeof(GLfloat), 6, fp) != 6) return ERROR_RW;
  if (fwrite (& img -> f_g.mode, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> f_g.based, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> f_g.density, sizeof(float), 1, fp) != 1) return ERROR_RW;
  if (fwrite (img -> f_g.depth, sizeof(float), 2, fp) != 2) return ERROR_RW;
  if (fwrite (& img -> f_g.color, sizeof(vec3_t), 1, fp) != 1) return ERROR_RW;

  if (fwrite (& img -> filled_type, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> step, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& img -> rep, sizeof(int), 1, fp) != 1) return ERROR_RW;
  for (i=0; i<4; i++)
  {
    if (i < 2)
    {
      for (j=0; j<sid; j++)
      {
        if (fwrite (img -> spcolor[i][j], sizeof(ColRGBA), this_proj -> coord -> ntg[i][j], fp) != this_proj -> coord -> ntg[i][j]) return ERROR_RW;
      }
    }
    else
    {
      j = (this_proj -> modelgl -> adv_bonding[i-2] && this_proj -> coord -> totcoord[i]) ? this_proj -> coord -> totcoord[i] : 0;
      if (fwrite (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
      if (j)
      {
        if (fwrite (img -> spcolor[i][0], sizeof(ColRGBA), this_proj -> coord -> totcoord[i], fp) != this_proj -> coord -> totcoord[i]) return ERROR_RW;
      }
    }
  }
  if (fwrite (& this_proj -> modelgl -> rings, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (this_proj -> modelgl -> rings)
  {
    for (i=0; i<5; i++)
    {
      if (save_rings_chains_data (fp, 0, this_proj -> rsparam[i][1], this_proj -> steps,
                                  this_proj -> modelgl -> ring_max[i],
                                  this_proj -> modelgl -> num_rings[i],
                                  this_proj -> modelgl -> show_rpoly[i],
                                  this_proj -> modelgl -> all_rings[i]) != OK) return ERROR_RINGS;
    }
  }
  if (fwrite (& this_proj -> modelgl -> chains, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (this_proj -> modelgl -> chains)
  {
     if (save_rings_chains_data (fp, 1, this_proj -> csparam[5], this_proj -> steps,
                                 this_proj -> modelgl -> chain_max,
                                 this_proj -> modelgl -> num_chains,
                                 NULL,
                                 this_proj -> modelgl -> all_chains) != OK) return ERROR_CHAINS;
  }
  if (this_proj -> modelgl -> volumes)
  {
    i = 1;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    for (i=0; i<FILLED_STYLES; i++)
    {
      if (fwrite (this_proj -> modelgl -> atoms_volume[i], sizeof(double), this_proj -> steps, fp) != this_proj -> steps) return ERROR_RW;
      if (fwrite (this_proj -> modelgl -> atoms_ppvolume[i], sizeof(double), this_proj -> steps, fp) != this_proj -> steps) return ERROR_RW;
      for (j=0; j<this_proj -> steps; j++)
      {
        if (fwrite (this_proj -> modelgl -> volume_box[i][j], sizeof(double), 9, fp) != 9) return ERROR_RW;
      }
    }
    if (fwrite (this_proj -> modelgl -> comp_vol, sizeof(gboolean), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (fwrite (img -> show_vol, sizeof(gboolean), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (fwrite (img -> vol_col, sizeof(ColRGBA), FILLED_STYLES, fp) != FILLED_STYLES) return ERROR_RW;
    if (this_proj -> modelgl -> adv_bonding[0])
    {
      if (fwrite (& this_proj -> coord -> totcoord[2], sizeof(int), 1, fp) != 1) return ERROR_RW;
      for (i=0; i<FILLED_STYLES; i++)
      {
        j = 0;
        for (k=0; k<this_proj -> steps; k++)
        {
          for (l=0; l<this_proj -> coord -> totcoord[2]; l++)
          {
            if (this_proj -> modelgl -> fm_comp_vol[0][i][k][l]) j++;
          }
        }
        if (fwrite (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (j)
        {
          for (k=0; k<this_proj -> steps; k++)
          {
            for (l=0; l<this_proj -> coord -> totcoord[2]; l++)
            {
              if (this_proj -> modelgl -> fm_comp_vol[0][i][k][l])
              {
                if (fwrite (& k, sizeof(int), 1, fp) != 1) return ERROR_RW;
                if (fwrite (& l, sizeof(int), 1, fp) != 1) return ERROR_RW;
                if (fwrite (& this_proj -> modelgl -> frag_mol_ppvolume[0][i][k][l], sizeof(double), 1, fp) != 1) return ERROR_RW;
                if (fwrite (this_proj -> modelgl -> frag_box[i][k][l], sizeof(double), 9, fp) != 9) return ERROR_RW;
              }
            }
          }
          if (fwrite (img -> fm_show_vol[0][i], sizeof(gboolean), this_proj -> coord -> totcoord[2], fp) != this_proj -> coord -> totcoord[2]) return ERROR_RW;
          if (fwrite (img -> fm_vol_col[0][i], sizeof(ColRGBA), this_proj -> coord -> totcoord[2], fp) != this_proj -> coord -> totcoord[2]) return ERROR_RW;
        }
      }
      if (this_proj -> modelgl -> adv_bonding[1])
      {
        if (fwrite (& this_proj -> coord -> totcoord[3], sizeof(int), 1, fp) != 1) return ERROR_RW;
        for (i=0; i<FILLED_STYLES; i++)
        {
          j = 0;
          for (k=0; k<this_proj -> steps; k++)
          {
            for (l=0; l<this_proj -> coord -> totcoord[3]; l++)
            {
              if (this_proj -> modelgl -> fm_comp_vol[1][i][k][l]) j++;
            }
          }
          if (fwrite (& j, sizeof(int), 1, fp) != 1) return ERROR_RW;
          if (j)
          {
            for (k=0; k<this_proj -> steps; k++)
            {
              for (l=0; l<this_proj -> coord -> totcoord[3]; l++)
              {
                if (this_proj -> modelgl -> fm_comp_vol[1][i][k][l])
                {
                  if (fwrite (& k, sizeof(int), 1, fp) != 1) return ERROR_RW;
                  if (fwrite (& l, sizeof(int), 1, fp) != 1) return ERROR_RW;
                  if (fwrite (& this_proj -> modelgl -> frag_mol_ppvolume[1][i][k][l], sizeof(double), 1, fp) != 1) return ERROR_RW;
                }
              }
            }
            if (fwrite (img -> fm_show_vol[1][i], sizeof(gboolean), this_proj -> coord -> totcoord[3], fp) != this_proj -> coord -> totcoord[3]) return ERROR_RW;
            if (fwrite (img -> fm_vol_col[1][i], sizeof(ColRGBA), this_proj -> coord -> totcoord[3], fp) != this_proj -> coord -> totcoord[3]) return ERROR_RW;
          }
        }
      }
      else
      {
        i = 0;
        if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
      }
    }
    else
    {
      i = 0;
      if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    }
  }
  else
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  }

  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j< this_proj -> natomes; j++)
    {
      if (save_atom_b (fp, this_proj, i, j) != OK) return ERROR_ATOM_B;
    }
  }

  // Finally selection lists, bonds, angles and dihedrals
  for (i=0; i<2; i++)
  {
    if (fwrite (& img -> selected[i] -> selected, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (img -> selected[i] -> selected)
    {
      atom_in_selection * at = img -> selected[i] -> first;
      for (j=0; j<img -> selected[i] -> selected; j++)
      {
        if (fwrite (& at -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (at -> next) at = at -> next;
      }
      if (img -> selected[i] -> selected >= 2 && img -> selected[i] -> selected <= 20)
      {
        j = num_bonds (img -> selected[i] -> selected);
        if (fwrite (img -> selected[i] -> selected_bonds, sizeof(int), j, fp) != j) return ERROR_RW;
        if (img -> selected[i] -> selected >= 3)
        {
          j = num_angles (img -> selected[i] -> selected);
          if (fwrite (img -> selected[i] -> selected_angles, sizeof(int), j, fp) != j) return ERROR_RW;
          if (img -> selected[i] -> selected >= 4 && img -> selected[i] -> selected <= 10)
          {
            j = num_dihedrals (img -> selected[i] -> selected);
            if (fwrite (img -> selected[i] -> selected_dihedrals, sizeof(int), j, fp) != j) return ERROR_RW;
          }
        }
      }
    }
  }

  return OK;
}
