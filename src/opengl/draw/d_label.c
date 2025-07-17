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
* @file d_label.c
* @short Functions to prepare the OpenGL rendering of the atomic label(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'd_label.c'
*
* Contains:
*

 - The functions to prepare the OpenGL rendering of the atomic label(s)

*
* List of functions:

  int create_label_lists ();

  void prepare_label (atom at, int id, double al);
  void clean_labels (int id);

  mat4_t create_label_matrices ();

*/

#include "global.h"
#include "glview.h"
#include "interface.h"
#include "dlp_field.h"

extern ColRGBA init_color (int id, int numid);
extern ColRGBA get_atom_color (int i, int j, double al, int picked, gboolean to_pick);
extern float get_sphere_radius (int style, int sp, int ac, int sel);

/*!
  \fn mat4_t create_label_matrices ()

  \brief create label projection matrices
*/
mat4_t create_label_matrices ()
{
  return m4_ortho (0.0, wingl -> view_port.y, 0.0, wingl -> view_port.z, 1.0, 0.0);
}

/*!
  \fn void prepare_label (atom at, int id, double al)

  \brief prepare an atomic label OpenGL rendering

  \param at the atom to label
  \param id the label id
  \param al opacity
*/
void prepare_label (atom at, int id, double al)
{
  int k, l;
  char * str = NULL;
  char * tmp = NULL;
  float shift[3];
  ColRGBA lcol;

  k = at.sp;
  double rad = get_sphere_radius ((at.style == NONE) ? plot -> style : at.style, k, id, (at.pick[0] || at.pick[1]));
  if (at.style == CYLINDERS || (plot -> style == CYLINDERS && at.style == NONE)) rad += (at.pick[0] || at.pick[1]) ? 0.9 : 1.0;
  for (l=0; l<3; l++) shift[l] = plot -> labels[id].shift[l];
  shift[2] += rad;

  if (field_color && (field_object < 7 || field_object > 14))
  {
    lcol = init_color (at.coord[4], num_field_objects);
    lcol.alpha = al*0.75;
  }
  else if (plot -> labels[id].color == NULL)
  {
    lcol = get_atom_color (k+id*proj_sp, at.id, al, 0, FALSE);
  }
  else
  {
    lcol = (plot -> labels[id].n_colors) ? plot -> labels[id].color[0] : plot -> labels[id].color[k];
    lcol.alpha = al;
  }
  switch (plot -> acl_format[id])
  {
    case ELEMENT_NAME:
      str = g_strdup_printf ("%s", exact_name(proj_gl -> chemistry -> element[k]));
      break;
    case SYMBOL:
      str = g_strdup_printf ("%s", exact_name(proj_gl -> chemistry -> label[k]));
      break;
    case SYMBOL_AND_NUM:
      str = g_strdup_printf ("%s%d", exact_name(proj_gl -> chemistry -> label[k]), at.id+1);
      break;
    case NUM:
      str = g_strdup_printf ("%d", at.id+1);
      break;
    case ID_IN_MOLECULE:
      if (field_object == 0)
      {
        str = g_strdup_printf ("%s%d", exact_name(proj_gl -> chemistry -> label[k]), at.id+1);
      }
      else
      {
        str = g_strdup_printf ("%s%d", exact_name(proj_gl -> chemistry -> label[k]), at.coord[4]+1);
      }
      break;
    default:
      str = g_strdup_printf ("%s%d", exact_name(proj_gl -> chemistry -> label[k]), at.id+1);
      break;
  }
  if (id)
  {
    tmp = g_strdup_printf ("%s*", str);
    g_free (str);
    str = g_strdup_printf ("%s", tmp);
    g_free (tmp);
  }
  prepare_string (str, id, lcol, vec3(at.x, at.y, at.z), shift, NULL, NULL, NULL);
  g_free (str);
}

/*!
  \fn void clean_labels (int id)

  \brief clean atomic label shaders

  \param id label id
*/
void clean_labels (int id)
{
  if (plot -> labels[id].list != NULL)
  {
    g_free (plot -> labels[id].list);
    plot -> labels[id].list = NULL;
  }
}

/*!
  \fn int create_label_lists ()

  \brief prepare atomic label(s) OpenGL rendering
*/
int create_label_lists ()
{
  int nshaders = 0;
  int i, j, k;
  float x, y, z;
  atom ato;

#ifdef DEBUG
  g_debug ("Label LIST");
#endif
  cleaning_shaders (wingl, LABEL);
  wingl -> create_shaders[LABEL] = FALSE;
  clean_labels (0);
  clean_labels (1);

  if (in_movie_encoding && plot -> at_data != NULL)
  {
    for (i=0; i<proj_at; i++)
    {
      if (plot -> at_data[i].show[0] && plot -> at_data[i].label[0])
      {
        prepare_label (proj_gl -> atoms[step][i], 0, 1.0);
      }
    }
    if (plot -> draw_clones)
    {
      for (i=0; i < wingl -> bonds[step][1]; i++)
      {
        x = wingl -> clones[step][i].x;
        y = wingl -> clones[step][i].y;
        z = wingl -> clones[step][i].z;
        j = wingl -> bondid[step][1][i][0];
        k = wingl -> bondid[step][1][i][1];
        ato.x = proj_gl -> atoms[step][j].x - x;
        ato.y = proj_gl -> atoms[step][j].y - y;
        ato.z = proj_gl -> atoms[step][j].z - z;
        ato.sp = proj_gl -> atoms[step][j].sp;
        ato.id = k;
        ato.pick[0] = plot -> at_data[k].pick[0];
        ato.pick[1] = plot -> at_data[k].pick[1];
        ato.style = plot -> at_data[k].style;
        if (plot -> at_data[k].show[1] && plot -> at_data[k].label[1]) prepare_label (ato, 1, 0.75);
        ato.x = proj_gl -> atoms[step][k].x + x;
        ato.y = proj_gl -> atoms[step][k].y + y;
        ato.z = proj_gl -> atoms[step][k].z + z;
        ato.sp = proj_gl -> atoms[step][k].sp;
        ato.id = j;
        ato.pick[0] = plot -> at_data[j].pick[0];
        ato.pick[1] = plot -> at_data[j].pick[1];
        ato.style = plot -> at_data[j].style;
        if (plot -> at_data[j].show[1] && plot -> at_data[j].label[1]) prepare_label (ato, 1, 0.75);
      }
    }
  }
  else
  {
    for (i=0; i<proj_at; i++)
    {
      if (proj_gl -> atoms[step][i].show[0] && proj_gl -> atoms[step][i].label[0])
      {
        prepare_label (proj_gl -> atoms[step][i], 0, 1.0);
      }
    }
    if (plot -> draw_clones)
    {
      for (i=0; i < wingl -> bonds[step][1]; i++)
      {
        x = wingl -> clones[step][i].x;
        y = wingl -> clones[step][i].y;
        z = wingl -> clones[step][i].z;
        j = wingl -> bondid[step][1][i][0];
        k = wingl -> bondid[step][1][i][1];
        ato.x = proj_gl -> atoms[step][j].x - x;
        ato.y = proj_gl -> atoms[step][j].y - y;
        ato.z = proj_gl -> atoms[step][j].z - z;
        ato.sp = proj_gl -> atoms[step][k].sp;
        ato.id = k;
        ato.pick[0] = proj_gl -> atoms[step][k].pick[0];
        ato.pick[1] = proj_gl -> atoms[step][k].pick[1];
        ato.style = proj_gl -> atoms[step][k].style;
        if (proj_gl -> atoms[step][k].show[1] && proj_gl -> atoms[step][k].label[1]) prepare_label (ato, 1, 0.75);
        ato.x = proj_gl -> atoms[step][k].x + x;
        ato.y = proj_gl -> atoms[step][k].y + y;
        ato.z = proj_gl -> atoms[step][k].z + z;
        ato.sp = proj_gl -> atoms[step][j].sp;
        ato.id = j;
        ato.pick[0] = proj_gl -> atoms[step][j].pick[0];
        ato.pick[1] = proj_gl -> atoms[step][j].pick[1];
        ato.style = proj_gl -> atoms[step][j].style;
        if (proj_gl -> atoms[step][j].show[1] && proj_gl -> atoms[step][j].label[1]) prepare_label (ato, 1, 0.75);
      }
    }
  }

  if (plot -> labels[0].list != NULL || plot -> labels[1].list != NULL)
  {
    nshaders = 0;
    if (plot -> labels[0].list != NULL) nshaders += (plot -> labels[0].render + 1) * (plot -> labels[0].list -> last -> id + 1);
    if (plot -> draw_clones && plot -> labels[1].list != NULL)
    {
      nshaders += (plot -> labels[1].render + 1) * (plot -> labels[1].list -> last -> id + 1);
    }
    wingl -> ogl_glsl[LABEL][0] = g_malloc0 (nshaders*sizeof*wingl -> ogl_glsl[LABEL][0]);
    for (i=0; i<2; i++) render_all_strings (LABEL, i);
  }
  return nshaders;
}
