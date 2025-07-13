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
* @file tab-2.c
* @short 2nd tab of the curve layout edition dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'tab-2.c'
*
* Contains:
*

 - The 2nd tab of the curve layout edition dialog

*
* List of functions:

  void set_data_style (gpointer data);

  static void fill_org_model (GtkListStore * store, gpointer data);

  G_MODULE_EXPORT void set_data_glyph (GtkComboBox * gbox, gpointer data);
  G_MODULE_EXPORT void set_data_dash (GtkComboBox * gbox, gpointer data);
  G_MODULE_EXPORT void set_data_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_data_thickness (GtkEntry * thickd, gpointer data);
  G_MODULE_EXPORT void set_data_glyph_size (GtkEntry * glsize, gpointer data);
  G_MODULE_EXPORT void set_data_hist_width (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_data_hist_opac (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_data_hist_pos (GtkComboBox * gbox, gpointer data);
  G_MODULE_EXPORT void set_data_glyph_freq (GtkEntry * glfreq, gpointer data);
  G_MODULE_EXPORT void choose_set (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_data_aspect (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void move_back_front (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data);
  G_MODULE_EXPORT void set_bshift (GtkCheckButton * shift, gpointer data);
  G_MODULE_EXPORT void set_bshift (GtkToggleButton * shift, gpointer data);

  GtkWidget * create_org_list (gpointer data);
  GtkWidget * create_tab_2 (gpointer data);

  DataLayout * get_extra_layout (int i);

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "cedit.h"

GtkWidget * data_shape = NULL;
GtkWidget * data_color = NULL;
GtkWidget * data_aspect = NULL;
GtkWidget * data_thickness = NULL;
GtkWidget * data_glyph = NULL;
GtkWidget * data_glyph_size = NULL;
GtkWidget * data_glyph_freq = NULL;
GtkWidget * data_hist_width = NULL;
GtkWidget * data_hist_opac = NULL;
GtkWidget * data_hist_pos = NULL;
GtkWidget * data_dash = NULL;
GtkWidget * stylearea = NULL;
GtkWidget * pixarea = NULL;

GtkWidget * Glyph_box = NULL;
GtkWidget * Hist_box = NULL;

GtkWidget * xyl[2];

GtkWidget * orgtree = NULL;
GtkTreeModel * orgmodel = NULL;
GtkWidget * datascroll = NULL;

extern qint dataxe[2];
extern int a, b, c, d;

/*!
  \fn cairo_surface_t * draw_surface (int aspect, double hwidth, double hopac, int da, double ti, ColRGBA dcol, ColRGBA bcol, int tglyph, double tgsize)

  \brief draw the data set preview

  \param aspect the data aspect (x/y or bars)
  \param hwidth the histogram bar width
  \param hopac the histogram bar opacity
  \param da the dash type
  \param ti the thickness
  \param dcol the data color
  \param bcol the background color
  \param tglyph the glyphe type
  \param tgsize the glyph size
*/
cairo_surface_t * draw_surface (int aspect, double hwidth, double hopac, int da, double ti, ColRGBA dcol, ColRGBA bcol,  int tglyph, double tgsize)
{
  cairo_surface_t * cst;
  cairo_t * tcst;
  curve_dash * tdash;
  double x, y;
  double x1, x2, y1, y2;
  switch (aspect)
  {
    case 1:
      cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 100);
      tcst = cairo_create(cst);
      cairo_set_source_rgb (tcst, bcol.red, bcol.green,  bcol.blue);
      cairo_paint (tcst);
      cairo_stroke (tcst);
      cairo_set_source_rgba (tcst, dcol.red, dcol.green,  dcol.blue, hopac);
      x1 = (100.0-hwidth*50.0)/2.0;
      x2 = hwidth*50.0;
      y1 = 15.0;
      y2 = 70.0;
      cairo_rectangle (tcst, x1, y1, x2, y2);
      cairo_fill (tcst);
      cairo_set_source_rgba (tcst, dcol.red, dcol.green,  dcol.blue, 1.0);
      cairo_stroke (tcst);
      if (da > 0)
      {
        tdash = selectdash (da);
        cairo_set_dash (tcst, tdash -> a, tdash -> b, 0);
        cairo_set_line_width (tcst, ti);
        x2 += x1;
        y2 = 100 - y1;
        cairo_move_to (tcst, x1, y2);
        cairo_line_to (tcst, x1, y1);
        cairo_move_to (tcst, x1, y1);
        cairo_line_to (tcst, x2, y1);
        cairo_move_to (tcst, x2, y1);
        cairo_line_to (tcst, x2, y2);
        cairo_stroke (tcst);
        g_free (tdash);
      }
      break;
    default:
      cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 100, 30);
      tcst = cairo_create(cst);
      cairo_set_source_rgb (tcst, bcol.red, bcol.green,  bcol.blue);
      cairo_paint (tcst);
      cairo_stroke (tcst);
      if (da > 0)
      {
        tdash = selectdash (da);
        cairo_set_dash (tcst, tdash -> a, tdash -> b, 0);
        cairo_set_source_rgb (tcst, dcol.red, dcol.green,  dcol.blue);
        cairo_set_line_width (tcst, ti);
        cairo_move_to (tcst, 0, 15);
        cairo_line_to (tcst, 100, 15);
        cairo_stroke (tcst);
        g_free (tdash);
      }
      x = 25.0;
      y = 15.0;
      draw_glyph (tcst, tglyph, x, y, dcol, tgsize);
      x = 75.0;
      draw_glyph (tcst, tglyph, x, y, dcol, tgsize);
      break;
  }
  cairo_destroy (tcst);
  return cst;
}

/*!
  \fn DataLayout * get_extra_layout (int i)

  \brief retrieve the i data layout

  \param i the id of data layout to retrieve
*/
DataLayout * get_extra_layout (int i)
{
  int j;
  CurveExtra * ctmp = get_project_by_id(a) -> curves[b][c] -> extrac -> first;
  for (j=0; j<i; j++)
  {
    ctmp = ctmp -> next;
  }
  return ctmp -> layout;
}

/*!
  \fn void set_data_style (gpointer data)

  \brief update the data style widgets

  \param data the associated data pointer
*/
void set_data_style (gpointer data)
{
  int i;
  cairo_surface_t * pix;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  i = combo_get_active (setcolorbox);
  project * this_proj = get_project_by_id(a);
  DataLayout * layout;
  if (i > 0)
  {
    layout = get_extra_layout (i-1);
  }
  else
  {
    layout = this_proj -> curves[b][c] -> layout;
  }
  pix = draw_surface (layout -> aspect,
                      layout -> hwidth,
                      layout -> hopac,
                      layout -> dash,
                      layout -> thickness,
                      layout -> datacolor,
                      this_proj -> curves[b][c] -> backcolor,
                      layout -> glyph,
                      layout -> gsize);
  stylearea = destroy_this_widget (stylearea);
  stylearea =  create_image_from_data (IMG_SURFACE, pix);
  cairo_surface_destroy (pix);
  show_the_widgets (stylearea);
#ifdef GTK4
  gtk_widget_set_hexpand (stylearea, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pixarea, stylearea, TRUE, TRUE, 0);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_data_glyph (GtkComboBox * gbox, gpointer data)

  \brief change glyph type

  \param gbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_glyph (GtkComboBox * gbox, gpointer data)
{
  int i, j;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  i = combo_get_active (setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (i-1) -> glyph = j;
  }
  else
  {
    get_project_by_id(a) -> curves[b][c] -> layout -> glyph = j;
  }
  if (j == 0)
  {
    widget_set_sensitive (data_glyph_size, 0);
    widget_set_sensitive (data_glyph_freq, 0);
  }
  else
  {
    widget_set_sensitive (data_glyph_size, 1);
    widget_set_sensitive (data_glyph_freq, 1);
  }
  set_data_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_data_dash (GtkComboBox * gbox, gpointer data)

  \brief change data dash style

  \param gbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_dash (GtkComboBox * gbox, gpointer data)
{
  int i, j;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  i = combo_get_active (setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (i-1) -> dash = j;
  }
  else
  {
    get_project_by_id(a) -> curves[b][c] -> layout -> dash = j;
  }
  if (j == 0)
  {
    widget_set_sensitive (data_thickness, 0);
  }
  else
  {
    widget_set_sensitive (data_thickness, 1);
  }
  set_data_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_data_color (GtkColorChooser * colob, gpointer data)

  \brief set data color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_color (GtkColorChooser * colob, gpointer data)
{
  int i;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (i > 0)
  {
    get_extra_layout (i-1) -> datacolor = get_button_color (colob);
  }
  else
  {
    this_proj -> curves[b][c] -> layout -> datacolor = get_button_color (colob);
  }
  set_data_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_data_thickness (GtkEntry * thickd, gpointer data)

  \brief set data thickness entry callback

  \param thickd the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_thickness (GtkEntry * thickd, gpointer data)
{
  int i;
  double k;
  const gchar * wid;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  wid = entry_get_text (thickd);
  k = string_to_double ((gpointer)wid);
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (i-1) -> thickness = k;
    }
    else
    {
      this_proj -> curves[b][c] -> layout -> thickness = k;
    }
    update_entry_double (thickd, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Line width must be > 0.0", this_proj -> curves[b][c] -> window);
    if (i > 0)
    {
      update_entry_double (thickd, get_extra_layout (i-1) -> thickness);
    }
    else
    {
      update_entry_double (thickd, this_proj -> curves[b][c] -> layout -> thickness);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_data_glyph_size (GtkEntry * glsize, gpointer data)

  \brief set glyph size entry callback

  \param glsize the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_glyph_size (GtkEntry * glsize, gpointer data)
{
  int i;
  double k;
  const gchar * wid;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  wid = entry_get_text (glsize);
  k = string_to_double ((gpointer)wid);
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (i-1) -> gsize = k;
    }
    else
    {
      this_proj -> curves[b][c] -> layout -> gsize = k;
    }
    update_entry_double (glsize, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Glyph size must be > 0.0", this_proj -> curves[b][c] -> window);
    if (i > 0)
    {
      update_entry_double (glsize, get_extra_layout (i-1) -> gsize);
    }
    else
    {
      update_entry_double (glsize, this_proj -> curves[b][c] -> layout -> gsize);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_data_hist_width (GtkEntry * entry, gpointer data)

  \brief set histogram bar width entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_hist_width (GtkEntry * entry, gpointer data)
{
  int i;
  double k;
  const gchar * wid;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  wid = entry_get_text (entry);
  k = string_to_double ((gpointer)wid);
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (i-1) -> hwidth = k;
    }
    else
    {
      this_proj -> curves[b][c] -> layout -> hwidth = k;
    }
    update_entry_double (entry, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Bar width must be > 0.0", this_proj -> curves[b][c] -> window);
    if (i > 0)
    {
      update_entry_double (entry, get_extra_layout (i-1) -> hwidth);
    }
    else
    {
      update_entry_double (entry, this_proj -> curves[b][c] -> layout -> hwidth);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_data_hist_opac (GtkEntry * entry, gpointer data)

  \brief set histogram bar opacity entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_hist_opac (GtkEntry * entry, gpointer data)
{
  int i;
  double k;
  const gchar * wid;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  wid = entry_get_text (entry);
  k = string_to_double ((gpointer)wid);
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (k >= 0.0 && k <= 1.0)
  {
    if (i > 0)
    {
      get_extra_layout (i-1) -> hopac = k;
    }
    else
    {
      this_proj -> curves[b][c] -> layout -> hopac = k;
    }
    update_entry_double (entry, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Bar opacity must be in [0.0 - 1.0]", this_proj -> curves[b][c] -> window);
    if (i > 0)
    {
      update_entry_double (entry, get_extra_layout (i-1) -> hopac);
    }
    else
    {
      update_entry_double (entry, this_proj -> curves[b][c] -> layout -> hopac);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_data_hist_pos (GtkComboBox * gbox, gpointer data)

  \brief change histogram bar position

  \param gbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_hist_pos (GtkComboBox * gbox, gpointer data)
{
  int i, j;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  i = combo_get_active (setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (i-1) -> hpos = j;
  }
  else
  {
    get_project_by_id(a) -> curves[b][c] -> layout -> hpos = j;
  }
  set_data_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_data_glyph_freq (GtkEntry * glfreq, gpointer data)

  \brief set glyph frequency entry callback

  \param glfreq the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_glyph_freq (GtkEntry * glfreq, gpointer data)
{
  int i, j;
  const gchar * wid;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  wid = entry_get_text (glfreq);
  j = string_to_double ((gpointer)wid);
  project * this_proj = get_project_by_id(a);
  i = combo_get_active (setcolorbox);
  if (j > 0)
  {
    if (i > 0)
    {
      get_extra_layout (i-1) -> gfreq = j;
    }
    else
    {
      this_proj -> curves[b][c] -> layout -> gfreq = j;
    }
    update_entry_int (glfreq, j);
    set_data_style (data);
  }
  else
  {
    show_warning ("Glyph frequency must be > 0", this_proj -> curves[b][c] -> window);
    if (i > 0)
    {
      update_entry_int (glfreq, get_extra_layout (i-1) -> thickness);
    }
    else
    {
      update_entry_int (glfreq, this_proj -> curves[b][c] -> layout -> gfreq);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void choose_set (GtkComboBox * box, gpointer data)

  \brief change the data set to customize

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void choose_set (GtkComboBox * box, gpointer data)
{
  int i, j, k, l;
  tint ad;

  i = combo_get_active ((GtkWidget *)box);
  j = dataxe[0].a;
  k = dataxe[0].b;
  l = dataxe[0].c;
  ad.a = j;
  ad.b = k;
  ad.c = l;
  DataLayout * layout;
  if (i > 0)
  {
    layout = get_extra_layout (i-1);
  }
  else
  {
    layout = get_project_by_id(j) -> curves[k][l] -> layout;
  }
  GdkRGBA col = colrgba_togtkrgba (layout -> datacolor);
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(data_color), & col);
  combo_set_active (data_dash, layout -> dash);
  update_entry_double (GTK_ENTRY(data_thickness), layout -> thickness);
  combo_set_active (data_glyph, layout -> glyph);
  update_entry_double (GTK_ENTRY(data_glyph_size), layout -> gsize);
  update_entry_int (GTK_ENTRY(data_glyph_freq), layout -> gfreq);
  update_entry_double (GTK_ENTRY(data_hist_width), layout -> hwidth);
  update_entry_double (GTK_ENTRY(data_hist_opac), layout -> hopac);
  combo_set_active (data_hist_pos, layout -> hpos);
  combo_set_active (data_aspect, layout -> aspect);
  set_data_style (& ad);
}

/*!
  \fn G_MODULE_EXPORT void set_data_aspect (GtkComboBox * box, gpointer data)

  \brief change data aspect (x/y or histogram bars)

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_data_aspect (GtkComboBox * box, gpointer data)
{
  int i, j;
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  i = combo_get_active (setcolorbox);
  j = combo_get_active ((GtkWidget *)box);
  if (j == 1)
  {
    combo_set_active (data_glyph, 0);
    widget_set_sensitive (data_glyph, 0);
    hide_the_widgets (Glyph_box);
    show_the_widgets (Hist_box);
  }
  else
  {
    widget_set_sensitive (data_glyph, 1);
    hide_the_widgets (Hist_box);
    show_the_widgets (Glyph_box);
  }
  if (i > 0)
  {
    get_extra_layout (i-1) -> aspect = j;
  }
  else
  {
    get_project_by_id(a) -> curves[b][c] -> layout -> aspect = j;
  }
  set_data_style (data);
  update_curve (data);
}

/*!
  \fn static void fill_org_model (GtkListStore * store, gpointer data)

  \brief fill the data set list store

  \param store the data set list store to fill
  \param data the associated data pointer
*/
static void fill_org_model (GtkListStore * store, gpointer data)
{
  GtkTreeIter curvelevel;
  int i, j, k, l;
  tint * cd = (tint *) data;
  gchar * str;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id (a);
  CurveExtra * ctmp;
  ctmp = this_proj -> curves[b][c] -> extrac -> first;
  if (this_proj -> curves[b][c] -> draw_id == this_proj -> curves[b][c] -> extrac -> extras)
  {
    gtk_list_store_append (store, & curvelevel);
    str = g_strdup_printf ("%s - %s", prepare_for_title(this_proj -> name), this_proj -> curves[b][c] -> name);
    gtk_list_store_set (store, & curvelevel, 0, a, 1, b, 2, c, 3, str, -1);
    g_free (str);
  }
  i = this_proj -> curves[b][c] -> extrac -> extras;
  while (ctmp)
  {
    gtk_list_store_append (store, & curvelevel);
    j = ctmp -> id.a;
    k = ctmp -> id.b;
    l = ctmp -> id.c;
    str = g_strdup_printf ("%s - %s", prepare_for_title(get_project_by_id(j) -> name),
                                      get_project_by_id(j) -> curves[k][l] -> name);
    gtk_list_store_set (store, & curvelevel, 0, j, 1, k, 2, l, 3, str, -1);
    g_free (str);
    i --;
    if (this_proj -> curves[b][c] -> draw_id == i)
    {
      gtk_list_store_append (store, & curvelevel);
      str = g_strdup_printf ("%s - %s", prepare_for_title(this_proj -> name), this_proj -> curves[b][c] -> name);
      gtk_list_store_set (store, & curvelevel, 0, a, 1, b, 2, c, 3, str, -1);
      g_free (str);
    }
    ctmp = ctmp -> next;
  }
}

/*!
  \fn G_MODULE_EXPORT void move_back_front (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data)

  \brief move up or down data set in the tree model to move it front or back in the data plot

  \param tree_model the GtkTreeModel sending the signal
  \param path the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void move_back_front (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data)
{
  tint * cid = (tint *) data;
  GtkTreeIter iter;
  gboolean valid;
  gboolean done;
  int i, j, k, l, m;
  tint cbid;
  CurveExtra * ctmpa, * ctmpb, * ctmpc, * ctmpd;
  project * this_proj = get_project_by_id(cid -> a);
  l = this_proj -> curves[cid -> b][cid -> c] -> extrac -> extras;
  m = combo_get_active (setcolorbox);
  if (m > 0)
  {
    ctmpa = this_proj -> curves[cid -> b][cid -> c] -> extrac -> first;
    for (i=0; i<m-1; i++) ctmpa = ctmpa -> next;
    cbid = ctmpa -> id;
  }
  ctmpa = this_proj -> curves[cid -> b][cid -> c] -> extrac -> first;
  valid = gtk_tree_model_get_iter_first (tree_model, & iter);
  while (valid)
  {
    gtk_tree_model_get (tree_model, & iter, 0, & i, 1, & j, 2, & k, -1);
    if (i == cid -> a && j == cid -> b && k == cid -> c)
    {
      this_proj -> curves[cid -> b][cid -> c] -> draw_id = l;
    }
    else if (ctmpa -> id.a != i || ctmpa -> id.b != j || ctmpa -> id.c != k)
    {
      ctmpb = ctmpa -> next;
      done = FALSE;
      while (! done && ctmpb)
      {
        if (ctmpb -> id.a == i && ctmpb -> id.b == j && ctmpb -> id.c == k)
        {
          done = TRUE;
        }
        else
        {
          ctmpb = ctmpb -> next;
          done = FALSE;
        }
      }

      if (done)
      {
        ctmpc = ctmpa -> prev;
        if (ctmpb -> prev != ctmpa)
        {
          ctmpb -> prev -> next = ctmpa;
          ctmpa -> prev = ctmpb -> prev;
          ctmpd = ctmpa -> next;
        }
        else
        {
          ctmpa -> prev = ctmpb;
          ctmpd = NULL;
        }

        if (ctmpb -> next)
        {
          ctmpb -> next -> prev = ctmpa;
          ctmpa -> next = ctmpb -> next;
        }
        else
        {
          ctmpa -> next = NULL;
          this_proj -> curves[cid -> b][cid -> c] -> extrac -> last = ctmpa;
        }

        if (ctmpc)
        {
          ctmpb -> prev = ctmpc;
          ctmpc -> next = ctmpb;
        }
        else
        {
          ctmpb -> prev = NULL;
          this_proj -> curves[cid -> b][cid -> c] -> extrac -> first = ctmpb;
        }
        if (ctmpd)
        {
          ctmpd -> prev = ctmpb;
          ctmpb -> next = ctmpd;
        }
        else
        {
          ctmpb -> next = ctmpa;
          ctmpa -> prev = ctmpb;
        }
        ctmpa = ctmpb;
      }
      ctmpa = ctmpa -> next;
    }
    else
    {
      ctmpa = ctmpa -> next;
    }
    l --;
    valid = gtk_tree_model_iter_next (tree_model, & iter);
  }
  if (m > 0)
  {
    ctmpa = this_proj -> curves[cid -> b][cid -> c] -> extrac -> first;
    m = 0;
    while (ctmpa)
    {
      if (cbid.a == ctmpa -> id.a && cbid.b == ctmpa -> id.b && cbid.c == ctmpa -> id.c) break;
      m ++;
      ctmpa = ctmpa -> next;
    }
    m ++;
  }
  setcolorbox = destroy_this_widget (setcolorbox);
  setcolorbox = create_combo ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, thesetbox, setcolorbox, FALSE, FALSE, 0);
  show_the_widgets (setcolorbox);
  prepbox (this_proj -> id, cid -> b, cid -> c);
  combo_set_active (setcolorbox, m);
  choose_set (GTK_COMBO_BOX(setcolorbox), NULL);
  update_curve (data);
}

/*!
  \fn GtkWidget * create_org_list (gpointer data)

  \brief create the data set organisation widget

  \param data the associated data pointer
*/
GtkWidget * create_org_list (gpointer data)
{
  int i;
  GtkTreeViewColumn * orgcol[4];
  GtkCellRenderer * orgcell[4];
  gchar * col_title[4] = {" ", " ", " ", "Data set(s)"};
  gchar * ctype[4] = {"text", "text", "text", "text"};
  GType col_type[4] = {G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING};
  GtkListStore * orglist = gtk_list_store_newv (4, col_type);
  orgmodel = GTK_TREE_MODEL(orglist);
  orgtree = gtk_tree_view_new_with_model(orgmodel);
  for (i=0; i<4; i++)
  {
    orgcell[i] = gtk_cell_renderer_text_new();
    orgcol[i] =  gtk_tree_view_column_new_with_attributes(col_title[i], orgcell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(orgtree), orgcol[i]);
    if (i < 3) gtk_tree_view_column_set_visible (orgcol[i], FALSE);
  }
  fill_org_model (orglist, data);
  g_object_unref (orglist);
  g_signal_connect (G_OBJECT(orgmodel), "row-deleted", G_CALLBACK(move_back_front), data);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(orgtree));
  gtk_tree_view_set_activate_on_single_click (GTK_TREE_VIEW(orgtree), TRUE);
/*
#ifdef GTK4
  add_widget_gesture_and_key_action (orgtree, "orgtree-pressed", NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL);
#endif
*/
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW(orgtree), TRUE);
  return orgtree;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_bshift (GtkCheckButton * shift, gpointer data)

  \brief shift / not histogram bars toggle callback GTK4

  \param shift the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_bshift (GtkCheckButton * shift, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_bshift (GtkToggleButton * shift, gpointer data)

  \brief shift / not histogram bars toggle callback GTK3

  \param shift the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_bshift (GtkToggleButton * shift, gpointer data)
#endif
{
  tint * cd = (tint *)data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> bshift = button_get_status ((GtkWidget *)shift);
  update_curve (data);
}

/*!
  \fn GtkWidget * create_tab_2 (gpointer data)

  \brief handle the creation of the 2nd tab of the curve edition dialog

  \param data the associated data pointer
*/
GtkWidget * create_tab_2 (gpointer data)
{
  GtkWidget * databox;
  GtkWidget * dhbox;

  tint * cd = (tint *) data;
  int i;

  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id(a);

  const int naspects = 2;
  char * aspects[2];
  aspects[0]="x/y";
  aspects[1]="bar";

  databox = create_vbox (BSEP);
  thesetbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, thesetbox, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, thesetbox, markup_label("<b>Select set: </b>", -1, -1, 0.0, 0.5), FALSE, FALSE, 20);
  setcolorbox = create_combo ();
  prepbox (a, b, c);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, thesetbox, setcolorbox, FALSE, FALSE, 10);

  pixarea = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, pixarea, FALSE, FALSE, 10);
  dhbox = create_vbox (BSEP);
  gtk_widget_set_size_request (dhbox, -1, 270);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pixarea, dhbox, FALSE, FALSE, 0);

  data_aspect = create_combo ();
  for ( i=0 ; i < naspects ; i++ )
  {
    combo_text_append (data_aspect, aspects[i]);
  }
  combo_set_active (data_aspect, this_proj -> curves[b][c] -> layout -> aspect);
  gtk_widget_set_size_request (data_aspect, 120, -1);
  g_signal_connect (G_OBJECT(data_aspect), "changed", G_CALLBACK(set_data_aspect), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Plot type:"), data_aspect, FALSE, FALSE, 0);
  if (b == MS)
  {
    widget_set_sensitive (data_aspect, 0);
  }

// Data color
  data_color = color_button (this_proj -> curves[b][c] -> layout -> datacolor, TRUE, 120, -1, G_CALLBACK(set_data_color), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Data color:"), data_color, FALSE, FALSE, 0);

// Line style
  data_dash = create_combo ();
  combo_text_append (data_dash, "No line");
  for ( i=1 ; i < ndash ; i++)
  {
     combo_text_append (data_dash, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (data_dash, 120, -1);
  combo_set_active (data_dash, this_proj -> curves[b][c] -> layout -> dash);
  g_signal_connect (G_OBJECT(data_dash), "changed", G_CALLBACK(set_data_dash), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Line style:"), data_dash, FALSE, FALSE, 0);

// Line line width
  data_thickness = create_entry (G_CALLBACK(set_data_thickness), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(data_thickness), this_proj -> curves[b][c] -> layout -> thickness);
  if (this_proj -> curves[b][c] -> layout -> dash == 0)
  {
    widget_set_sensitive (data_thickness, 0);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Line width:"), data_thickness, FALSE, FALSE, 0);

  GtkWidget * data_shape = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dhbox, data_shape, FALSE, FALSE, 0);
  Glyph_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, data_shape, Glyph_box, FALSE, FALSE, 0);
// Glyph type
  data_glyph = create_combo ();
  combo_text_append (data_glyph, "No glyph");
  for ( i=1 ; i < nglyph ; i++)
  {
     combo_text_append (data_glyph, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (data_glyph, 120, -1);
  combo_set_active (data_glyph, this_proj -> curves[b][c] -> layout -> glyph);
  g_signal_connect (G_OBJECT(data_glyph), "changed", G_CALLBACK(set_data_glyph), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Glyph_box, "Glyph type:"), data_glyph, FALSE, FALSE, 0);

// Glyph size
  data_glyph_size = create_entry (G_CALLBACK(set_data_glyph_size), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(data_glyph_size), this_proj -> curves[b][c] -> layout -> gsize);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Glyph_box, "Glyph size:"), data_glyph_size, FALSE, FALSE, 0);

// Glyph frequency
  data_glyph_freq = create_entry (G_CALLBACK(set_data_glyph_freq), 120, 10, FALSE, data);
  update_entry_int (GTK_ENTRY(data_glyph_freq), this_proj -> curves[b][c] -> layout -> gfreq);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Glyph_box, "Glyph freq.:"), data_glyph_freq, FALSE, FALSE, 0);
  if (this_proj -> curves[b][c] -> layout -> glyph == 0)
  {
    widget_set_sensitive (data_glyph_size, 0);
    widget_set_sensitive (data_glyph_freq, 0);
  }

  Hist_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, data_shape, Hist_box, FALSE, FALSE, 0);
  // Histogram width
  data_hist_width = create_entry (G_CALLBACK(set_data_hist_width), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(data_hist_width), this_proj -> curves[b][c] -> layout -> hwidth);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Hist_box, "Bar width:"), data_hist_width, FALSE, FALSE, 0);
  // Histogram opacity
  data_hist_opac = create_entry (G_CALLBACK(set_data_hist_opac), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(data_hist_opac), this_proj -> curves[b][c] -> layout -> hopac);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Hist_box, "Color opacity:"), data_hist_opac, FALSE, FALSE, 0);

  data_hist_pos = create_combo ();
  combo_text_append (data_hist_pos, "Transparent");
  combo_text_append (data_hist_pos, "Plain");
  gtk_widget_set_size_request (data_hist_pos, 120, -1);
  combo_set_active (data_hist_pos, this_proj -> curves[b][c] -> layout -> hpos);
  g_signal_connect (G_OBJECT(data_hist_pos), "changed", G_CALLBACK(set_data_hist_pos), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (Hist_box, "Bar opacity:"), data_hist_pos, FALSE, FALSE, 0);


  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
  GtkWidget * hbox;
  if (b != MS)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, hbox, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                         check_button ("Automatic <i>x axis</i> shift for bar diagram  (to improve visibility)", -1, -1, this_proj -> curves[b][c] -> bshift, G_CALLBACK(set_bshift), data),
                         FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
  }
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, hbox, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>Layers organization: </b>", -1, -1, 0.0, 0.5), FALSE, FALSE, 20);
  GtkWidget * shbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, shbox, FALSE, FALSE, 5);

  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, shbox, hbox, FALSE, FALSE, 75);
  datascroll = create_scroll (hbox, 350, 200, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, datascroll, create_org_list(data));
  widget_set_sensitive (orgtree, this_proj -> curves[b][c] -> extrac -> extras);
#ifndef GTK4
  gchar * str = "\tMove up/down to adjust layer position (up to front, down to back)";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
#endif
  return databox;
}
