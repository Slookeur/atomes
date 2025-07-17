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
* @file m_axis.c
* @short Functions to create the 'View -> Axis' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_axis.c'
*
* Contains:
*

 - The functions to create the 'View -> Axis' submenu

*
* List of functions:

  void menu_axis (GtkWidget * menu_ab, glwin * view, int id);
  void menu_axis (GMenu * menu_ab, glwin * view, int popm);

  G_MODULE_EXPORT void set_axis_template_pos (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void change_axis_pos_radio (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * axis_position_submenu (glwin * view, int id);

  GMenu * position_submenu (glwin * view, int popm, int pos);
  GMenu * axis_position_submenu (glwin * view, int popm);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"
#include "submenus.h"

extern G_MODULE_EXPORT void axis_advanced (GtkWidget * widg, gpointer data);
extern GtkWidget * create_layout_widget (gchar * str, GtkWidget * menu, int vab, gpointer data);

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void to_axis_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief create the axis advanced parameters window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_axis_advanced (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  axis_advanced (NULL, data);
}
#endif

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT void set_axis_template_pos (GtkWidget * widg, gpointer data)

  \brief handle change axis position signal GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_axis_template_pos (GtkWidget * widg, gpointer data)
{
/* TOP_RIGHT = 0,
   TOP_LEFT = 1,
   BOTTOM_RIGHT = 2,
   BOTTOM_LEFT = 3,
   CENTER = 4 */
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i = this_proj -> modelgl -> anim -> last -> img -> xyz -> t_pos;
  int j = the_data -> b;
  if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg) && i != j)
  {
    this_proj -> modelgl -> anim -> last -> img -> xyz -> t_pos = NONE;
    if (i != CUSTOM)
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_box_axis[1][8+i], FALSE);
    }
    if (widg != this_proj -> modelgl -> ogl_box_axis[1][8+j])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_box_axis[1][8+j], TRUE);
    }
    this_proj -> modelgl -> anim -> last -> img -> xyz -> t_pos = j;
    this_proj -> modelgl -> create_shaders[MAXIS] = TRUE;
    update (this_proj -> modelgl);
    if (this_proj -> modelgl -> axis_win)
    {
      if (this_proj -> modelgl -> axis_win -> templates && GTK_IS_WIDGET(this_proj -> modelgl -> axis_win -> templates))
      {
        combo_set_active (this_proj -> modelgl -> axis_win -> templates, j);
      }
    }
  }
  else if (i == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    if (i != CUSTOM)
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_box_axis[1][8+i], TRUE);
    }
  }
}

/*!
  \fn GtkWidget * axis_position_submenu (glwin * view, int id)

  \brief  create the 'Axis -> Position' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * axis_position_submenu (glwin * view, int id)
{
  int i, j, k;
  GtkWidget * menup = gtk_menu_new ();
  GtkWidget * ud, * udm, * widg;
  gchar * udlab[2] = {"Top", "Bottom"};
  gchar * lrlab[2] = {"Right Corner", "Left Corner"};

  i = 0;
  for (j=0; j<2; j++)
  {
    ud = create_menu_item (FALSE, udlab[j]);
    gtk_menu_shell_append ((GtkMenuShell *)menup, ud);
    udm = gtk_menu_new ();
    gtk_menu_item_set_submenu ((GtkMenuItem *)ud, udm);
    for (k=0; k<2; k++)
    {
      if (id == 0)
      {
        view -> ogl_box_axis[1][8+i] = gtk3_menu_item (udm, lrlab[k], IMG_NONE, NULL, G_CALLBACK(set_axis_template_pos), & view -> colorp[i][0],
                                                       FALSE, 0, 0, TRUE, TRUE, (view -> anim -> last -> img -> xyz -> t_pos == i) ? TRUE: FALSE);
        if (view -> anim -> last -> img -> xyz -> axis == NONE) widget_set_sensitive (view -> ogl_box_axis[1][8+i], 0);
      }
      else
      {
        widg = gtk3_menu_item (udm, lrlab[k], IMG_NONE, NULL, G_CALLBACK(set_axis_template_pos), & view -> colorp[i][0],
                               FALSE, 0, 0, TRUE, TRUE, (view -> anim -> last -> img -> xyz -> t_pos == i) ? TRUE: FALSE);
        if (view -> anim -> last -> img -> xyz -> axis == NONE) widget_set_sensitive (widg, 0);
      }
      i += 1;
    }
  }
  if (id == 0)
  {
    view -> ogl_box_axis[1][8+i] = gtk3_menu_item (menup, "Center", IMG_NONE, NULL, G_CALLBACK(set_axis_template_pos), & view -> colorp[i][0],
                                                   FALSE, 0, 0, TRUE, TRUE, (view -> anim -> last -> img -> xyz -> t_pos == i) ? TRUE: FALSE);
    if (view -> anim -> last -> img -> xyz -> axis == NONE) widget_set_sensitive (view -> ogl_box_axis[1][8+i], 0);
  }
  else
  {
    widg = gtk3_menu_item (menup, "Center", IMG_NONE, NULL, G_CALLBACK(set_axis_template_pos), & view -> colorp[i][0],
                           FALSE, 0, 0, TRUE, TRUE, (view -> anim -> last -> img -> xyz -> t_pos == i) ? TRUE: FALSE);
    if (view -> anim -> last -> img -> xyz -> axis == NONE) widget_set_sensitive (widg, 0);
  }
  return menup;
}

/*!
  \fn void menu_axis (GtkWidget * menu_ab, glwin * view, int id)

  \brief create 'Axis' menu GTK3

  \param menu_ab the GtkWidget sending the signal
  \param view the target glwin
  \param id main app (0) or popup (1)
*/
void menu_axis (GtkWidget * menu_ab, glwin * view, int id)
{
  GtkWidget * widg = create_menu_item (FALSE, "Length");
  gtk_menu_shell_append ((GtkMenuShell *)menu_ab, widg);
  GtkWidget * menul = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menul);

  gchar * str = g_strdup_printf (" Length [ %f Å ]", view -> anim -> last -> img -> xyz -> length);
  if (id == 0)
  {
    view -> ogl_box_axis[1][7] = create_layout_widget (str, menul, view -> anim -> last -> img -> xyz -> axis, & view -> colorp[0][AXIS]);
    if (view -> anim -> last -> img -> xyz -> axis == NONE)
    {
      widget_set_sensitive (view -> ogl_box_axis[1][7], 0);
    }
  }
  else
  {
    widg = create_layout_widget (str, menul, view -> anim -> last -> img -> xyz -> axis, & view -> colorp[0][AXIS]);
    if (view -> anim -> last -> img -> xyz -> axis == NONE)
    {
      widget_set_sensitive (widg, 0);
    }
  }
  g_free (str);
  gtk_menu_shell_append ((GtkMenuShell *)menu_ab, menu_item_new_with_submenu ("Position", TRUE, axis_position_submenu(view, id)));
  add_advanced_item (menu_ab, G_CALLBACK(axis_advanced), (gpointer)view, FALSE, 0, 0);
}
#else
/*!
  \fn G_MODULE_EXPORT void change_axis_pos_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief handle change axis position signal GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_axis_pos_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  /* TOP_RIGHT = 0,
     TOP_LEFT = 1,
     BOTTOM_RIGHT = 2,
     BOTTOM_LEFT = 3,
     CENTER = 4 */
  tint * the_data = (tint *)data;
  glwin * view = get_project_by_id(the_data -> a) -> modelgl;
  const gchar * pos = g_variant_get_string (parameter, NULL);
  int lgt = strlen (pos);
  gchar * name = g_strdup_printf ("%c%c", pos[lgt-2], pos[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, pos);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-axis-pos", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    int i;
    gchar * pos_name = NULL;
    for (i=0; i<5; i++)
    {
      pos_name = g_strdup_printf ("set-axis-pos.%d.0", i);
      if (g_strcmp0(pos, (const gchar *)pos_name) == 0)
      {
        view -> anim -> last -> img -> xyz -> t_pos = i;
        g_free (pos_name);
        pos_name = NULL;
        break;
      }
      g_free (pos_name);
      pos_name = NULL;
    }
    g_action_change_state (G_ACTION (action), parameter);
    view -> create_shaders[MAXIS] = TRUE;
    update (view);
    g_action_change_state (G_ACTION (action), parameter);
    if (view -> axis_win)
    {
      if (view -> axis_win -> templates && GTK_IS_WIDGET(view -> axis_win -> templates))
      {
        combo_set_active (view -> axis_win -> templates, i);
      }
    }
  }
}

/*!
  \fn GMenu * position_submenu (glwin * view, int popm, int pos)

  \brief create elements of the 'Axis -> Position' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param pos position id
*/
GMenu * position_submenu (glwin * view, int popm, int pos)
{
  GMenu * menu = g_menu_new ();
  gchar * lrlab[2] = {"Right Corner", "Left Corner"};
  int i, j;
  i = pos*2;
  for (j=0; j<2; j++)
  {
    append_opengl_item (view, menu, lrlab[j], "axis-pos", popm, i, NULL, IMG_NONE, NULL, FALSE,
                        G_CALLBACK(change_axis_pos_radio), & view -> colorp[i][0],
                        FALSE, (view -> anim -> last -> img -> xyz -> t_pos == i) ? TRUE: FALSE, TRUE, TRUE);
    i ++;
  }
  return menu;
}

/*!
  \fn GMenu * axis_position_submenu (glwin * view, int popm)

  \brief create the 'Axis -> Position' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * axis_position_submenu (glwin * view, int popm)
{
  int i;
  GMenu * menu = g_menu_new ();
  gchar * udlab[2] = {"Top", "Bottom"};
  for (i=0; i<2; i++)
  {
    append_submenu (menu, udlab[i], position_submenu(view, popm, i));
  }
  append_opengl_item (view, menu, "Center", "axis-pos", popm, 4, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(change_axis_pos_radio), & view -> colorp[4][0],
                      FALSE, (view -> anim -> last -> img -> xyz -> t_pos == 4) ? TRUE: FALSE, TRUE, TRUE);
  return menu;
}

/*!
  \fn void menu_axis (GMenu * menu_ab, glwin * view, int popm)

  \brief create the 'Axis' submenu GTK4

  \param menu_ab the menu to attach the new menu to
  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
void menu_axis (GMenu * menu_ab, glwin * view, int popm)
{
  GMenuItem * item = g_menu_item_new ("Length", (view -> anim -> last -> img -> xyz -> axis) != NONE ? NULL : "None");
  g_menu_item_set_attribute (item, "custom", "s", "axis-length", NULL);
  g_menu_item_set_submenu (item, (GMenuModel *)axis_box_param (view, popm, AXIS, NONE));
  g_menu_append_item (menu_ab, item);
  append_submenu (menu_ab, "Position", axis_position_submenu(view, popm));
  append_opengl_item (view, menu_ab, "Advanced", "axis-advanced", popm, popm, NULL, IMG_STOCK, DPROPERTIES, FALSE,
                      G_CALLBACK(to_axis_advanced), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
}
#endif
