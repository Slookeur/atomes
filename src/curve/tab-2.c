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

  DataLayout * get_extra_layout (gpointer data, int i);

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

GtkTreeModel * orgmodel;

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
  \fn DataLayout * get_extra_layout (gpointer data, int i)

  \brief retrieve the i data layout

  \param data the target data pointer
  \param i the id of data layout to retrieve
*/
DataLayout * get_extra_layout (gpointer data, int i)
{
  int j;
  CurveExtra * ctmp = get_curve_from_pointer (data) -> extrac -> first;
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
  Curve * this_curve = get_curve_from_pointer (data);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  DataLayout * layout;
  if (i > 0)
  {
    layout = get_extra_layout (data, i-1);
  }
  else
  {
    layout = this_curve -> layout;
  }
  pix = draw_surface (layout -> aspect,
                      layout -> hwidth,
                      layout -> hopac,
                      layout -> dash,
                      layout -> thickness,
                      layout -> datacolor,
                      this_curve -> backcolor,
                      layout -> glyph,
                      layout -> gsize);
  this_curve -> curve_edit -> stylearea = destroy_this_widget (this_curve -> curve_edit -> stylearea);
  this_curve -> curve_edit -> stylearea = create_image_from_data (IMG_SURFACE, pix);
  cairo_surface_destroy (pix);
  show_the_widgets (this_curve -> curve_edit -> stylearea);
#ifdef GTK4
  gtk_widget_set_hexpand (this_curve -> curve_edit -> stylearea, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_curve -> curve_edit -> pixarea, this_curve -> curve_edit -> stylearea, TRUE, TRUE, 0);
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
  curve_edition * cedit = get_curve_from_pointer(data) -> curve_edit;
  i = combo_get_active (cedit -> setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (data, i-1) -> glyph = j;
  }
  else
  {
    get_curve_from_pointer (data) -> layout -> glyph = j;
  }
  if (j == 0)
  {
    widget_set_sensitive (cedit -> data_glyph_size, 0);
    widget_set_sensitive (cedit -> data_glyph_freq, 0);
  }
  else
  {
    widget_set_sensitive (cedit -> data_glyph_size, 1);
    widget_set_sensitive (cedit -> data_glyph_freq, 1);
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
  curve_edition * cedit = get_curve_from_pointer(data) -> curve_edit;
  i = combo_get_active (cedit -> setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (data, i-1) -> dash = j;
  }
  else
  {
    get_curve_from_pointer (data) -> layout -> dash = j;
  }
  if (j == 0)
  {
    widget_set_sensitive (cedit -> data_thickness, 0);
  }
  else
  {
    widget_set_sensitive (cedit -> data_thickness, 1);
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
  i = combo_get_active (get_curve_from_pointer(data) -> curve_edit -> setcolorbox);
  if (i > 0)
  {
    get_extra_layout (data, i-1) -> datacolor = get_button_color (colob);
  }
  else
  {
    get_curve_from_pointer (data) -> layout -> datacolor = get_button_color (colob);
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
  Curve * this_curve = get_curve_from_pointer (data);
  wid = entry_get_text (thickd);
  k = string_to_double ((gpointer)wid);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (data, i-1) -> thickness = k;
    }
    else
    {
      this_curve -> layout -> thickness = k;
    }
    update_entry_double (thickd, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Line width must be > 0.0", this_curve -> window);
    if (i > 0)
    {
      update_entry_double (thickd, get_extra_layout (data, i-1) -> thickness);
    }
    else
    {
      update_entry_double (thickd, this_curve -> layout -> thickness);
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
  Curve * this_curve = get_curve_from_pointer (data);
  wid = entry_get_text (glsize);
  k = string_to_double ((gpointer)wid);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (data, i-1) -> gsize = k;
    }
    else
    {
      this_curve -> layout -> gsize = k;
    }
    update_entry_double (glsize, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Glyph size must be > 0.0", this_curve -> window);
    if (i > 0)
    {
      update_entry_double (glsize, get_extra_layout (data, i-1) -> gsize);
    }
    else
    {
      update_entry_double (glsize, this_curve -> layout -> gsize);
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
  Curve * this_curve = get_curve_from_pointer (data);
  wid = entry_get_text (entry);
  k = string_to_double ((gpointer)wid);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  if (k > 0.0)
  {
    if (i > 0)
    {
      get_extra_layout (data, i-1) -> hwidth = k;
    }
    else
    {
      this_curve -> layout -> hwidth = k;
    }
    update_entry_double (entry, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Bar width must be > 0.0", this_curve -> window);
    if (i > 0)
    {
      update_entry_double (entry, get_extra_layout (data, i-1) -> hwidth);
    }
    else
    {
      update_entry_double (entry, this_curve -> layout -> hwidth);
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
  Curve * this_curve = get_curve_from_pointer (data);
  wid = entry_get_text (entry);
  k = string_to_double ((gpointer)wid);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  if (k >= 0.0 && k <= 1.0)
  {
    if (i > 0)
    {
      get_extra_layout (data, i-1) -> hopac = k;
    }
    else
    {
      this_curve -> layout -> hopac = k;
    }
    update_entry_double (entry, k);
    set_data_style (data);
  }
  else
  {
    show_warning ("Bar opacity must be in [0.0 - 1.0]", this_curve -> window);
    if (i > 0)
    {
      update_entry_double (entry, get_extra_layout (data, i-1) -> hopac);
    }
    else
    {
      update_entry_double (entry, this_curve -> layout -> hopac);
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
  i = combo_get_active (get_curve_from_pointer (data) -> curve_edit -> setcolorbox);
  j = combo_get_active ((GtkWidget *)gbox);
  if (i > 0)
  {
    get_extra_layout (data, i-1) -> hpos = j;
  }
  else
  {
    get_curve_from_pointer (data) -> layout -> hpos = j;
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
  wid = entry_get_text (glfreq);
  j = string_to_double ((gpointer)wid);
  Curve * this_curve = get_curve_from_pointer (data);
  i = combo_get_active (this_curve -> curve_edit -> setcolorbox);
  if (j > 0)
  {
    if (i > 0)
    {
      get_extra_layout (data, i-1) -> gfreq = j;
    }
    else
    {
      this_curve -> layout -> gfreq = j;
    }
    update_entry_int (glfreq, j);
    set_data_style (data);
  }
  else
  {
    show_warning ("Glyph frequency must be > 0", this_curve -> window);
    if (i > 0)
    {
      update_entry_int (glfreq, get_extra_layout (data, i-1) -> thickness);
    }
    else
    {
      update_entry_int (glfreq, this_curve -> layout -> gfreq);
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
  int i;
  i = combo_get_active ((GtkWidget *)box);
  DataLayout * layout;
  if (i > 0)
  {
    layout = get_extra_layout (data, i-1);
  }
  else
  {
    layout = get_curve_from_pointer (data) -> layout;
  }
  curve_edition * cedit = get_curve_from_pointer (data) -> curve_edit;
  GdkRGBA col = colrgba_togtkrgba (layout -> datacolor);
  gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(cedit -> data_color), & col);
  combo_set_active (cedit -> data_dash, layout -> dash);
  update_entry_double (GTK_ENTRY(cedit -> data_thickness), layout -> thickness);
  combo_set_active (cedit -> data_glyph, layout -> glyph);
  update_entry_double (GTK_ENTRY(cedit -> data_glyph_size), layout -> gsize);
  update_entry_int (GTK_ENTRY(cedit -> data_glyph_freq), layout -> gfreq);
  update_entry_double (GTK_ENTRY(cedit -> data_hist_width), layout -> hwidth);
  update_entry_double (GTK_ENTRY(cedit -> data_hist_opac), layout -> hopac);
  combo_set_active (cedit -> data_hist_pos, layout -> hpos);
  combo_set_active (cedit -> data_aspect, layout -> aspect);
  set_data_style (data);
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
  curve_edition * cedit = get_curve_from_pointer (data) -> curve_edit;
  i = combo_get_active (cedit -> setcolorbox);
  j = combo_get_active ((GtkWidget *)box);
  if (j == 1)
  {
    combo_set_active (cedit -> data_glyph, 0);
    widget_set_sensitive (cedit -> data_glyph, 0);
    hide_the_widgets (cedit -> Glyph_box);
    show_the_widgets (cedit -> Hist_box);
  }
  else
  {
    widget_set_sensitive (cedit -> data_glyph, 1);
    hide_the_widgets (cedit -> Hist_box);
    show_the_widgets (cedit -> Glyph_box);
  }
  if (i > 0)
  {
    get_extra_layout (data, i-1) -> aspect = j;
  }
  else
  {
    get_curve_from_pointer (data) -> layout -> aspect = j;
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
  Curve * this_curve = get_curve_from_pointer (data);
  gchar * str;
  CurveExtra * ctmp;
  ctmp = this_curve -> extrac -> first;
  if (this_curve -> draw_id == this_curve -> extrac -> extras)
  {
    gtk_list_store_append (store, & curvelevel);
    str = g_strdup_printf ("%s - %s", get_project_by_id(((tint *)data) -> a) -> name, this_curve -> name);
    gtk_list_store_set (store, & curvelevel, 0, ((tint *)data) -> a, 1, ((tint *)data) -> b, 2, ((tint *)data) -> c, 3, str, -1);
    g_free (str);
  }
  i = this_curve -> extrac -> extras;
  while (ctmp)
  {
    gtk_list_store_append (store, & curvelevel);
    j = ctmp -> id.a;
    k = ctmp -> id.b;
    l = ctmp -> id.c;
#ifdef NEW_ANA
    str = g_strdup_printf ("%s - %s", get_project_by_id(j) -> name, get_project_by_id(j) -> analysis[k].curves[l] -> name);
#else
    str = g_strdup_printf ("%s - %s", get_project_by_id(j) -> name, get_project_by_id(j) -> curves[k][l] -> name);
#endif
    gtk_list_store_set (store, & curvelevel, 0, j, 1, k, 2, l, 3, str, -1);
    g_free (str);
    i --;
    if (this_curve -> draw_id == i)
    {
      gtk_list_store_append (store, & curvelevel);
      str = g_strdup_printf ("%s - %s", get_project_by_id(((tint *)data) -> a) -> name, this_curve -> name);
      gtk_list_store_set (store, & curvelevel, 0, ((tint *)data) -> a, 1, ((tint *)data) -> b, 2, ((tint *)data) -> c, 3, str, -1);
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
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  m = combo_get_active (cedit -> setcolorbox);
  ctmpa = NULL;
  if (m > 0)
  {
    ctmpa = this_curve -> extrac -> first;
    for (i=0; i<m-1; i++) ctmpa = ctmpa -> next;
    cbid = ctmpa -> id;
  }
  l = this_curve -> extrac -> extras;
  valid = gtk_tree_model_get_iter_first (tree_model, & iter);
  ctmpa = this_curve -> extrac -> first;
  while (valid)
  {
    gtk_tree_model_get (tree_model, & iter, 0, & i, 1, & j, 2, & k, -1);
    if (i == cid -> a && j == cid -> b && k == cid -> c)
    {
      // To set the draw order for the host curve
      this_curve -> draw_id = l;
    }
    else if (ctmpa -> id.a != i || ctmpa -> id.b != j || ctmpa -> id.c != k)
    {
      ctmpb = ctmpa -> next;
      done = FALSE;
      // Setting the pointer ctmpb to the corresponding (i,j,k) extra curve
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
      // At this point fixed ctmpb to (i,j,k) extra curve
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
          this_curve -> extrac -> last = ctmpa;
        }

        if (ctmpc)
        {
          ctmpb -> prev = ctmpc;
          ctmpc -> next = ctmpb;
        }
        else
        {
          ctmpb -> prev = NULL;
          this_curve -> extrac -> first = ctmpb;
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
    }
    l --;
    valid = gtk_tree_model_iter_next (tree_model, & iter);
  }
  if (m > 0)
  {
    ctmpa = this_curve -> extrac -> first;
    m = 0;
    while (ctmpa)
    {
      if (cbid.a == ctmpa -> id.a && cbid.b == ctmpa -> id.b && cbid.c == ctmpa -> id.c) break;
      m ++;
      ctmpa = ctmpa -> next;
    }
  }
  cedit -> setcolorbox = destroy_this_widget (cedit -> setcolorbox);
  cedit -> setcolorbox = create_combo ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> thesetbox, cedit -> setcolorbox, FALSE, FALSE, 0);
  show_the_widgets (cedit -> setcolorbox);
  prepbox (data);
  combo_set_active (cedit -> setcolorbox, m);
  choose_set (GTK_COMBO_BOX(cedit -> setcolorbox), data);
  update_curve (data);
}

/*!
  \fn void curve_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief Curve edit window, tab-2, Pango markup in tree view

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void curve_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, 3, & str, -1);
  g_object_set (renderer, "markup", str, NULL, NULL);
  g_free (str);
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
  curve_edition * cedit = get_curve_from_pointer (data) -> curve_edit;
  cedit -> orgtree = gtk_tree_view_new_with_model(orgmodel);
  for (i=0; i<4; i++)
  {
    orgcell[i] = gtk_cell_renderer_text_new();
    orgcol[i] =  gtk_tree_view_column_new_with_attributes(col_title[i], orgcell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(cedit -> orgtree), orgcol[i]);
    if (i < 3)
    {
      gtk_tree_view_column_set_visible (orgcol[i], FALSE);
    }
    else
    {
      gtk_tree_view_column_set_cell_data_func (orgcol[i], orgcell[i], curve_set_markup, NULL, NULL);
    }
  }
  fill_org_model (orglist, data);
  g_object_unref (orglist);
  g_signal_connect (G_OBJECT(orgmodel), "row-deleted", G_CALLBACK(move_back_front), data);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(cedit -> orgtree));
  gtk_tree_view_set_activate_on_single_click (GTK_TREE_VIEW(cedit -> orgtree), TRUE);
/*
#ifdef GTK4
  add_widget_gesture_and_key_action (cedit -> orgtree, "orgtree-pressed", NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL);
#endif
*/
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW(cedit -> orgtree), TRUE);
  return cedit -> orgtree;
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
  get_curve_from_pointer (data) -> bshift = button_get_status ((GtkWidget *)shift);
  update_curve (data);
}

/*!
  \fn GtkWidget * create_tab_2 (curve_edition * cedit, gpointer data)

  \brief handle the creation of the 2nd tab of the curve edition dialog

  \param cedit the target curve_edition pointer
  \param data the associated data pointer
*/
GtkWidget * create_tab_2 (curve_edition * cedit, gpointer data)
{
  GtkWidget * dhbox;
  Curve * this_curve = get_curve_from_pointer (data);
  int i;
  const int naspects = 2;
  char * aspects[2];
  aspects[0]="x/y";
  aspects[1]="bar";
  GtkWidget * databox = create_vbox (BSEP);
  cedit -> thesetbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, cedit -> thesetbox, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> thesetbox, markup_label("<b>Select set: </b>", -1, -1, 0.0, 0.5), FALSE, FALSE, 20);
  cedit -> setcolorbox = create_combo ();
  prepbox (data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> thesetbox, cedit -> setcolorbox, FALSE, FALSE, 10);

  cedit -> pixarea = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, cedit -> pixarea, FALSE, FALSE, 10);
  dhbox = create_vbox (BSEP);
  gtk_widget_set_size_request (dhbox, -1, 270);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> pixarea, dhbox, FALSE, FALSE, 0);

  cedit -> data_aspect = create_combo ();
  for ( i=0 ; i < naspects ; i++ )
  {
    combo_text_append (cedit -> data_aspect, aspects[i]);
  }
  combo_set_active (cedit -> data_aspect, this_curve -> layout -> aspect);
  gtk_widget_set_size_request (cedit -> data_aspect, 120, -1);
  g_signal_connect (G_OBJECT(cedit -> data_aspect), "changed", G_CALLBACK(set_data_aspect), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Plot type:"), cedit -> data_aspect, FALSE, FALSE, 0);
  if (((tint *)data) -> b == MS)
  {
    widget_set_sensitive (cedit -> data_aspect, 0);
  }

// Data color
  cedit -> data_color = color_button (this_curve -> layout -> datacolor, TRUE, 120, -1, G_CALLBACK(set_data_color), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Data color:"), cedit -> data_color, FALSE, FALSE, 0);

// Line style
  cedit -> data_dash = create_combo ();
  combo_text_append (cedit -> data_dash, "No line");
  for ( i=1 ; i < ndash ; i++)
  {
     combo_text_append (cedit -> data_dash, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (cedit -> data_dash, 120, -1);
  combo_set_active (cedit -> data_dash, this_curve -> layout -> dash);
  g_signal_connect (G_OBJECT(cedit -> data_dash), "changed", G_CALLBACK(set_data_dash), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Line style:"), cedit -> data_dash, FALSE, FALSE, 0);

// Line line width
  cedit -> data_thickness = create_entry (G_CALLBACK(set_data_thickness), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(cedit -> data_thickness), this_curve -> layout -> thickness);
  if (this_curve -> layout -> dash == 0)
  {
    widget_set_sensitive (cedit -> data_thickness, 0);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (dhbox, "Line width:"), cedit -> data_thickness, FALSE, FALSE, 0);

  GtkWidget * data_shape = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dhbox, data_shape, FALSE, FALSE, 0);
  cedit -> Glyph_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, data_shape, cedit -> Glyph_box, FALSE, FALSE, 0);
// Glyph type
  cedit -> data_glyph = create_combo ();
  combo_text_append (cedit -> data_glyph, "No glyph");
  for ( i=1 ; i < nglyph ; i++)
  {
     combo_text_append (cedit -> data_glyph, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (cedit -> data_glyph, 120, -1);
  combo_set_active (cedit -> data_glyph, this_curve -> layout -> glyph);
  g_signal_connect (G_OBJECT(cedit -> data_glyph), "changed", G_CALLBACK(set_data_glyph), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Glyph_box, "Glyph type:"), cedit -> data_glyph, FALSE, FALSE, 0);

// Glyph size
  cedit -> data_glyph_size = create_entry (G_CALLBACK(set_data_glyph_size), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(cedit -> data_glyph_size), this_curve -> layout -> gsize);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Glyph_box, "Glyph size:"), cedit -> data_glyph_size, FALSE, FALSE, 0);

// Glyph frequency
  cedit -> data_glyph_freq = create_entry (G_CALLBACK(set_data_glyph_freq), 120, 10, FALSE, data);
  update_entry_int (GTK_ENTRY(cedit -> data_glyph_freq), this_curve -> layout -> gfreq);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Glyph_box, "Glyph freq.:"), cedit -> data_glyph_freq, FALSE, FALSE, 0);
  if (this_curve -> layout -> glyph == 0)
  {
    widget_set_sensitive (cedit -> data_glyph_size, 0);
    widget_set_sensitive (cedit -> data_glyph_freq, 0);
  }

  cedit -> Hist_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, data_shape, cedit -> Hist_box, FALSE, FALSE, 0);
  // Histogram width
  cedit -> data_hist_width = create_entry (G_CALLBACK(set_data_hist_width), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(cedit -> data_hist_width), this_curve -> layout -> hwidth);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Hist_box, "Bar width:"), cedit -> data_hist_width, FALSE, FALSE, 0);
  // Histogram opacity
  cedit -> data_hist_opac = create_entry (G_CALLBACK(set_data_hist_opac), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(cedit -> data_hist_opac), this_curve -> layout -> hopac);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Hist_box, "Color opacity:"), cedit -> data_hist_opac, FALSE, FALSE, 0);

  cedit -> data_hist_pos = create_combo ();
  combo_text_append (cedit -> data_hist_pos, "Transparent");
  combo_text_append (cedit -> data_hist_pos, "Plain");
  gtk_widget_set_size_request (cedit -> data_hist_pos, 120, -1);
  combo_set_active (cedit -> data_hist_pos, this_curve -> layout -> hpos);
  g_signal_connect (G_OBJECT(cedit -> data_hist_pos), "changed", G_CALLBACK(set_data_hist_pos), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> Hist_box, "Bar opacity:"), cedit -> data_hist_pos, FALSE, FALSE, 0);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
  GtkWidget * hbox;
  if (((tint *)data) -> b != MS)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, hbox, FALSE, FALSE, 5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,
                         check_button ("Automatic <i>x axis</i> shift for bar diagram  (to improve visibility)", -1, -1, this_curve -> bshift, G_CALLBACK(set_bshift), data),
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
  cedit -> datascroll = create_scroll (hbox, 350, 200, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, cedit -> datascroll, create_org_list(data));
  widget_set_sensitive (cedit -> orgtree, this_curve -> extrac -> extras);
#ifndef GTK4
  gchar * str = "\tMove up/down to adjust layer position (up to front, down to back)";
  add_box_child_start (GTK_ORIENTATION_VERTICAL, databox, markup_label(str, -1, -1, 0.0, 0.5), FALSE, FALSE, 5);
#endif
  return databox;
}
