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
* @file draw.c
* @short Function to draw a curve
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'draw.c'
*
* Contains:
*

- The function to draw a curve

*
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <math.h>
#include <cairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn void draw_curve (cairo_t * cr,
                    int cid,
                    int rid,
                    project * this_proj,
                    int points,
                    ColRGBA withcolor,
                    int xscale,
                    int yscale,
                    int asp,
                    int vdash,
                    double thick,
                    int glyp,
                    double gize,
                    int freq,
                    double hwidth,
                    double hopac,
                    int hpos,
                   int extra,
                    int pid)

  \brief draw target curve to the cairo drawing context

  \param cr the cairo drawing context to use for the draw
  \param cid the target curve id
  \param rid the target calculation id
  \param this_proj the target project
  \param points the number of data point(s)
  \param withcolor the data color
  \param xscale x axis scale type (0 = linear, 1 = log)
  \param yscale y axis scale type (0 = linear, 1 = log)
  \param asp data aspect (0 = x/y normal, 1 = historgram)
  \param vdash dash type
  \param thick line tthickness
  \param glyp glyph type
  \param gize glyph size
  \param freq glyph frequency
  \param hwidth histogram bar width
  \param hopac histogram bar opacity value
  \param hpos histogram opacity treatment (0 = no opacity, 1 = use opacity)
  \param extra the number of extra data sets
  \param pid 0 (x/y plot) or plot id number (historgram)
*/
void draw_curve (cairo_t * cr,
                 int cid,
                 int rid,
                 project * this_proj,
                 int points,
                 ColRGBA withcolor,
                 int xscale,
                 int yscale,
                 int asp,
                 int vdash,
                 double thick,
                 int glyp,
                 double gize,
                 int freq,
                 double hwidth,
                 double hopac,
                 int hpos,
                 int extra,
                 int pid)
{
  int i, j, k;
  double x, y;
  double x1, x2, y1, y2;
  double dx1, dx2, dy1, dy2;
  double slope, bval;
  double ** plotdata;
  gboolean plot;
  gboolean dglyp;
  curve_dash * dasht;
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = this_proj -> analysis[rid].curves[cid];
#else
  this_curve = this_proj -> curves[rid][cid];
#endif

  plotdata = allocddouble (points, 2);
  for ( i=0 ; i < points; i++ )
  {
    if (xscale == 0)
    {
      plotdata[i][0] = x_min + XDRAW * (this_curve -> data[0][i] - cxy[0])/ xmax;
    }
    else
    {
#ifdef NEW_ANA
      x = (i+1) * this_proj -> analysis[rid].num_delta * this_proj -> analysis[rid].delta * pow(10, dxlog);
#else
      x = (i+1) * this_proj -> num_delta[rid] * this_proj -> delta[rid] * pow(10, dxlog);
#endif
      x = log(x) / log(pow(10, xlog));
      plotdata[i][0] = x_min + XDRAW * x;
    }
    if (yscale == 0)
    {
      plotdata[i][1] = y_min + YDRAW * (this_curve -> data[1][i] - cxy[1]) / ymax;
    }
    else
    {
      y = this_curve -> data[1][i] * pow(10, dylog);
      y = log(y) / log(pow(10, ylog));
      plotdata[i][1] = y_min + YDRAW * y;
    }
#ifdef DEBUG
    // g_debug ("CURVE: DRAWCURVE: x= %f, y= %f", plotdata[i][0], plotdata[i][1]);
#endif
  }
  if (vdash > 0)
  {
    dasht = selectdash (vdash);
    cairo_set_dash(cr, dasht -> a, dasht -> b, 0.0);
    cairo_set_line_width (cr, thick);
    g_free (dasht);
  }
  else
  {
    cairo_set_line_width (cr, 0.0);
  }
  cairo_set_source_rgba (cr, withcolor.red,
                             withcolor.green,
                             withcolor.blue, 1.0);

  if (asp == 0)
  {
    j = 0;
    k = 1;
    if (rid == RI) j = 2;
    for ( i = j ; i < points - k ; i ++)
    {
      plot = TRUE;
      dglyp = FALSE;
      slope = (plotdata[i+k][1] - plotdata[i][1])/ (plotdata[i+k][0] - plotdata[i][0]);
      bval = plotdata[i][1] - slope*plotdata[i][0];
      dy1 = slope*x_min + bval;
      dy2 = slope*x_max + bval;
      if (slope < 0)
      {
        dx1 = (y_min - bval) / slope;
        dx2 = (y_max - bval) / slope;
      }
      else
      {
        dx1 = (y_max - bval) / slope;
        dx2 = (y_min - bval) / slope;
      }
      if (plotdata[i][0] < x_min && plotdata[i+k][0] < x_min)
      {
        plot = FALSE;
      }
      else if (plotdata[i][0] > x_max && plotdata[i+k][0] > x_max)
      {
        plot = FALSE;
      }
      if (plotdata[i][1] < y_max && plotdata[i+k][1] < y_max)
      {
        plot = FALSE;
      }
      else if (plotdata[i][1] > y_min && plotdata[i+k][1] > y_min)
      {
        plot = FALSE;
      }
      if (plot)
      {
        if (plotdata[i][0] >= x_min && plotdata[i][0] <= x_max)
        {
          if(plotdata[i][1] >= y_max && plotdata[i][1] <= y_min)
          {
            x1 = plotdata[i][0];
            y1 = plotdata[i][1];
            dglyp = TRUE;
          }
          else
          {
            x1 = dx1;
            y1 = (slope < 0) ? y_min : y_max;
          }
        }
        else
        {
          x1 = x_min;
          y1 = dy1;
        }
        if (plotdata[i+k][0] >= x_min && plotdata[i+k][0] <= x_max)
        {
          if (plotdata[i+k][1] >= y_max && plotdata[i+k][1] <= y_min)
          {
            x2 = plotdata[i+k][0];
            y2 = plotdata[i+k][1];
          }
          else
          {
            x2 = dx2;
            y2 = (slope < 0) ? y_max : y_min;
          }
        }
        else
        {
          x2 = x_max;
          y2 = dy2;
        }
        cairo_move_to (cr, x1, y1);
        cairo_line_to (cr, x2, y2);
        cairo_stroke (cr);
        if (i % freq == 0 && dglyp) draw_glyph (cr, glyp, x1, y1, withcolor, gize);
        if (i == points - 2 && (i+k) % freq == 0 && dglyp)
        {
          draw_glyph (cr, glyp, x2, y2, withcolor, gize);
        }
      }
    }
  }
  else if (asp == 1)
  {
    j = 0;
    k = 1;
    if (rid == RI) j = 2;
    // g_debug ("x_min= %f, x_max= %f, y_min= %f, y_max= %f", x_min, x_max, y_min, y_max);
    for ( i = j ; i < points ; i ++)
    {
      if (this_curve -> data[1][i] != 0.0)
      {
        if (plotdata[i][0] >= x_min && plotdata[i][0] <= x_max)
        {
          if (plotdata[i][1] <= y_min || (cxy[1] <= 0.0 && plotdata[i][1] > y_min))
          {
            x1 = plotdata[i][0] - hwidth * 0.5 * XDRAW / xmax;
            if (pid) x1 -= hwidth * 0.5 * ((float)pid / (float)extra) * XDRAW / xmax;
            x2 = hwidth * XDRAW / xmax;
            if (x1 < x_min)
            {
              x2 -= (x_min - x1);
              x1 = x_min;
            }
            if (x1+x2 > x_max)
            {
              x2 = x_max - x1;
            }
            y1 = plotdata[i][1];
            if (plotdata[i][1] > y_min)
            {
              y1 = y_min;
            }
            else if (plotdata[i][1] < y_max)
            {
              y1 = y_max;
            }
            y2 =  y_min - y1;
            if (cxy[1] <= 0.0)
            {
              y2 = y2 - cxy[1] * YDRAW / ymax;
            }
            if (hpos)
            {
              cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);
              cairo_rectangle (cr, x1, y1, x2, y2);
              cairo_fill(cr);
            }
            cairo_set_source_rgba (cr, withcolor.red,
                                       withcolor.green,
                                       withcolor.blue, hopac);
            cairo_rectangle (cr, x1, y1, x2, y2);
            cairo_fill(cr);
            cairo_set_source_rgba (cr, withcolor.red,
                                       withcolor.green,
                                       withcolor.blue, 1.0);
            y2 = y_min;
            if (cxy[1] <= 0.0)
            {
              y2 = y2 - cxy[1] * YDRAW / ymax;
            }
            cairo_move_to (cr, x1, y2);
            cairo_line_to (cr, x1, y1);
            cairo_move_to (cr, x1, y1);
            x2 = plotdata[i][0] + hwidth * 0.5 * XDRAW / xmax;
            if (pid) x2 -= hwidth * 0.5 * ((float)pid / (float)extra) * XDRAW / xmax;
            if (x2 > x_max)
            {
              x2 = x_max;
              cairo_line_to(cr, x2, y1);
              cairo_move_to(cr, x2, y1);
            }
            else
            {
              cairo_line_to(cr, x2, y1);
              cairo_move_to(cr, x2, y1);
              cairo_line_to(cr, x2, y2);
            }
            cairo_stroke(cr);
          }
        }
      }
    }
  }
  cairo_stroke(cr);
  g_free (plotdata);
}
