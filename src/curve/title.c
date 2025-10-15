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
* @file title.c
* @short Functions to draw the title
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'title.c'
*
* Contains:
*

 - The functions to draw the curve title

*
* List of functions:

  const gchar * default_title (int ax, gpointer data);

  void show_title (cairo_t * cr, Curve * this_curve);

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#include "global.h"
#include "curve.h"

/*!
  \fn const gchar * default_title (int ax, gpointer data)

  \brief default title string

  \param ax axis
  \param data the associated data pointer
*/
const gchar * default_title (int ax, gpointer data)
{
  int rid = ((tint *)data) -> b;
  project * this_proj = get_project_by_id (((tint *)data) -> a);
  if (ax == 0)
  {
#ifdef NEW_ANA
    if (rid != MS)
    {
      return this_proj -> analysis[rid].x_title;
    }
#else
    if (rid == GR || rid == GK)
    {
      return ("r [Å]");
    }
    else if (rid == SQ || rid == SK)
    {
      return ("q [Å-1]");
    }
    else if (rid == BD)
    {
      return ("Dij [Å]");
    }
    else if (rid == AN)
    {
      return ("θ [°]");
    }
    else if (rid == RI)
    {
      return ("Size n of the ring [total number of nodes]");
    }
    else if (rid == CH)
    {
      return ("Size n of the chain [total number of nodes]");
    }
    else if (rid == SP)
    {
      return ("Ql");
    }
#endif // NEW_ANA
    else
    {
      return g_strdup_printf ("t [%s]", untime[this_proj -> tunit]);
    }
  }
  else
  {
    return get_curve_from_pointer (data) -> name;
  }
}

/*!
  \fn void show_title (cairo_t * cr, Curve * this_curve)

  \brief draw title

  \param cr the cairo drawing context to use for the draw
  \param this_curve the target curve
*/
void show_title (cairo_t * cr, Curve * this_curve)
{
  double x, y;

  x = this_curve -> title_pos[0] * resol[0];
  y = this_curve -> title_pos[1] * resol[1];
  cairo_set_source_rgba (cr, this_curve -> title_color.red,
                             this_curve -> title_color.green,
                             this_curve -> title_color.blue,
                             this_curve -> title_color.alpha);
  pango_layout_set_font_description (layout, pango_font_description_from_string (this_curve -> title_font));
  pango_layout_set_text (layout, this_curve -> title, -1);
  cairo_move_to (cr, x, y);
  pango_cairo_update_layout (cr, layout);
  pango_cairo_show_layout (cr, layout);
  cairo_stroke (cr);
}
