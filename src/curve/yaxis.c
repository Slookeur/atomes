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
* @file yaxis.c
* @short Functions to draw the y axis
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'yaxis.c'
*
* Contains:
*

 - The functions to draw the y axis

*
* List of functions:

  void autoscale_axis (project * this_proj, int rid, int cid, int aid);
  void setup_yaxis_linear (cairo_t * cr, Curve * this_curve);
  void setup_yaxis_log (cairo_t * cr, Curve * this_curve, gboolean draw_it);

*/

#include <math.h>
#include <cairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn void autoscale_axis (project * this_proj, Curve * this_curve, int rid, int cid, int aid)

  \brief autoscale axis

  \param this_proj the target project
  \param this_curve the target curve
  \param rid the analysis id
  \param cid the curve id
  \param aid the axis id
*/
void autoscale_axis (project * this_proj, Curve * this_curve, int rid, int cid, int aid)
{
  int i, j, k, l, m, n;
  if (! aid && (rid == RIN ||rid == CHA))
  {
     i = cid / ((this_proj -> nspec+1) * 4);
     this_curve -> axmax[aid] = (rid == RIN) ? this_proj -> rsparam[i][1]: this_proj -> csparam[5];
     this_curve -> axmax[aid] += 1.0;
     this_curve -> axmin[aid] = (rid == RIN) ? 2 : 1;
  }
  else
  {
    n = (rid == SPH && aid == 1) ? 1 : 0;
    this_curve -> axmax[aid] = this_curve -> data[aid][n];
    this_curve -> axmin[aid] = this_curve -> data[aid][n];
    for ( i=n ; i < this_curve -> ndata ; i++ )
    {
      this_curve -> axmax[aid] = max(this_curve -> axmax[aid], this_curve -> data[aid][i]);
      this_curve -> axmin[aid] = min(this_curve -> axmin[aid], this_curve -> data[aid][i]);
    }
    CurveExtra * ctmp = this_curve -> extrac -> first;
    project * that_proj;
    for ( j=0 ; j < this_curve -> extrac -> extras ; j++ )
    {
      m = ctmp -> id.a;
      k = ctmp -> id.b;
      l = ctmp -> id.c;
      that_proj = get_project_by_id(m);
#ifdef NEW_ANA
      for ( i=n ; i < that_proj -> analysis[k] -> curves[l] -> ndata ; i++ )
      {
        this_curve -> axmax[aid] = max(this_curve -> axmax[aid], that_proj -> analysis[k] -> curves[l] -> data[aid][i]);
        this_curve -> axmin[aid] = min(this_curve -> axmin[aid], that_proj -> analysis[k] -> curves[l] -> data[aid][i]);
      }
#else
      for ( i=n ; i < that_proj -> curves[k][l] -> ndata ; i++ )
      {
        this_curve -> axmax[aid] = max(this_curve -> axmax[aid], that_proj -> curves[k][l] -> data[aid][i]);
        this_curve -> axmin[aid] = min(this_curve -> axmin[aid], that_proj -> curves[k][l] -> data[aid][i]);
      }
#endif
      if (ctmp -> next != NULL) ctmp = ctmp -> next;
    }
  }

  if (aid == 1)
  {
    this_curve -> cmin[aid] = this_curve -> axmin[aid];
    this_curve -> cmax[aid] = this_curve -> axmax[aid];
    this_curve -> axmin[aid] = this_curve -> cmin[aid] - fabs(this_curve -> cmin[aid]) / 10.0;
    this_curve -> axmax[aid] = this_curve -> cmax[aid] + fabs(this_curve -> cmax[aid]) / 10.0;
//  Adjust autoscale information if required
    if (rid > GDK && rid < MSD)
    {
      this_curve -> axmin[aid] = 0.0;
    }
  }
}

/*!
  \fn void setup_yaxis_linear (cairo_t * cr, Curve * this_curve)

  \brief setup y axis using a linear scale

  \param cr the cairo drawing context
  \param this_curve the target curve
*/
void setup_yaxis_linear (cairo_t * cr, Curve * this_curve)
{
  int k;
  double u, v;

  k = cxy[1] / mticks;
  v = cxy[1] - k * mticks;
  if (k * mticks < cxy[1]) v = mticks - v;
  v = fabs(v);
  if (k * mticks > fabs(cxy[1])) v = mticks - v;
  for ( u = 0.0 ; u < ymax ; u = u + mticks )
  {
    if (y_min + (u + v) * YDRAW / ymax >= y_max)
    {
      switch (labpos)
      {
        case 0:
          ax = x_min - x_shift;
          ay = y_min - y_shift + (u + v) * YDRAW / ymax;
          label_curve (cr, cxy[1] + v + u, 1, 0, this_curve);
          break;
        case 1:
          ax = x_max + x_shift;
          ay = y_min - y_shift + (u + v) * YDRAW / ymax;
          label_curve (cr, cxy[1] + v + u, 1, 1, this_curve);
          break;
        case 2:
          ax = x_min - x_shift;
          ay = y_min - y_shift + (u + v) * YDRAW / ymax;
          label_curve (cr, cxy[1] + v + u, 1, 0, this_curve);
          ax = x_max + x_shift;
          ay = y_min - y_shift + (u + v) * YDRAW / ymax;
          label_curve (cr, cxy[1] + v + u, 1, 1, this_curve);
          break;
      }
      switch (tickpos)
      {
        case 0:
          cairo_move_to(cr, x_min, y_min + (u + v) * YDRAW / ymax);
          cairo_line_to(cr, x_min - amajt, y_min + (u + v) * YDRAW / ymax);
          break;
        case 1:
          cairo_move_to(cr, x_max, y_min + (u + v) * YDRAW / ymax);
          cairo_line_to(cr, x_max + amajt, y_min + (u + v) * YDRAW / ymax);
          break;
        case 2:
          cairo_move_to(cr, x_min, y_min + (u + v) * YDRAW / ymax);
          cairo_line_to(cr, x_min - amajt, y_min + (u + v) * YDRAW / ymax);
          cairo_move_to(cr, x_max, y_min + (u + v) * YDRAW / ymax);
          cairo_line_to(cr, x_max + amajt, y_min + (u + v) * YDRAW / ymax);
          break;
      }
    }
  }
  cairo_stroke (cr);

  for (u = v ; u > mticks / nticks ; u -= mticks / nticks);
  v = u;
  for (u = 0.0 ; u < ymax ; u = u + mticks / nticks)
  {
    if (y_min + (u + v) * YDRAW / ymax >= y_max)
    {
      if (dogrid)
      {
        cairo_stroke(cr);
        cairo_set_line_width (cr, GRIDSIZE);
        cairo_set_dash (cr, pdashed, lenp, 0.0);
        cairo_move_to(cr, x_min, y_min + (u + v) * YDRAW / ymax);
        cairo_line_to(cr, x_max, y_min + (u + v) * YDRAW / ymax);
        cairo_stroke(cr);
        prep_frame (cr, this_curve -> frame_dash, this_curve -> frame_thickness, this_curve -> frame_color);
      }
      if (fmod(u+v, mticks) != 0.0)
      {
        switch (tickpos)
        {
          case 0:
            cairo_move_to(cr, x_min, y_min + (u + v) * YDRAW / ymax);
            cairo_line_to(cr, x_min - amint, y_min + (u + v) * YDRAW / ymax);
            break;
          case 1:
            cairo_move_to(cr, x_max, y_min + (u + v) * YDRAW / ymax);
            cairo_line_to(cr, x_max + amint, y_min + (u + v) * YDRAW / ymax);
            break;
          case 2:
            cairo_move_to(cr, x_min, y_min + (u + v) * YDRAW / ymax);
            cairo_line_to(cr, x_min - amint, y_min + (u + v) * YDRAW / ymax);
            cairo_move_to(cr, x_max, y_min + (u + v) * YDRAW / ymax);
            cairo_line_to(cr, x_max + amint, y_min + (u + v) * YDRAW / ymax);
            break;
        }
      }
    }
    cairo_stroke (cr);
  }
}

/*!
  \fn void setup_yaxis_log (cairo_t * cr, Curve * this_curve, gboolean draw_it)

  \brief setup y axis using a log scale

  \param cr the cairo drawing context
  \param this_curve the target curve
  \param draw_it 1/0 draw or not
*/
void setup_yaxis_log (cairo_t * cr, Curve * this_curve, gboolean draw_it)
{
  int i, k, l;
  gboolean istrue;
  double v;

  istrue = TRUE;
  ylog = 1;
  if (cxy[1] != 0.0)
  {
    k = (ymax+cxy[1])/cxy[1];
  }
  while (istrue)
  {
    k = k/10;
    if (k > 0)
    {
      ylog ++;
    }
    else
    {
      ylog ++;
      istrue = FALSE;
    }
  }
  istrue = TRUE;
  dylog = 0;
  i = 100;
  while (istrue)
  {
    if (cxy[1] >= pow(10, i))
    {
      istrue = FALSE;
    }
    else
    {
      dylog ++;
      i=i-1;
    }
  }
  dylog = dylog - 100;
  if (draw_it)
  {
    for ( l = 0 ; l < ylog ; l ++ )
    {
      v = pow (10, l-dylog);
      if (dogrid)
      {
         cairo_stroke(cr);
         cairo_set_line_width (cr, GRIDSIZE);
         cairo_set_dash (cr, pdashed, lenp, 0.0);
         cairo_move_to(cr, x_min, y_min + YDRAW * l / ylog);
         cairo_line_to(cr, x_max, y_min + YDRAW * l / ylog);
         cairo_stroke(cr);
         prep_frame (cr, this_curve -> frame_dash, this_curve -> frame_thickness, this_curve -> frame_color);
      }
      switch (labpos)
      {
        case 0:
          ax = x_min - x_shift;
          ay = y_min - y_shift + YDRAW *l / ylog;
          label_curve (cr, v, 1, 0, this_curve);
          break;
        case 1:
          ax = x_max + x_shift;
          ay = y_min - y_shift + YDRAW *l / ylog;
          label_curve (cr, v, 1, 1, this_curve);
          break;
        case 2:
          ax = x_min - x_shift;
          ay = y_min - y_shift + YDRAW *l / ylog;
          label_curve (cr, v, 1, 0, this_curve);
          ax = x_max + x_shift;
          ay = y_min - y_shift + YDRAW *l / ylog;
          label_curve (cr, v, 1, 1, this_curve);
          break;
      }
      switch (tickpos)
      {
        case 0:
          cairo_move_to(cr, x_min, y_min + YDRAW * l / ylog);
          cairo_line_to(cr, x_min - amajt, y_min + YDRAW * l / ylog);
          break;
        case 1:
          cairo_move_to(cr, x_max, y_min + YDRAW * l / ylog);
          cairo_line_to(cr, x_max - amajt, y_min + YDRAW * l / ylog);
          break;
        case 2:
          cairo_move_to(cr, x_min, y_min + YDRAW * l / ylog);
          cairo_line_to(cr, x_min - amajt, y_min + YDRAW * l / ylog);
          cairo_move_to(cr, x_max, y_min + YDRAW * l / ylog);
          cairo_line_to(cr, x_max - amajt, y_min + YDRAW * l / ylog);
          break;
      }
      for ( k = 2 ; k < 10 ; k ++ )
      {
        if (dogrid)
        {
          cairo_stroke(cr);
          cairo_set_line_width (cr, GRIDSIZE);
          cairo_set_dash (cr, pdashed, lenp, 0.0);
          cairo_move_to(cr, x_min, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
          cairo_line_to(cr, x_max, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
          cairo_stroke(cr);
          prep_frame (cr, this_curve -> frame_dash, this_curve -> frame_thickness, this_curve -> frame_color);
        }
        switch (tickpos)
        {
          case 0:
            cairo_move_to(cr, x_min, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
            cairo_line_to(cr, x_min - amint, y_min + YDRAW *(l + log(k) / log(10.0)) / ylog);
            break;
          case 1:
            cairo_move_to(cr, x_max, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
            cairo_line_to(cr, x_max + amint, y_min + YDRAW *(l + log(k) / log(10.0)) / ylog);
            break;
          case 2:
            cairo_move_to(cr, x_min, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
            cairo_line_to(cr, x_min - amint, y_min + YDRAW *(l + log(k) / log(10.0)) / ylog);
            cairo_move_to(cr, x_max, y_min + YDRAW * (l + log(k) / log(10.0)) / ylog);
            cairo_line_to(cr, x_max + amint, y_min + YDRAW *(l + log(k) / log(10.0)) / ylog);
            break;
        }
      }
    }
    cairo_stroke (cr);
  }
}
