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
* @file w_axis.c
* @short Functions to create the axis parameters edition window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_axis.c'
*
* Contains:
*

 - The functions to create the axis parameters edition window

*
* List of functions:

  G_MODULE_EXPORT gboolean scroll_set_axis_position (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);

  void activate_pos_box (glwin * view, gboolean val);
  void init_axis_colors (glwin * view);
  void axis_position_has_changed (gpointer data, double v);

  G_MODULE_EXPORT void set_axis_template (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_axis_combo_style (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_show_axis_toggle (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_show_axis_toggle (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void use_axis_default_positions (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void use_axis_default_positions (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void use_axis_default_colors (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void use_axis_default_colors (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_axis_color (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void set_axis_position (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_axis_labels (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_axis_labels (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_axis_title (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void axis_advanced (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"

#define AXIS_STYLES 2
#define AXIS_TEMPLATES 5

gchar * axis[3] = {"X", "Y", "Z"};
gchar * axis_style[AXIS_STYLES] = {"Wireframe", "Cylinders"};
gchar * al[3] = {"% of the window width", "% of the window height", "% of the window depth"};
gchar * axis_template[AXIS_TEMPLATES] = {"Top Right Corner *", "Top Left Corner *", "Bottom Right Corner *", "Bottom Left Corner *", "Center **"};

double axis_init_color[3][3] = {{0.0, 0.0, 1.0},{0.0, 1.0, 0.0},{1.0, 0.0, 0.0}};
double axis_range[3][2] = {{0.0,100.0}, {0.0, 100.0}, {0.0, 100.0}};

extern G_MODULE_EXPORT void set_labels_render (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_labels_font (GtkFontButton * fontb, gpointer data);
#ifdef GTK4
extern G_MODULE_EXPORT void set_labels_scale (GtkCheckButton * but, gpointer data);
#else
extern G_MODULE_EXPORT void set_labels_scale (GtkToggleButton * but, gpointer data);
#endif
extern G_MODULE_EXPORT void update_bond_parameter (GtkEntry * res, gpointer data);

GtkWidget * axis_data = NULL;
GtkWidget * axis_position_box;
GtkWidget * templates;
GtkWidget * styles = NULL;
GtkWidget * width_box;
GtkWidget * radius_box;
GtkWidget * axis_color_title[3];
GtkWidget * ax_title[3];

/*!
  \fn void activate_pos_box (glwin * view, gboolean val)

  \brief update axis position data

  \param view the target glwin
  \param val template widget sensitivy
*/
void activate_pos_box (glwin * view, gboolean val)
{
  int i;
  if (val)
  {
    i = view -> anim -> last -> img -> axispos;
  }
  else
  {
    i = NONE;
  }
  gtk_combo_box_set_active (GTK_COMBO_BOX(templates), i);
  widget_set_sensitive (templates, val);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_template (GtkComboBox * box, gpointer data)

  \brief set axis position callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_template (GtkComboBox * box, gpointer data)
{
  glwin * view = (glwin *)data;
  int i = gtk_combo_box_get_active (box);
#ifdef GTK4
  view -> anim -> last -> img -> axispos = i;
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
#else
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][8+i], TRUE);
#endif
}

/*!
  \fn G_MODULE_EXPORT void set_axis_combo_style (GtkComboBox * box, gpointer data)

  \brief set axis style callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_combo_style (GtkComboBox * box, gpointer data)
{
  glwin * view = (glwin *)data;
  switch (gtk_combo_box_get_active (box))
  {
    case 0:
      view -> anim -> last -> img -> box_axis[1] = WIREFRAME;
      if (is_the_widget_visible(radius_box)) hide_the_widgets (radius_box);
      if (! is_the_widget_visible(width_box)) show_the_widgets (width_box);
#ifdef GTK3
      // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][1], TRUE);
#endif
      break;
    case 1:
      view -> anim -> last -> img -> box_axis[1] = CYLINDERS;
      if (is_the_widget_visible(width_box)) hide_the_widgets (width_box);
      if (! is_the_widget_visible(radius_box)) show_the_widgets (radius_box);
#ifdef GTK3
      // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][2], TRUE);
#endif
      break;
  }
#ifdef GTK4
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_show_axis_toggle (GtkCheckButton * but, gpointer data)

  \brief show / hide axis callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_axis_toggle (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_show_axis_toggle (GtkToggleButton * but, gpointer data)

  \brief show / hide axis callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_axis_toggle (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean val;
  glwin * view = (glwin *)data;
#ifdef GTK4
  val = gtk_check_button_get_active (but);
#else
  val = gtk_toggle_button_get_active (but);
#endif
  if (val)
  {
#ifdef GTK4
    // set_box_axis_style ? then what about the menu items ... refresh the menu !
    view -> anim -> last -> img -> box_axis[1] = WIREFRAME;
#else
    // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], TRUE);
#endif
    if (GTK_IS_WIDGET(styles) && styles != NULL) gtk_combo_box_set_active (GTK_COMBO_BOX(styles), WIREFRAME-1);
  }
  else
  {
#ifdef GTK4
    // set_box_axis_style ? then what about the menu items ... refresh the menu !
    view -> anim -> last -> img -> box_axis[1] = NONE;
#else
    // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], FALSE);
#endif
    if (GTK_IS_WIDGET(styles) && styles != NULL) gtk_combo_box_set_active (GTK_COMBO_BOX(styles), NONE);
  }
  widget_set_sensitive (axis_data, val);
#ifdef GTK4
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void use_axis_default_positions (GtkCheckButton * but, gpointer data)

  \brief use axis default colors callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_axis_default_positions (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void use_axis_default_positions (GtkToggleButton * but, gpointer data)

  \brief use axis default colors callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_axis_default_positions (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean val;
  glwin * view = (glwin *)data;
#ifdef GTK4
  val = gtk_check_button_get_active (but);
#else
  val = gtk_toggle_button_get_active (but);
#endif
  widget_set_sensitive (axis_position_box, ! val);
  if (val)
  {
#ifdef GTK4
    view -> anim -> last -> img -> axispos = 2;
#else
    // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][10], TRUE);
#endif
  }
  else
  {
    view -> anim -> last -> img -> axispos = CUSTOM;
    int i;
    for (i=8; i<OGL_AXIS; i++)
    {
#ifdef GTK3
      // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][i], FALSE);
#endif
    }
  }
  activate_pos_box (view, val);
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
  // Update menu_bar
}

/*!
  \fn void init_axis_colors (glwin * view)

  \brief initialize axis colors

  \param view the target glwin
*/
void init_axis_colors (glwin * view)
{
  int i;
  for (i = 0; i < 3; i++)
  {
    view -> anim -> last -> img -> axis_color[i].red = axis_init_color[i][0];
    view -> anim -> last -> img -> axis_color[i].green = axis_init_color[i][1];
    view -> anim -> last -> img -> axis_color[i].blue = axis_init_color[i][2];
    view -> anim -> last -> img -> axis_color[i].alpha = 1.0;
    GdkRGBA col = colrgba_togtkrgba (view -> anim -> last -> img -> axis_color[i]);
    gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(axis_color_title[i]), & col);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void use_axis_default_colors (GtkCheckButton * but, gpointer data)

  \brief  use axis default color callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_axis_default_colors (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void use_axis_default_colors (GtkToggleButton * but, gpointer data)

  \brief use axis default color callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void use_axis_default_colors (GtkToggleButton * but, gpointer data)
#endif
{
  int i;
  gboolean val;
#ifdef GTK4
  val = gtk_check_button_get_active (but);
#else
  val = gtk_toggle_button_get_active (but);
#endif
  glwin * view = (glwin *)data;
  if (val)
  {
    if (view -> anim -> last -> img -> axis_color != NULL)
    {
      init_axis_colors (view);
      g_free (view -> anim -> last -> img -> axis_color);
    }
    view -> anim -> last -> img -> axis_color = NULL;
  }
  else
  {
    view -> anim -> last -> img -> axis_color = g_malloc (3*sizeof*view -> anim -> last -> img -> axis_color);
    init_axis_colors (view);
  }
  for (i=0; i<3; i++)  widget_set_sensitive (axis_color_title[i], ! val);
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_color (GtkColorChooser * colob, gpointer data)

  \brief change axis color

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_color (GtkColorChooser * colob, gpointer data)
{
  tint * dat = (tint *)data;
  glwin * view = get_project_by_id(dat -> a) -> modelgl;
  view -> anim -> last -> img -> axis_color[dat -> b] = get_button_color (colob);
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
}

/*!
  \fn void axis_position_has_changed (gpointer data, double v)

  \brief change axis position

  \param data the associated data pointer
  \param v the new value
*/
void axis_position_has_changed (gpointer data, double v)
{
  tint * dat = (tint *)data;
  glwin * view = get_project_by_id(dat -> a) -> modelgl;
  if (v >= 0.0 && v <= 100.0) view -> anim -> last -> img -> axis_pos[dat -> b] = v;
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_axis_position (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief change axis position - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_axis_position (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  axis_position_has_changed (data, value);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_axis_position (GtkRange * range, gpointer data)

  \brief change axis position - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_position (GtkRange * range, gpointer data)
{
  axis_position_has_changed (data, gtk_range_get_value (range));
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_axis_labels (GtkCheckButton * but, gpointer data)

  \brief  set axis labels callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_labels (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_axis_labels (GtkToggleButton * but, gpointer data)

  \brief set axis labels callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_labels (GtkToggleButton * but, gpointer data)
#endif
{
  int i;
  gboolean val;
  glwin * view = (glwin *)data;
#ifdef GTK4
  val = gtk_check_button_get_active (but);
#else
  val = gtk_toggle_button_get_active (but);
#endif
  view -> anim -> last -> img -> axis_labels = val;
  for (i=0; i<3; i++)  widget_set_sensitive (ax_title[i], val);
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_title (GtkEntry * entry, gpointer data)

  \brief set axis title callback

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_title (GtkEntry * entry, gpointer data)
{
  tint * dat = (tint *)data;
  glwin * view = get_project_by_id(dat ->a) -> modelgl;
  const gchar * m = entry_get_text (entry);
  g_free (view -> anim -> last -> img -> axis_title[dat -> b]);
  view -> anim -> last -> img -> axis_title[dat -> b] = g_strdup_printf ("%s", m);
  view -> create_shaders[MAXIS] = TRUE;
  update (view);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void axis_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief create the axis advanced parameters window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void axis_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data)

  \brief create the axis advanced parameters window callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  opengl_project_changed (view -> proj);
  GtkWidget * win = dialogmodal ("Advanced axis settings", GTK_WINDOW(view -> win));
  GtkWidget * vbox = dialog_get_content_area (win);
  GtkWidget * box;
  int i;
  gboolean ac;
  if (view -> anim -> last -> img -> box_axis[AXIS] != NONE)
  {
    ac = TRUE;
  }
  else
  {
    ac = FALSE;
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, check_button ("Show / hide axis", 100, 40, ac, G_CALLBACK(set_show_axis_toggle), data), FALSE, FALSE, 0);

  axis_data = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, axis_data, TRUE, TRUE, 0);
  widget_set_sensitive (axis_data, ac);

  GtkWidget * pos_box = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, pos_box, FALSE, FALSE, 0);
  //add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, pos_box, TRUE, TRUE, 0);
  if (view -> anim -> last -> img -> axispos != CUSTOM)
  {
    ac = TRUE;
  }
  else
  {
    ac = FALSE;
  }
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pos_box,
                       check_button ("Use template positions:", 120, 30, ac,
                                     G_CALLBACK(use_axis_default_positions), data), FALSE, FALSE, 0);
  templates = create_combo ();
  for (i=0; i < AXIS_TEMPLATES; i++)
  {
    combo_text_append (templates, axis_template[i]);
  }
  activate_pos_box (view, ac);
  gtk_widget_set_size_request (templates, 150, -1);
  g_signal_connect (G_OBJECT (templates), "changed", G_CALLBACK(set_axis_template), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pos_box, templates, FALSE, FALSE, 10);

  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, markup_label("\t\t* In front of the atomic model", -1, -1, 0.0, 0.5), FALSE, TRUE, 3);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, markup_label("\t\t** Inside the atomic model", -1, -1, 0.0, 0.5), FALSE, TRUE, 3);

  GtkWidget * chbox;
  GtkWidget * ax_name;
  axis_position_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, axis_position_box, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_position_box,
                       markup_label("Please choose axis position:", -1, -1, 0.0, 0.5), FALSE, TRUE, 5);

  // use custom position
  for (i=0; i<2; i++)
  {
    chbox = create_hbox (0);
    ax_name = gtk_label_new (axis[i]);
    gtk_widget_set_size_request (ax_name, 20, -1);
    gtk_label_align (ax_name, 0.5, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_name, FALSE, TRUE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, create_hscale (axis_range[i][0], axis_range[i][1], 1.0,
                                                     view -> anim -> last -> img -> axis_pos[i],
                                                     GTK_POS_LEFT, 0, 170, G_CALLBACK(set_axis_position),
                                                     G_CALLBACK(scroll_set_axis_position), & view -> colorp[i][0]),
                                                     FALSE, TRUE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_position_box, chbox, FALSE, TRUE, 5);
    ax_name = gtk_label_new (al[i]);
    gtk_label_align (ax_name, -1, 0.5);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_name, FALSE, TRUE, 0);
  }
  widget_set_sensitive (axis_position_box, ! ac);

  box = abox (axis_data, "Length [&#xC5;]: ", 0);
  GtkWidget * length  = create_entry (G_CALLBACK(update_bond_parameter), 150, 10, FALSE, (gpointer)GINT_TO_POINTER(-5));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, length, FALSE, FALSE, 0);
  update_entry_double (GTK_ENTRY(length), view -> anim -> last -> img -> axis_length);

  box = abox (axis_data, "Style: ", 0);
  styles  = create_combo ();
  for (i=0; i<AXIS_STYLES; i++)
  {
    combo_text_append (styles, axis_style[i]);
  }
  if (view -> anim -> last -> img -> box_axis[AXIS] == NONE) i = NONE;
  if (view -> anim -> last -> img -> box_axis[AXIS] == WIREFRAME) i = 0;
  if (view -> anim -> last -> img -> box_axis[AXIS] == CYLINDERS) i = 1;
  gtk_combo_box_set_active (GTK_COMBO_BOX(styles), i);
  gtk_widget_set_size_request (styles, 150, -1);
  g_signal_connect (G_OBJECT (styles), "changed", G_CALLBACK(set_axis_combo_style), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, styles, FALSE, FALSE, 0);

  width_box = abox (axis_data, "Line width [pts]: ", 0);
  GtkWidget * width  = create_entry (G_CALLBACK(update_bond_parameter), 150, 10, FALSE, (gpointer)GINT_TO_POINTER(-4));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, width_box, width, FALSE, FALSE, 0);
  update_entry_double (GTK_ENTRY(width), view -> anim -> last -> img -> box_axis_line[AXIS]);
  radius_box = abox (axis_data, "Cylinder radius [&#xC5;]: ", 0);
  GtkWidget * radius  = create_entry (G_CALLBACK(update_bond_parameter), 150, 10, FALSE, (gpointer)GINT_TO_POINTER(-4));
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, radius_box, radius, FALSE, FALSE, 0);
  update_entry_double (GTK_ENTRY(radius), view -> anim -> last -> img -> box_axis_rad[AXIS]);

  // Labels
  box = abox (axis_data, "Labels:", 0);
  GtkWidget * lab_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, lab_box, FALSE, FALSE, 0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, lab_box,
                       check_button ("Label axis", 100, 40,
                                   view -> anim -> last -> img -> axis_labels,
                                   G_CALLBACK(set_axis_labels), data), FALSE, FALSE, 0);

  GtkWidget * axis_label_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, lab_box, axis_label_box, FALSE, FALSE, 0);
  box = abox (axis_label_box, "Rendering: ", 0);
  GtkWidget * config  = create_combo ();
  combo_text_append (config, "Basic text");
  combo_text_append (config, "Highlighted");
  gtk_combo_box_set_active (GTK_COMBO_BOX(config), view -> anim -> last -> img -> labels_render[2]);
  gtk_widget_set_size_request (config, 150, -1);
  g_signal_connect (G_OBJECT (config), "changed", G_CALLBACK(set_labels_render), & view -> colorp[2][0]);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, config, FALSE, FALSE, 0);
  box = abox (axis_label_box, "Font:", 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                      font_button(view -> anim -> last -> img -> labels_font[2], 150, 30, G_CALLBACK(set_labels_font), & view -> colorp[2][0]),
                      FALSE, FALSE, 0);
  // Size / scale
  box = abox (axis_label_box, "Size:", 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                       check_button ("scale with zoom in/out", 150, -1, view -> anim -> last -> img -> labels_scale[2], G_CALLBACK(set_labels_scale), & view -> colorp[2][0]),
                       FALSE, FALSE, 10);

  // Colors
  box = abox (axis_data, "Color:", 0);

  GtkWidget * col_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_data, col_box, FALSE, FALSE, 0);
  if ( view -> anim -> last -> img -> axis_color == NULL)
  {
    ac = TRUE;
  }
  else
  {
    ac = FALSE;
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, col_box, check_button ("Use base colors", 100, 40, ac,
                                              G_CALLBACK(use_axis_default_colors), data), FALSE, FALSE, 0);
  GtkWidget * axis_param_box = create_vbox (BSEP);
  for (i=0; i<3; i++)
  {
    chbox = create_hbox (0);
    ax_title[i] = create_entry (G_CALLBACK(set_axis_title), 80, 10, FALSE, & view -> colorp[i][0]);
    update_entry_text (GTK_ENTRY(ax_title[i]), view -> anim -> last -> img -> axis_title[i]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_title[i], FALSE, FALSE, 20);
    widget_set_sensitive (ax_title[i], view -> anim -> last -> img -> axis_labels);
    if ( view -> anim -> last -> img -> axis_color != NULL)
    {
      axis_color_title[i] = color_button (view -> anim -> last -> img -> axis_color[i], TRUE, 80, -1, G_CALLBACK(set_axis_color), & view -> colorp[i][0]);
    }
    else
    {
      ColRGBA col;
      col.red = axis_init_color[i][0];
      col.green = axis_init_color[i][1];
      col.blue = axis_init_color[i][2];
      col.alpha = 1.0;
      axis_color_title[i] = color_button (col, TRUE, 80, -1, G_CALLBACK(set_axis_color),  & view -> colorp[i][0]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, axis_color_title[i], FALSE, FALSE, 0);
    widget_set_sensitive (axis_color_title[i], ! ac);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, axis_param_box, chbox, FALSE, FALSE, 0);

  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, col_box, axis_param_box, FALSE, FALSE, 0);

  g_signal_connect (G_OBJECT(win), "response", G_CALLBACK(run_destroy_dialog), NULL);
  show_the_widgets (win);
  if (view -> anim -> last -> img -> box_axis[AXIS] == CYLINDERS)
  {
    hide_the_widgets (width_box);
  }
  else
  {
    hide_the_widgets (radius_box);
  }
  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
#ifdef GTK4
  update_menu_bar (view);
#endif
  styles = NULL;
  axis_data = NULL;
}
