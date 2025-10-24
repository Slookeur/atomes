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
* @file read_curve.c
* @short Functions to read curve data in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_curve.c'
*
* Contains:
*

 - The functions to read curve data in the atomes project file format

*
* List of functions:

  int read_project_curve (FILE * fp, int wid, int pid);

  gboolean read_data_layout (FILE * fp, DataLayout * layout);

*/

#include "global.h"
#include "project.h"

extern gboolean version_2_9_and_above;

/*!
  \fn gboolean read_data_layout (FILE * fp, DataLayout * layout)

  \brief read data layout from file

  \param fp the file pointer
  \param layout the data layout to store the data
*/
gboolean read_data_layout (FILE * fp, DataLayout * layout)
{
  if (fread (& layout -> datacolor, sizeof(ColRGBA), 1, fp) != 1) return FALSE;
  if (fread (& layout -> thickness, sizeof(double), 1, fp) != 1) return FALSE;
  if (fread (& layout -> dash, sizeof(int), 1, fp) != 1) return FALSE;
  if (fread (& layout -> glyph, sizeof(int), 1, fp) != 1) return FALSE;
  if (fread (& layout -> gsize, sizeof(double), 1, fp) != 1) return FALSE;
  if (fread (& layout -> gfreq, sizeof(int), 1, fp) != 1) return FALSE;
  if (fread (& layout -> hwidth, sizeof(double), 1, fp) != 1) return FALSE;
  if (fread (& layout -> hopac, sizeof(double), 1, fp) != 1) return FALSE;
  if (fread (& layout -> hpos, sizeof(int), 1, fp) != 1) return FALSE;
  if (fread (& layout -> aspect, sizeof(int), 1, fp) != 1) return FALSE;
  return TRUE;
}

/*!
  \fn int read_project_curve (FILE * fp, int wid, int pid)

  \brief read a project curve from file

  \param fp the file pointer
  \param wid the total number of projects in the workspace
  \param pid the active project id
*/
int read_project_curve (FILE * fp, int wid, int pid)
{
  int i, j;
  int pic, rid, cid;
  if (wid > 0)
  {
    if (fread (& pic, sizeof(int), 1, fp) != 1) return ERROR_RW;
  }
  else
  {
    pic = pid;
  }
  project * this_proj = get_project_by_id (pic);
  if (fread (& rid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& cid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  Curve * this_curve = this_proj -> analysis[rid] -> curves[cid];
  if (rid == SPH)
  {
    this_curve -> name = read_this_string (fp);
  }
  if (fread (& this_curve -> displayed, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fread (& this_curve -> ndata, sizeof(int), 1, fp) != 1) return ERROR_RW;
  this_curve -> data[0] = allocdouble (this_curve -> ndata);
  if (fread (this_curve -> data[0], sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  this_curve -> data[1] = allocdouble (this_curve -> ndata);
  if (fread (this_curve -> data[1], sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (i)
  {
    this_curve -> err = allocdouble (this_curve -> ndata);
    if (fread (this_curve -> err, sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  }

  if (this_curve -> displayed)
  {
    if (fread (this_curve -> wsize, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> cmin, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> cmax, sizeof(double), 2, fp) != 2) return ERROR_RW;
    // Title
    if (fread (& this_curve -> show_title, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> default_title, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (! this_curve -> default_title)
    {
      this_curve -> title = read_this_string (fp);
      if (this_curve -> title == NULL) return ERROR_RW;
    }
    if (fread (this_curve -> title_pos, sizeof(double), 2, fp) != 2) return ERROR_RW;
    this_curve -> title_font = read_this_string (fp);
    if (this_curve -> title_font == NULL) return ERROR_RW;
    if (fread (& this_curve ->  title_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    // Axis
    if (fread (this_curve -> axmin, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> axmax, sizeof(double), 2, fp) != 2) return ERROR_RW;
    for (j=0; j<2; j++)
    {
      this_curve -> axis_title[j] = read_this_string (fp);
      if (this_curve -> axis_title[j] == NULL) return ERROR_RW;
      this_curve -> axis_title_font[j] = read_this_string (fp);
      if (this_curve -> axis_title_font[j] == NULL) return ERROR_RW;
    }
    if (fread (this_curve -> axis_title_x, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> axis_title_y, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> scale, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> axis_defaut_title, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> autoscale, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> majt, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> mint, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> ticks_io, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> ticks_pos, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> majt_size, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> mint_size, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> labels_pos, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> labels_digit, sizeof(int), 2, fp) != 2) return ERROR_RW;
    for (j=0; j<2; j++)
    {
      this_curve -> labels_font[j] = read_this_string (fp);
      if (this_curve -> labels_font[j] == NULL) return ERROR_RW;
    }
    if (fread (this_curve -> labels_angle, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> labels_shift_x, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> labels_shift_y, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> show_grid, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fread (this_curve -> show_axis, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;

    // Legend
    if (fread (& this_curve -> show_legend, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    this_curve -> legend_font = read_this_string (fp);
    if (this_curve -> legend_font == NULL) return ERROR_RW;
    if (fread (this_curve -> legend_pos, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fread (& this_curve -> legend_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> show_legend_box, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> legend_box_dash, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> legend_box_thickness, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> legend_box_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    // Frame
    if (fread (& this_curve -> show_frame, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> frame_type, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> frame_dash, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> frame_thickness, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> frame_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (fread (this_curve -> frame_pos, sizeof(this_curve -> frame_pos), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> backcolor, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    // Data
    this_curve -> layout = g_malloc0 (sizeof*this_curve -> layout);
    if (! read_data_layout (fp, this_curve -> layout)) return ERROR_RW;
    if (fread (& this_curve -> draw_id, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fread (& this_curve -> bshift, sizeof(int), 1, fp) != 1) return ERROR_RW;

    this_curve -> extrac = g_malloc0 (sizeof*this_curve -> extrac);
    if (fread (& this_curve -> extrac -> extras, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (this_curve -> extrac -> extras > 0)
    {
      this_curve -> extrac -> first = g_malloc0 (sizeof*this_curve -> extrac -> first);
      this_curve -> extrac -> last = g_malloc0 (sizeof*this_curve -> extrac -> last);
      CurveExtra * ctmp = this_curve -> extrac -> first;
      for (i=0; i<this_curve -> extrac -> extras; i++)
      {
        if (fread (& ctmp -> id.a, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (fread (& ctmp -> id.b, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (fread (& ctmp -> id.c, sizeof(int), 1, fp) != 1) return ERROR_RW;
        ctmp -> layout = g_malloc0 (sizeof*ctmp -> layout);
        if (! read_data_layout (fp, ctmp -> layout)) return ERROR_RW;
        if (i < this_curve -> extrac -> extras - 1)
        {
          ctmp -> next = g_malloc0 (sizeof*ctmp -> next);
          ctmp -> next -> prev = ctmp;
          ctmp = ctmp -> next;
        }
        else if (i == this_curve -> extrac -> extras - 1)
        {
          this_curve -> extrac -> last = ctmp;
        }
      }
    }
    if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (i == 1)
    {
      this_curve -> cfile = read_this_string (fp);
      if (this_curve -> cfile == NULL) return ERROR_RW;
    }
  }
#ifdef DEBUG
  // debugiocurve (this_proj, win, rid, cid, "READ");
#endif
  return OK;
}
