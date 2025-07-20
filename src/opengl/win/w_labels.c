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
* @file w_labels.c
* @short Functions to create the 'atomic labels' tab of the atom(s) / clone(s) advanced configuration window \n
         Functions to create the measure labels window of the 'Measures' window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_labels.c'
*
* Contains:
*

 - The functions to create the 'atomic labels' tab of the atom(s) / clone(s) advanced configuration window
 - The functions to create the measure labels window of the 'Measures' window

*
* List of functions:

  G_MODULE_EXPORT gboolean scroll_set_label_shift (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean scroll_set_measure_factor (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean scroll_set_measure_width (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void init_labels_colors (image * img, int sp, int id);
  void label_shift_has_changed (gpointer data, double value);
  void mesure_factor_has_changed (gpointer data, double value);
  void measure_width_has_changed (gpointer data, double value);

  G_MODULE_EXPORT void set_measure_style (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_labels_format (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_labels_render (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void use_atom_default_colors (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void use_atom_default_colors (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_labels_font (GtkFontButton * fontb, gpointer data);
  G_MODULE_EXPORT void set_label_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_labels_position (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_label_shift (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_labels_scale (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_labels_scale (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_labels_tilt (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_measure_factor (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_measure_width (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void enable_lines (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void enable_lines (GtkToggleButton * but, gpointer data);

  GtkWidget * labels_tab (glwin * view, int id);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "preferences.h"

#define LABEL_FORMATS 4

gchar * lab_formats[LABEL_FORMATS] = {"Element name", "Atomic symbol", "Atomic symbol + ID number", "ID number"};

GtkWidget * atom_color_box;
GtkWidget ** color_title;
GtkWidget * tilt;

/*!
  \fn G_MODULE_EXPORT void set_measure_style (GtkComboBox * box, gpointer data)

  \brief change measure style

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_measure_style (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *) data;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> mpattern[id -> b] = combo_get_active ((GtkWidget *)box);
    view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_mpattern[id -> b] = combo_get_active ((GtkWidget *)box);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_labels_format (GtkComboBox * box, gpointer data)

  \brief change label(s) format

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_format (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *) data;
  int i = combo_get_active ((GtkWidget *)box);
  if (! preferences)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    if (i != view -> anim -> last -> img -> acl_format[id -> b])
    {
      view -> anim -> last -> img -> acl_format[id -> b] = i;
      if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
      if (id -> b == 3 || id -> b == 4) view -> create_shaders[MEASU] = TRUE;
      update (view);
    }
  }
  else
  {
    tmp_acl_format[id -> b] = i;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_labels_render (GtkComboBox * box, gpointer data)

  \brief change label(s) rendering mode

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_render (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *) data;
  int i = combo_get_active ((GtkWidget *)box);
  if (! preferences)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    if (i != view -> anim -> last -> img -> labels[id -> b].render)
    {
      view -> anim -> last -> img -> labels[id -> b].render = i;
      if (id -> b < 2)
      {
        view -> create_shaders[LABEL] = TRUE;
      }
      else if (id -> b == 2)
      {
        view -> create_shaders[MAXIS] = TRUE;
      }
      else if (id -> b == 3 || id -> b == 4)
      {
        combo_set_active (tilt, view -> anim -> last -> img -> mtilt[id -> b-3]);
        view -> create_shaders[MEASU] = TRUE;
      }
      update (view);
    }
  }
  else
  {
    tmp_label[id -> b] -> render = i;
  }
}

/*!
  \fn void init_labels_colors (image * img, int sp, int id)

  \brief initialize atomic labels colors

  \param img the target image
  \param sp the total number of chemical species
  \param id atom(s) 0 or clone(s) 1
*/
void init_labels_colors (image * img, int sp, int id)
{
  int i;
  for (i = 0; i < sp; i++)
  {
    img -> labels[id].color[i].red = img -> at_color[i+id*sp].red;
    img -> labels[id].color[i].green = img -> at_color[i+id*sp].green;
    img -> labels[id].color[i].blue = img -> at_color[i+id*sp].blue;
    img -> labels[id].color[i].alpha = 1.0;
    GdkRGBA col = colrgba_togtkrgba (img -> labels[id].color[i]);
    gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(color_title[i]), & col);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void use_atom_default_colors (GtkCheckButton * but, gpointer data)

  \brief use default atom colors - toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_atom_default_colors (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void use_atom_default_colors (GtkToggleButton * but, gpointer data)

  \brief use default atom colors - toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_atom_default_colors (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *) data;
  gboolean val = button_get_status ((GtkWidget *)but);
  int b = id -> b;
  project * this_proj = get_project_by_id(id -> a);
  if (val)
  {
    if (this_proj -> modelgl -> anim -> last -> img -> labels[b].color != NULL)
    {
      init_labels_colors (this_proj -> modelgl -> anim -> last -> img, this_proj -> nspec, b);
      g_free (this_proj -> modelgl -> anim -> last -> img -> labels[b].color);
    }
    this_proj -> modelgl -> anim -> last -> img -> labels[b].color = NULL;
    this_proj -> modelgl -> anim -> last -> img -> labels[b].n_colors = 0;
  }
  else
  {
    this_proj -> modelgl -> anim -> last -> img -> labels[b].color = g_malloc (2*this_proj -> nspec*sizeof*this_proj -> modelgl -> anim -> last -> img -> labels[b].color);
    init_labels_colors (this_proj -> modelgl -> anim -> last -> img, this_proj -> nspec, b);
    this_proj -> modelgl -> anim -> last -> img -> labels[b].n_colors = 2*this_proj -> nspec;
  }
  widget_set_sensitive (atom_color_box, ! val);
  this_proj -> modelgl -> create_shaders[LABEL] = TRUE;
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void set_labels_font (GtkFontButton * fontb, gpointer data)

  \brief change label(s) font

  \param fontb the GtkFontButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_font (GtkFontButton * fontb, gpointer data)
{
  tint * id = (tint *) data;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    g_free (view -> anim -> last -> img -> labels[id -> b].font);
    view -> anim -> last -> img -> labels[id -> b].font = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
    if (id -> b < 2)
    {
      view -> create_shaders[LABEL] = TRUE;
    }
    else if (id -> b == 2)
    {
      view -> create_shaders[MAXIS] = TRUE;
    }
    else if (id -> b == 3 || id -> b == 4)
    {
      view -> create_shaders[MEASU] = TRUE;
    }
    update (view);
  }
  else
  {
    tmp_label[id -> b] -> font = g_strdup_printf ("%s", gtk_font_chooser_get_font (GTK_FONT_CHOOSER(fontb)));
  }
}

/*!
  \fn G_MODULE_EXPORT void set_label_color (GtkColorChooser * colob, gpointer data)

  \brief change label(s) color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_label_color (GtkColorChooser * colob, gpointer data)
{
  tint * id = (tint *) data;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> labels[id -> b].color[id -> c] = get_button_color (colob);
    if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
    if (id -> b == 3 || id -> b == 4) view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_label[id -> b] -> color[0] = get_button_color (colob);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_labels_position (GtkComboBox * box, gpointer data)

  \brief change label(s) position

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_position (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *) data;
  int i = combo_get_active ((GtkWidget *)box);
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> labels[id -> b].position = i;
    if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
    if (id -> b == 3 || id -> b == 4) view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_label[id -> b] -> position = i;
  }
}

/*!
  \fn void label_shift_has_changed (gpointer data, double value)

  \brief change label(s) shift

  \param data the associated data pointer
  \param value the new label(s) shift
*/
void label_shift_has_changed (gpointer data, double value)
{
  tint * id = (tint *) data;
  int i = id -> b / 10;
  int j = id -> b - i * 10;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> labels[i].shift[j] = value;
    if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
    update (view);
  }
  else
  {
    tmp_label[i] -> shift[j] = value;
  }
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_label_shift (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief change label(s) shift - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_label_shift (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  label_shift_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_label_shift (GtkRange * range, gpointer data)

  \brief change label(s) shift - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_label_shift (GtkRange * range, gpointer data)
{
  label_shift_has_changed (data, gtk_range_get_value (range));
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_labels_scale (GtkCheckButton * but, gpointer data)

  \brief change label(s) scale - toggle callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_scale (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_labels_scale (GtkToggleButton * but, gpointer data)

  \brief change label(s) scale - toggle callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_scale (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *) data;
  int i = button_get_status ((GtkWidget *)but);
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> labels[id -> b].scale = i;
    if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
    if (id -> b == 2) view -> create_shaders[MAXIS] = TRUE;
    if (id -> b == 3 || id -> b == 4) view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_label[id -> b] -> scale = i;
  }
}

/*!
  \fn G_MODULE_EXPORT void set_labels_tilt (GtkComboBox * box, gpointer data)

  \brief change label(s) tilt

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_labels_tilt (GtkComboBox * box, gpointer data)
{
  tint * id = (tint *) data;
  int i = combo_get_active ((GtkWidget *)box);
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> mtilt[id -> b] = i;
    if (id -> b < 2) view -> create_shaders[LABEL] = TRUE;
    if (id -> b == 3 || id -> b == 4) view  -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_mtilt[id -> b] = i;
  }
}

/*!
  \fn void mesure_factor_has_changed (gpointer data, double value)

  \brief change measure scale factor

  \param data the associated data pointer
  \param value the new scale factor
*/
void mesure_factor_has_changed (gpointer data, double value)
{
  tint * id = (tint *) data;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> mfactor[id -> b] = (int)value;
    view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_mfactor[id -> b] = (int)value;
  }
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_measure_factor (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief change measure scall factor - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_measure_factor (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  mesure_factor_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_measure_factor (GtkRange * range, gpointer data)

  \brief change measure scall factor - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_measure_factor (GtkRange * range, gpointer data)
{
  mesure_factor_has_changed (data, gtk_range_get_value (range));
}

/*!
  \fn void measure_width_has_changed (gpointer data, double value)

  \brief change measure width

  \param data the associated data pointer
  \param value the new width value
*/
void measure_width_has_changed (gpointer data, double value)
{
  tint * id = (tint *) data;
  if (id -> a != -1)
  {
    glwin * view = get_project_by_id(id -> a) -> modelgl;
    view -> anim -> last -> img -> mwidth[id -> b] = value;
    view  -> create_shaders[MEASU] = TRUE;
    update (view);
  }
  else
  {
    tmp_mwidth[id -> b] = value;
  }
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_measure_width (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief change measure width - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_measure_width (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  measure_width_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_measure_width (GtkRange * range, gpointer data)

  \brief change measure width - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_measure_width (GtkRange * range, gpointer data)
{
  measure_width_has_changed (data, gtk_range_get_value (range));
}

GtkWidget * line_box;
GtkWidget * lstyle;

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void enable_lines (GtkCheckButton * but, gpointer data)

  \brief toggle enable measure lines callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void enable_lines (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void enable_lines (GtkToggleButton * but, gpointer data)

  \brief toggle enable measure lines callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void enable_lines (GtkToggleButton * but, gpointer data)
#endif
{
  tint * id = (tint *) data;
  glwin * view;
  if (id -> a != -1) view = get_project_by_id(id -> a) -> modelgl;
  int i, j;
  i = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (line_box, i);
  if (i)
  {
    if (id -> a != -1)
    {
      j = view -> anim -> last -> img -> mpattern[id -> b] = 0;
    }
    else
    {
      j = tmp_mpattern[id -> b] = 0;
    }
  }
  else
  {
    if (id -> a != -1)
    {
      j = view -> anim -> last -> img -> mpattern[id -> b] = -1;
    }
    else
    {
      j = tmp_mpattern[id -> b] = -1;
    }
  }
  combo_set_active (lstyle, j);
  if (id -> a != -1)
  {
    view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
}

/*!
  \fn GtkWidget * labels_tab (glwin * view, int lid)

  \brief create atomic label(s) tab for the atom(s) / clone(s) window

  \param view the target glwin
  \param lid label type (0 = atoms, 1 = clones, 3 = analysis measures, 4 = edition mode measures)
*/
GtkWidget * labels_tab (glwin * view, int lid)
{
  int i;
  gchar * lpos[3] = {"x", "y", "z"};
  project * this_proj;
  screen_label * label;
  int acl_format;
  gboolean mtilt;
  int mpattern;
  int mfactor;
  double mwidth;
  tint * lab_pointer;
  tint * measure_pointer;
  tint * shift_pointer[2];
  if (! preferences)
  {
    this_proj = get_project_by_id (view -> proj);
    label = & view -> anim -> last -> img -> labels[lid];
    lab_pointer = & view -> colorp[lid][0];
    measure_pointer = (lid > 2) ? & view -> colorp[lid-3][0] : NULL;
    for (i=0; i<2; i++) shift_pointer[i] = & view -> colorp[lid*10+i][0];
    if (lid < 2)
    {
      acl_format = view -> anim -> last -> img -> acl_format[lid];
    }
    else
    {
      mtilt = view -> anim -> last -> img -> mtilt[lid-3];
      mpattern = view -> anim -> last -> img -> mpattern[lid-3];
      mfactor = view -> anim -> last -> img -> mfactor[lid-3];
      mwidth = view -> anim -> last -> img -> mwidth[lid-3];
    }
  }
  else
  {
    label = tmp_label[lid];
    lab_pointer = & pref_pointer[lid];
    measure_pointer = (lid > 2) ? & pref_pointer[lid-3] : NULL;
    for (i=0; i<2; i++) shift_pointer[i] = & pref_pointer[lid*10+i];
    if (lid < 2)
    {
      acl_format = tmp_acl_format[lid];
    }
    else
    {
      mtilt = tmp_mtilt[lid-3];
      mpattern = tmp_mpattern[lid-3];
      mfactor = tmp_mfactor[lid-3];
      mwidth = tmp_mwidth[lid-3];
    }
  }
  GtkWidget * tbox = create_vbox (BSEP);
  GtkWidget * vbox = create_vbox (5);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, tbox, vbox, FALSE, FALSE, 5);

  GtkWidget * box;
  if (lid < 2)
  {
    box = abox (vbox, "Templates ", 0);
    GtkWidget * formats  = create_combo ();
    for (i=0; i<LABEL_FORMATS; i++)
    {
      combo_text_append (formats, lab_formats[i]);
    }
    combo_set_active (formats, acl_format);
    gtk_widget_set_size_request (formats, 220, -1);
    g_signal_connect (G_OBJECT (formats), "changed", G_CALLBACK(set_labels_format), lab_pointer);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, formats, FALSE, FALSE, 10);
  }
  else
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b><u>Label(s):</u></b>", -1, 40, 0.0, 0.5), FALSE, FALSE, 0);
  }
  // Rendering
  box = abox (vbox, "Rendering ", 0);
  GtkWidget * config  = create_combo ();
  combo_text_append (config, "Basic text");
  combo_text_append (config, "Highlighted");
  combo_set_active (config, label -> render);
  gtk_widget_set_size_request (config, 220, -1);
  g_signal_connect (G_OBJECT (config), "changed", G_CALLBACK(set_labels_render), lab_pointer);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, config, FALSE, FALSE, 10);
  // Font
  box = abox (vbox, "Font", 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, font_button (label -> font, 220, -1, G_CALLBACK(set_labels_font), lab_pointer), FALSE, FALSE, 10);
  if (lid == 3 || lid == 4)
  {
    box = abox (vbox, "Font color", 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, color_button (label -> color[0], TRUE, 220, -1, G_CALLBACK(set_label_color), lab_pointer), FALSE, FALSE, 10);
  }

  // Position
  box = abox (vbox, "Position", 0);
  GtkWidget * position = create_combo ();
  combo_text_append (position, "Always visible");
  combo_text_append (position, "Normal");
  combo_set_active (position, label -> position);
  gtk_widget_set_size_request (position, 220, -1);
  g_signal_connect (G_OBJECT (position), "changed", G_CALLBACK(set_labels_position), lab_pointer);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, position, FALSE, FALSE, 10);

  // Size / scale
  box = abox (vbox, "Size", 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                       check_button ("scale with zoom in/out", 220, -1, label -> scale, G_CALLBACK(set_labels_scale), lab_pointer),
                       FALSE, FALSE, 10);

  if (lid == 3 || lid == 4)
  {
    // Tilt
    box = abox (vbox, "Tilt", 0);
    tilt = create_combo ();
    combo_text_append (tilt, "None");
    combo_text_append (tilt, "Adapted");
    combo_set_active (tilt, mtilt);
    gtk_widget_set_size_request (tilt, 220, -1);
    g_signal_connect (G_OBJECT (tilt), "changed", G_CALLBACK(set_labels_tilt), measure_pointer);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, tilt, FALSE, FALSE, 10);
  }

  GtkWidget * chbox;
  if (lid < 3)
  {
    box = abox (vbox, "Distance to atom [&#xC5;]", 0);
    chbox = create_hbox (0);
    for (i=0; i<2; i++)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, markup_label(lpos[i], 30, -1, 0.5, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox,
                           create_hscale(-5.0, 5.0, 0.01, label -> shift[i], (preferences) ? GTK_POS_RIGHT : GTK_POS_TOP, 3, 100, G_CALLBACK(set_label_shift), G_CALLBACK(scroll_set_label_shift), shift_pointer[i]),
                           FALSE, FALSE, 0);
    }
    if (preferences)
    {
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, chbox, FALSE, FALSE, 0);
    }
    else
    {
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, chbox, FALSE, FALSE, 0);
    }
  }

  // Colors
  gboolean ac;
  if (label -> color == NULL)
  {
    ac = TRUE;
  }
  else
  {
    ac = FALSE;
  }
  if (lid < 3 && ! preferences)
  {
    box = abox (vbox, "Color(s)", 0);
    GtkWidget * col_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, col_box, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, col_box,
                         check_button ("Use atom colors", 100, 40, ac, G_CALLBACK(use_atom_default_colors), (gpointer)lab_pointer),
                         FALSE, FALSE, 0);
    atom_color_box = create_vbox (5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, col_box, atom_color_box, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, atom_color_box, markup_label ("Please select atom label colors:", -1, -1, 0.25, 0.5), FALSE, FALSE, 5);
    color_title = g_malloc (this_proj -> nspec*sizeof*color_title);
    for (i=0; i< this_proj -> nspec; i++)
    {
      chbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, markup_label(this_proj -> chemistry -> label[i], 120, -1, 0.5, 0.5), FALSE, FALSE, 20);
      if (label -> color == NULL)
      {
        color_title[i] = color_button(view -> anim -> last -> img -> at_color[i], TRUE, 80, -1, G_CALLBACK(set_label_color), & view -> colorp[lid][i]);
      }
      else
      {
        color_title[i] = color_button(label -> color[i], TRUE, 80, -1, G_CALLBACK(set_label_color), & view -> colorp[lid][i]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, color_title[i], FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, atom_color_box, chbox, FALSE, FALSE, 0);
    }
    widget_set_sensitive (atom_color_box, ! ac);
  }
  else if (lid >= 3)
  {
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, markup_label("<b><u>Line(s):</u></b>", -1, 40, 0.0, 0.5), FALSE, FALSE, 0);
    GtkWidget * hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, check_button ("Show / hide", -1, 40, mpattern+1, G_CALLBACK(enable_lines), measure_pointer), FALSE, FALSE, 30);
    line_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, line_box, TRUE, TRUE, 0);
    box = abox (line_box, "Pattern", 0);
    GtkListStore * store = gtk_list_store_new (1, GDK_TYPE_PIXBUF);
    GtkTreeIter iter;
    for (i=0; i<NDOTS; i++)
    {
      gtk_list_store_append (store, & iter);
      gtk_list_store_set (store, & iter, 0, gdk_pixbuf_new_from_file (dots[i], NULL), -1);
    }
    lstyle = gtk_combo_box_new_with_model (GTK_TREE_MODEL(store));
    g_object_unref (G_OBJECT(store));
    GtkCellRenderer * renderer;
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(lstyle), renderer, FALSE );
    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(lstyle), renderer, "pixbuf", 0, NULL );
    combo_set_active (lstyle, mpattern);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, lstyle, TRUE, TRUE, 10);
    gtk_widget_set_size_request (lstyle, 100, 35);
    g_signal_connect (G_OBJECT (lstyle), "changed", G_CALLBACK(set_measure_style), measure_pointer);

    box = abox (line_box, "Factor", 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                         create_hscale(1.0, 10.0, 1.0, (double)mfactor, GTK_POS_RIGHT, 0, 100, G_CALLBACK(set_measure_factor), G_CALLBACK(scroll_set_measure_factor), measure_pointer),
                         TRUE, TRUE, 0);

    box = abox (line_box, "Width", 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                         create_hscale(1.0, 10.0, 1.0, mwidth, GTK_POS_RIGHT, 0, 100, G_CALLBACK(set_measure_width), G_CALLBACK(scroll_set_measure_width), measure_pointer),
                         TRUE, TRUE, 0);
  }
  return tbox;
}
