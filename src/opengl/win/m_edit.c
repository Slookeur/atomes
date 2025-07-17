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
* @file m_edit.c
* @short Functions to create the 'Tools -> Edit' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_edit.c'
*
* Contains:
*

 - The functions to create the 'Tools -> Edit' submenu

*
* List of functions:

  G_MODULE_EXPORT void wrapping_coord (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void wrapping_coord (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void to_run_rebuild (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * menu_cell_edit (glwin * view , int id, int j);
  GtkWidget * menu_edit (glwin * view, int id);

  GMenu * menu_cell_edit (glwin * view, int popm, int sensitive);
  GMenu * menu_atom_edit (glwin * view, int popm);
  GMenu * extract_section (glwin * view, int popm);
  GMenu * menu_edit (glwin * view, int popm);

*/

#include "cell_edit.h"
#include "atom_edit.h"

#ifdef GTK4
extern G_MODULE_EXPORT void crystal_window (GSimpleAction * action, GVariant * parameter, gpointer data);
extern G_MODULE_EXPORT void turn_rebuild (GSimpleAction * action, GVariant * parameter, gpointer data);
#else
extern G_MODULE_EXPORT void crystal_window (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void turn_rebuild (GtkWidget * widg, gpointer data);
#endif

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void wrapping_coord (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief wrapp coordinates callback

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void wrapping_coord (GSimpleAction * action, GVariant * parameter, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT void wrapping_coord (GtkWidget * widg, gpointer data)

  \brief wrapp coordinates callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void wrapping_coord (GtkWidget * widg, gpointer data)
#endif
{
  glwin * view = (glwin *)data;
  wrapping (view);
}

#ifdef GTK3
/*!
  \fn GtkWidget * menu_cell_edit (glwin * view , int id, int j)

  \brief create the 'Edit -> Cell' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
  \param j menu items sensitivity (cell + single MD step)
*/
GtkWidget * menu_cell_edit (glwin * view , int id, int j)
{
  int i, k, l;
  GtkWidget * menu = gtk_menu_new ();
  if (id == 0)
  {
    for (i=0; i<6; i++)
    {
      view -> ogl_box[2+i] = create_menu_item (FALSE, edit_names[i]);
      if (i == 0)
      {
        g_signal_connect (G_OBJECT (view -> ogl_box[2+i]), "activate", G_CALLBACK(wrapping_coord), view);
      }
      else if (i == 3)
      {
        g_signal_connect (G_OBJECT (view -> ogl_box[2+i]), "activate", G_CALLBACK(super_cell), view);
      }
      else
      {
        k = (i < 3) ? i-1 : i-2;
        g_signal_connect (G_OBJECT (view -> ogl_box[2+i]), "activate", G_CALLBACK(edition_win), & view -> colorp[k][0]);
      }
      if (i > 2) widget_set_sensitive (view -> ogl_box[2+i], j);
      if (i == 0) widget_set_sensitive (view -> ogl_box[2+i], ! view -> wrapped);
      if (i == 3)
      {
        k = 0;
        for (l=0; l<3; l++) k += view -> anim -> last -> img-> abc -> extra_cell[l];
        widget_set_sensitive (view -> ogl_box[2+i], k);
      }
      gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_box[2+i]);
    }
  }
  else
  {
    for (i=0; i<6; i++)
    {
      GtkWidget * widg = create_menu_item (FALSE, edit_names[i]);
      if (i == 0)
      {
        g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(wrapping_coord), view);
      }
      else if (i == 3)
      {
        g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(super_cell), view);
      }
      else
      {
        k = (i < 3) ? i-1 : i-2;
        g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(edition_win), & view -> colorp[k][0]);
      }
      if (i > 2) widget_set_sensitive (widg, j);
      if (i == 0) widget_set_sensitive (widg, ! view -> wrapped);
      if (i == 3)
      {
        k = 0;
        for (l=0; l<3; l++) k += view -> anim -> last -> img-> abc -> extra_cell[l];
        widget_set_sensitive (widg, k);
      }
      gtk_menu_shell_append ((GtkMenuShell *)menu, widg);
    }
  }
  return menu;
}

/*!
  \fn GtkWidget * menu_edit (glwin * view, int id)

  \brief create the 'Edit' submenu GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_edit (glwin * view, int id)
{
  int i, j;
  project * this_proj = get_project_by_id(view -> proj);
  GtkWidget * menu = gtk_menu_new ();
  GtkWidget * widg;
  if (id == 0)
  {
    view -> cbuilder = create_menu_item (FALSE, "Crystal Builder");
    g_signal_connect (G_OBJECT (view -> cbuilder), "activate", G_CALLBACK(crystal_window), & view -> colorp[0][0]);
    gtk_menu_shell_append ((GtkMenuShell *)menu, view -> cbuilder);
  }
  else
  {
    widg = create_menu_item (FALSE, "Crystal Builder");
    g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(crystal_window), & view -> colorp[0][0]);
    gtk_menu_shell_append ((GtkMenuShell *)menu, widg);
  }
  j = (this_proj -> cell.ltype && this_proj -> steps == 1) ? 1 : 0;
  if (id == 0)
  {
    view -> ogl_box[1] = menu_item_new_with_submenu ("Cell", (this_proj -> natomes) ? this_proj -> cell.ltype : 0, menu_cell_edit(view, id, j));
    gtk_menu_shell_append ((GtkMenuShell *)menu, view -> ogl_box[1]);
  }
  else
  {
    gtk_menu_shell_append ((GtkMenuShell *)menu, menu_item_new_with_submenu ("Cell", (this_proj -> natomes) ? this_proj -> cell.ltype : 0, menu_cell_edit(view, id, j)));
  }

  GtkWidget * ats = create_menu_item (FALSE, "Atoms");
  gtk_menu_shell_append ((GtkMenuShell *)menu, ats);
  GtkWidget * menua = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)ats, menua);
  j = (this_proj -> steps == 1) ? 1 : 0;
  for (i=0; i<5; i++)
  {
    widg = create_menu_item (TRUE, action_name[i]);
    g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(action_window), & view -> colorp[i][0]);
    gtk_menu_shell_append ((GtkMenuShell *)menua, widg);
    widget_set_sensitive (widg, this_proj -> nspec);
    widget_set_sensitive (widg, (i == 3) ? j : (this_proj -> natomes) ? j : 0);
  }

  if (id == 0)
  {
    gchar * rtext[2] = {"Extract/Rebuild on Motion", "Extract/Rebuild on Copy"};
    for (i=0; i<2; i++)
    {
      view -> rbuild[i] = gtk3_menu_item (menu, rtext[i], IMG_STOCK, (gpointer)ECUT, G_CALLBACK(turn_rebuild), & view -> colorp[i][0], FALSE, 0, 0, TRUE, FALSE, view -> rebuild[i][0]);
      widget_set_sensitive (view -> rbuild[i], (this_proj -> steps == 1) ? 1 : 0);
    }
  }
  return menu;
}
#else
/*!
  \fn GMenu * menu_cell_edit (glwin * view, int popm, int sensitive)

  \brief create the 'Edit -> Cell' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
  \param sensitive menu items sensitivity (cell + single MD step)
*/
GMenu * menu_cell_edit (glwin * view, int popm, int sensitive)
{
  GMenu * menu = g_menu_new ();
  gboolean sens;
  GCallback edit_handler[3] = {G_CALLBACK(wrapping_coord), G_CALLBACK(super_cell), G_CALLBACK(edition_win)};
  int i, j, k, l;
  gchar * act;
  for (i=0; i<6; i++)
  {
    j = (i == 0) ? i : (i == 3) ? 1 : 2;
    if (i > 2) sens = sensitive;
    if (i == 0) sens = ! view -> wrapped;
    if (i == 3)
    {
      k = 0;
      for (l=0; l<3; l++) k += view -> anim -> last -> img-> abc -> extra_cell[l];
      sens = k;
    }
    act = g_strdup_printf ("ceed-%d", i);
    if (i == 0 || i == 3)
    {
      append_opengl_item (view, menu, edit_names[i], act, popm, i, NULL, IMG_NONE, NULL, FALSE, edit_handler[j], (gpointer)view, FALSE, FALSE, FALSE, sens);
    }
    else
    {
      k = (i < 3) ? i-1 : i-2;
      append_opengl_item (view, menu, edit_names[i], act, popm, i, NULL, IMG_NONE, NULL, FALSE, edit_handler[j], & view -> colorp[k][0], FALSE, FALSE, FALSE, sens);
    }
    g_free (act);
  }
  return menu;
}

/*!
  \fn GMenu * menu_atom_edit (glwin * view, int popm)

  \brief create the 'Edit -> Atom(s)' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_atom_edit (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  gchar * act;
  int i, j;
  project * this_proj = get_project_by_id(view -> proj);
  j = (this_proj -> steps == 1) ? 1 : 0;
  for (i=0; i<5; i++)
  {
    act = g_strdup_printf ("ated-%d", i);
    append_opengl_item (view, menu, action_name[i], act, popm, 0, NULL, IMG_NONE, NULL, FALSE,
                        G_CALLBACK(action_window), & view -> colorp[i][0], FALSE, FALSE, FALSE, (i == 3) ? j : (this_proj -> natomes) ? j : 0);
    g_free (act);
  }
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_run_rebuild (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief Extract/Rebuild menu items callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_run_rebuild (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  tint * dat =(tint *)data;
  glwin * view = get_project_by_id(dat -> a) -> modelgl;
  gboolean doit = TRUE;
  GVariant * state = g_action_get_state (G_ACTION (action));
  const gchar * rebuild = g_action_get_name ((GAction *)action);
  int lgt = strlen (rebuild);
  gchar * name = g_strdup_printf ("%c%c", rebuild[lgt-2], rebuild[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, rebuild);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, (const gchar *)name, NULL);
    g_free (name);
    doit = FALSE;
  }
  if (doit)
  {
    turn_rebuild (NULL, NULL, data);
    g_action_change_state (G_ACTION (action), g_variant_new_boolean (! g_variant_get_boolean (state)));
    g_variant_unref (state);
  }
}

/*!
  \fn GMenu * extract_section (glwin * view, int popm)

  \brief create the 'Extract/Rebuild' menu items GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * extract_section (glwin * view, int popm)
{
  int i;
  project * this_proj = get_project_by_id(view -> proj);
  GMenu * menu = g_menu_new ();
  gchar * rtext[2] = {"Extract/Rebuild on Motion", "Extract/Rebuild on Copy"};
  for (i=0; i<2; i++)
  {
    append_opengl_item (view, menu, rtext[i], "aext", popm, i, NULL, IMG_STOCK, (gpointer)ECUT, FALSE, G_CALLBACK(to_run_rebuild), & view -> colorp[i][0],
                        TRUE, view -> rebuild[i][0], FALSE, (this_proj -> steps == 1) ? 1 : 0);
  }
  return menu;
}

/*!
  \fn GMenu * menu_edit (glwin * view, int popm)

  \brief create the 'Edit' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_edit (glwin * view, int popm)
{
  project * this_proj = get_project_by_id(view -> proj);
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, "Crystal Builder", "cbuilder", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(crystal_window), & view -> colorp[0][0], FALSE, FALSE, FALSE, TRUE);
  append_submenu (menu, "Cell", menu_cell_edit(view, popm, (this_proj -> cell.ltype && this_proj -> steps == 1) ? 1 : 0));
  append_submenu (menu, "Atom(s)", menu_atom_edit(view, popm));
  if (! popm) g_menu_append_section (menu, NULL, (GMenuModel*)extract_section(view, popm));
  return menu;
}
#endif
