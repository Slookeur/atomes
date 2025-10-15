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
* @file tab-4.c
* @short 4th tab of the curve layout edition dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'tab-4.c'
*
* Contains:
*

 - The 4th tab of the curve layout edition dialog

*
* List of functions:

  int get_active_axis (this_curve -> curve_edit);

  G_MODULE_EXPORT gboolean scroll_set_ticks_angle (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void ticks_angle_has_changed (gpointer data, double value);

  G_MODULE_EXPORT void set_axis_min (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_axis_max (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_max_div (GtkEntry * maj, gpointer data);
  G_MODULE_EXPORT void set_min_div_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_ticks_size_major_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_ticks_size_minor_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_lab_digit_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_lab_shift_x_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_lab_shift_y_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_axis_title_x_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_axis_title_y_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void set_io_ticks (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_pos_ticks (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_pos_labels (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_ticks_labels_font (GtkFontButton * fontb, gpointer data);
  G_MODULE_EXPORT void set_ticks_angle (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void to_axis_title (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void to_axis_title (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_grid (GtkCheckButton * grid, gpointer data);
  G_MODULE_EXPORT void set_grid (GtkToggleButton * grid, gpointer data);
  G_MODULE_EXPORT void set_autoscale (GtkButton * autosc, gpointer data);
  G_MODULE_EXPORT void set_axis (GtkCheckButton * axis, gpointer data);
  G_MODULE_EXPORT void set_axis (GtkToggleButton * axis, gpointer data);
  G_MODULE_EXPORT void set_axis_legend (GtkEntry * xtit, gpointer data);
  G_MODULE_EXPORT void set_axis_title_font (GtkFontButton * fontb, gpointer data);
  G_MODULE_EXPORT void set_scale (GtkComboBox * sbox, gpointer data);
  G_MODULE_EXPORT void update_axis (GtkComboBox * widg, gpointer data);

  GtkWidget * create_tab_4 (gpointer data);

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
  \fn int get_active_axis (curve_edition * cedit)

  \brief get active axis

  \param cedit the target curve_edition pointer
*/
int get_active_axis (curve_edition * cedit)
{
  return combo_get_active (cedit -> axischoice);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_min (GtkEntry * res, gpointer data)

  \brief set axis min

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_min (GtkEntry * res, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  if (i > -1)
  {
    const gchar * m;
    m = entry_get_text (res);
    if (string_to_double ((gpointer)m) < this_curve -> axmax[i])
    {
      this_curve -> axmin[i] = string_to_double ((gpointer)m);
    }
    else
    {
      show_warning ("Axis min must be < to axis max", this_curve -> window);
    }
    update_entry_double (res, this_curve -> axmin[i]);
    update_curve (data);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_axis_max (GtkEntry * res, gpointer data)

  \brief set axis max

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_max (GtkEntry * res, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  const gchar * m;
  m = entry_get_text (res);
  if (string_to_double ((gpointer)m) > this_curve -> axmin[i])
  {
    this_curve -> axmax[i] = string_to_double ((gpointer)m);
  }
  else
  {
    show_warning ("Axis max must be > to axis min", this_curve -> window);
  }
  update_entry_double (res, this_curve -> axmax[i]);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_max_div (GtkEntry * maj, gpointer data)

  \brief set number of major tick divisions

  \param maj the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_max_div (GtkEntry * maj, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  double tmp;
  const gchar * m;
  m = entry_get_text (maj);
  tmp = string_to_double ((gpointer)m);
  if (tmp != 0.0)
  {
    this_curve -> majt[i] = tmp;
    update_curve (data);
  }
  else
  {
    show_warning ("Major tick must be > 0.0", this_curve -> window);
  }
  update_entry_double (maj, this_curve -> majt[i]);
}

/*!
  \fn G_MODULE_EXPORT void set_min_div_spin (GtkSpinButton * res, gpointer data)

  \brief set number of minor tick division(s)

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_min_div_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  get_curve_from_pointer ((gpointer)& cd) -> mint[ad -> d] = gtk_spin_button_get_value_as_int(res) + 1;
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_ticks_size_major_spin (GtkSpinButton * res, gpointer data)

  \brief set major ticks size

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ticks_size_major_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  get_curve_from_pointer ((gpointer)& cd) -> majt_size[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_ticks_size_minor_spin (GtkSpinButton * res, gpointer data)

  \brief set minor ticks size

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ticks_size_minor_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  this_curve -> mint_size[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_entry_int (GTK_ENTRY(res), this_curve -> mint_size[ad -> d]);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_lab_digit_spin (GtkSpinButton * res, gpointer data)

  \brief set ticks label number of digit(s)

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_lab_digit_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  this_curve -> labels_digit[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_entry_int (GTK_ENTRY(res), this_curve -> labels_digit[ad -> d]);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_lab_shift_x_spin (GtkSpinButton * res, gpointer data)

  \brief set ticks label position shift on x axis

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_lab_shift_x_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  this_curve -> labels_shift_x[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_entry_int (GTK_ENTRY(res), this_curve -> labels_shift_x[ad -> d]);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_lab_shift_y_spin (GtkSpinButton * res, gpointer data)

  \brief set ticks label position shift on y axis

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_lab_shift_y_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  Curve * this_curve = get_curve_from_pointer ((gpointer)& cd);
  this_curve -> labels_shift_y[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_entry_int (GTK_ENTRY(res), this_curve -> labels_shift_y[ad -> d]);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_title_x_spin (GtkSpinButton * res, gpointer data)

  \brief set axis title position shift on x axis

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_title_x_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  get_curve_from_pointer ((gpointer)& cd) -> axis_title_x[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_title_y_spin (GtkSpinButton * res, gpointer data)

  \brief set axis title position shift on y axis

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_title_y_spin (GtkSpinButton * res, gpointer data)
{
  qint * ad = (qint *) data;
  tint cd;
  cd.a = ad -> a;
  cd.b = ad -> b;
  cd.c = ad -> c;
  get_curve_from_pointer ((gpointer)& cd) -> axis_title_y[ad -> d] = gtk_spin_button_get_value_as_int(res);
  update_curve ((gpointer)& cd);
}

/*!
  \fn G_MODULE_EXPORT void set_io_ticks (GtkComboBox * box, gpointer data)

  \brief change tick marks location

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_io_ticks (GtkComboBox * box, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer(data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> ticks_io[i] = combo_get_active ((GtkWidget *)box);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_pos_ticks (GtkComboBox * box, gpointer data)

  \brief change tick marks position

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_pos_ticks (GtkComboBox * box, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer(data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> ticks_pos[i] = combo_get_active ((GtkWidget *)box);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_pos_labels (GtkComboBox * box, gpointer data)

  \brief change ticks labels position

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_pos_labels (GtkComboBox * box, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer(data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> labels_pos[i] = combo_get_active ((GtkWidget *)box);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_ticks_labels_font (GtkFontButton * fontb, gpointer data)

  \brief set ticks labels font

  \param fontb the GtkFontButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ticks_labels_font (GtkFontButton * fontb, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  g_free (this_curve -> labels_font[i]);
  this_curve -> labels_font[i] = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
  update_curve (data);
}

/*!
  \fn void ticks_angle_has_changed (gpointer data, double value)

  \brief  set ticks angle

  \param data the associated data pointer
  \param value the new ticks angle
*/
void ticks_angle_has_changed (gpointer data, double value)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> labels_angle[i] = value * (pi/180.0);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_ticks_angle (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief  set ticks angle callback - scroll

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_ticks_angle (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  ticks_angle_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_ticks_angle (GtkRange * range, gpointer data)

  \brief set ticks angle callback - range

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_ticks_angle (GtkRange * range, gpointer data)
{
  ticks_angle_has_changed (data, gtk_range_get_value (range));
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_axis_title (GtkCheckButton * but, gpointer data)

  \brief show / hide axis title toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_axis_title (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void to_axis_title (GtkToggleButton * but, gpointer data)

  \brief show / hide axis title toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_axis_title (GtkToggleButton * but, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  int i = get_active_axis (cedit);
  if (button_get_status ((GtkWidget *)but))
  {
    widget_set_sensitive (cedit -> axis_title, 0);
    g_free (this_curve -> axis_title[i]);
    this_curve -> axis_title[i] = g_strdup_printf ("%s", default_title (i, data));
    update_entry_text (GTK_ENTRY(cedit -> axis_title), this_curve -> axis_title[i]);
  }
  else
  {
    widget_set_sensitive (cedit -> axis_title, 1);
  }
  update_curve (data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_grid (GtkCheckButton * grid, gpointer data)

  \brief show / hide grid toggle callback GTK4

  \param grid the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_grid (GtkCheckButton * grid, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_grid (GtkToggleButton * grid, gpointer data)

  \brief show / hide grid toggle callback GTK3

  \param grid the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_grid (GtkToggleButton * grid, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> show_grid[i] = button_get_status ((GtkWidget *)grid);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_autoscale (GtkButton * autosc, gpointer data)

  \brief autoscale axis

  \param autosc the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_autoscale (GtkButton * autosc, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> autoscale[i] = TRUE;
  update_curve (data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_axis (GtkCheckButton * axis, gpointer data)

  \brief show / hide axis toggle callback GTK4

  \param axis the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis (GtkCheckButton * axis, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_axis (GtkToggleButton * axis, gpointer data)

  \brief show / hide axis toggle callback GTK3

  \param axis the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis (GtkToggleButton * axis, gpointer data)
#endif
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  this_curve -> show_axis[i] = button_get_status ((GtkWidget *)axis);
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_legend (GtkEntry * xtit, gpointer data)

  \brief set axis legend entry callback

  \param xtit the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_legend (GtkEntry * xtit, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  g_free (this_curve -> axis_title[i]);
  this_curve -> axis_title[i] = g_strdup_printf ("%s", entry_get_text (xtit));
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_title_font (GtkFontButton * fontb, gpointer data)

  \brief set axis title font

  \param fontb the GtkFontButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_title_font (GtkFontButton * fontb, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  g_free (this_curve -> axis_title_font[i]);
  this_curve -> axis_title_font[i] = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void set_scale (GtkComboBox * sbox, gpointer data)

  \brief change the axis scale

  \param sbox the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_scale (GtkComboBox * sbox, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i = get_active_axis (this_curve -> curve_edit);
  int j = combo_get_active((GtkWidget *)sbox);
  this_curve -> scale[i] = j;
  widget_set_sensitive (this_curve -> curve_edit -> majt, ! j);
  widget_set_sensitive (this_curve -> curve_edit -> nmi[i], ! j);
  this_curve -> autoscale[i] = TRUE;
  update_curve (data);
}

/*!
  \fn G_MODULE_EXPORT void update_axis (GtkComboBox * box, gpointer data)

  \brief change the axis

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_axis (GtkComboBox * box, gpointer data)
{
  int i;
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  i = combo_get_active ((GtkWidget *)box);
  update_entry_double (GTK_ENTRY(cedit -> vmin), this_curve -> axmin[i]);
  update_entry_double (GTK_ENTRY(cedit -> vmax), this_curve -> axmax[i]);
  update_entry_double (GTK_ENTRY(cedit -> majt), this_curve -> majt[i]);
  combo_set_active (cedit -> ticks_inout_box, this_curve -> ticks_io[i]);
  combo_set_active (cedit -> ticks_pos_box, this_curve -> ticks_pos[i]);
  combo_set_active (cedit -> labels_pos_box, this_curve -> labels_pos[i]);
  gtk_font_chooser_set_font (GTK_FONT_CHOOSER(cedit -> ticks_labels_font), this_curve -> labels_font[i]);
  gtk_range_set_value (GTK_RANGE(cedit -> ticks_labels_angle), this_curve -> labels_angle[i] * (180.0/pi));
  if (((tint *)data) -> b < MS)
  {
    widget_set_sensitive (cedit -> scale_box, 0);
  }
  else
  {
    g_signal_handler_disconnect (G_OBJECT(cedit -> scale_box), cedit -> handler_id);
    combo_set_active (cedit -> scale_box, this_curve -> scale[i]);
    cedit -> handler_id = g_signal_connect (G_OBJECT(cedit -> scale_box), "changed", G_CALLBACK(set_scale), data);
    widget_set_sensitive (cedit -> scale_box, 1);
    /* widget_set_sensitive (cedit -> vmax, ! this_curve -> scale[i]);
    widget_set_sensitive (cedit -> vmin, ! this_curve -> scale[i]); */
    widget_set_sensitive (cedit -> majt, ! this_curve -> scale[i]);
    widget_set_sensitive (cedit -> nmi[i], ! this_curve -> scale[i]);
  }
  hide_the_widgets (cedit -> nmi[! i]);
  hide_the_widgets (cedit -> ndi[! i]);
  hide_the_widgets (cedit -> mats[! i]);
  hide_the_widgets (cedit -> mits[! i]);
  hide_the_widgets (cedit -> nptx[! i]);
  hide_the_widgets (cedit -> npty[! i]);
  hide_the_widgets (cedit -> tptx[! i]);
  hide_the_widgets (cedit -> tpty[! i]);

  show_the_widgets (cedit -> nmi[i]);
  show_the_widgets (cedit -> ndi[i]);
  show_the_widgets (cedit -> mats[i]);
  show_the_widgets (cedit -> mits[i]);
  show_the_widgets (cedit -> nptx[i]);
  show_the_widgets (cedit -> npty[i]);
  show_the_widgets (cedit -> tptx[i]);
  show_the_widgets (cedit -> tpty[i]);

  button_set_status (cedit -> show_axis, this_curve -> show_axis[i]);
  button_set_status (cedit -> show_grid, this_curve -> show_grid[i]);
  button_set_status (cedit -> axis_default_title, this_curve -> axis_defaut_title[i]);

  widget_set_sensitive (cedit -> axis_title, ! this_curve -> axis_defaut_title[i]);
  update_entry_text (GTK_ENTRY(cedit -> axis_title), this_curve -> axis_title[i]);
  gtk_font_chooser_set_font (GTK_FONT_CHOOSER(cedit -> axis_title_font), this_curve -> axis_title_font[i]);
}

/*!
  \fn GtkWidget * create_tab_4 (curve_edition * cedit, gpointer data)

  \brief handle the creation of the 4th tab of the curve edition dialog

  \param cedit the target curve_edition pointer
  \param data the associated data pointer
*/
GtkWidget * create_tab_4 (curve_edition * cedit, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  int i;
  GtkWidget * axisbox;
#ifdef GTK4
  axisbox = create_vbox (3);
#else
  axisbox = create_vbox (BSEP);
#endif
  GtkWidget * ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("<b>Select axis:</b>", 175, -1, 1.0, 0.5), FALSE, FALSE, 0);
  cedit -> axischoice = create_combo ();
  combo_text_append (cedit -> axischoice, "X axis");
  combo_text_append (cedit -> axischoice, "Y axis");
  combo_set_active (cedit -> axischoice, 0);
  gtk_widget_set_size_request (cedit -> axischoice, 80, 35);
  g_signal_connect (G_OBJECT(cedit -> axischoice), "changed", G_CALLBACK(update_axis), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> axischoice, FALSE, FALSE, 25);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);

  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox,  markup_label("Scale type:", 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
  cedit -> scale_box = create_combo ();
  combo_text_append (cedit -> scale_box, "Linear");
  combo_text_append (cedit -> scale_box, "Log");
  combo_set_active (cedit -> scale_box, this_curve -> scale[0]);
  gtk_widget_set_size_request (cedit -> scale_box, 80, -1);
  cedit -> handler_id = g_signal_connect (G_OBJECT(cedit -> scale_box), "changed", G_CALLBACK(set_scale), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> scale_box, FALSE, FALSE, 0);
  cedit -> auto_scale = create_button ("Autoscale axis", IMG_NONE, NULL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(set_autoscale), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> auto_scale, FALSE, FALSE, 30);

// Axis min and max
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("Min:", 30, -1, 0.5, 0.5), FALSE, FALSE, 20);
  cedit -> vmin = create_entry (G_CALLBACK(set_axis_min), 100, 15, FALSE, data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> vmin, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("Max:", 30, -1, 0.5, 0.5), FALSE, FALSE, 20);
  cedit -> vmax = create_entry (G_CALLBACK(set_axis_max), 100, 15, FALSE, data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> vmax, FALSE, FALSE, 0);

// Major ticks spacing
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("Major ticks spacing:", 160, -1, 0.0, 0.5), FALSE, FALSE, 10);
  cedit -> majt = create_entry (G_CALLBACK(set_max_div), 100, 15, FALSE, data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> majt, FALSE, FALSE, 0);

// Number of minors ticks
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("Number of minor ticks:", 160, -1, 0.0, 0.5), FALSE, FALSE, 10);
  for (i=0; i<2; i++)
  {
    cedit -> nmi[i] = spin_button (G_CALLBACK(set_min_div_spin), (double)this_curve -> mint[i]-1.0, 0.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> nmi[i], FALSE, FALSE, 0);
  }

// Ticks ...
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("<u>Tick marks:</u>", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);

// ... position ...
  ahbox = abox (axisbox, "Location:", 1);
  cedit -> ticks_inout_box = create_combo ();
  combo_text_append (cedit -> ticks_inout_box, "In");
  combo_text_append (cedit -> ticks_inout_box, "Out");
  gtk_widget_set_size_request (cedit -> ticks_inout_box, 60, -1);
  g_signal_connect (G_OBJECT(cedit -> ticks_inout_box), "changed", G_CALLBACK(set_io_ticks), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> ticks_inout_box, FALSE, FALSE, 3);

  cedit -> ticks_pos_box = create_combo ();
  combo_text_append (cedit -> ticks_pos_box, "Normal");
  combo_text_append (cedit -> ticks_pos_box, "Opposite");
  combo_text_append (cedit -> ticks_pos_box, "Both");
  combo_text_append (cedit -> ticks_pos_box, "None");
  gtk_widget_set_size_request (cedit -> ticks_pos_box, 100, -1);
  g_signal_connect (G_OBJECT(cedit -> ticks_pos_box), "changed", G_CALLBACK(set_pos_ticks), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> ticks_pos_box, FALSE, FALSE, 3);

//  ... sizes ...

  ahbox = abox (axisbox, "Major ticks size:", 1);
  for (i=0; i<2; i++)
  {
    cedit -> mats[i] = spin_button (G_CALLBACK(set_ticks_size_major_spin), (double)this_curve -> majt_size[i], 0.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> mats[i], FALSE, FALSE, 0);
  }
  ahbox = abox (axisbox, "Minor ticks size:", 1);
  for (i=0; i<2; i++)
  {
    cedit -> mits[i] = spin_button (G_CALLBACK(set_ticks_size_minor_spin), (double)this_curve -> mint_size[i], 0.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> mits[i], FALSE, FALSE, 0);
  }

// Ticks labels ...
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE, 5);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("<u>Tick labels:</u>", -1, -1, 0.0, 0.5), FALSE, FALSE, 0);

// ... position ...
  ahbox = abox (axisbox, "Location:", 1);
  cedit -> labels_pos_box = create_combo ();
  combo_text_append (cedit -> labels_pos_box, "Normal");
  combo_text_append (cedit -> labels_pos_box, "Opposite");
  combo_text_append (cedit -> labels_pos_box, "Both");
  combo_text_append (cedit -> labels_pos_box, "None");
  gtk_widget_set_size_request (cedit -> labels_pos_box, 150, -1);
  g_signal_connect (G_OBJECT(cedit -> labels_pos_box), "changed", G_CALLBACK(set_pos_labels), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> labels_pos_box, FALSE, FALSE, 0);

// ... significant digits ...
  ahbox = abox (axisbox, "Significant digits:", 1);
  for (i=0; i<2; i++)
  {
    cedit -> ndi[i] = spin_button (G_CALLBACK(set_lab_digit_spin), (double)this_curve -> labels_digit[i], 0.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> ndi[i], FALSE, FALSE, 0);
  }

// ... font ...
  cedit -> ticks_labels_font = font_button (this_curve -> labels_font[0], 150, 35, G_CALLBACK(set_ticks_labels_font), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, abox (axisbox, "Font:", 1), cedit -> ticks_labels_font, FALSE, FALSE, 5);

// ... angle ..
  cedit -> ticks_labels_angle = create_hscale (-180.0, 180.0, 1.0, 0.0, GTK_POS_LEFT, 0, 150, G_CALLBACK(set_ticks_angle), G_CALLBACK(scroll_set_ticks_angle), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, abox (axisbox, "Angle:", 1), cedit -> ticks_labels_angle, FALSE, FALSE, 5);

// ...distance to axis
  ahbox = abox (axisbox, "Position: ", 1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("x:", -1, -1, 0.9, 0.5), FALSE, FALSE, 10);
  for (i=0; i<2; i++)
  {
    cedit -> nptx[i] = spin_button (G_CALLBACK(set_lab_shift_x_spin), (double)this_curve -> labels_shift_x[i], -100.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> nptx[i], FALSE, FALSE, 0);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("y:", -1, -1, 0.9, 0.5), FALSE, FALSE, 10);
  for (i=0; i<2; i++)
  {
    cedit -> npty[i] = spin_button (G_CALLBACK(set_lab_shift_y_spin), (double) this_curve -> labels_shift_y[i], -100.0, 100.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> npty[i], FALSE, FALSE, 0);
  }

  cedit -> show_axis = check_button ("Show/hide axis (if min[axis] &lt; 0.0 &lt; max[axis])", 100, 30, FALSE, G_CALLBACK(set_axis), data);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, cedit -> show_axis, FALSE, FALSE, 3);
  cedit -> show_grid = check_button ("Show/hide axis grid", 100, 30, FALSE, G_CALLBACK(set_grid), data);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, cedit -> show_grid, FALSE, FALSE, 1);

// Axis title
  ahbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axisbox, ahbox, FALSE, FALSE,3);
  cedit -> axis_default_title = check_button ("Use default axis title", 175, -1, FALSE, G_CALLBACK(to_axis_title), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> axis_default_title, FALSE, FALSE, 0);
  cedit -> axis_title = create_entry (G_CALLBACK(set_axis_legend), 150, 15, FALSE, data);
  gtk_entry_set_alignment (GTK_ENTRY(cedit -> axis_title), 0.0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> axis_title, FALSE, FALSE, 0);

  cedit -> axis_title_font = font_button (this_curve -> axis_title_font[0], 150, 35, G_CALLBACK(set_axis_title_font), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, abox (axisbox, "Font:", 3), cedit -> axis_title_font, FALSE, FALSE, 0);

  ahbox = abox (axisbox, "Position: ", 3);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("x:", -1, -1, 0.9, 0.5), FALSE, FALSE, 10);
  for (i=0; i<2; i++)
  {
    cedit -> tptx[i] = spin_button (G_CALLBACK(set_axis_title_x_spin), (double)this_curve -> axis_title_x[i], -500.0, 500.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> tptx[i], FALSE, FALSE, 0);
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, markup_label("y:", -1, -1, 0.9, 0.5), FALSE, FALSE, 10);
  for (i=0; i<2; i++)
  {
    cedit -> tpty[i] = spin_button (G_CALLBACK(set_axis_title_y_spin), (double)this_curve -> axis_title_y[i], -500.0, 500.0, 1.0, 0, 50, & cedit -> dataxe[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, ahbox, cedit -> tpty[i], FALSE, FALSE, 0);
  }
  return axisbox;
}
