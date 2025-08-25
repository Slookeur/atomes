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
* @file w_box.c
* @short Functions to create the box properties window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_box.c'
*
* Contains:
*

 - The functions to create the box properties window

*
* List of functions:

  G_MODULE_EXPORT void update_box_parameter (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_box_combo_style (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_show_box_toggle (GtkCheckButton * but, gpointer data);
  G_MODULE_EXPORT void set_show_box_toggle (GtkToggleButton * but, gpointer data);
  G_MODULE_EXPORT void set_color_box (GtkColorChooser * colob, gpointer data);
  G_MODULE_EXPORT void box_advanced (GtkWidget * widg, gpointer data);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "preferences.h"

#define BOX_STYLES 2

gchar * box_style[BOX_STYLES] = {"Wireframe", "Cylinders"};

gboolean from_box_or_axis = FALSE;

/*!
  \fn G_MODULE_EXPORT void update_box_parameter (GtkEntry * res, gpointer data)

  \brief update box parameter callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_box_parameter (GtkEntry * res, gpointer data)
{
  glwin * view;
  int box_type;
  double box_line;
  double box_rad;
  const gchar * n = entry_get_text (res);
#ifdef GTK3
  gchar * str;
#endif // GTK3
  double v = string_to_double ((gpointer)n);
  if (preferences)
  {
    box_type = tmp_box -> box;
    box_line = tmp_box -> line;
    box_rad = tmp_box -> rad;
  }
  else
  {
    view = (glwin *)data;
    box_type = view -> anim -> last -> img -> abc -> box;
    box_line = view -> anim -> last -> img -> abc -> line;
    box_rad = view -> anim -> last -> img -> abc -> rad;
  }
  if (box_type == CYLINDERS)
  {
    if (v > 0.0) box_rad = v;
    v = box_rad;
    if (! preferences)
    {
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("_Radius [ %f Å ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_box_axis[0][6]), str);
      g_free (str);
#endif
      view -> anim -> last -> img -> abc -> rad = v;
    }
    else
    {
      tmp_box -> rad = v;
    }
  }
  else
  {
    if (v > 0.0) box_line = v;
    v = box_line;
    if (! preferences)
    {
#ifdef GTK3
      // GTK3 Menu Action To Check
      str = g_strdup_printf ("_Width [ %f pts ]", v);
      gtk_menu_item_set_label (GTK_MENU_ITEM(view -> ogl_box_axis[0][4]), str);
      g_free (str);
#endif
      view -> anim -> last -> img -> abc -> line = v;
    }
    else
    {
      tmp_box -> line = v;
    }
  }
  update_entry_double (res, v);
  if (! preferences)
  {
    view -> create_shaders[MDBOX] = TRUE;
    update (view);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_box_combo_style (GtkWidget * widg, gpointer data)

  \brief set box style callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_box_combo_style (GtkWidget * widg, gpointer data)
{
  int i = combo_get_active (widg);
  glwin * view;
  box_edition * box_win;
  if (! preferences)
  {
    view = (glwin *)data;
    box_win = view -> box_win;
  }
  else
  {
    box_win = pref_box_win;
  }
#ifdef GTK4
  if (! preferences)
  {
    view -> anim -> last -> img -> abc -> box = (i < 0) ? (NONE) : (i == 0) ? WIREFRAME : CYLINDERS;
    view -> create_shaders[MDBOX] = TRUE;
    update (view);
  }
#else
  from_box_or_axis = TRUE;
#endif
  if (i == 1)
  {
    if (is_the_widget_visible(box_win -> width_box)) hide_the_widgets (box_win -> width_box);
    if (! is_the_widget_visible(box_win -> radius_box)) show_the_widgets (box_win -> radius_box);
    if (! preferences)
    {
#ifdef GTK3
    // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][2], TRUE);
#endif
    }
    else
    {
      tmp_box -> box = CYLINDERS;
    }
  }
  else if (i == 0)
  {
    if (is_the_widget_visible(box_win -> radius_box)) hide_the_widgets (box_win -> radius_box);
    if (! is_the_widget_visible(box_win -> width_box)) show_the_widgets (box_win -> width_box);
    if (! preferences)
    {
#ifdef GTK3
    // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][1], TRUE);
#endif
    }
    else
    {
      tmp_box -> box = WIREFRAME;
    }
  }
  if (! preferences)
  {
    view -> create_shaders[MDBOX] = TRUE;
    update (view);
#ifdef GTK4
    update_menu_bar (view);
#endif
  }
#ifdef GTK3
  from_box_or_axis = FALSE;
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void set_show_box_toggle (GtkCheckButton * but, gpointer data)

  \brief toggle show / hide box callback GTK4

  \param but the GtkCheckButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_box_toggle (GtkCheckButton * but, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void set_show_box_toggle (GtkToggleButton * but, gpointer data)

  \brief toggle show / hide box callback GTK3

  \param but the GtkToggleButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_show_box_toggle (GtkToggleButton * but, gpointer data)
#endif
{
  gboolean val = button_get_status ((GtkWidget *)but);
  glwin * view;
  box_edition * box_win;
  if (! preferences)
  {
    view = (glwin *)data;
    box_win = view -> box_win;
  }
  else
  {
    box_win = pref_box_win;
  }
#ifdef GTK3
  from_box_or_axis = TRUE;
#endif // GTK3
  if (val)
  {
    if (! preferences)
    {
#ifdef GTK3
    // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][0], TRUE);
#endif
    }
    combo_set_active (box_win -> styles, WIREFRAME-1);
  }
  else
  {
    if (! preferences)
    {
#ifdef GTK3
    // GTK3 Menu Action To Check
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_box_axis[0][0], FALSE);
#endif
    }
    combo_set_active (box_win -> styles, NONE);
  }
#ifdef GTK3
  from_box_or_axis = FALSE;
#endif // GTK3
  widget_set_sensitive (box_win -> box_data, val);
#ifdef GTK4
  if (! preferences) update_menu_bar (view);
#endif
}

/*!
  \fn G_MODULE_EXPORT void set_color_box (GtkColorChooser * colob, gpointer data)

  \brief set box color callback

  \param colob the GtkColorChooser sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_color_box (GtkColorChooser * colob, gpointer data)
{
  if (preferences)
  {
    tmp_box -> color = get_button_color (colob);
  }
  else
  {
    glwin * view = (glwin *)data;
    view -> anim -> last -> img -> abc -> color = get_button_color (colob);
    view -> create_shaders[MDBOX] = TRUE;
    update (view);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_box_delete (GtkWindow * widg, gpointer data)

  \brief box window delete event - GTK4

  \param widg
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_box_delete (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_box_delete (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief box window delete event - GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_box_delete (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  view -> box_win -> win = destroy_this_widget (view -> box_win -> win);
  g_free (view -> box_win);
  view -> box_win = NULL;
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void box_advanced (GtkWidget * widg, gpointer data)

  \brief create the box edition window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void box_advanced (GtkWidget * widg, gpointer data)
{
  int i;
  glwin * view;
  box_edition * the_box;
  int box_type;
  double box_line;
  double box_rad;
  ColRGBA box_color;

  if (preferences)
  {
    the_box = pref_box_win;
    box_type = tmp_box -> box;
    box_color = tmp_box -> color;
    box_line = tmp_box -> line;
    box_rad = tmp_box -> rad;
  }
  else
  {
    view = (glwin *)data;
    view -> box_win = g_malloc0(sizeof*view -> box_win);
    the_box = view -> box_win;
    box_type = view -> anim -> last -> img -> abc -> box;
    box_color = view -> anim -> last -> img -> abc -> color;
    box_line = view -> anim -> last -> img -> abc -> line;
    box_rad = view -> anim -> last -> img -> abc -> rad;
  }

  GtkWidget * vbox = create_vbox (BSEP);
  if (preferences)
  {
    the_box -> win = create_vbox (BSEP);
    adv_box (the_box -> win, "<b>Box settings</b>", 5, 120, 0.0);
    GtkWidget * hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_box -> win, hbox, FALSE, FALSE, 20);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 60);
  }
  else
  {
    gchar * str = g_strdup_printf ("%s - box settings", get_project_by_id(view -> proj)->name);
    the_box -> win = create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
    add_container_child (CONTAINER_WIN, the_box -> win, vbox);
  }
  GtkWidget * box;
  gboolean ac;
  if (box_type != NONE)
  {
    ac = TRUE;
  }
  else
  {
    ac = FALSE;
  }
  the_box -> show_hide = check_button ("Show / hide box", 100, 40, ac, G_CALLBACK(set_show_box_toggle), data);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_box -> show_hide, FALSE, FALSE, 0);
  the_box -> box_data = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, the_box -> box_data, TRUE, TRUE, 10);
  widget_set_sensitive (the_box -> box_data, ac);

  GtkWidget * pos_box = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, the_box -> box_data, pos_box, TRUE, TRUE, 0);

  box = abox (the_box -> box_data, "Style ", 5);
  the_box -> styles  = create_combo ();
  for (i=0; i<BOX_STYLES; i++)
  {
    combo_text_append (the_box -> styles, box_style[i]);
  }
  if (box_type == NONE) i = NONE;
  if (box_type == WIREFRAME) i = 0;
  if (box_type == CYLINDERS) i = 1;
  combo_set_active (the_box -> styles, i);
  gtk_widget_set_size_request (the_box -> styles, 120, -1);
  g_signal_connect (G_OBJECT (the_box -> styles), "changed", G_CALLBACK(set_box_combo_style), data);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, the_box -> styles, FALSE, FALSE, 10);
  the_box -> width_box = abox (the_box -> box_data, "Line width [pts] ", 0);
  the_box -> width  = create_entry (G_CALLBACK(update_box_parameter), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(the_box -> width), box_line);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, the_box -> width_box, the_box -> width, FALSE, FALSE, 10);
  the_box -> radius_box = abox (the_box -> box_data, "Cylinder radius [&#xC5;] ", 0);
  the_box -> radius = create_entry (G_CALLBACK(update_box_parameter), 120, 10, FALSE, data);
  update_entry_double (GTK_ENTRY(the_box -> radius), box_rad);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, the_box -> radius_box, the_box -> radius, FALSE, FALSE, 10);

  // Colors
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, abox (the_box -> box_data, "Color ", 5), color_button(box_color, TRUE, 120, -1, G_CALLBACK(set_color_box), data), FALSE, FALSE, 10);

  if (! preferences)
  {
    add_global_option (vbox, & view -> colorp[1][0]);
    add_gtk_close_event (the_box -> win, G_CALLBACK(on_box_delete), view);
    show_the_widgets (the_box -> win);
    if (box_type)
    {
     hide_the_widgets (the_box -> width_box);
    }
    else
    {
      hide_the_widgets (the_box -> radius_box);
    }
  }
}
