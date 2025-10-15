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
* @file save_curve.c
* @short Functions to save curves information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_curve.c'
*
* Contains:
*

 - The functions to save curves information in the atomes project file format

*
* List of functions:

  int save_project_curve (FILE * fp, int wid, project * this_proj, int rid, int cid);

  gboolean write_data_layout (FILE * fp, DataLayout * layout);

*/

#include "global.h"
#include "project.h"

/*!
  \fn gboolean write_data_layout (FILE * fp, DataLayout * layout)

  \brief save curve data layout to file

  \param fp the file pointer
  \param layout the data layout to save
*/
gboolean write_data_layout (FILE * fp, DataLayout * layout)
{
  if (fwrite (& layout -> datacolor, sizeof(ColRGBA), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> thickness, sizeof(double), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> dash, sizeof(int), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> glyph, sizeof(int), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> gsize, sizeof(double), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> gfreq, sizeof(int), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> hwidth, sizeof(double), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> hopac, sizeof(double), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> hpos, sizeof(int), 1, fp) != 1) return FALSE;
  if (fwrite (& layout -> aspect, sizeof(int), 1, fp) != 1) return FALSE;
  return TRUE;
}

/*!
  \fn int save_project_curve (FILE * fp, int wid, project * this_proj, int rid, int cid)

  \brief save project curve to file

  \param fp the file pointer
  \param wid the total number of project file in the workspace
  \param this_proj the target project
  \param rid the calculation to save
  \param cid the curve id to save
*/
int save_project_curve (FILE * fp, int wid, project * this_proj, int rid, int cid)
{
  int i, j;
  if (wid > 0)
  {
    if (fwrite (& this_proj -> id, sizeof(int), 1, fp) != 1) return ERROR_RW;
  }
  if (fwrite (& rid, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& cid, sizeof(int), 1, fp) != 1) return ERROR_RW;
#ifdef NEW_ANA
  Curve * this_curve = this_proj -> analysis[rid].curves[cid];
#else
  Curve * this_curve = this_proj -> curves[rid][cid];
#endif // NEW_ANA
  if (fwrite (& this_curve -> displayed, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_curve -> ndata, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (this_curve -> data[0],
              sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  if (fwrite (this_curve -> data[1],
              sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  i = 0;
  if (this_curve -> err != NULL) i = 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (this_curve -> err != NULL)
  {
    if (fwrite (this_curve -> err,
                sizeof(double), this_curve -> ndata, fp) != this_curve -> ndata) return ERROR_RW;
  }

  if (this_curve -> displayed)
  {
    if (fwrite (this_curve -> wsize, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> cmin, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> cmax, sizeof(double), 2, fp) != 2) return ERROR_RW;
    // Title
    if (fwrite (& this_curve -> show_title, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> default_title, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (! this_curve -> default_title)
    {
      if (save_this_string (fp, this_curve -> title) != OK) return ERROR_RW;
    }
    if (fwrite (this_curve -> title_pos, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (save_this_string (fp, this_curve -> title_font) != OK) return ERROR_RW;
    if (fwrite (& this_curve ->  title_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    // Axis
    if (fwrite (this_curve -> axmin, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> axmax, sizeof(double), 2, fp) != 2) return ERROR_RW;
    for (j=0; j<2; j++)
    {
      if (save_this_string (fp, this_curve -> axis_title[j]) != OK) return ERROR_RW;
      if (save_this_string (fp, this_curve -> axis_title_font[j]) != OK) return ERROR_RW;
    }
    if (fwrite (this_curve -> axis_title_x, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> axis_title_y, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> scale, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> axis_defaut_title, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> autoscale, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> majt, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> mint, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> ticks_io, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> ticks_pos, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> majt_size, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> mint_size, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> labels_pos, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> labels_digit, sizeof(int), 2, fp) != 2) return ERROR_RW;
    for (j=0; j<2; j++)
    {
      if (save_this_string (fp, this_curve -> labels_font[j]) != OK) return ERROR_RW;
    }
    if (fwrite (this_curve -> labels_angle, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> labels_shift_x, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> labels_shift_y, sizeof(int), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> show_grid, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    if (fwrite (this_curve -> show_axis, sizeof(gboolean), 2, fp) != 2) return ERROR_RW;
    // Legend
    if (fwrite (& this_curve -> show_legend, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (save_this_string (fp, this_curve -> legend_font) != OK) return ERROR_RW;
    if (fwrite (this_curve -> legend_pos, sizeof(double), 2, fp) != 2) return ERROR_RW;
    if (fwrite (& this_curve -> legend_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> show_legend_box, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> legend_box_dash, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> legend_box_thickness, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> legend_box_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    // Frame
    if (fwrite (& this_curve -> show_frame, sizeof(gboolean), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> frame_type, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> frame_dash, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> frame_thickness, sizeof(double), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> frame_color, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (fwrite (this_curve -> frame_pos, sizeof(this_curve -> frame_pos), 1, fp) != 1) return ERROR_RW;
    // Data
    if (fwrite (& this_curve -> backcolor, sizeof(ColRGBA), 1, fp) != 1) return ERROR_RW;
    if (! write_data_layout (fp, this_curve -> layout)) return ERROR_RW;
    if (fwrite (& this_curve -> draw_id, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> bshift, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (fwrite (& this_curve -> extrac -> extras, sizeof(int), 1, fp) != 1) return ERROR_RW;
    if (this_curve -> extrac -> extras > 0)
    {
      CurveExtra * ctmp = this_curve -> extrac -> first;
      for (i=0; i<this_curve -> extrac -> extras; i++)
      {
        if (fwrite (& ctmp -> id.a, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (fwrite (& ctmp -> id.b, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (fwrite (& ctmp -> id.c, sizeof(int), 1, fp) != 1) return ERROR_RW;
        if (! write_data_layout (fp, ctmp -> layout)) return ERROR_RW;
        if (ctmp -> next != NULL) ctmp = ctmp -> next;
      }
    }
    if (save_this_string (fp, this_curve -> cfile) != OK) return ERROR_RW;
  }
#ifdef DEBUG
//  debugiocurve (this_proj, win, rid, cid, "WRITE");
#endif
  return OK;
}
