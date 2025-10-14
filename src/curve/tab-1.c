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
* @file tab-1.c
* @short 1st tab of the curve layout edition dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'tab-1.c'
*
* Contains:
*

 - The 1st tab of the curve layout edition dialog

*
* List of functions:

  void set_frame_style (gpointer data);

  G_MODULE_EXPORT void set_window_size (GtkEntry * maj, gpointer data);
  G_MODULE_EXPORT void set_title (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_title (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_title_default (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_title_default (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_title_custom (GtkEntry * tit, gpointer data);
  G_MODULE_EXPORT void set_title_font (GtkFontButton * fontb, gpointer data);
  G_MODULE_EXPORT void set_title_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_title_pos (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_show_frame (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_show_frame (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_background_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_frame_type (GtkComboBox * fbox, gpointer data);
  G_MODULE_EXPORT void set_frame_line (GtkComboBox * fbox, gpointer data);
  G_MODULE_EXPORT void set_frame_thickness (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void set_frame_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_frame_pos (GtkEntry * fen, gpointer data);

  cairo_surface_t * draw_frame_surface (int tf,
                                        int da,
                                        double ti,
                                        double x[2],
                                        double y[2],
                                        ColRGBA dcol,
                                        ColRGBA bcol);

  GtkWidget * create_tab_1 (gpointer data);

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

extern void set_data_style (gpointer data);

char * ctext[2] = {"x ∈ [0.0, 1.0]", "y ∈ [0.0, 1.0]"};

/*!
  \fn G_MODULE_EXPORT void set_window_size (GtkEntry * maj, gpointer data)

  \brief change window size entry callback

  \param maj the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_window_size (GtkEntry * maj, gpointer data)
{
  const gchar *m;
  char * text[2];
  int i;
  qint * ad = (qint *)data;
#ifdef NEW_ANA
  Curve * this_curve = get_project_by_id (ad -> a) -> curves[ad -> b][ad -> c];
#else
  Curve * this_curve = get_project_by_id (ad -> a) -> analysis[ad -> b].curves[ad -> c];
#endif
  int shift = get_curve_shift (this_curve);
  text[0] = "X size must be > 0";
  text[1] = "Y size must be > 0";
  m = entry_get_text (maj);

  i = string_to_double ((gpointer)m);
  if (i > 0)
  {
    switch (ad -> d)
    {
      case 0:
        resize_this_window (this_curve -> window, i, this_curve -> wsize[1] + shift);
        break;
      case 1:
        resize_this_window (this_curve -> window, this_curve -> wsize[0], i + shift);
        break;
    }
    this_curve -> wsize[ad -> d] = i;
  }
  else
  {
    show_warning (text[ad -> d], this_curve -> window);
  }
  update_entry_int (maj, this_curve -> wsize[ad -> d]);
}

/*!
  \fn cairo_surface_t * draw_frame_surface (int tf,
                                         int da,
                                         double ti,
                                         double x[2],
                                         double y[2],
                                         ColRGBA dcol,
                                         ColRGBA bcol)

  \brief draw frame preview

  \param tf frame type
  \param da frame dash
  \param ti frame thickness
  \param x x positions (min / max)
  \param y y positions (min / max)
  \param dcol frame color
  \param bcol background color
*/
cairo_surface_t * draw_frame_surface (int tf,
                                      int da,
                                      double ti,
                                      double x[2],
                                      double y[2],
                                      ColRGBA dcol,
                                      ColRGBA bcol)
{
  cairo_surface_t * cst;
  cairo_t * tcst;
  int r[2];
  r[0] = 100;
  r[1] = 100;
  cst = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 100, 100);
  tcst = cairo_create (cst);
  cairo_set_source_rgb (tcst, bcol.red, bcol.green, bcol.blue);
  cairo_paint (tcst);
  show_frame (tcst, tf, da, r, ti, x, y, dcol);
  cairo_destroy (tcst);
  return cst;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_title (GtkCheckButton * but, gpointer data)

  \brief show / hide title toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_title (GtkToggleButton * but, gpointer data)

  \brief show / hide title toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title (GtkToggleButton * but, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> show_title = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (this_curve -> curve_edit -> title_box, this_curve -> show_title);
  update_curve (data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_title_default (GtkCheckButton * but, gpointer data)

  \brief use / not default title toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_default (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_title_default (GtkToggleButton * but, gpointer data)

  \brief use / not default title toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_default (GtkToggleButton * but, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> default_title = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (this_curve -> curve_edit -> custom_title, ! this_curve -> default_title);
  if (this_curve -> default_title)
  {
    g_free (this_curve -> title);
    this_curve -> title = g_strdup_printf ("%s - %s", prepare_for_title(get_project_by_id(((tint *)data) -> a) -> name), this_curve -> name);
  }
  update_entry_text (GTK_ENTRY(this_curve -> curve_edit -> custom_title), this_curve -> title);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_title_custom (GtkEntry * tit, gpointer data)

  \brief set custom title entry callback

  \param tit the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_custom (GtkEntry * tit, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  g_free (this_curve -> title);
  this_curve -> title = g_strdup_printf ("%s", entry_get_text (tit));
  update_entry_text (tit, this_curve -> title);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_title_font (GtkFontButton * fontb, gpointer data)

  \brief set title font

  \param fontb the GtkFontButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_font (GtkFontButton * fontb, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  g_free (this_curve -> title_font);
  this_curve -> title_font = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_title_color (GtkColorChooser * colob, gpointer data)

  \brief set title color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_color (GtkColorChooser * colob, gpointer data)
{
  get_curve_from_pointer (data) -> title_color = get_button_color (colob);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_title_pos (GtkEntry * entry, gpointer data)

  \brief set axis title position entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_title_pos (GtkEntry * entry, gpointer data)
{
  const gchar * p;
  double v;
  qint * ad = (qint *)data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  p = entry_get_text (entry);
  v = string_to_double ((gpointer)p);
  if (v >= 0.0 && v <= 1.0)
  {
    this_curve -> title_pos[ad -> d] = v;
  }
  else
  {
    show_warning (ctext[ad -> d], this_curve -> window);
  }
  update_entry_double (entry, this_curve -> title_pos[ad -> d]);

  update_curve ((gpointer)& cd);
}

/*!
  \fn void set_frame_style (gpointer data)

  \brief create frame preview

  \param data the associated data pointer
*/
void set_frame_style (gpointer data)
{
  cairo_surface_t * surf;
  Curve * this_curve = get_curve_from_pointer (data);
  surf = draw_frame_surface (this_curve -> frame_type,
                             this_curve -> frame_dash,
                             this_curve -> frame_thickness,
                             this_curve -> frame_pos[0],
                             this_curve -> frame_pos[1],
                             this_curve -> frame_color,
                             this_curve -> backcolor);
  this_curve -> curve_edit -> frame_style_area = destroy_this_widget (this_curve -> curve_edit -> frame_style_area);
  this_curve -> curve_edit -> frame_style_area = create_image_from_data (IMG_SURFACE, (gpointer)surf);
  cairo_surface_destroy (surf);
  widget_set_sensitive (this_curve -> curve_edit -> frame_style_area, this_curve -> show_frame);
  show_the_widgets (this_curve -> curve_edit -> frame_style_area);
#ifdef GTK4
  gtk_widget_set_hexpand (this_curve -> curve_edit -> frame_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, this_curve -> curve_edit -> frame_pix_box, this_curve -> curve_edit -> frame_style_area, TRUE, TRUE, 20);
  update_curve (data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_show_frame (GtkCheckButton * but, gpointer data)

  \brief show / hide frame toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_frame (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_show_frame (GtkToggleButton * but, gpointer data)

  \brief show / hide frame toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_frame (GtkToggleButton * but, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> show_frame = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (this_curve -> curve_edit -> frame_box, this_curve -> show_frame);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_background_color (GtkColorChooser * colob, gpointer data)

  \brief change background color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_background_color (GtkColorChooser * colob, gpointer data)
{
  get_curve_from_pointer (data) -> backcolor = get_button_color (colob);
  set_data_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_frame_type (GtkComboBox * fbox, gpointer data)

  \brief change frame type

  \param fbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_frame_type (GtkComboBox * fbox, gpointer data)
{
  get_curve_from_pointer (data) -> frame_type = combo_get_active ((GtkWidget *)fbox);
  set_frame_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_frame_line (GtkComboBox * fbox, gpointer data)

  \brief change frame line type

  \param fbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_frame_line (GtkComboBox * fbox, gpointer data)
{
  get_curve_from_pointer (data) -> frame_dash = combo_get_active ((GtkWidget *)fbox) + 1;
  set_frame_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_frame_thickness (GtkEntry * entry, gpointer data)

  \brief set frame thickness entry callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_frame_thickness (GtkEntry * entry, gpointer data)
{
  const gchar * str;
  str = entry_get_text (entry);
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> frame_thickness = string_to_double ((gpointer)str);
  update_entry_double (entry, this_curve -> frame_thickness);
  set_frame_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_frame_color (GtkColorChooser * colob, gpointer data)

  \brief set frame color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_frame_color (GtkColorChooser * colob, gpointer data)
{
  get_curve_from_pointer (data) -> frame_color = get_button_color (colob);
  set_frame_style (data);
}

/*!
  \fn G_MODULE_EXPORT void set_frame_pos (GtkEntry * fen, gpointer data)

  \brief set frame position entry callback

  \param fen the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_frame_pos (GtkEntry * fen, gpointer data)
{
  qint * cd = (qint *)data;
  int k;
  const gchar * m;
  double z;
  Curve * this_curve = get_curve_from_pointer (data);
  m = entry_get_text (fen);
  z = string_to_double ((gpointer)m);
  if (cd -> d < 2)
  {
    if (cd -> d == 0)
    {
      k = 0;
      if (z < this_curve -> frame_pos[0][1])
      {
        this_curve -> frame_pos[0][0] = z;
      }
      else
      {
        show_warning ("Frame x min must be < to frame x max", this_curve -> window);
      }
    }
    else if (cd -> d == 1)
    {
      k = 1;
      if (z > this_curve -> frame_pos[0][0])
      {
        this_curve -> frame_pos[0][1] = z;
      }
      else
      {
        show_warning ("Frame x max must be > to frame x min", this_curve -> window);
      }
    }
    update_entry_double (fen, this_curve -> frame_pos[0][k]);
  }
  else
  {
    if (cd -> d == 2)
    {
      k = 0;
      if (z > this_curve -> frame_pos[1][1])
      {
        this_curve -> frame_pos[1][0] = z;
      }
      else
      {
        show_warning ("Frame y min must be > to frame y max", this_curve -> window);
      }
    }
    else
    {
      k = 1;
      if (z < this_curve -> frame_pos[1][0])
      {
        this_curve -> frame_pos[1][1] = z;
      }
      else
      {
        show_warning ("Frame y max must be < to frame y min", this_curve -> window);
      }
    }
    update_entry_double (fen, this_curve -> frame_pos[1][k]);
  }
  set_frame_style (data);
}

/*!
  \fn GtkWidget * create_tab_1 (curve_edition * cedit, gpointer data)

  \brief handle the creation of the 1st tab of the curve edition dialog

  \param cedit the target curve_edition pointer
  \param data the associated data pointer
*/
GtkWidget * create_tab_1 (curve_edition * cedit, gpointer data)
{
  GtkWidget * graphbox;
  GtkWidget * fbox;
  GtkWidget * ghbox;
  GtkWidget * gvbox;
  GtkWidget * frame_style_box;
  GtkWidget * frame_thickness;
  GtkWidget * xyf;

  gchar * str[2];
  str[0] = "min";
  str[1] = "max";
  gchar * axl[2];
  axl[0] = "x";
  axl[1] = "y";
  gchar * ftb[5];
  ftb[0] = "closed";
  ftb[1] = "top open";
  ftb[2] = "right open";
  ftb[3] = "left open";
  ftb[4] = "open";
  int xlgt, ylgt;
  int i, j, k;

  Curve * this_curve = get_curve_from_pointer (data);
  // Axis related signals
  for ( i=0 ; i < 2 ; i++ )
  {
    cedit -> dataxe[i].a = ((tint *)data) -> a;
    cedit -> dataxe[i].b = ((tint *)data) -> b;
    cedit -> dataxe[i].c = ((tint *)data) -> c;
    cedit -> dataxe[i].d = i;
    cedit -> xyp[i] = create_entry (G_CALLBACK(set_window_size), 100, 10, FALSE, (gpointer)& cedit -> dataxe[i]);
  }
  graphbox = create_vbox (BSEP);
  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, ghbox, FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("Size:", 50, 35, 1.0, 0.5), FALSE, FALSE, 20);
  xlgt = get_widget_width (this_curve -> plot);
  ylgt = get_widget_height (this_curve -> plot);
  update_entry_int (GTK_ENTRY(cedit -> xyp[0]), xlgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, cedit -> xyp[0], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("x", -1, -1, 0.5, 0.5), FALSE, FALSE, 10);
  update_entry_int (GTK_ENTRY(cedit -> xyp[1]), ylgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, cedit -> xyp[1], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("pixels", -1, -1, 0.0, 0.5), FALSE, FALSE, 20);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox,
                       check_button ("Insert title", -1, -1, this_curve -> show_title, G_CALLBACK(set_title), data),
                       FALSE, FALSE, 10);

  cedit -> title_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, cedit -> title_box, FALSE, FALSE, 0);
  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, cedit -> title_box, ghbox, FALSE, FALSE, 2);
  gvbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, gvbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, gvbox,
                       check_button ("Default title", 100, 35, this_curve -> default_title, G_CALLBACK(set_title_default), data),
                       FALSE, FALSE, 40);
  cedit -> custom_title = create_entry (G_CALLBACK(set_title_custom), 200, 15, TRUE, data);
  gtk_entry_set_alignment (GTK_ENTRY(cedit -> custom_title), 0.0);
  if (this_curve -> show_title)
  {
    widget_set_sensitive (cedit -> custom_title, ! this_curve -> default_title);
  }
  else
  {
    widget_set_sensitive (cedit -> custom_title, 0);
  }
  update_entry_text (GTK_ENTRY(cedit -> custom_title), this_curve -> title);
  add_box_child_end (gvbox, cedit -> custom_title, FALSE, FALSE, 0);

  add_box_child_end (bbox (cedit -> title_box, "Font:"),
                     font_button (this_curve -> title_font, 150, 35, G_CALLBACK(set_title_font), data),
                     FALSE, FALSE, 0);

  add_box_child_end (bbox (cedit -> title_box, "Color:"),
                     color_button (this_curve -> title_color, TRUE, 150, 30, G_CALLBACK(set_title_color), data),
                     FALSE, FALSE, 0);

  ghbox = bbox (cedit -> title_box, "Position:");
  GtkWidget * txyc;
  for ( i=0 ; i < 2 ; i++ )
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label(lapos[i], (i==0)?10:30, -1, 1.0, 0.5), FALSE, FALSE, 5);
    txyc = create_entry (G_CALLBACK(set_title_pos), 100, 10, FALSE, (gpointer)& cedit -> dataxe[i]);
    update_entry_double (GTK_ENTRY(txyc), this_curve -> title_pos[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, txyc, FALSE, FALSE, 5);
  }
  widget_set_sensitive (cedit -> title_box, this_curve -> show_title);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, ghbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("Background color:", 120, 30, 0.0, 0.5), FALSE, FALSE, 40);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox,
                      color_button (this_curve -> backcolor, TRUE, 100, -1, G_CALLBACK(set_background_color), data),
                      FALSE, FALSE, 40);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

// Frame
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox,
                       check_button ("Show/hide frame", -1, -1, this_curve -> show_frame, G_CALLBACK(set_show_frame), data),
                       FALSE, FALSE, 10);

  cedit -> frame_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, cedit -> frame_box, FALSE, FALSE, 0);
  cedit -> frame_pix_box = create_hbox (0);
  frame_style_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, cedit -> frame_box, cedit -> frame_pix_box, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> frame_pix_box, frame_style_box, FALSE, FALSE, 0);

// Frame style
  fbox = create_combo ();
  for (i=0; i<5; i++)
  {
    combo_text_append (fbox, ftb[i]);
  }
  gtk_widget_set_size_request (fbox, 150, 30);
  combo_set_active (fbox, this_curve -> frame_type);
  g_signal_connect (G_OBJECT(fbox), "changed", G_CALLBACK(set_frame_type), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Type:"), fbox, FALSE, FALSE, 0);

// Frame line style
  fbox = create_combo ();
  for ( i=1 ; i < ndash ; i++)
  {
     combo_text_append (fbox, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (fbox, 150, 30);
  combo_set_active (fbox, this_curve -> frame_dash - 1);
  g_signal_connect (G_OBJECT(fbox), "changed", G_CALLBACK(set_frame_line), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Line style:"), fbox, FALSE, FALSE, 0);

// Frame line width
  frame_thickness = create_entry (G_CALLBACK(set_frame_thickness), -1, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(frame_thickness), this_curve -> frame_thickness);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Line width:"), frame_thickness, FALSE, FALSE, 0);

// Frame color
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Color:"),
                       color_button (this_curve -> frame_color, TRUE, 150, 30, G_CALLBACK(set_frame_color), data),
                       FALSE, FALSE, 0);

// Frame pix
  cairo_surface_t * frame = draw_frame_surface (this_curve -> frame_type,
                                                this_curve -> frame_dash,
                                                this_curve -> frame_thickness,
                                                this_curve -> frame_pos[0],
                                                this_curve -> frame_pos[1],
                                                this_curve -> frame_color,
                                                this_curve -> backcolor);
  cedit -> frame_style_area = create_image_from_data (IMG_SURFACE, (gpointer)frame);
  cairo_surface_destroy (frame);

  gtk_widget_set_size_request (cedit -> frame_pix_box, -1, 150);
#ifdef GTK4
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> frame_pix_box, markup_label(" ", 20, -1, 0.0, 0.0), FALSE, FALSE, 0);
  gtk_widget_set_hexpand (cedit -> frame_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> frame_pix_box, cedit -> frame_style_area, FALSE, FALSE, 20);

  bbox (cedit -> frame_box, "Position: ");
  GtkWidget * fxyc;
  k = -1;
  for (i=0; i<2; i++)
  {
    ghbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, cedit -> frame_box, ghbox, FALSE, FALSE, 0);
    for (j=0; j<2; j++)
    {
      k = k + 1;
      cedit -> framxe[k].a = ((tint *)data) -> a;
      cedit -> framxe[k].b = ((tint *)data) -> b;
      cedit -> framxe[k].c = ((tint *)data) -> c;
      cedit -> framxe[k].d = k;
      xyf = markup_label (g_strdup_printf ("%s %s: ",axl[i], str[j]), 70, -1, 1.0, 0.5);
      widget_set_sensitive (xyf, this_curve -> show_frame);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, xyf, FALSE, FALSE, 20);
      fxyc = create_entry (G_CALLBACK(set_frame_pos), 100, 10, FALSE, (gpointer)& cedit -> framxe[k]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, fxyc, FALSE, FALSE, 0);
      update_entry_double (GTK_ENTRY(fxyc), this_curve -> frame_pos[i][j]);
    }
  }

  widget_set_sensitive (cedit -> frame_box, this_curve -> show_frame);
  return graphbox;
}
