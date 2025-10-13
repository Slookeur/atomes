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
* @file xaxis.c
* @short Functions to draw the x axis
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'xaxis.c'
*
* Contains:
*

 - The functions to draw the x axis

*
* List of functions:

  void setup_xaxis_linear (cairo_t * cr, Curve * this_curve);
  void setup_xaxis_log (cairo_t * cr, Curve * this_curve, gboolean draw_it);

*/

#include <math.h>
#include <cairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn void setup_xaxis_linear (cairo_t * cr, Curve * this_curve)

  \brief setup x axis using a linear scale

  \param cr the cairo drawing context
  \param this_curve the target curve
*/
void setup_xaxis_linear (cairo_t * cr, Curve * this_curve)
{
  int k;
  double u, v;

  k = cxy[0] / mticks;
  v = cxy[0] - k * mticks;
  if (k * mticks < cxy[0]) v = mticks - v;
  v = fabs(v);
  for ( u = 0.0 ; u < xmax; u = u + mticks )
  {
    if (x_min + (u + v) * XDRAW / xmax <= x_max)
    {
      switch (labpos)
      {
        case 0:
          ax = x_min - x_shift + (u + v) * XDRAW / xmax;
          ay = y_min + y_shift;
          label_curve (cr, cxy[0] + u + v, 0, 0, this_curve);
          break;
        case 1:
          ax = x_min - x_shift + (u + v) * XDRAW / xmax;
          ay = y_max - y_shift;
          label_curve (cr, cxy[0] + u + v, 0, 1, this_curve);
          break;
        case 2:
          ax = x_min - x_shift + (u + v) * XDRAW / xmax;
          ay = y_min + y_shift;
          label_curve (cr, cxy[0] + u + v, 0, 0, this_curve);
          ax = x_min - x_shift + (u + v) * XDRAW / xmax;
          ay = y_max - y_shift;
          label_curve (cr, cxy[0] + u + v, 0, 1, this_curve);
          break;
      }
      switch (tickpos)
      {
        case 0:
          cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_min);
          cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_min + amajt);
          break;
        case 1:
          cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_max);
          cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_max - amajt);
          break;
        case 2:
          cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_min);
          cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_min + amajt);
          cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_max);
          cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_max - amajt);
          break;
      }
    }
  }
  cairo_stroke (cr);

  for (u = v ; u > mticks / nticks ; u -= mticks / nticks);
  v = u;
  for ( u = 0.0 ; u < xmax ; u = u + mticks / nticks )
  {
    if (x_min + (u + v) * XDRAW / xmax <= x_max)
    {
      if (dogrid)
      {
        cairo_stroke(cr);
        cairo_set_dash (cr, pdashed, lenp, 0.0);
        cairo_set_line_width (cr, GRIDSIZE);
        cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_min);
        cairo_line_to(cr, x_min + (u + v)* XDRAW / xmax, y_max);
        cairo_stroke(cr);
        prep_frame (cr, this_curve -> frame_dash, this_curve -> frame_thickness, this_curve -> frame_color);
      }
      if (fmod(u+v, mticks) != 0.0)
      {
        switch (tickpos)
        {
          case 0:
            cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_min);
            cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_min + amint);
            break;
          case 1:
            cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_max);
            cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_max - amint);
            break;
          case 2:
            cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_min);
            cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_min + amint);
            cairo_move_to(cr, x_min + (u + v) * XDRAW / xmax, y_max);
            cairo_line_to(cr, x_min + (u + v) * XDRAW / xmax, y_max - amint);
            break;
        }
      }
    }
    cairo_stroke(cr);
  }
}

/*!
  \fn void setup_xaxis_log (cairo_t * cr, Curve * this_curve, int rid, int cid, gboolean draw_it)

  \brief setup x axis using a log scale

  \param cr the cairo drawing context
  \param this_curve the target curve
  \param rid the analysis id
  \param cid the curve id
  \param draw_it 1/0 draw or not
*/
void setup_xaxis_log (cairo_t * cr, Curve * this_curve, int rid, int cid, gboolean draw_it)
{
  int i, k, l;
  gboolean istrue;
  double v;
  istrue = TRUE;
  xlog = 1;
  if (cxy[0] != 0.0)
  {
    k = (xmax+cxy[0])/cxy[0];
  }
  while (istrue)
  {
    k = k/10;
    if (k > 0)
    {
      xlog ++;
    }
    else
    {
      istrue = FALSE;
    }
  }
  istrue = TRUE;
  dxlog = 0;
  i = 100;
  while (istrue)
  {
    if (cxy[0] >= pow(10, i))
    {
      istrue = FALSE;
      dxlog --;
    }
    else
    {
      dxlog ++;
      i=i-1;
    }
  }
  dxlog = dxlog - 100;
  if (draw_it)
  {
    for ( l = 0 ; l < xlog ; l = l + 1 )
    {
      v = pow (10, l-dxlog);
      if (dogrid)
      {
        cairo_stroke(cr);
        cairo_set_line_width (cr, GRIDSIZE);
        cairo_set_dash (cr, pdashed, lenp, 0.0);
        cairo_move_to(cr, x_min + l * XDRAW / xlog, y_min);
        cairo_line_to(cr, x_min + l * XDRAW / xlog, y_max);
        cairo_stroke(cr);
        prep_frame (cr, this_curve -> frame_dash,
                        this_curve -> frame_thickness,
                        this_curve -> frame_color);
      }
      switch (labpos)
      {
        case 0:
          ax = x_min - x_shift + l * XDRAW / xlog;
          ay = y_min + y_shift;
          label_curve (cr, v, 0, 0, this_curve);
          break;
        case 1:
          ax = x_min - x_shift + l * XDRAW / xlog;
          ay = y_max - y_shift;
          label_curve (cr, v, 0, 1, this_curve);
          break;
        case 2:
          ax = x_min - x_shift + l * XDRAW / xlog;
          ay = y_min + y_shift;
          label_curve (cr, v, 0, 0, this_curve);
          ax = x_min - x_shift + l * XDRAW / xlog;
          ay = y_max - y_shift;
          label_curve (cr, v, 0, 1, this_curve);
          break;
      }
      switch (tickpos)
      {
        case 0:
          cairo_move_to(cr, x_min + l * XDRAW / xlog, y_min);
          cairo_line_to(cr, x_min + l * XDRAW / xlog, y_min + amajt);
          break;
        case 1:
          cairo_move_to(cr, x_min + l * XDRAW / xlog, y_max);
          cairo_line_to(cr, x_min + l * XDRAW / xlog, y_max - amajt);
          break;
        case 2:
          cairo_move_to(cr, x_min + l * XDRAW / xlog, y_min);
          cairo_line_to(cr, x_min + l * XDRAW / xlog, y_min + amajt);
          cairo_move_to(cr, x_min + l * XDRAW / xlog, y_max);
          cairo_line_to(cr, x_min + l * XDRAW / xlog, y_max - amajt);
          break;
      }
      for ( k = 2 ; k < 10 ; k = k + 1 )
      {
        if (dogrid)
        {
          cairo_stroke(cr);
          cairo_set_line_width (cr, GRIDSIZE);
          cairo_set_dash (cr, pdashed, lenp, 0.0);
          cairo_move_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_min);
          cairo_line_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_max);
          cairo_stroke(cr);
          prep_frame (cr, this_curve -> frame_dash,
                          this_curve -> frame_thickness,
                          this_curve -> frame_color);
        }
        switch (tickpos)
        {
          case 0:
            cairo_move_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_min);
            cairo_line_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_min + amint);
            break;
          case 1:
            cairo_move_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_max);
            cairo_line_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_max - amint);
            break;
          case 2:
            cairo_move_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_min);
            cairo_line_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_min + amint);
            cairo_move_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_max);
            cairo_line_to(cr, x_min + XDRAW * (l + log(k) / log(10.0)) / xlog, y_max - amint);
            break;
        }
      }
    }
    cairo_stroke (cr);
  }
}
