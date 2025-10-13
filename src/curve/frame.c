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
* @file frame.c
* @short Functions to draw the frame
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'frame.c'
*
* Contains:
*

 - The functions to draw the frame

*
* List of functions:

  void prep_frame (cairo_t * fr, int da, double ti, ColRGBA dcol);
  void prep_axis_data (project * this_proj, int rid, int cid, int ax);
  void draw_frame (cairo_t * cr, project * this_proj, int rid, int cid);

*/

#include <stdlib.h>
#include <gdk/gdk.h>
#include <cairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn void prep_frame (cairo_t * fr, int da, double ti, ColRGBA dcol)

  \brief draw frame line

  \param fr the cairo drawing context to use for the draw
  \param da dash type
  \param ti line width
  \param dcol color
*/
void prep_frame (cairo_t * fr, int da, double ti, ColRGBA dcol)
{
  curve_dash * tdash;

  tdash = selectdash (da);
  cairo_set_dash (fr, tdash -> a, tdash -> b, 0.0);
  cairo_set_source_rgba (fr, dcol.red,  dcol.green,  dcol.blue, dcol.alpha);
  cairo_set_line_width (fr, ti);
  g_free (tdash);
}

/*!
  \fn void show_frame (cairo_t * cd, int tf, int da, int res[2], double ti, double x[2], double y[2], ColRGBA dcol)

  \brief draw frame

  \param cd the cairo drawing context to use for the draw
  \param tf frame type
  \param da dash type
  \param res image size
  \param ti line width
  \param x x positions (right, left)
  \param y y positions (top, bottom)
  \param dcol color
*/
void show_frame (cairo_t * cd, int tf, int da, int res[2], double ti, double x[2], double y[2], ColRGBA dcol)
{
  double x1, x2;
  double y1, y2;

  prep_frame (cd, da, ti, dcol);
  x1 = x[0] * res[0];
  x2 = x[1] * res[0];
  y1 = y[0] * res[1];
  y2 = y[1] * res[1];
  switch (tf)
  {
    case 0:
      cairo_move_to (cd, x1, y2);
      cairo_line_to (cd, x2, y2);
      cairo_line_to (cd, x2, y1);
      cairo_line_to (cd, x1, y1);
      cairo_line_to (cd, x1, y2);
      break;
    case 1:
      cairo_move_to (cd, x1, y2);
      cairo_line_to (cd, x1, y1);
      cairo_line_to (cd, x2, y1);
      cairo_line_to (cd, x2, y2);
      break;
    case 2:
      cairo_move_to (cd, x1, y2);
      cairo_line_to (cd, x1, y1);
      cairo_line_to (cd, x2, y1);
      break;
    case 3:
      cairo_move_to (cd, x2, y2);
      cairo_line_to (cd, x2, y1);
      cairo_line_to (cd, x1, y1);
      break;
    case 4:
      cairo_move_to (cd, x1, y1);
      cairo_line_to (cd, x2, y1);
      break;
  }
  cairo_stroke (cd);
}

/*!
  \fn void prep_axis_data (Curve * this_curve, int ax)

  \brief prepare axis data

  \param this_curve the target Curve
  \param ax the axis
*/
void prep_axis_data (Curve * this_curve, int ax)
{
  dogrid = this_curve -> show_grid[ax];
  x_shift = this_curve -> labels_shift_x[ax];
  y_shift = this_curve -> labels_shift_y[ax];
  mticks = this_curve -> majt[ax];
  nticks = this_curve -> mint[ax];
  if (this_curve -> ticks_io[ax] == 1)
  {
    amint = this_curve -> mint_size[ax];
    amajt = this_curve -> majt_size[ax];
  }
  else
  {
    amint = - this_curve -> mint_size[ax];
    amajt = - this_curve -> majt_size[ax];
  }
  tickpos = this_curve -> ticks_pos[ax];
  labpos = this_curve -> labels_pos[ax];
}

/*!
  \fn void draw_frame (cairo_t * cr, Curve * this_curve, int rid, int cid)

  \brief draw frame and axis data

  \param cr the cairo drawing context to use for the draw
  \param this_curve the target curve
  \param rid the calculation id
  \param cid the curve id
*/
void draw_frame (cairo_t * cr, Curve * this_curve, int rid, int cid)
{
  show_frame (cr, this_curve -> frame_type, this_curve -> frame_dash, resol,
              this_curve -> frame_thickness, this_curve -> frame_pos[0], this_curve -> frame_pos[1], this_curve -> frame_color);
// X axis

// Draw X axis ticks and labels
  pango_layout_set_font_description (layout, pango_font_description_from_string (this_curve -> labels_font[0]));
  prep_axis_data (this_curve, 0);
  if (this_curve -> scale[0] == 0)
  {
    setup_xaxis_linear (cr, this_curve);
  }
  else
  {
    setup_xaxis_log (cr, this_curve, rid, cid, TRUE);
  }

// Draw X axis title
  cairo_move_to (cr, x_min  + XDRAW / 2.0 + this_curve -> axis_title_x[0], y_min + this_curve -> axis_title_y[0]);
  pango_layout_set_font_description (layout, pango_font_description_from_string (this_curve -> axis_title_font[0]));
  pango_layout_set_text (layout, this_curve -> axis_title[0], -1);
  pango_cairo_update_layout (cr, layout);
  pango_cairo_show_layout (cr, layout);
  cairo_stroke(cr);

// Y axis

// Draw Y axis ticks and labels
  pango_layout_set_font_description (layout, pango_font_description_from_string (this_curve -> labels_font[1]));
  prep_axis_data (this_curve, 1);
  if (this_curve -> scale[1] == 0)
  {
    setup_yaxis_linear (cr, this_curve);
  }
  else
  {
    setup_yaxis_log (cr, this_curve, TRUE);
  }
  cairo_move_to (cr, x_min - this_curve -> axis_title_x[1], y_min + YDRAW/2 - this_curve -> axis_title_y[1]);
  pango_layout_set_font_description (layout, pango_font_description_from_string (this_curve -> axis_title_font[1]));
  pango_layout_set_text (layout, this_curve -> axis_title[1], -1);
  cairo_rotate (cr, -pi/2.0);
  pango_cairo_update_layout (cr, layout);
  pango_cairo_show_layout (cr, layout);
  cairo_stroke (cr);
  cairo_rotate (cr, pi/2.0);
  pango_cairo_update_layout (cr, layout);
}
