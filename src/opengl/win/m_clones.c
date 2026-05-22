/* This file is part of the 'atomes' software

'atomes' is free software: you can redistribute it and/or modify it under the terms
of the GNU Affero General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

'atomes' is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with 'atomes'.
If not, see <https://www.gnu.org/licenses/>

Copyright (C) 2022-2026 by CNRS and University of Strasbourg */

/*!
* @file m_clones.c
* @short Functions to create the 'Model -> Clone(s)' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_clones.c'
*
* Contains:
*

 - The functions to create the 'Model -> Clone(s)' submenu

*
* List of functions:

  G_MODULE_EXPORT void show_hide_clones (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void show_hide_clones (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * create_clone_widget (char * name, gboolean clone, GtkWidget * menu, glwin * view);
  GtkWidget * menu_clones (glwin * view, int id);

  GMenu * menu_clones (glwin * view, int popm);

*/

#include "global.h"
#include "glview.h"
#include "glwindow.h"
#include "submenus.h"

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT void show_hide_clones (GtkWidget * widg, gpointer data)

  \brief show/hide clone(s) callback GTK3

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_clones (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *)data;
  gboolean j;
  j = gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg);
  if (widg != view -> ogl_clones[0])
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)view -> ogl_clones[0], j);
  }
  view -> anim -> last -> img -> draw_clones = j;
  widget_set_sensitive (view -> ogl_atoms[5], j);
  widget_set_sensitive (view -> ogl_atoms[7], j);
  widget_set_sensitive (view -> ogl_bonds[9], j);
  widget_set_sensitive (view -> ogl_bonds[11], j);
  widget_set_sensitive (view -> ogl_bonds[13], j);
  widget_set_sensitive (view -> ogl_clones[1], j);
  widget_set_sensitive (view -> ogl_clones[2], j);
  widget_set_sensitive (view -> ogl_clones[3], j);
  widget_set_sensitive (view -> ogl_clones[4], j);
  int i;
  for (i=0; i<get_project_by_id(view -> proj) -> nspec; i++)
  {
    widget_set_sensitive (view -> ogl_spec[1][i], j);
    widget_set_sensitive (view -> ogl_lab[1][i], j);
  }
  init_default_shaders (view);
  update (view);
}

/*!
  \fn GtkWidget * create_clone_widget (char * name, gboolean clone, GtkWidget * menu, glwin * view)

  \brief create the 'Show/Hide clone(s)' menu item GTK3

  \param name label of the new menu item
  \param clone draw clones (1) or not (0)
  \param menu the GtkMenu to attach the new item to
  \param view the target glwin
*/
GtkWidget * create_clone_widget (char * name, gboolean clone, GtkWidget * menu, glwin * view)
{
  GtkWidget * clone_widget = gtk3_menu_item (menu, name, IMG_NONE, NULL, G_CALLBACK(show_hide_clones), view, FALSE, 0, 0, TRUE, FALSE, clone);
  widget_set_sensitive (clone_widget, view -> allbonds[1]);
  return clone_widget;
}

/*!
  \fn GtkWidget * menu_clones (glwin * view, int id)

  \brief create the 'Clone(s)' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_clones (glwin * view, int id)
{
  GtkWidget * menu = gtk_menu_new ();
  if (id == 0)
  {
    view -> ogl_clones[0] = create_clone_widget (_("Show/Hide"), view -> anim -> last -> img -> draw_clones, menu, view);
  }
  else
  {
    create_clone_widget (_("Show/Hide"), view -> anim -> last -> img -> draw_clones, menu, view);
  }
  gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu (_("Atom(s)"), TRUE, menu_atoms (view, id, 1)));
  if ((id == 0) || (view -> anim -> last -> img -> style != SPHERES && view -> anim -> last -> img -> style != PUNT))
  {
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu (_("Bond(s)"), TRUE, menu_bonds (view, id, 1)));
  }
  return menu;
}
#else
/*!
  \fn G_MODULE_EXPORT void show_hide_clones (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief show/hide clones menu item callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_hide_clones (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * name = g_action_get_name ((GAction *)action);
  if (g_strcmp0(name, "set-show-clones.1.1") == 0)
  {
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-show-clones.0.0", NULL);
  }
  else
  {
    GVariant * state;
    gboolean show;
    if (action)
    {
      state = g_action_get_state (G_ACTION (action));
      show = ! g_variant_get_boolean (state);
    }
    else
    {
      show = ! view -> anim -> last -> img -> draw_clones;
    }
    view -> anim -> last -> img -> draw_clones = show;
    init_default_shaders (view);
    update (view);
    // Update the menu bar is reqiured to udpate the clone(s) submenus
    update_menu_bar (view);
    if (action)
    {
      g_variant_unref (state);
    }
  }
}

/*!
  \fn GMenu * menu_clones (glwin * view, int popm)

  \brief create the 'Clone(s)' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_clones (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, _("Show/Hide"), "show-clones", popm, popm, NULL, IMG_NONE, NULL, FALSE,
                      G_CALLBACK(show_hide_clones), (gpointer)view, TRUE, view -> anim -> last -> img -> draw_clones, FALSE, (view -> allbonds[1]) ? TRUE : FALSE);
  append_submenu (menu, _("Atom(s)"), menu_atoms(view, popm, 1));
  if (view -> anim -> last -> img -> style != SPHERES && view -> anim -> last -> img -> style != PUNT)
  {
    append_submenu (menu, _("Bonds(s)"), menu_bonds(view, popm, 1));
  }
  return menu;
}
#endif
