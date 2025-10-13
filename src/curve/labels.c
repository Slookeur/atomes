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
* @file labels.c
* @short Function to draw the axis labels
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'labels.c'
*
* Contains:
*

 - Function to draw the axis labels

*
* List of functions:

  void label_curve (cairo_t * cr, double val, int axe, int p, project * this_proj);

*/

#include <string.h>
#include <math.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn void label_curve (cairo_t * cr, double val, int axe, int p, Curve * this_curve)

  \brief draw axis label

  \param cr the cairo drawing context to use for the draw
  \param val the value to display
  \param axe axis (0 = x, 1 = y)
  \param p label position (0 = bottom, 1 = top)
  \param this_curve the target curve
*/
void label_curve (cairo_t * cr, double val, int axe, int p, Curve * this_curve)
{
  gchar * label;
  gchar * lab;
  gchar * tmp;
  int i, j;
  double u, v;

  lab = g_strdup_printf ("%.10lf", val);
  for (i=0; i < strlen(lab); i++)
  {
    tmp = g_strdup_printf ("%c", lab[i]);
    if (g_strcmp0 ((char *)tmp, ".") == 0) j = i;
    g_free (tmp);
  }
  for (i=0; i < j + this_curve -> labels_digit[axe] + 1; i++)
  {
    if (i == 0)
    {
      label = g_strdup_printf ("%c", lab[i]);
    }
    else
    {
      tmp = g_strdup_printf ("%s%c", label, lab[i]);
      g_free (label);
      label = g_strdup_printf ("%s", tmp);
      g_free (tmp);
    }
  }
  pango_layout_set_text (layout, label, -1);
  if (p == 1)
  {
    pango_layout_get_size (layout, & i, & j);
    if (axe == 0)
    {
      u = sin (this_curve -> labels_angle[axe]) * (double)i;
      v = sin (pi / 2.0 - this_curve -> labels_angle[axe]) * (double)j;
      ay = ay - (u + v)/PANGO_SCALE;
    }
    else
    {
      u = sqrt (1.0*i*i + 1.0*j*j);
      v = atan ((1.0*j) / (1.0*i));
      u = u * cos (this_curve -> labels_angle[axe] + v);
      ax = ax - u  / PANGO_SCALE;
    }
  }
  cairo_move_to (cr, ax, ay);
  cairo_rotate (cr, this_curve -> labels_angle[axe]);
  pango_cairo_update_layout (cr, layout);
  pango_cairo_show_layout (cr, layout);
  cairo_stroke (cr);
  cairo_rotate (cr, -this_curve -> labels_angle[axe]);
  pango_cairo_update_layout (cr, layout);
}
