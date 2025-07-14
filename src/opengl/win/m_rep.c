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
* @file m_rep.c
* @short Functions to create the 'View -> Representation' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_rep.c'
*
* Contains:
*

 - The functions to create the 'View -> Representation' submenu

*
* List of functions:

  G_MODULE_EXPORT gboolean scroll_set_camera (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data);
  G_MODULE_EXPORT gboolean on_rep_delete (GtkWindow * widg, gpointer data);
  G_MODULE_EXPORT gboolean on_rep_delete (GtkWidget * widg, GdkEvent * event, gpointer data);

  void update_labels (glwin * view);
  void camera_has_changed (gdouble value, gpointer data);

  G_MODULE_EXPORT void reset_view (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void to_reset_view (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_reset_view (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_camera (GtkRange * range, gpointer data);
  G_MODULE_EXPORT void set_camera_spin (GtkSpinButton * res, gpointer data);
  G_MODULE_EXPORT void representation_advanced (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_rep (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void change_rep_radio (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_rep_advanced (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_center_molecule (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * menu_rep (glwin * view, int id);

  GMenu * menu_rep (glwin * view, int popm);
  GMenu * menu_reset (glwin * view, int popm);
  GMenu * menu_fullscreen (glwin * view, int popm);
  GMenu * menu_view (glwin * view, int popm);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"
#include "submenus.h"
#include "preferences.h"

extern void save_rotation_quaternion (glwin * view);
extern void rotate_x_y (glwin * view, double angle_x, double angle_y);
#ifdef GTK4
extern G_MODULE_EXPORT void set_full_screen (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
extern G_MODULE_EXPORT void set_full_screen (GtkWidget * widg, gpointer data);
#endif

gchar * text_reps[OGL_REPS] = {"Orthographic", "Perspective"};

/*!
  \fn void update_labels (glwin * view)

  \brief update labels (on representation data update)

  \param view the target glwin
*/
void update_labels (glwin * view)
{
  int i;
  for (i=0; i<2; i++) if (view -> anim -> last -> img -> labels[i].scale) view -> create_shaders[LABEL] = TRUE;
  if (view -> anim -> last -> img -> labels[2].scale) view -> create_shaders[MAXIS] = TRUE;
  if (view -> anim -> last -> img -> labels[3].scale) view -> create_shaders[MEASU] = TRUE;
}

/*!
  \fn void camera_has_changed (gdouble value, gpointer data)

  \brief update camera data

  \param value the new value
  \param data the associated data pointer
*/
void camera_has_changed (gdouble value, gpointer data)
{
  tint * cid = (tint *)data;
  project * this_proj = get_project_by_id(cid -> a);
  double v;
  switch (cid -> b)
  {
    case 0:
      this_proj -> modelgl -> anim -> last -> img -> zoom = 2.0*(1.0-value);
      // gtk_spin_button_set_increments ((GtkSpinButton *)this_proj -> modelgl -> rep_win -> camera_widg[0], this_proj -> modelgl -> zoom_factor, this_proj -> modelgl -> zoom_factor);
      break;
    case 1:
      // > camera depth
      if (value > this_proj -> modelgl -> anim -> last -> img -> gnear)
      {
        this_proj -> modelgl -> anim -> last -> img -> p_depth = value;
      }
      else
      {
        this_proj -> modelgl -> anim -> last -> img -> p_depth = this_proj -> modelgl -> anim -> last -> img -> gnear + 0.01;
        gtk_spin_button_set_value ((GtkSpinButton *)this_proj -> modelgl -> rep_win -> camera_widg[1], this_proj -> modelgl -> anim -> last -> img -> p_depth);
      }
      break;
    case 2:
      // < perspective depth
      if (value < this_proj -> modelgl -> anim -> last -> img -> p_depth)
      {
        this_proj -> modelgl -> anim -> last -> img -> gnear = value;
      }
      else
      {
        this_proj -> modelgl -> anim -> last -> img -> gnear = this_proj -> modelgl -> anim -> last -> img -> p_depth - 0.01;
        gtk_spin_button_set_value ((GtkSpinButton *)this_proj -> modelgl -> rep_win -> camera_widg[2], this_proj -> modelgl -> anim -> last -> img -> gnear);
      }
      break;
    default:
      if (cid -> b < 5)
      {
        if (value != this_proj -> modelgl -> anim -> last -> img -> c_angle[cid -> b - 3])
        {
          v = this_proj -> modelgl -> anim -> last -> img -> c_angle[cid -> b - 3] - value;
          save_rotation_quaternion (this_proj -> modelgl);
          if (cid -> b == 3)
          {
            rotate_x_y (this_proj -> modelgl, v, 0.0);
          }
          else
          {
            rotate_x_y (this_proj -> modelgl, 0.0, v);
          }
        }
      }
      else
      {
        this_proj -> modelgl -> anim -> last -> img -> c_shift[cid -> b - 5] = (value == 0.0) ? value : - value;
      }
      break;
  }
  update_labels (this_proj -> modelgl);
  update (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void reset_view (GtkButton * but, gpointer data)

  \brief reset view callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void reset_view (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *)data;
  int i = view -> mode;
  view -> mode = ANALYZE;
  init_camera (get_project_by_id(view -> proj), FALSE);
  view -> mode = i;
  update_labels (view);
  update (view);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_reset_view (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief reset view callback - GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_reset_view (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void to_reset_view (GtkWidget * widg, gpointer data)

  \brief reset view callback - GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_reset_view (GtkWidget * widg, gpointer data)
#endif
{
  reset_view (NULL, data);
}

/*!
  \fn G_MODULE_EXPORT gboolean scroll_set_camera (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)

  \brief update camera data callback - scroll callback

  \param range the GtkRange sending the signal
  \param scroll the associated scroll type
  \param value the range value
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean scroll_set_camera (GtkRange * range, GtkScrollType scroll, gdouble value, gpointer data)
{
  camera_has_changed (value, data);
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_camera (GtkRange * range, gpointer data)

  \brief update camera data callback - range callback

  \param range the GtkRange sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_camera (GtkRange * range, gpointer data)
{
  camera_has_changed (gtk_range_get_value (range), data);
}

/*!
  \fn G_MODULE_EXPORT void set_camera_spin (GtkSpinButton * res, gpointer data)

  \brief update camera data callback - spin button

  \param res the GtkSpinButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_camera_spin (GtkSpinButton * res, gpointer data)
{
  camera_has_changed (gtk_spin_button_get_value(res), data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean on_rep_delete (GtkWindow * widg, gpointer data)

  \brief representation window delete event - GTK4

  \param widg
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_rep_delete (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_rep_delete (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief representation window delete event - GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_rep_delete (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  int i;
  for (i=0; i<7; i++)
  {
    if (view -> rep_win -> camera_widg[i]) view -> rep_win -> camera_widg[i] = destroy_this_widget(view -> rep_win -> camera_widg[i]);
  }
  destroy_this_widget ((GtkWidget *)widg);
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void representation_advanced (GtkWidget * widg, gpointer data)

  \brief open advanced representation dialog

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void representation_advanced (GtkWidget * widg, gpointer data)
{
  gchar * cam_opts[7]={"Zoom", "<b>P</b>erspective depth", "<b>C</b>amera depth",
                       "Camera pitch", "Camera heading",
                       "Camera right/left", "Camera up/down"};
  gchar * str;
  double smax[7] = {1.0, 100.0, 100.0, 180.0, 180.0, 100.0, 100.0};
  double smin[7] = {-2.0, 0.0, 0.0, -180.0, -180.0, -100.0, -100.0};
  double sdel[7] = {0.001, 0.01, 0.01, 0.1, 0.1, 0.01, 0.01};
  int sdig[7] = {3, 2, 2, 1, 1, 2, 2};
  int i;
  double v;
  glwin * view;
  rep_edition * the_rep;
  GtkWidget * vbox = create_vbox (BSEP);
  int rep;
  if (preferences)
  {
    the_rep = pref_rep_win;
    the_rep -> win = create_vbox (BSEP);
    adv_box (the_rep -> win, "<b>OpenGL camera set-up</b>", 10, 120, 0.0);
    GtkWidget * hbox = create_hbox (BSEP);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vbox, FALSE, FALSE, 30);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, the_rep -> win, hbox, FALSE, FALSE, 10);
    rep = tmp_rep -> rep;
  }
  else
  {
    view = (glwin *)data;
    view -> rep_win =  g_malloc0(sizeof*view -> rep_win);
    the_rep = view -> rep_win;
    str = g_strdup_printf ("OpenGL camera set-up - %s", get_project_by_id(view -> proj)->name);
    the_rep -> win =  create_win (str, view -> win, FALSE, FALSE);
    g_free (str);
    add_container_child (CONTAINER_WIN, the_rep -> win, vbox);
    rep = view -> anim -> last -> img -> rep;
  }
  GtkWidget * box;
  for (i=0; i<7; i++)
  {
    box = abox (vbox, cam_opts[i], 0);
    switch (i)
    {
      case 0:
        v = 1.0-0.5*((preferences) ? tmp_rep -> zoom : view -> anim -> last -> img -> zoom);
        break;
      case 1:
        v = (preferences) ? tmp_rep -> p_depth : view -> anim -> last -> img -> p_depth;
        break;
      case 2:
        v = (preferences) ? tmp_rep -> gnear : view -> anim -> last -> img -> gnear;
        break;
      default:
        if (i < 5)
        {
          v = (preferences) ? tmp_rep -> c_angle[i-3] : view -> anim -> last -> img -> c_angle[i-3];
        }
        else
        {
          if (preferences)
          {
            v = (tmp_rep -> c_shift[i-5] == 0.0) ? 0.0 : - tmp_rep -> c_shift[i-5];
          }
          else
          {
            v = (view -> anim -> last -> img -> c_shift[i-5] == 0.0) ? 0.0 : - view -> anim -> last -> img -> c_shift[i-5];
          }
        }
        break;
    }
    if (the_rep -> camera_widg[i]) the_rep -> camera_widg[i] = destroy_this_widget (the_rep -> camera_widg[i]);
    the_rep -> camera_widg[i] = spin_button (G_CALLBACK(set_camera_spin), v, smin[i], smax[i], sdel[i], sdig[i], 150, (preferences) ? & pref_pointer[i] : & view -> colorp[i][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, the_rep -> camera_widg[i], FALSE, FALSE, 10);
    if (i > 2 || i == 0)
    {
      str = g_strdup_printf ("in [%.1f, %.1f]", smin[i], smax[i]);
    }
    else if (i == 1)
    {
      str = g_strdup_printf ("in [<b>C</b>. depth, %.1f]", smax[i]);
    }
    else
    {
      str = g_strdup_printf ("in [%.1f, <b>P</b>. depth]", smin[i]);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, markup_label(str, 25, -1, 0.0, 0.5), FALSE, FALSE, 5);
    g_free (str);
    if(i < 3 && rep == ORTHOGRAPHIC) widget_set_sensitive (the_rep -> camera_widg[i], 0);
  }
  if (! preferences)
  {
    box = create_hbox(0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, box, FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, create_button("Reset view", IMG_NONE, NULL, 100, 25, GTK_RELIEF_NORMAL, G_CALLBACK(reset_view), view), FALSE, FALSE, 200);
    add_gtk_close_event (the_rep -> win, G_CALLBACK(on_rep_delete), view);
    show_the_widgets (the_rep -> win);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_rep (GtkWidget * widg, gpointer data)

  \brief change representation callback - GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_rep (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i, j;
  i = this_proj -> modelgl -> anim -> last -> img -> rep;
  j = the_data -> b;
#ifdef GTK4
  if (i != j)
#else
  if (i != j && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
#endif
  {
    this_proj -> modelgl -> anim -> last -> img -> rep = NONE;
#ifdef GTK3
    // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_rep[i], FALSE);
    if (widg != this_proj -> modelgl -> ogl_rep[j])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_rep[j], TRUE);
    }
#endif
    this_proj -> modelgl -> anim -> last -> img -> rep = j;
#ifdef GTK3
    // GTK3 Menu Action To Check
    for (i=2; i<4; i++)
    {
      if (this_proj -> modelgl -> rep_win)
      {
        if (this_proj -> modelgl -> rep_win -> camera_widg[i] && GTK_IS_WIDGET(this_proj -> modelgl -> rep_win -> camera_widg[i]))
        {
          widget_set_sensitive (this_proj -> modelgl -> rep_win -> camera_widg[i], j);
        }
      }
    }
#endif
    this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
    update (this_proj -> modelgl);
  }
#ifdef GTK3
  // GTK3 Menu Action To Check
  else if (i == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_rep[j], TRUE);
  }
#endif
}

#ifdef GTK3
/*!
  \fn GtkWidget * menu_rep (glwin * view, int id)

  \brief create the 'View' submenu - GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_rep (glwin * view, int id)
{
  int i, j;
  GtkWidget * menur = gtk_menu_new ();
  j = view -> anim -> last -> img -> rep;
  if (id == 0)
  {
    for (i=0; i<OGL_REPS; i++)
    {
      view -> ogl_rep[i] = gtk3_menu_item (menur, text_reps[i], IMG_NONE, NULL, G_CALLBACK(set_rep), & view -> colorp[i][0], FALSE, 0, 0, TRUE, TRUE, (i == j) ? TRUE : FALSE);
    }
  }
  else
  {
    for (i=0; i<OGL_REPS; i++)
    {
      gtk3_menu_item (menur, text_reps[i], IMG_NONE, NULL, G_CALLBACK(set_rep), & view -> colorp[i][0], FALSE, 0, 0, TRUE, TRUE, (i == j) ? TRUE : FALSE);
    }
  }
  add_advanced_item (menur, G_CALLBACK(representation_advanced), (gpointer)view, FALSE, 0, 0);
  return menur;
}
#else
/*!
  \fn G_MODULE_EXPORT void change_rep_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief change representation radio items callback - GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_rep_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * rep = g_variant_get_string (parameter, NULL);
  int lgt = strlen (rep);
  gchar * name = g_strdup_printf ("%c%c", rep[lgt-2], rep[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, rep);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-rep", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    const gchar * rep = g_variant_get_string (parameter, NULL);
    gchar * rep_name = NULL;
    int i;
    for (i=0; i<OGL_REPS; i++)
    {
      rep_name = g_strdup_printf ("set-rep.%d.0", i);
      if (g_strcmp0(rep, (const gchar *)rep_name) == 0)
      {
        set_rep (NULL, & view -> colorp[i][0]);
        g_free (rep_name);
        rep_name = NULL;
        break;
      }
      g_free (rep_name);
      rep_name = NULL;
    }
    g_action_change_state (G_ACTION (action), parameter);
  }
}

/*!
  \fn G_MODULE_EXPORT void to_rep_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief change representation callback - GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_rep_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  representation_advanced (NULL, data);
}

/*!
  \fn GMenu * menu_rep (glwin * view, int popm)

  \brief create 'View -> Representation' submenu items - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_rep (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  int i, j;
  i = view -> anim -> last -> img -> rep;
  for (j=0; j<OGL_REPS; j++)
  {
    append_opengl_item (view, menu, text_reps[j], "rep", popm, j, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(change_rep_radio), (gpointer)view, FALSE, (i == j) ? TRUE : FALSE, TRUE, TRUE);
  }
  append_opengl_item (view, menu, "Advanced", "rep-adv", popm, j, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_rep_advanced), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_center_molecule (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief center molecule callback - GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_center_molecule (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  center_this_molecule (data);
}

/*!
  \fn GMenu * menu_reset (glwin * view, int popm)

  \brief create the reset menu items - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_reset (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Reset view", "reset-view", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_reset_view), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  append_opengl_item (view, menu, "Center molecule", "center-mol", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_center_molecule), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn GMenu * menu_fullscreen (glwin * view, int popm)

  \brief create the 'Fullscreen' menu item - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_fullscreen (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Fullscreen", "full", popm, popm, "<CTRL>F", IMG_STOCK, (gpointer)FULLSCREEN, FALSE, G_CALLBACK(set_full_screen), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn GMenu * menu_view (glwin * view, int popm)

  \brief create the 'View' submenu - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_view (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Representation", menu_rep(view, popm));
  append_submenu (menu, "Projection", menu_proj(view, popm));
  append_submenu (menu, "Background", menu_back(view, popm));
  if (get_project_by_id(view -> proj) -> nspec) g_menu_append_item (menu, menu_box_axis (view, popm, 1));
  if (! popm)
  {
    g_menu_append_section (menu, NULL, (GMenuModel*)menu_reset(view, popm));
    g_menu_append_section (menu, NULL, (GMenuModel*)menu_fullscreen(view, popm));
  }
  return menu;
}

#endif
