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
* @file show.c
* @short Functions to draw the complete graph
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'show.c'
*
* Contains:
*

 - The functions to draw the complete graph

*
* List of functions:

  gboolean show_curve (GtkWidget * grwdget, cairo_t * cr, gpointer curve);

  G_MODULE_EXPORT gboolean to_hide_curve (GtkWindow * thecurve, gpointer data);
  G_MODULE_EXPORT gboolean to_hide_curve (GtkWidget * thecurve, GdkEvent * event, gpointer data);

  void show_curve (GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer curve);
  void hide_curve (gpointer data);

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-svg.h>

#include "global.h"
#include "curve.h"

extern void adjust_tool_model (int calc, int curve, gchar * string_path);

#ifdef GTK3
/*!
  \fn gboolean show_curve (GtkWidget * grwdget, cairo_t * cr, gpointer curve)

  \brief show curve callback GTK4

  \param grwdget the GtkWidget sending the signal
  \param cr the cairo drawing context to use for the draw
  \param curve the associated data pointer
*/
gboolean show_curve (GtkWidget * grwdget, cairo_t * cr, gpointer curve)
#else
/*!
  \fn void show_curve (GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer curve)

  \brief show curve callback GTK3

  \param area the GtkDrawingArea sending the signal
  \param cr the cairo drawing context to use for the draw
  \param width drawing area width
  \param height drawing area height
  \param curve the associated pointer data
*/
void show_curve (GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer curve)
#endif
{
  int pid, rid, cid;
  int where;
  int i, j, k;
  double alp;
  gboolean upm;
  Curve * this_curve = get_curve_from_pointer (curve);
  pid = ((tint *)curve) -> a;
  rid = ((tint *)curve) -> b;
  cid = ((tint *)curve) -> c;

  cairo_surface_t * surface = NULL;
  project * this_proj = get_project_by_id(pid);

  where = this_curve -> format;
  // g_print ("where= %d, width = %d, height= %d\n", where, width, height);
  this_curve -> format = 0;
  if (where == 0)
  {
    alp=1.0;
#ifdef GTK3
    this_curve -> wsize[0] = get_widget_width (grwdget);
    this_curve -> wsize[1] = get_widget_height (grwdget);
#else
    this_curve -> wsize[0] = width;
    this_curve -> wsize[1] = height;
#endif
    for (j=0; j<2; j++)
    {
      resol[j] = this_curve -> wsize[j];
      if (this_curve -> curve_edit)
      {
        if (this_curve -> curve_edit -> xyp[j] != NULL) update_entry_int (GTK_ENTRY(this_curve -> curve_edit -> xyp[j]), this_curve -> wsize[j]);
      }
    }
  }
  else
  {
    alp=0.0;
    if (where == 1)
    {
      alp = back_alpha;
      surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, resol[0], resol[1]);
    }
    else if (where == 2)
    {
      alp=1.0;
      surface = cairo_pdf_surface_create (curve_image_file, (double)resol[0], (double)resol[1]);
    }
    else if (where == 3)
    {
      alp = back_alpha;
      surface = cairo_svg_surface_create (curve_image_file, (double)resol[0], (double)resol[1]);
    }
    else if (where == 4)
    {
      surface = cairo_ps_surface_create (curve_image_file, (double)resol[0], (double)resol[1]);
    }
    cr = cairo_create (surface);
  }
// Painting the background
  cairo_set_source_rgba (cr, this_curve -> backcolor.red,
                             this_curve -> backcolor.green,
                             this_curve -> backcolor.blue, alp);
  cairo_paint (cr);
  cairo_set_source_rgba (cr, this_curve -> backcolor.red,
                             this_curve -> backcolor.green,
                             this_curve -> backcolor.blue, 1.0);

// Creating Pango Font Layout
  layout = pango_cairo_create_layout (cr);

  upm = FALSE;
// Autoscale and autoticking for X and or Y axis
  for (j=0; j<2; j++)
  {
    if (this_curve -> autoscale[j])
    {
      // Autoscale
      autoscale_axis (this_proj, this_curve, rid, cid, j);
      this_curve -> autoscale[j] = FALSE;
      // Autoticking
      this_curve -> majt[j] = scale (this_curve -> axmax[j] - this_curve -> axmin[j]);
      this_curve -> mint[j] = 2;
      upm = TRUE;
    }
    cxy[j] = this_curve -> axmin[j];
  }
  xmax = this_curve -> axmax[0] - cxy[0];
  ymax = this_curve -> axmax[1] - cxy[1];
// Ensuring no log scale issues
  if (this_curve -> scale[0]) setup_xaxis_log (cr, this_curve, rid, cid, FALSE);
  if (this_curve -> scale[1]) setup_yaxis_log (cr, this_curve, FALSE);

  if (upm)
  {
    if (this_curve -> curve_edit)
    {
      if (this_curve -> curve_edit -> axischoice != NULL)
      {
        j = combo_get_active (this_curve -> curve_edit -> axischoice);
        update_entry_double (GTK_ENTRY(this_curve -> curve_edit -> vmin), this_curve -> axmin[j]);
        update_entry_double (GTK_ENTRY(this_curve -> curve_edit -> vmax), this_curve -> axmax[j]);
        update_entry_double (GTK_ENTRY(this_curve -> curve_edit -> majt), this_curve -> majt[j]);
        update_entry_int (GTK_ENTRY(this_curve -> curve_edit -> nmi[j]), this_curve -> mint[j] - 1);
      }
    }
  }

  prep_plot (this_curve);

// let's go for the plot
// first we start by the possible extra curves
  i = 0;
  CurveExtra * ctmp = this_curve -> extrac -> last;
  for ( j=0 ; j < this_curve -> extrac -> extras + 1; j++ )
  {
    if (this_curve -> draw_id == j)
    {
      if (this_curve -> bshift && this_curve -> layout -> aspect && this_curve -> extrac -> extras) i ++;
      k = (this_curve -> layout -> aspect) ? i : 0;
      draw_curve (cr,
                  cid,
                  rid,
                  this_proj,
                  this_curve -> ndata,
                  this_curve -> layout -> datacolor,
                  this_curve -> scale[0],
                  this_curve -> scale[1],
                  this_curve -> layout -> aspect,
                  this_curve -> layout -> dash,
                  this_curve -> layout -> thickness,
                  this_curve -> layout -> glyph,
                  this_curve -> layout -> gsize,
                  this_curve -> layout -> gfreq,
                  this_curve -> layout -> hwidth,
                  this_curve -> layout -> hopac,
                  this_curve -> layout -> hpos,
                  this_curve -> extrac -> extras,
                  k);

    }
    else
    {
      if (this_curve -> bshift && ctmp -> layout -> aspect) i++;
      k = (ctmp -> layout -> aspect) ? i : 0;
      draw_curve (cr,
                  ctmp -> id.c,
                  ctmp -> id.b,
                  get_project_by_id(ctmp -> id.a),
#ifdef NEW_ANA
                  get_project_by_id(ctmp -> id.a) -> analysis[ctmp -> id.b].scurves[ctmp -> id.c] -> ndata,
#else
                  get_project_by_id(ctmp -> id.a) -> curves[ctmp -> id.b][ctmp -> id.c] -> ndata,
#endif
                  ctmp -> layout -> datacolor,
                  this_curve -> scale[0],
                  this_curve -> scale[1],
                  ctmp -> layout -> aspect,
                  ctmp -> layout -> dash,
                  ctmp -> layout -> thickness,
                  ctmp -> layout -> glyph,
                  ctmp -> layout -> gsize,
                  ctmp -> layout -> gfreq,
                  ctmp -> layout -> hwidth,
                  ctmp -> layout -> hopac,
                  ctmp -> layout -> hpos,
                  this_curve -> extrac -> extras,
                  k);
      if (ctmp -> prev != NULL) ctmp = ctmp -> prev;
    }
  }

  // Draw the axis bars - in option
  if (this_curve -> show_axis[1])
  {
    if (this_curve -> axmin[0] < 0.0 && this_curve -> axmax[0] > 0.0)
    {
      cairo_set_line_width (cr, 1.0);
      cairo_set_dash (cr, pdashed, lenp, 0.0);
      cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
      cairo_move_to (cr, x_min - this_curve -> axmin[0] * XDRAW / xmax, y_min);
      cairo_line_to (cr, x_min - this_curve -> axmin[0] * XDRAW / xmax, y_max);
      cairo_stroke (cr);
    }
  }

  if (this_curve -> show_axis[0])
  {
    if (this_curve -> axmin[1] < 0.0 && this_curve -> axmax[1] > 0.0)
    {
      cairo_set_line_width (cr, 1.0);
      cairo_set_dash (cr, pdashed, lenp, 0.0);
      cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
      cairo_move_to (cr, x_min, y_min - this_curve -> axmin[1] * YDRAW / ymax);
      cairo_line_to (cr, x_max, y_min - this_curve -> axmin[1] * YDRAW / ymax);
      cairo_stroke (cr);
    }
  }
  // Maybe show the frame
  if (this_curve -> show_frame) draw_frame (cr, this_curve, rid, cid);
  // maybe for the title
  if (this_curve -> show_title) show_title (cr, this_curve);
  // and maybe for the legend
  if (this_curve -> show_legend) show_legend (cr, this_proj, rid, cid);

  // and we plot the complete curve
  if (where == 0)
  {
    this_curve -> surface = cairo_surface_reference (cairo_get_target (cr));
  }
  else if (where == 1)
  {
     cairo_surface_write_to_png (surface, curve_image_file);
     cairo_surface_destroy (surface);
  }
  else if (where == 2 || where == 4)
  {
    cairo_show_page (cr);
    cairo_surface_destroy (surface);
  }
  else if (where == 3)
  {
    cairo_surface_destroy (surface);
  }
  if (where > 0) cairo_destroy (cr);
  this_curve -> displayed = TRUE;
#ifdef GTK3
  return TRUE;
#endif
}

/*!
  \fn void hide_curve (gpointer data)

  \brief hide curve

  \param data the associated data pointer
*/
void hide_curve (gpointer data)
{
  tint * idc = (tint *)data;
  Curve * this_curve = get_curve_from_pointer (data);
  hide_the_widgets (this_curve -> window);
  if (idc -> a == activep)
  {
    adjust_tool_model (idc -> b, idc -> c, this_curve -> path);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean to_hide_curve (GtkWindow * thecurve, gpointer data)

  \brief hide curve callback GTK4

  \param thecurve the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean to_hide_curve (GtkWindow * thecurve, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean to_hide_curve (GtkWidget * thecurve, GdkEvent * event, gpointer data)

  \brief hide curve callback GTK3

  \param thecurve the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean to_hide_curve (GtkWidget * thecurve, GdkEvent * event, gpointer data)
#endif
{
  hide_curve (data);
  return TRUE;
}
