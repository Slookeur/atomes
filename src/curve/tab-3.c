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
* @file tab-3.c
* @short 3rd tab of the curve layout edition dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'tab-3.c'
*
* Contains:
*

 - The 3rd tab of the curve layout edition dialog

*
* List of functions:

  void set_legend_box_style (gpointer data);

  G_MODULE_EXPORT void show_data_legend (GtkCheckButton * leg, gpointer data);
  G_MODULE_EXPORT void show_data_legend (GtkToggleButton * leg, gpointer data);
  G_MODULE_EXPORT void show_data_legend_box (GtkCheckButton * bleg, gpointer data);
  G_MODULE_EXPORT void show_data_legend_box (GtkToggleButton * bleg, gpointer data);
  G_MODULE_EXPORT void set_legend_font (GtkFontButton * fontb, gpointer data);
  G_MODULE_EXPORT void set_legend_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_legend_pos (GtkEntry * p, gpointer data);
  G_MODULE_EXPORT void set_legend_box_line (GtkComboBox * fbox, gpointer data);
  G_MODULE_EXPORT void set_legend_box_thickness (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_legend_box_color (GtkColorChooser * colob, gpointer data);

  GtkWidget * create_tab_3 (gpointer data);

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

/*!
  \fn cairo_surface_t * draw_legend_surface (int da, double ti, ColRGBA dcol, ColRGBA bcol)

  \brief draw legend preview

  \param da the dash type
  \param ti the thickness
  \param dcol the legend color
  \param bcol the background color
*/
cairo_surface_t * draw_legend_surface (int da, double ti, ColRGBA dcol, ColRGBA bcol)
{
  cairo_surface_t * cst;
  cairo_t * tcst;
  curve_dash * tdash;
  cst = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 80, 80);
  tcst = cairo_create (cst);
  tdash = selectdash (da);
  cairo_set_dash (tcst, tdash -> a, tdash -> b, 0);
  cairo_set_source_rgb (tcst, bcol.red, bcol.green, bcol.blue);
  cairo_paint (tcst);
  cairo_set_source_rgb (tcst, dcol.red, dcol.green, dcol.blue);
  cairo_set_line_width (tcst, ti);
  cairo_move_to (tcst, 10, 10);
  cairo_line_to (tcst, 70, 10);
  cairo_line_to (tcst, 70, 70);
  cairo_line_to (tcst, 10, 70);
  cairo_line_to (tcst, 10, 10);
  cairo_stroke (tcst);
  g_free (tdash);
  cairo_destroy (tcst);
  return cst;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void show_data_legend (GtkCheckButton * leg, gpointer data)

  \brief show / hide legend toggle callback GTK4

  \param leg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_data_legend (GtkCheckButton * leg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void show_data_legend (GtkToggleButton * leg, gpointer data)

  \brief show / hide legend toggle callback GTK3

  \param leg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_data_legend (GtkToggleButton * leg, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> show_legend = button_get_status ((GtkWidget *)leg);
  widget_set_sensitive (this_curve -> curve_edit -> legend_box, this_curve -> show_legend);
  if (! this_curve -> show_legend)
  {
    widget_set_sensitive (this_curve -> curve_edit -> legend_box_style, 0);
  }
  else
  {
    widget_set_sensitive (this_curve -> curve_edit -> legend_box_style, this_curve -> show_legend_box);
  }
  update_curve (data);
}

/*!
  \fn void set_legend_box_style (gpointer data)

  \brief update legend style widgets

  \param data the associated data pointer
*/
void set_legend_box_style (gpointer data)
{
  cairo_surface_t * pix;
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  pix = draw_legend_surface (this_curve -> legend_box_dash,
                             this_curve -> legend_box_thickness,
                             this_curve -> legend_box_color,
                             this_curve -> backcolor);
  cedit -> legend_style_area = destroy_this_widget (cedit -> legend_style_area);
  cedit -> legend_style_area = create_image_from_data (IMG_SURFACE, (gpointer)pix);
  cairo_surface_destroy (pix);
  widget_set_sensitive (cedit -> legend_style_area, this_curve -> show_legend_box);
  show_the_widgets (cedit -> legend_style_area);
#ifdef GTK4
  gtk_widget_set_hexpand (cedit -> legend_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> legend_box_style, cedit -> legend_style_area, TRUE, TRUE, 0);
  update_curve (data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void show_data_legend_box (GtkCheckButton * bleg, gpointer data)

  \brief show / hide legend box toggle callback GTK4

  \param bleg the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_data_legend_box (GtkCheckButton * bleg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void show_data_legend_box (GtkToggleButton * bleg, gpointer data)

  \brief show / hide legend box toggle callback GTK3

  \param bleg the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_data_legend_box (GtkToggleButton * bleg, gpointer data)
#endif
{
  get_curve_from_pointer (data) -> show_legend_box = button_get_status ((GtkWidget *)bleg);
  widget_set_sensitive (get_curve_from_pointer (data) -> curve_edit -> legend_box_style, get_curve_from_pointer (data) -> show_legend_box);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_font (GtkFontButton * fontb, gpointer data)

  \brief change legend font

  \param fontb the GtkFontButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_font (GtkFontButton * fontb, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  g_free (this_curve -> legend_font);
  this_curve -> legend_font = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_color (GtkColorChooser * colob, gpointer data)

  \brief change legend color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_color (GtkColorChooser * colob, gpointer data)
{
  get_curve_from_pointer (data) -> legend_color = get_button_color (colob);
  set_legend_box_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_pos (GtkEntry * entry, gpointer data)

  \brief set the legend position entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_pos (GtkEntry * entry, gpointer data)
{
  const gchar *f;
  double z;
  qint * ad = (qint *)data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  f = entry_get_text (entry);
  z = string_to_double ((gpointer)f);
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  if (z >= 0.0 && z <= 1.0)
  {
    this_curve -> legend_pos[ad -> d] = z;
  }
  else
  {
    show_warning (ctext[ad -> d], this_curve -> window);
  }
  update_entry_double (entry, this_curve -> legend_pos[ad -> d]);

  update_curve (& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_box_line (GtkComboBox * fbox, gpointer data)

  \brief change the legend box line type

  \param fbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_box_line (GtkComboBox * fbox, gpointer data)
{
  get_curve_from_pointer (data) -> legend_box_dash = combo_get_active ((GtkWidget *)fbox) + 1;
  set_legend_box_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_box_thickness (GtkEntry * entry, gpointer data)

  \brief set legend box thickness entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_box_thickness (GtkEntry * entry, gpointer data)
{
  const gchar * str;
  Curve * this_curve = get_curve_from_pointer (data);
  str = entry_get_text (entry);
  this_curve -> legend_box_thickness = string_to_double ((gpointer)str);
  update_entry_double (entry, this_curve -> legend_box_thickness);
  set_legend_box_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_legend_box_color (GtkColorChooser * colob, gpointer data)

  \brief set legend color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_legend_box_color (GtkColorChooser * colob, gpointer data)
{
  get_curve_from_pointer (data) -> legend_box_color = get_button_color (colob);
  set_legend_box_style (data);
}

/*!
  \fn GtkWidget * create_tab_3 (curve_edition * cedit, gpointer data)

  \brief handle the creation of the 3rd tab of the curve edition dialog

  \param cedit the target curve_edition pointer
  \param data the associated data pointer
*/
GtkWidget * create_tab_3 (curve_edition * cedit, gpointer data)
{
  GtkWidget * dhbox;
  GtkWidget * legend_area;
  GtkWidget * legend_style_box;
  GtkWidget * legend_dash_box;
  GtkWidget * legend_thickness;

  int i;
  Curve * this_curve = get_curve_from_pointer (data);
  GtkWidget * legendbox = create_vbox (BSEP);
  GtkWidget * leghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, legendbox, leghbox, FALSE, FALSE, 10);
  GtkWidget * legvbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, leghbox, legvbox, FALSE, FALSE, 10);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, legvbox,
                      check_button ("Show/hide legend", -1, -1, this_curve -> show_legend, G_CALLBACK(show_data_legend), data),
                      FALSE, TRUE, 5);

  legend_area = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, legvbox, legend_area, FALSE, FALSE, 10);
  cedit -> legend_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, legend_area, cedit -> legend_box, FALSE, FALSE, 0);

  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> legend_box, "Font:"),
                      font_button (this_curve -> legend_font, 150, -1, G_CALLBACK(set_legend_font), data),
                      FALSE, FALSE, 0);

  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (cedit -> legend_box, "Color:"),
                       color_button (this_curve -> legend_color, TRUE, 150, 30, G_CALLBACK(set_legend_color), data),
                       FALSE, FALSE, 0);

  dhbox = bbox (cedit -> legend_box, "Position:");
  GtkWidget * lxyc;
  for ( i=0 ; i < 2 ; i++ )
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, dhbox, markup_label(lapos[i], (i==0)?10:30, -1, 1.0, 0.5), FALSE, FALSE, 5);
    lxyc = create_entry (G_CALLBACK(set_legend_pos), 100, 10, FALSE, (gpointer)& cedit -> dataxe[i]);
    update_entry_double (GTK_ENTRY(lxyc), this_curve -> legend_pos[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, dhbox, lxyc, FALSE, FALSE, 5);
  }

  dhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, cedit -> legend_box, dhbox, FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, dhbox,
                       check_button ("Show/hide legend box", -1, -1, this_curve -> show_legend_box, G_CALLBACK(show_data_legend_box), data),
                       FALSE, FALSE, 40);

  cedit -> legend_box_style = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, cedit -> legend_box, cedit -> legend_box_style, FALSE, FALSE, 0);
  legend_style_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> legend_box_style, legend_style_box, FALSE, FALSE, 0);

// Legend box style
  legend_dash_box = create_combo ();
  for ( i=1 ; i < ndash ; i++)
  {
     combo_text_append (legend_dash_box, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (legend_dash_box, 120, -1);
  combo_set_active (legend_dash_box, this_curve -> legend_box_dash - 1);
  g_signal_connect (G_OBJECT(legend_dash_box), "changed", G_CALLBACK(set_legend_box_line), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cbox (legend_style_box, "Line style:"), legend_dash_box, FALSE, FALSE, 0);

// Legend box linewidth
  legend_thickness = create_entry (G_CALLBACK(set_legend_box_thickness), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(legend_thickness), this_curve -> legend_box_thickness);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cbox (legend_style_box, "Line width:"), legend_thickness, FALSE, FALSE, 0);

// Legend box line color
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cbox (legend_style_box, "Color:"),
                       color_button (this_curve -> legend_box_color, TRUE, 120, -1, G_CALLBACK(set_legend_box_color), data),
                       FALSE, FALSE, 0);

// Legend box pix
  cairo_surface_t * legend_pix = draw_legend_surface (this_curve -> legend_box_dash,
                                                      this_curve -> legend_box_thickness,
                                                      this_curve -> legend_box_color,
                                                      this_curve -> backcolor);
  cedit -> legend_style_area =  create_image_from_data (IMG_SURFACE, (gpointer)legend_pix);
  cairo_surface_destroy (legend_pix);
#ifdef GTK4
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> legend_box_style, markup_label(" ", 20, -1, 0.0, 0.0), FALSE, FALSE, 0);
  gtk_widget_set_hexpand (cedit -> legend_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> legend_box_style, cedit -> legend_style_area, FALSE, FALSE, 0);

  widget_set_sensitive (cedit -> legend_box_style, this_curve -> show_legend_box);
  widget_set_sensitive (cedit -> legend_box, this_curve -> show_legend);
  return legendbox;
}
