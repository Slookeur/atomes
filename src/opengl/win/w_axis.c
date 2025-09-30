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
  void init_axis_colors (ColRGBA axcol[3], axis_edition * axis_win);
  void axis_position_has_changed (gpointer data, double v);

  G_MODULE_EXPORT void update_axis_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void update_axis_length (GtkEntry * res, gpointer data);
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

  G_MODULE_EXPORT gboolean on_axis_delete (GtkWindow * widg, gpointer data);
  G_MODULE_EXPORT gboolean on_axis_delete (GtkWidget * widg, GdkEvent * event, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "preferences.h"

#define AXIS_STYLES 2
#define AXIS_TEMPLATES 5

gchar * axis_symbols[3] = {"X", "Y", "Z"};
gchar * axis_style[AXIS_STYLES] = {"Wireframe", "Cylinders"};
gchar * al[3] = {"% of the window width", "% of the window height", "% of the window depth"};

double axis_init_color[3][3] = {{0.0, 0.0, 1.0},{0.0, 1.0, 0.0},{1.0, 0.0, 0.0}};
double axis_range[3][2] = {{0.0,100.0}, {0.0, 100.0}, {0.0, 100.0}};

extern gboolean from_box_or_axis;
extern G_MODULE_EXPORT void set_labels_render (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_labels_font (GtkFontButton * fontb, gpointer data);
#ifdef GTK4
extern G_MODULE_EXPORT void set_labels_scale (GtkCheckButton * but, gpointer data);
#else
extern G_MODULE_EXPORT void set_labels_scale (GtkToggleButton * but, gpointer data);
#endif

/*!
  \fn G_MODULE_EXPORT void update_axis_parameter (GtkEntry * res, gpointer data)

  \brief update axis parameter callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_axis_parameter (GtkEntry * res, gpointer data)
{
  glwin * view;
  int axis_type;
  double * axis_line;
  double * axis_rad;
#ifdef GTK3
  gchar * str;
#endif // GTK3
  const gchar * n = entry_get_text (res);
  double v = string_to_double ((gpointer)n);
  if (preferences)
  {
    axis_type = tmp_axis -> axis;
    axis_line = & tmp_axis -> line;
    axis_rad = & tmp_axis -> rad;
  }
  else
  {
    view = (glwin *)data;
    axis_type = view -> anim -> last -> img -> xyz -> axis;
    axis_line = & view -> anim -> last -> img -> xyz -> line;
    axis_rad = & view -> anim -> last -> img -> xyz -> rad;
  }
  if (axis_type == CYLINDERS)
  {
    if (v > 0.0) * axis_rad = v;
    v = * axis_rad;
    if (! preferences)
    {
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("_Radius [ %f Å ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_box_axis[1][6]), str);
      g_free (str);
#endif
    }
  }
  else
  {
    if (v > 0.0) * axis_line = v;
    v = * axis_line;
    if (! preferences)
    {
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("_Width [ %f pts ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_box_axis[1][4]), str);
      g_free (str);
#endif
    }
  }
  update_entry_double (res, v);
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
}

/*!
  \fn G_MODULE_EXPORT void update_axis_length (GtkEntry * res, gpointer data)

  \brief update axis length callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_axis_length (GtkEntry * res, gpointer data)
{
  glwin * view;
  double * axis_length;
#ifdef GTK3
  gchar * str;
#endif // GTK3
  const gchar * n = entry_get_text (res);
  double v = string_to_double ((gpointer)n);
  if (preferences)
  {
    axis_length = & tmp_axis -> length;
  }
  else
  {
    view = (glwin *)data;
    axis_length = & view -> anim -> last -> img -> xyz -> length;
  }
  if (v > 0.0) * axis_length = v;
  v = * axis_length;
  update_entry_double (res, v);
  if (! preferences)
  {
#ifdef GTK3
    // GTK3 Menu Action To Check
    str = g_strdup_printf ("_Radius [ %f Å ]", v);
    gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_box_axis[1][7]), str);
    g_free (str);
#endif
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
}

/*!
  \fn void activate_pos_box (glwin * view, gboolean val)

  \brief update axis position data

  \param view the target glwin
  \param val template widget sensitivity
*/
void activate_pos_box (glwin * view, gboolean val)
{
  int i;
  if (val)
  {
    i = (preferences) ? tmp_axis -> t_pos : view -> anim -> last -> img -> xyz -> t_pos;
  }
  else
  {
    i = NONE;
  }
  combo_set_active ((preferences) ? pref_axis_win -> templates : view -> axis_win -> templates, i);
  widget_set_sensitive ((preferences) ? pref_axis_win -> templates : view -> axis_win -> templates, val);
}

/*!
  \fn G_MODULE_EXPORT void set_axis_template (GtkComboBox * box, gpointer data)

  \brief set axis position callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_template (GtkComboBox * box, gpointer data)
{
  int i = combo_get_active ((GtkWidget *)box);
  glwin * view;
  if (preferences)
  {
    tmp_axis -> t_pos = i;
  }
  else
  {
    view = (glwin *)data;
#ifdef GTK4
    view -> anim -> last -> img -> xyz -> t_pos = i;
#endif
  }
  if (! preferences)
  {
#ifdef GTK4
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
    update_menu_bar (view);
#else
  // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][8+i], TRUE);
#endif
  }
}

/*!
  \fn G_MODULE_EXPORT void set_axis_combo_style (GtkComboBox * box, gpointer data)

  \brief set axis style callback

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_combo_style (GtkComboBox * box, gpointer data)
{
  glwin * view;
  axis_edition * the_axis;
  int * axis;
  if (preferences)
  {
    the_axis = pref_axis_win;
    axis = & tmp_axis -> axis;
  }
  else
  {
    view = (glwin *)data;
    the_axis = view -> axis_win;
    axis = & view -> anim -> last -> img -> xyz -> axis;
  }
  switch (combo_get_active ((GtkWidget *)box))
  {
    case 0:
      * axis = WIREFRAME;
      if (is_the_widget_visible(the_axis -> radius_box)) hide_the_widgets (the_axis -> radius_box);
      if (! is_the_widget_visible(the_axis -> width_box)) show_the_widgets (the_axis -> width_box);
      if (! preferences)
      {
#ifdef GTK3
        // GTK3 Menu Action To Check
        from_box_or_axis = TRUE;
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][1], TRUE);
        from_box_or_axis = FALSE;
#endif
      }
      break;
    case 1:
      * axis = CYLINDERS;
      if (is_the_widget_visible(the_axis -> width_box)) hide_the_widgets (the_axis -> width_box);
      if (! is_the_widget_visible(the_axis -> radius_box)) show_the_widgets (the_axis -> radius_box);
      if (! preferences)
      {
#ifdef GTK3
        // GTK3 Menu Action To Check
        from_box_or_axis = TRUE;
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][2], TRUE);
        from_box_or_axis = FALSE;
#endif
      }
      break;
  }
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
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
  glwin * view;
  axis_edition * the_axis;
  int * axis_type;
  if (preferences)
  {
    the_axis = pref_axis_win;
    axis_type = & tmp_axis -> axis;
  }
  else
  {
    view = (glwin *)data;
    the_axis = view -> axis_win;
#ifdef GTK4
    axis_type = & view -> anim -> last -> img -> xyz -> axis;
#endif // GTK4
  }
  val = button_get_status ((GtkWidget *)but);
#ifdef GTK3
  from_box_or_axis = TRUE;
#endif // GTK3
  if (val)
  {
    if (! preferences)
    {
#ifdef GTK4
      * axis_type = WIREFRAME;
#else
    // GTK3 Menu Action To Check
       gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], TRUE);
#endif
    }
    else
    {
      * axis_type = WIREFRAME;
    }
    if (the_axis -> styles && GTK_IS_WIDGET(the_axis -> styles)) combo_set_active (the_axis -> styles, WIREFRAME-1);
  }
  else
  {
    if (! preferences)
    {
#ifdef GTK4
      * axis_type = NONE;
#else
      // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][0], FALSE);
#endif
    }
    else
    {
      * axis_type = NONE;
    }
    if (the_axis -> styles && GTK_IS_WIDGET(the_axis -> styles)) combo_set_active (the_axis -> styles, NONE);
  }
#ifdef GTK3
  from_box_or_axis = FALSE;
#endif // GTK3
  widget_set_sensitive (the_axis -> axis_data, val);
#ifdef GTK4
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
    update_menu_bar (view);
  }
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
  glwin * view = NULL;
  axis_edition * the_axis;
  int * pos;
  if (preferences)
  {
    the_axis = pref_axis_win;
    pos = & tmp_axis -> t_pos;
  }
  else
  {
    view = (glwin *)data;
    the_axis = view -> axis_win;
    pos = & view -> anim -> last -> img -> xyz -> t_pos;
  }
  val = button_get_status ((GtkWidget *)but);
  widget_set_sensitive (the_axis -> axis_position_box, ! val);
  if (val)
  {
#ifdef GTK4
    * pos = 2;
#else
    // GTK3 Menu Action To Check
    if (! preferences) gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][10], TRUE);
#endif
  }
  else
  {
    * pos = CUSTOM;
#ifdef GTK3
    int i;
    if (! preferences)
    {
      for (i=8; i<OGL_AXIS; i++)
      {
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[1][i], FALSE);
      }
    }
#endif
  }
  activate_pos_box (view, val);
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
}

/*!
  \fn void init_axis_colors (ColRGBA axcol[3], axis_edition * axis_win)

  \brief initialize axis colors

  \param axcol the target axis colors
  \param view the target axis_edition data structure
*/
void init_axis_colors (ColRGBA axcol[3], axis_edition * axis_win)
{
  int i;
  for (i = 0; i < 3; i++)
  {
    axcol[i].red = axis_init_color[i][0];
    axcol[i].green = axis_init_color[i][1];
    axcol[i].blue = axis_init_color[i][2];
    axcol[i].alpha = 1.0;
    GdkRGBA col = colrgba_togtkrgba (axcol[i]);
    gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER(axis_win -> axis_color_title[i]), & col);
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
  glwin * view = NULL;
  axis_edition * the_axis;
  ColRGBA * col;
  if (preferences)
  {
    the_axis = pref_axis_win;
    col = tmp_axis -> color;
  }
  else
  {
    view = (glwin *)data;
    the_axis = view -> axis_win;
    col = view -> anim -> last -> img -> xyz -> color;
  }
  val = button_get_status ((GtkWidget *)but);
  if (val)
  {
    if (col != NULL)
    {
      init_axis_colors ((preferences) ? tmp_axis -> color : view -> anim -> last -> img -> xyz -> color, the_axis);
      if (! preferences)
      {
        g_free (view -> anim -> last -> img -> xyz -> color);
        view -> anim -> last -> img -> xyz -> color = NULL;
      }
      else
      {
        g_free (tmp_axis -> color);
        tmp_axis -> color = NULL;
      }
    }
  }
  else
  {
    if (! preferences)
    {
      view -> anim -> last -> img -> xyz -> color = g_malloc (3*sizeof*view -> anim -> last -> img -> xyz -> color);
      init_axis_colors (view -> anim -> last -> img -> xyz -> color, the_axis);
    }
    else
    {
      tmp_axis -> color = g_malloc (3*sizeof*tmp_axis -> color);
      init_axis_colors (tmp_axis -> color, the_axis);
    }

  }
  for (i=0; i<3; i++)  widget_set_sensitive (the_axis -> axis_color_title[i], ! val);
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
  }
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
  if (! preferences)
  {
    glwin * view = get_project_by_id(dat -> a) -> modelgl;
    view -> anim -> last -> img -> xyz -> color[dat -> b] = get_button_color (colob);
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
  }
  else
  {
    tmp_axis -> color[dat -> b] = get_button_color (colob);
  }
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
  if (! preferences)
  {
    glwin * view = get_project_by_id(dat -> a) -> modelgl;
    if (v >= 0.0 && v <= 100.0) view -> anim -> last -> img -> xyz -> c_pos[dat -> b] = v;
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
  else
  {
    if (v >= 0.0 && v <= 100.0) tmp_axis -> c_pos[dat -> b] = v;
  }
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
  gboolean val;
  glwin * view = NULL;
  axis_edition * the_axis;
  int * axis_labels;
  if (preferences)
  {
    the_axis = pref_axis_win;
    axis_labels = & tmp_axis -> labels;
  }
  else
  {
    view = (glwin *)data;
    the_axis = view -> axis_win;
    axis_labels = & view -> anim -> last -> img -> xyz -> labels;
  }
  val = button_get_status ((GtkWidget *)but);
  * axis_labels = val;
  int i;
  for (i=0; i<2; i++) widget_set_sensitive (the_axis -> axis_label_box[i], val);
  if (! preferences)
  {
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
  }
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
  glwin * view = NULL;
  const gchar * m = entry_get_text (entry);
  if (preferences)
  {
    if (tmp_axis -> title[dat -> b]) g_free (tmp_axis -> title[dat -> b]);
    tmp_axis -> title[dat -> b] = g_strdup_printf ("%s", m);
  }
  else
  {
    view = get_project_by_id (dat ->a) -> modelgl;
    if (view -> anim -> last -> img -> xyz -> title[dat -> b]) g_free (view -> anim -> last -> img -> xyz -> title[dat -> b]);
    view -> anim -> last -> img -> xyz -> title[dat -> b] = g_strdup_printf ("%s", m);
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_axis_delete (GtkWindow * widg, gpointer data)

  \brief axis window delete event - GTK4

  \param widg
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_axis_delete (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_axis_delete (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief axis window delete event - GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_axis_delete (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  view -> axis_win -> win = destroy_this_widget (view -> axis_win -> win);
  g_free (view -> axis_win);
  view -> axis_win = NULL;
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data)

  \brief create the axis advanced parameters window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data)
{
  glwin * view;
  axis_edition * the_axis;
  int axis_type;
  double axis_line;
  double axis_rad;
  double axis_length;
  int axis_labels;
  int axis_tpos;
  GLfloat * axis_cpos;
  ColRGBA * axis_color = NULL;
  screen_label * axis_label;
  gboolean build_win = TRUE;
  int i;
  if (preferences)
  {
    the_axis = pref_axis_win;
    axis_type = tmp_axis -> axis;
    axis_line = tmp_axis -> line;
    axis_rad = tmp_axis -> rad;
    axis_tpos = tmp_axis -> t_pos;
    axis_length = tmp_axis -> length;
    axis_labels = tmp_axis -> labels;
    axis_color = tmp_axis -> color;
    axis_cpos = tmp_axis -> c_pos;
    axis_label = tmp_label[2];
  }
  else
  {
    view = (glwin *)data;
    if (view -> axis_win)
    {
      if (view -> axis_win -> win && GTK_IS_WIDGET(view -> axis_win -> win))
      {
        build_win = FALSE;
        show_the_widgets (view -> axis_win -> win);
      }
    }
    if (build_win)
    {
      view -> axis_win = g_malloc0(sizeof*view -> axis_win);
      the_axis = view -> axis_win;
      axis_type = view -> anim -> last -> img -> xyz -> axis;
      axis_line = view -> anim -> last -> img -> xyz -> line;
      axis_rad = view -> anim -> last -> img -> xyz -> rad;
      axis_tpos = view -> anim -> last -> img -> xyz -> t_pos;
      axis_length = view -> anim -> last -> img -> xyz -> length;
      axis_labels = view -> anim -> last -> img -> xyz -> labels;
      axis_color = view -> anim -> last -> img -> xyz -> color;
      axis_cpos = view -> anim -> last -> img -> xyz -> c_pos;
      axis_label = & view -> anim -> last -> img -> labels[2];
    }
  }
  if (build_win)
  {
    GtkWidget * vbox = create_vbox (BSEP);
    if (preferences)
    {
      the_axis -> win = create_vbox (BSEP);
      adv_box (the_axis -> win, "<b>Axis settings</b>", 10, 120, 0.0);
      GtkWidget * hbox = create_hbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 60);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> win, hbox, FALSE, FALSE, 10);
    }
    else
    {
      the_axis -> win = create_vbox (BSEP);
      gchar * str = g_strdup_printf ("%s - axis settings", get_project_by_id(view -> proj)->name);
      the_axis -> win = create_win (str, view -> win, FALSE, FALSE);
      g_free (str);
      add_container_child (CONTAINER_WIN, the_axis -> win, vbox);
    }
    GtkWidget * box;
    gboolean ac;
    if (axis_type != NONE)
    {
      ac = TRUE;
    }
    else
    {
      ac = FALSE;
    }
    the_axis -> show_hide = check_button ("Show / hide axis", 100, 20, ac, G_CALLBACK(set_show_axis_toggle), data);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_axis -> show_hide, FALSE, FALSE, 5);

    the_axis -> axis_data = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_axis -> axis_data, TRUE, TRUE, 0);
    widget_set_sensitive (the_axis -> axis_data, ac);

    GtkWidget * pos_box = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_data, pos_box, FALSE, FALSE, 5);
    if (axis_tpos != CUSTOM)
    {
      ac = TRUE;
    }
    else
    {
      ac = FALSE;
    }
    gchar * axis_template[AXIS_TEMPLATES] = {"Top Right Corner <sup>*</sup>", "Top Left Corner <sup>*</sup>", "Bottom Right Corner <sup>*</sup>", "Bottom Left Corner <sup>*</sup>", "Center <sup>**</sup>"};
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pos_box, check_button ("Use template positions", 120, 30, ac, G_CALLBACK(use_axis_default_positions), data), FALSE, FALSE, 0);
    the_axis -> templates = create_combo ();
    for (i=0; i < AXIS_TEMPLATES; i++)
    {
      combo_text_append (the_axis -> templates, axis_template[i]);
    }
    combo_set_markup (the_axis -> templates);
    activate_pos_box (view, ac);
    gtk_widget_set_size_request (the_axis -> templates, 150, -1);
    g_signal_connect (G_OBJECT (the_axis -> templates), "changed", G_CALLBACK(set_axis_template), data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, pos_box, the_axis -> templates, FALSE, FALSE, 10);

    GtkWidget * chbox;
    GtkWidget * ax_name;
    the_axis -> axis_position_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_data, the_axis -> axis_position_box, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_position_box, markup_label("Please choose axis position:", -1, -1, 0.0, 0.5), FALSE, TRUE, 5);

    // use custom position
    for (i=0; i<2; i++)
    {
      chbox = create_hbox (0);
      ax_name = gtk_label_new (axis_symbols[i]);
      gtk_widget_set_size_request (ax_name, 20, -1);
      gtk_label_align (ax_name, 0.5, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_name, FALSE, TRUE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, create_hscale (axis_range[i][0], axis_range[i][1], 1.0,
                                                       axis_cpos[i],
                                                       GTK_POS_LEFT, 0, 170, G_CALLBACK(set_axis_position),
                                                       G_CALLBACK(scroll_set_axis_position), (preferences) ? & pref_pointer[i] : & view -> colorp[i][0]),
                                                       FALSE, TRUE, 0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_position_box, chbox, FALSE, TRUE, 5);
      ax_name = gtk_label_new (al[i]);
      gtk_label_align (ax_name, -1, 0.5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_name, FALSE, TRUE, 0);
    }
    widget_set_sensitive (the_axis -> axis_position_box, ! ac);

    box = abox (the_axis -> axis_data, "Length [&#xC5;] ", 0);
    the_axis -> length  = create_entry (G_CALLBACK(update_axis_length), 150, 10, FALSE, data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, the_axis -> length, FALSE, FALSE, 0);
    update_entry_double (GTK_ENTRY(the_axis -> length), axis_length);

    box = abox (the_axis -> axis_data, "Style ", 0);
    the_axis -> styles  = create_combo ();
    for (i=0; i<AXIS_STYLES; i++)
    {
      combo_text_append (the_axis -> styles, axis_style[i]);
    }
    if (axis_type == NONE) i = NONE;
    if (axis_type == WIREFRAME) i = 0;
    if (axis_type == CYLINDERS) i = 1;
    combo_set_active (the_axis -> styles, i);
    gtk_widget_set_size_request (the_axis -> styles, 150, -1);
    g_signal_connect (G_OBJECT (the_axis -> styles), "changed", G_CALLBACK(set_axis_combo_style), data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, the_axis -> styles, FALSE, FALSE, 0);

    the_axis -> width_box = abox (the_axis -> axis_data, "Line width [pts] ", 0);
    the_axis -> width  = create_entry (G_CALLBACK(update_axis_parameter), 150, 10, FALSE, data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, the_axis -> width_box, the_axis -> width, FALSE, FALSE, 0);
    update_entry_double (GTK_ENTRY(the_axis -> width), axis_line);
    the_axis -> radius_box = abox (the_axis -> axis_data, "Cylinder radius [&#xC5;] ", 0);
    the_axis -> radius  = create_entry (G_CALLBACK(update_axis_parameter), 150, 10, FALSE, data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, the_axis -> radius_box, the_axis -> radius, FALSE, FALSE, 0);
    update_entry_double (GTK_ENTRY(the_axis -> radius), axis_rad);

    // Labels
    GtkWidget * lab_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_data, lab_box, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, lab_box, check_button ("Label axis", 100, 40, axis_labels, G_CALLBACK(set_axis_labels), data), FALSE, FALSE, 0);

    the_axis -> axis_label_box[0] = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, lab_box, the_axis -> axis_label_box[0], FALSE, FALSE, 0);
    box = abox (the_axis -> axis_label_box[0], "Rendering ", 0);
    GtkWidget * config  = create_combo ();
    combo_text_append (config, "Basic text");
    combo_text_append (config, "Highlighted");
    combo_set_active (config, axis_label -> render);
    gtk_widget_set_size_request (config, 150, -1);
    g_signal_connect (G_OBJECT (config), "changed", G_CALLBACK(set_labels_render), (preferences) ? & pref_pointer[2] : & view -> colorp[2][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, config, FALSE, FALSE, 0);
    box = abox (the_axis -> axis_label_box[0], "Font", 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                        font_button(axis_label -> font, 150, 30, G_CALLBACK(set_labels_font), (preferences) ? & pref_pointer[2] : & view -> colorp[2][0]),
                        FALSE, FALSE, 0);
    // Size / scale
    box = abox (the_axis -> axis_label_box[0], "Size", 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box,
                         check_button ("scale with zoom in/out", 150, -1, axis_label -> scale, G_CALLBACK(set_labels_scale), (preferences) ? & pref_pointer[2] : & view -> colorp[2][0]),
                         FALSE, FALSE, 10);

    GtkWidget * lab_h_box = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, lab_box, lab_h_box, FALSE, FALSE, 0);
    the_axis -> axis_label_box[1] = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, lab_h_box, the_axis -> axis_label_box[1], FALSE, FALSE, 0);
    box = abox (the_axis -> axis_label_box[1], "Legends", 0);
    GtkWidget * ax_title[3];
    for (i=0; i<3; i++)
    {
      chbox = create_hbox (0);
      ax_title[i] = create_entry (G_CALLBACK(set_axis_title), 80, 10, FALSE, (preferences) ? & pref_pointer[i] : & view -> colorp[i][0]);
      update_entry_text (GTK_ENTRY(ax_title[i]), (preferences) ? tmp_axis -> title[i] : view -> anim -> last -> img -> xyz -> title[i]);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, ax_title[i], FALSE, FALSE, 20);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, the_axis -> axis_label_box[1], chbox, FALSE, FALSE, 2);
    }

    // Colors
    GtkWidget * col_v_box = create_vbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, lab_h_box, col_v_box, FALSE, FALSE, 0);
    box = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, col_v_box, box, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, markup_label("<b>.</b>", 5, -1, 0.0, 0.25), FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, markup_label("Colors", 50, 30, 0.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, check_button ("Use base colors", -1, -1, ac, G_CALLBACK(use_axis_default_colors), data), FALSE, FALSE, 10);

    if (axis_color == NULL)
    {
      ac = TRUE;
    }
    else
    {
      ac = FALSE;
    }
    for (i=0; i<3; i++)
    {
      chbox = create_hbox (0);
      if (axis_color != NULL)
      {
        the_axis -> axis_color_title[i] = color_button (axis_color[i], TRUE, 100, -1, G_CALLBACK(set_axis_color), (preferences) ? & pref_pointer[i] : & view -> colorp[i][0]);
      }
      else
      {
        ColRGBA col;
        col.red = axis_init_color[i][0];
        col.green = axis_init_color[i][1];
        col.blue = axis_init_color[i][2];
        col.alpha = 1.0;
        the_axis -> axis_color_title[i] = color_button (col, TRUE, 100, -1, G_CALLBACK(set_axis_color), (preferences) ? & pref_pointer[i] : & view -> colorp[i][0]);
      }
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, chbox, the_axis -> axis_color_title[i], FALSE, FALSE, 40);
      widget_set_sensitive (the_axis -> axis_color_title[i], ! ac);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, col_v_box, chbox, FALSE, FALSE, 0);
    }

    append_comments (the_axis -> axis_data, "<sup>*</sup>", "In front of the atomic model");
    append_comments (the_axis -> axis_data, "<sup>**</sup>", "Inside the atomic model");
    if (! preferences)
    {
      add_global_option (vbox, & view -> colorp[2][0]);
      add_gtk_close_event (the_axis -> win, G_CALLBACK(on_axis_delete), view);
      show_the_widgets (the_axis -> win);
      if (axis_type == CYLINDERS)
      {
        hide_the_widgets (the_axis -> width_box);
      }
      else
      {
        hide_the_widgets (the_axis -> radius_box);
      }
    }
  }
}
