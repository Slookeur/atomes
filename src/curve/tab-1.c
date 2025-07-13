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
#include "cedit.h"

extern void set_data_style (gpointer data);

GtkWidget * title_box = NULL;
GtkWidget * frame[11];
GtkWidget * frame_box = NULL;
GtkWidget * frame_style_area = NULL;
GtkWidget * frame_pix_box = NULL;
char * ctext[2];
qint dataxe[2];
qint framxe[4];
GtkWidget * custom_title = NULL;
int a, b, c, d;

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

  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  d = ad -> d;
  project * this_proj = get_project_by_id(a);
  int shift = get_curve_shift (this_proj, b, c);
  text[0] = "X size must be > 0";
  text[1] = "Y size must be > 0";
  m = entry_get_text (maj);

  i = string_to_double ((gpointer)m);
  if (i > 0)
  {
    switch (d)
    {
      case 0:
        resize_this_window (this_proj -> curves[b][c] -> window, i, this_proj -> curves[b][c] -> wsize[1] + shift);
        break;
      case 1:
        resize_this_window (this_proj -> curves[b][c] -> window, this_proj -> curves[b][c] -> wsize[0], i + shift);
        break;
    }
    this_proj -> curves[b][c] -> wsize[d] = i;
  }
  else
  {
    show_warning (text[d], this_proj -> curves[b][c] -> window);
  }
  update_entry_int (maj, this_proj -> curves[b][c] -> wsize[d]);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> show_title = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (title_box, this_proj -> curves[b][c] -> show_title);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> default_title = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (custom_title, ! this_proj -> curves[b][c] -> default_title);
  if (this_proj -> curves[b][c] -> default_title)
  {
    g_free (this_proj -> curves[b][c] -> title);
    this_proj -> curves[b][c] -> title = g_strdup_printf ("%s - %s", prepare_for_title(this_proj -> name), this_proj -> curves[b][c] -> name);
  }
  update_entry_text (GTK_ENTRY(custom_title), this_proj -> curves[b][c] -> title);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  g_free (this_proj -> curves[b][c] -> title);
  this_proj -> curves[b][c] -> title = g_strdup_printf ("%s", entry_get_text (tit));
  update_entry_text (tit, this_proj -> curves[b][c] -> title);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  g_free (this_proj -> curves[b][c] -> title_font);
  this_proj -> curves[b][c] -> title_font = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> title_color = get_button_color (colob);
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
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  d = ad -> d;
  project * this_proj = get_project_by_id(a);
  p = entry_get_text (entry);
  v = string_to_double ((gpointer)p);
  if (v >= 0.0 && v <= 1.0)
  {
    this_proj -> curves[b][c] -> title_pos[d] = v;
  }
  else
  {
    show_warning (ctext[ad -> d], this_proj -> curves[b][c] -> window);
  }
  update_entry_double (entry, this_proj -> curves[b][c] -> title_pos[d]);
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  surf = draw_frame_surface (this_proj -> curves[b][c] -> frame_type,
                             this_proj -> curves[b][c] -> frame_dash,
                             this_proj -> curves[b][c] -> frame_thickness,
                             this_proj -> curves[b][c] -> frame_pos[0],
                             this_proj -> curves[b][c] -> frame_pos[1],
                             this_proj -> curves[b][c] -> frame_color,
                             this_proj -> curves[b][c] -> backcolor);
  frame_style_area = destroy_this_widget (frame_style_area);
  frame_style_area = create_image_from_data (IMG_SURFACE, (gpointer)surf);
  cairo_surface_destroy (surf);
  widget_set_sensitive (frame_style_area, this_proj -> curves[b][c] -> show_frame);
  show_the_widgets (frame_style_area);
#ifdef GTK4
  gtk_widget_set_hexpand (frame_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, frame_pix_box, frame_style_area, TRUE, TRUE, 20);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> show_frame = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (frame_box, this_proj -> curves[b][c] -> show_frame);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> backcolor = get_button_color (colob);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  get_project_by_id(a) -> curves[b][c] -> frame_type = combo_get_active ((GtkWidget *)fbox);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  get_project_by_id(a) -> curves[b][c] -> frame_dash = combo_get_active ((GtkWidget *)fbox) + 1;
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  str = entry_get_text (entry);
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> frame_thickness = string_to_double ((gpointer)str);
  update_entry_double (entry, this_proj -> curves[b][c] -> frame_thickness);
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
  tint * ad = (tint *)data;
  a = ad -> a;
  b = ad -> b;
  c = ad -> c;
  project * this_proj = get_project_by_id(a);
  this_proj -> curves[b][c] -> frame_color = get_button_color (colob);
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
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  d = cd -> d;
  project * this_proj = get_project_by_id(a);
  m = entry_get_text (fen);
  z= string_to_double ((gpointer)m);
  if (d < 2)
  {
    if (d == 0)
    {
      k = 0;
      if (z < this_proj -> curves[b][c] -> frame_pos[0][1])
      {
        this_proj -> curves[b][c] -> frame_pos[0][0] = z;
      }
      else
      {
        show_warning ("Frame x min must be < to frame x max", this_proj -> curves[b][c] -> window);
      }
    }
    else if (d == 1)
    {
      k = 1;
      if (z > this_proj -> curves[b][c] -> frame_pos[0][0])
      {
        this_proj -> curves[b][c] -> frame_pos[0][1] = z;
      }
      else
      {
        show_warning ("Frame x max must be > to frame x min", this_proj -> curves[b][c] -> window);
      }
    }
    update_entry_double (fen, this_proj -> curves[b][c] -> frame_pos[0][k]);
  }
  else
  {
    if (d == 2)
    {
      k = 0;
      if (z > this_proj -> curves[b][c] -> frame_pos[1][1])
      {
        this_proj -> curves[b][c] -> frame_pos[1][0] = z;
      }
      else
      {
        show_warning ("Frame y min must be > to frame y max", this_proj -> curves[b][c] -> window);
      }
    }
    else
    {
      k = 1;
      if (z < this_proj -> curves[b][c] -> frame_pos[1][0])
      {
        this_proj -> curves[b][c] -> frame_pos[1][1] = z;
      }
      else
      {
        show_warning ("Frame y max must be < to frame y min", this_proj -> curves[b][c] -> window);
      }
    }
    update_entry_double (fen, this_proj -> curves[b][c] -> frame_pos[1][k]);
  }
  set_frame_style (data);
}

/*!
  \fn GtkWidget * create_tab_1 (gpointer data)

  \brief handle the creation of the 1st tab of the curve edition dialog

  \param data the associated data pointer
*/
GtkWidget * create_tab_1 (gpointer data)
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

  tint * cd = (tint *) data;
  a = cd -> a;
  b = cd -> b;
  c = cd -> c;
  project * this_proj = get_project_by_id(a);
  // Axis related signals
  for ( i=0 ; i < 2 ; i++ )
  {
    dataxe[i].a = a;
    dataxe[i].b = b;
    dataxe[i].c = c;
    dataxe[i].d = i;
    xyp[i] = create_entry (G_CALLBACK(set_window_size), 100, 10, FALSE, (gpointer)& dataxe[i]);
  }
  graphbox = create_vbox (BSEP);
  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, ghbox, FALSE, FALSE, 15);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("Size:", 50, 35, 1.0, 0.5), FALSE, FALSE, 20);
  xlgt = get_widget_width (this_proj -> curves[b][c] -> plot);
  ylgt = get_widget_height (this_proj -> curves[b][c] -> plot);
  update_entry_int (GTK_ENTRY(xyp[0]), xlgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, xyp[0], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("x", -1, -1, 0.5, 0.5), FALSE, FALSE, 10);
  update_entry_int (GTK_ENTRY(xyp[1]), ylgt);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, xyp[1], FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("pixels", -1, -1, 0.0, 0.5), FALSE, FALSE, 20);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox,
                       check_button ("Insert title", -1, -1, this_proj -> curves[b][c] -> show_title, G_CALLBACK(set_title), data),
                       FALSE, FALSE, 10);

  title_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, title_box, FALSE, FALSE, 0);

  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, title_box, ghbox, FALSE, FALSE, 2);
  gvbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, gvbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, gvbox,
                       check_button ("Default title", 100, 35, this_proj -> curves[b][c] -> default_title, G_CALLBACK(set_title_default), data),
                       FALSE, FALSE, 40);
  custom_title = create_entry (G_CALLBACK(set_title_custom), 200, 15, TRUE, data);
  gtk_entry_set_alignment (GTK_ENTRY(custom_title), 0.0);
  if (this_proj -> curves[b][c] -> show_title)
  {
    widget_set_sensitive (custom_title, ! this_proj -> curves[b][c] -> default_title);
  }
  else
  {
    widget_set_sensitive (custom_title, 0);
  }
  update_entry_text (GTK_ENTRY(custom_title), this_proj -> curves[b][c] -> title);
  add_box_child_end (gvbox, custom_title, FALSE, FALSE, 0);

  add_box_child_end (bbox (title_box, "Font:"),
                     font_button (this_proj -> curves[b][c] -> title_font, 150, 35, G_CALLBACK(set_title_font), data),
                     FALSE, FALSE, 0);

  add_box_child_end (bbox (title_box, "Color:"),
                     color_button (this_proj -> curves[b][c] -> title_color, TRUE, 150, 30, G_CALLBACK(set_title_color), data),
                     FALSE, FALSE, 0);

  ghbox = bbox (title_box, "Position:");
  GtkWidget * txyc;
  for ( i=0 ; i < 2 ; i++ )
  {
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label(lapos[i], (i==0)?10:30, -1, 1.0, 0.5), FALSE, FALSE, 5);
    txyc = create_entry (G_CALLBACK(set_title_pos), 100, 10, FALSE, (gpointer)& dataxe[i]);
    update_entry_double (GTK_ENTRY(txyc), this_proj -> curves[b][c] -> title_pos[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, txyc, FALSE, FALSE, 5);
  }
  widget_set_sensitive (title_box, this_proj -> curves[b][c] -> show_title);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

  ghbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, ghbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, markup_label("Background color:", 120, 30, 0.0, 0.5), FALSE, FALSE, 40);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox,
                      color_button (this_proj -> curves[b][c] -> backcolor, TRUE, 100, -1, G_CALLBACK(set_background_color), data),
                      FALSE, FALSE, 40);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);

// Frame
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox,
                      check_button ("Show/hide frame", -1, -1, this_proj -> curves[b][c] -> show_frame, G_CALLBACK(set_show_frame), data),
                      FALSE, FALSE, 10);

  frame_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, graphbox, frame_box, FALSE, FALSE, 0);
  frame_pix_box = create_hbox (0);
  frame_style_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, frame_box, frame_pix_box, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, frame_pix_box, frame_style_box, FALSE, FALSE, 0);

// Frame style
  fbox = create_combo ();
  for (i=0; i<5; i++)
  {
    combo_text_append (fbox, ftb[i]);
  }
  gtk_widget_set_size_request (fbox, 150, 30);
  combo_set_active (fbox, this_proj -> curves[b][c] -> frame_type);
  g_signal_connect (G_OBJECT(fbox), "changed", G_CALLBACK(set_frame_type), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Type:"), fbox, FALSE, FALSE, 0);

// Frame line style
  fbox = create_combo ();
  for ( i=1 ; i < ndash ; i++)
  {
     combo_text_append (fbox, g_strdup_printf("%d", i));
  }
  gtk_widget_set_size_request (fbox, 150, 30);
  combo_set_active (fbox, this_proj -> curves[b][c] -> frame_dash - 1);
  g_signal_connect (G_OBJECT(fbox), "changed", G_CALLBACK(set_frame_line), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Line style:"), fbox, FALSE, FALSE, 0);

// Frame line width
  frame_thickness = create_entry (G_CALLBACK(set_frame_thickness), -1, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(frame_thickness), this_proj -> curves[b][c] -> frame_thickness);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Line width:"), frame_thickness, FALSE, FALSE, 0);

// Frame color
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, bbox (frame_style_box, "Color:"),
                       color_button (this_proj -> curves[b][c] -> frame_color, TRUE, 150, 30, G_CALLBACK(set_frame_color), data),
                       FALSE, FALSE, 0);

// Frame pix
  cairo_surface_t * frame = draw_frame_surface (this_proj -> curves[b][c] -> frame_type,
                                                this_proj -> curves[b][c] -> frame_dash,
                                                this_proj -> curves[b][c] -> frame_thickness,
                                                this_proj -> curves[b][c] -> frame_pos[0],
                                                this_proj -> curves[b][c] -> frame_pos[1],
                                                this_proj -> curves[b][c] -> frame_color,
                                                this_proj -> curves[b][c] -> backcolor);
  frame_style_area = create_image_from_data (IMG_SURFACE, (gpointer)frame);
  cairo_surface_destroy (frame);

  gtk_widget_set_size_request (frame_pix_box, -1, 150);
#ifdef GTK4
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, frame_pix_box, markup_label(" ", 20, -1, 0.0, 0.0), FALSE, FALSE, 0);
  gtk_widget_set_hexpand (frame_style_area, TRUE);
#endif
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, frame_pix_box, frame_style_area, FALSE, FALSE, 20);

  bbox (frame_box, "Position: ");
  GtkWidget * fxyc;
  k = -1;
  for (i=0; i<2; i++)
  {
    ghbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, frame_box, ghbox, FALSE, FALSE, 0);
    for (j=0; j<2; j++)
    {
      k = k + 1;
      framxe[k].a = a;
      framxe[k].b = b;
      framxe[k].c = c;
      framxe[k].d = k;
      xyf = markup_label (g_strdup_printf ("%s %s: ",axl[i], str[j]), 70, -1, 1.0, 0.5);
      widget_set_sensitive (xyf, this_proj -> curves[b][c] -> show_frame);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, xyf, FALSE, FALSE, 20);
      fxyc = create_entry (G_CALLBACK(set_frame_pos), 100, 10, FALSE, (gpointer)& framxe[k]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ghbox, fxyc, FALSE, FALSE, 0);
      update_entry_double (GTK_ENTRY(fxyc), this_proj -> curves[b][c] -> frame_pos[i][j]);
    }
  }

  widget_set_sensitive (frame_box, this_proj -> curves[b][c] -> show_frame);
  return graphbox;
}
