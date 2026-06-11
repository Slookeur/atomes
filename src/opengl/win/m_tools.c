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
* @file m_tools.c
* @short Functions to create the 'Tools' submenu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'm_tools.c'
*
* Contains:
*

 - The functions to create the 'Tools' submenu

*
* List of functions:

  void set_motion_sensitive (glwin * view, int status);
  void invert_visible (project * this_proj);

  G_MODULE_EXPORT void set_selection_mode (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void invert_this (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void to_window_measures (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_window_volumes (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void change_mouse_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void change_sel_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_create_field (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void to_invert_this (GSimpleAction * action, GVariant * parameter, gpointer data);

  GtkWidget * menu_tools (glwin * view, int id);

  GMenu * measure_section (glwin * view, int popm);
  GMenu * volume_section (glwin * view, int popm);
  GMenu * edit_section (glwin * view, int popm);
  GMenu * mouse_mode_menu (glwin * view, int popm);
  GMenu * selection_mode_menu (glwin * view, int popm);
  GMenu * modes_section (glwin * view, int popm);
  GMenu * md_menu (glwin * view, int popm);
  GMenu * inv_menu (glwin * view, int popm);
  GMenu * add_section_item_with_menu (glwin * view, gchar * item_name, GMenu * men);
  GMenu * menu_tools (glwin * view, int popm);

*/

#include "cell_edit.h"
#include "atom_edit.h"
#include "submenus.h"

extern G_MODULE_EXPORT void window_measures (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void window_volumes (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void create_field (GtkWidget * widg, gpointer data);
extern gboolean spin (gpointer data);
extern void check_hidden_visible (project * this_proj);

char * input_types[NINPUTS] = {i18n("Classical: DL-POLY"),
                               i18n("Classical: LAMMPS"),
                               i18n("First-Principles: CPMD"),
                               i18n("First-Principles: CP2K"),
                               i18n("QM-MM: CPMD - [Soon]"),
                               i18n("QM-MM: CP2K - [Soon]")};

gchar * modes[3]={i18n("Analysis "), i18n("Edition "), i18n("Input(s)")};

gchar * smodes[NSELECTION]={i18n("Atom/Bond"), 
                            i18n("Coordination Sphere"), 
                            i18n("Fragment"), 
                            i18n("Molecule"), 
                            i18n("Single Fragment"), 
                            i18n("Single Molecule"), 
                            i18n("Measures (Edition Mode Only)")};

gchar * invl[2]={i18n("Selection"), i18n("Visible/Hidden")};

/*!
  \fn void set_motion_sensitive (glwin * view, int status)

  \brief change motion parameters following a change in the mouse mode

  \param view the target glwin
  \param status initialize or restore spin
*/
void set_motion_sensitive (glwin * view, int status)
{
  int i;
#ifdef GTK3
  // GTK3 Menu Action To Check
  for (i=0; i<2; i++) widget_set_sensitive (view -> ogl_anim[i], status);
#endif
  if (view -> player != NULL) widget_set_sensitive (view -> player -> win, status);
  if (view -> spiner != NULL) widget_set_sensitive (view -> spiner -> win, status);
  if (view -> rec != NULL) widget_set_sensitive (view -> rec -> win, status);
  if (! status)
  {
    if (view -> spin[0] || view -> spin[1])
    {
      for (i=0; i<2; i++)
      {
        view -> spin[i+2] = view -> spin[i];
        view -> spin_speed[i+2] = view -> spin_speed[i];
        view -> spin[i] = FALSE;
      }
    }
  }
  else if (view -> spin[2] || view -> spin[3])
  {
    for (i=0; i<2; i++)
    {
      view -> spin[i] = view -> spin[i+2];
      view -> spin_speed[i] = view -> spin_speed[i+2];
      g_timeout_add (REFRESH, (GSourceFunc) spin, & view -> colorp[0][i]);
      view -> spin[i+2] = FALSE;
      view -> spin_speed[i+2] = 0;
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_selection_mode (GtkWidget * widg, gpointer data)

  \brief set selection mode callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_selection_mode (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i = this_proj -> modelgl -> selection_mode;
  int j = the_data -> b;
#ifdef GTK4
  if (i != j)
  {
#else
  // GTK3 Menu Action To Check
  if (i != j && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    this_proj -> modelgl -> selection_mode = NONE;
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_smode[i], FALSE);
    if (widg != this_proj -> modelgl -> ogl_smode[j])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_smode[j], TRUE);
    }
#endif
    this_proj -> modelgl -> selection_mode = j;
  }
#ifdef GTK3
  // GTK3 Menu Action To Check
  else if (i == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_smode[j], TRUE);
  }
#endif
}

/*!
  \fn G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data)

  \brief set mouse mode callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  int i = this_proj -> modelgl -> mode;
  int j = the_data -> b;

  if (! (j == EDITION && is_atom_win_active(this_proj -> modelgl)))
  {
#ifdef GTK4
  if (i != j)
#else
  if (i != j && gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
#endif
  {
    this_proj -> modelgl -> mode = NONE;
#ifdef GTK3
    // GTK3 Menu Action To Check
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_mode[i], FALSE);
    if (widg != this_proj -> modelgl -> ogl_mode[j])
    {
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_mode[j], TRUE);
    }
#endif
    this_proj -> modelgl -> mode = j;
    if (this_proj -> modelgl -> mode == EDITION)
    {
      for (i=1; i<3; i++) init_coordinates (this_proj, i, FALSE, TRUE);
      set_motion_sensitive (this_proj -> modelgl, 0);
    }
    else
    {
      for (i=1; i<3; i++)
      {
        if (this_proj -> modelgl -> saved_coord[i] != NULL)
        {
          g_free (this_proj -> modelgl -> saved_coord[i]);
          this_proj -> modelgl -> saved_coord[i] = NULL;
        }
      }
      set_motion_sensitive (this_proj -> modelgl, 1);
      if (this_proj -> modelgl -> selection_mode == NSELECTION-1)
      {
#ifdef GTK4
        set_selection_mode (NULL, & this_proj -> modelgl -> colorp[0][0]);
#else
        // GTK3 Menu Action To Check
        gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_smode[0], TRUE);
        set_selection_mode (this_proj -> modelgl -> ogl_smode[0], & this_proj -> modelgl -> colorp[0][0]);
#endif
      }
    }
    i = activep;
    active_project_changed (this_proj -> id);
    active_project_changed (i);
#ifdef GTK3
    // GTK3 Menu Action To Check
    widget_set_sensitive (this_proj -> modelgl -> ogl_smode[NSELECTION-1], (this_proj -> modelgl -> mode == EDITION) ? 1 : 0);
#endif
    i = (this_proj -> modelgl -> mode == EDITION) ? EDITION : ANALYZE;
    gchar * str = g_strdup_printf (_("%s - 3D view - [%s mode]"), prepare_for_title(this_proj -> name), _(mode_name[i]));
    gtk_window_set_title (GTK_WINDOW (this_proj -> modelgl -> win), str);
    g_free (str);
    this_proj -> modelgl -> create_shaders[MEASU] = TRUE;
    update (this_proj -> modelgl);

  }
#ifdef GTK3
  // GTK3 Menu Action To Check
  else if (i == j && ! gtk_check_menu_item_get_active ((GtkCheckMenuItem *)widg))
  {
    gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_mode[j], TRUE);
  }
#endif
  }
}

/*!
  \fn void invert_visible (project * this_proj)

  \brief invert visible atom(s)

  \param this_proj the target project
*/
void invert_visible (project * this_proj)
{
  int i, j, k;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> natomes; j++)
    {
      for (k=0; k<2; k++) this_proj -> atoms[i][j].show[k] = ! this_proj -> atoms[i][j].show[k];
    }
  }
  check_hidden_visible (this_proj);
  init_default_shaders (this_proj -> modelgl);
}

/*!
  \fn G_MODULE_EXPORT void invert_this (GtkWidget * widg, gpointer data)

  \brief invert selection or visible callback

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void invert_this (GtkWidget * widg, gpointer data)
{
  tint * the_data = (tint *)data;
  project * this_proj = get_project_by_id(the_data -> a);
  switch (the_data -> b)
  {
    case 0:
      invert_selection (this_proj);
      init_default_shaders (this_proj -> modelgl);
      break;
    case 1:
      invert_visible (this_proj);
      break;
  }
}

#ifdef GTK3
extern G_MODULE_EXPORT void window_volumes (GtkWidget * widg, gpointer data);
/*!
  \fn GtkWidget * menu_tools (glwin * view, int id)

  \brief create the 'Tools' submenu - GTK3

  \param view the target glwin
  \param id main app (0) or popup (1)
*/
GtkWidget * menu_tools (glwin * view, int id)
{
  int i;
  GtkWidget * menut = gtk_menu_new ();
  gtk3_menu_item (menut, _("Measures"), IMG_NONE, NULL, G_CALLBACK(window_measures), (gpointer)view, TRUE, GDK_KEY_m, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  GtkWidget * widg = gtk3_menu_item (menut, _("Volumes"), IMG_NONE, NULL, G_CALLBACK(window_volumes), (gpointer)view, FALSE, 0, 0, FALSE, FALSE, FALSE);
  widget_set_sensitive (widg, (get_project_by_id(view -> proj) -> steps > 1) ? 0 : 1);
  add_menu_separator (menut);
  gtk_menu_shell_append ((GtkMenuShell *)menut, menu_item_new_with_submenu(_("Edit"), TRUE, menu_edit(view, id)));
  add_menu_separator (menut);
  widg = create_menu_item (FALSE, _("Mouse Mode"));
  gtk_menu_shell_append ((GtkMenuShell *)menut, widg);
  GtkWidget * menum = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menum);
  guint accel[3]={GDK_KEY_a, GDK_KEY_e, GDK_KEY_i};
  guint acces[4]={GDK_KEY_A, GDK_KEY_C, GDK_KEY_F, GDK_KEY_M};
  if (id == 0)
  {
    for (i=0; i<2; i++)
    {
      view -> ogl_mode[i] = gtk3_menu_item (menum, _(modes[i]), IMG_NONE, NULL, G_CALLBACK(set_mode), & view -> colorp[i][0],
                                            TRUE, accel[i], GDK_MOD1_MASK, TRUE, TRUE, (i == view -> mode) ? TRUE : FALSE);
      widget_set_sensitive (view -> ogl_mode[i], (get_project_by_id(view -> proj) -> steps > 1) ? 0 : 1);
    }
  }
  else
  {
    for (i=0; i<2; i++)
    {
      widg = gtk3_menu_item (menum, _(modes[i]), IMG_NONE, NULL, G_CALLBACK(set_mode), & view -> colorp[i][0],
                             TRUE, accel[i], GDK_MOD1_MASK, TRUE, TRUE, (i == view -> mode) ? TRUE : FALSE);
      widget_set_sensitive (widg, (get_project_by_id(view -> proj) -> steps > 1) ? 0 : 1);
    }
  }
  /*widg = create_menu_item (TRUE, _(modes[2]));
  gtk_menu_shell_append ((GtkMenuShell *)menum, widg);
  GtkWidget * menui = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menui);
  widget_set_sensitive (widg, (get_project_by_id(view -> proj) -> steps > 1) ? 0 : 1);
  if (id == 0)
  {
    for (i=0; i<NINPUTS; i++)
    {
      view -> ogl_mode[i+2] = gtk3_menu_item (menum, _(modes[i]), IMG_NONE, NULL, G_CALLBACK(set_mode), & view -> colorp[2+i][0],
                                              FALSE, 0, 0, TRUE, TRUE, (i+2 == view -> mode) ? TRUE : FALSE);
      widget_set_sensitive (view -> ogl_mode[i+2], 0);
    }
  }
  else
  {
    for (i=0; i<NINPUTS; i++)
    {
      widg = gtk3_menu_item (menum, _(modes[i]), IMG_NONE, NULL, G_CALLBACK(set_mode), & view -> colorp[2+i][0],
                             FALSE, 0, 0, TRUE, TRUE, (i+2 == view -> mode) ? TRUE : FALSE);
      widget_set_sensitive (widg, 0);
    }
  }*/
  widg = create_menu_item (FALSE, _("Selection Mode"));
  gtk_menu_shell_append ((GtkMenuShell *)menut, widg);
  GtkWidget * menusm = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menusm);

  if (id == 0)
  {
    for (i=0; i<NSELECTION; i++)
    {
      if (i < 4)
      {
        view -> ogl_smode[i] = gtk3_menu_item (menusm, _(smodes[i]), IMG_NONE, NULL, G_CALLBACK(set_selection_mode), & view -> colorp[i][0],
                                               TRUE, acces[i], GDK_SHIFT_MASK, TRUE, TRUE, (i == view -> selection_mode) ? TRUE : FALSE);
      }
      else
      {
        view -> ogl_smode[i] = gtk3_menu_item (menusm, _(smodes[i]), IMG_NONE, NULL, G_CALLBACK(set_selection_mode), & view -> colorp[i][0],
                                               FALSE, 0, 0, TRUE, TRUE, (i == view -> selection_mode) ? TRUE : FALSE);
      }
      if (i == NSELECTION-1) widget_set_sensitive (view -> ogl_smode[i], (view -> mode == EDITION) ? 1 : 0);
    }
  }
  else
  {
    for (i=0; i<NSELECTION; i++)
    {
      if (i < 4)
      {
        widg = gtk3_menu_item (menusm, _(smodes[i]), IMG_NONE, NULL, G_CALLBACK(set_selection_mode), & view -> colorp[i][0],
                               TRUE, acces[i], GDK_SHIFT_MASK, TRUE, TRUE, (i == view -> selection_mode) ? TRUE : FALSE);
      }
      else
      {
        widg = gtk3_menu_item (menusm, _(smodes[i]), IMG_NONE, NULL, G_CALLBACK(set_selection_mode), & view -> colorp[i][0],
                               FALSE, 0, 0, TRUE, TRUE, (i == view -> selection_mode) ? TRUE : FALSE);
      }
      if (i == NSELECTION-1) widget_set_sensitive (widg, (view -> mode == EDITION) ? 1 : 0);
    }
  }

  add_menu_separator (menut);

  GtkWidget * menuf = NULL;
  widg = create_menu_item (FALSE, _("Molecular Dynamics"));
  widget_set_sensitive (widg, get_project_by_id(view -> proj) -> nspec);
  gtk_menu_shell_append ((GtkMenuShell *)menut, widg);
  menuf = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menuf);
  if (id == 0)
  {
    for (i=0; i<NINPUTS; i++)
    {
      view -> ogl_mode[i+2+NINPUTS] = create_menu_item (TRUE, _(input_types[i]));
      gtk_menu_shell_append ((GtkMenuShell *)menuf, view -> ogl_mode[i+2+NINPUTS]);
      g_signal_connect (G_OBJECT (view -> ogl_mode[i+2+NINPUTS]), "activate", G_CALLBACK(create_field), & view -> colorp[i][0]);
      if (i > 3) widget_set_sensitive (view -> ogl_mode[i+2+NINPUTS], 0);
    }
    set_advanced_bonding_menus (view);
  }
  else
  {
    for (i=0; i<NINPUTS; i++)
    {
      widg = create_menu_item (TRUE, _(input_types[i]));
      gtk_menu_shell_append ((GtkMenuShell *)menuf, widg);
      g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(create_field), & view -> colorp[i][0]);
      // if (i < 2 || i > 3) widget_set_sensitive (widg, view -> adv_bonding[1]);
      if (i < 2) widget_set_sensitive (widg, view -> adv_bonding[1]);
      if (i > 3) widget_set_sensitive (widg, 0);
    }
  }

  add_menu_separator (menut);
  GtkWidget * menuin = NULL;
  widg = create_menu_item (FALSE, _("Invert"));
  gtk_menu_shell_append ((GtkMenuShell *)menut, widg);
  menuin = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)widg, menuin);
  for (i=0; i<2; i++)
  {
    widg = create_menu_item (FALSE, _(invl[i]));
    gtk_menu_shell_append ((GtkMenuShell *)menuin, widg);
    g_signal_connect (G_OBJECT (widg), "activate", G_CALLBACK(invert_this), & view -> colorp[i][0]);
  }
  return menut;
}
#else
/*!
  \fn G_MODULE_EXPORT void to_window_measures (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief open the measurement window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_window_measures (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  window_measures (NULL, data);
}

/*!
  \fn GMenu * measure_section (glwin * view, int popm)

  \brief create the 'Tools -> Measures' menu item GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * measure_section (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, _("Measures"), "measures", popm, popm, "<CTRL>M", IMG_NONE, NULL, FALSE, G_CALLBACK(to_window_measures), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_window_volumes (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief open the volumes window callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_window_volumes (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  window_volumes (NULL, data);
}

/*!
  \fn GMenu * volume_section (glwin * view, int popm)

  \brief create the 'Tools -> Volumes' menu item GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * volume_section (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_opengl_item (view, menu, _("Volumes"), "volumes", popm, popm, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_window_volumes), (gpointer)view, FALSE, FALSE, FALSE, TRUE);
  return menu;
}

/*!
  \fn GMenu * edit_section (glwin * view, int popm)

  \brief create the 'Tools -> Edit' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * edit_section (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, _("Edit"), menu_edit(view, popm));
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void change_mouse_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief change mouse radio menu item callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_mouse_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * mode = g_variant_get_string (parameter, NULL);
  int lgt = strlen (mode);
  gchar * name = g_strdup_printf ("%c%c", mode[lgt-2], mode[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, mode);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-mouse-mode", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    gchar * mode_name = NULL;
    int i;
    for (i=0; i<2; i++)
    {
      mode_name = g_strdup_printf ("set-mouse-mode.%d.0", i);
      if (g_strcmp0(mode, (const gchar *)mode_name) == 0)
      {
        set_mode (NULL, & view -> colorp[i][0]);
        g_free (mode_name);
        mode_name = NULL;
        break;
      }
      g_free (mode_name);
      mode_name = NULL;
    }
    // Update the menu bar is required to activate / deactivate the selection mode option
    update_menu_bar (view);
  }
}

/*!
  \fn GMenu * mouse_mode_menu (glwin * view, int popm)

  \brief create the 'Tools -> Mouse Mode' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * mouse_mode_menu (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  gchar * accel[3] = {"<ALT>A", "<ALT>E", "<ALT>I"};
  int i, j;
  j = (get_project_by_id(view -> proj) -> steps > 1) ? 0 : 1;
  for (i=0; i<2; i++)
  {
    append_opengl_item (view, menu, _(modes[i]), "mouse-mode", popm, i, accel[i], IMG_NONE, NULL, FALSE,
                        G_CALLBACK(change_mouse_mode_radio), (gpointer)view, FALSE, (i == view -> mode) ? TRUE : FALSE, TRUE, j);
  }
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void change_sel_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief change selection mode callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void change_sel_mode_radio (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  glwin * view = (glwin *)data;
  const gchar * mode = g_variant_get_string (parameter, NULL);
  int lgt = strlen (mode);
  gchar * name = g_strdup_printf ("%c%c", mode[lgt-2], mode[lgt-1]);
  if (g_strcmp0(name, ".1") == 0)
  {
    g_free (name);
    name = g_strdup_printf ("%.*s.0", lgt-2, mode);
    g_action_group_activate_action ((GActionGroup *)view -> action_group, "set-sel-mode", g_variant_new_string((const gchar *)name));
    g_free (name);
  }
  else
  {
    gchar * mode_name = NULL;
    int i;
    for (i=0; i<NSELECTION; i++)
    {
      mode_name = g_strdup_printf ("set-sel-mode.%d.0", i);
      if (g_strcmp0(mode, (const gchar *)mode_name) == 0)
      {
        set_selection_mode (NULL, & view -> colorp[i][0]);
        g_free (mode_name);
        mode_name = NULL;
        break;
      }
      g_free (mode_name);
      mode_name = NULL;
    }
    g_action_change_state (G_ACTION (action), parameter);
  }
}

/*!
  \fn GMenu * selection_mode_menu (glwin * view, int popm)

  \brief create the 'Tools -> Selection Mode' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * selection_mode_menu (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  gchar * acces[4]={"A", "C", "F", "M"};
  gchar * str;
  int i, j;
  for (i=0; i<NSELECTION; i++)
  {
    j = (i == NSELECTION-1) ? (view -> mode == EDITION) ? 1 : 0 : TRUE;
    if (i < 4)
    {
      str = g_strdup_printf ("<SHIFT>%s", acces[i]);
      append_opengl_item (view, menu, _(smodes[i]), "sel-mode", popm, i, str, IMG_NONE, NULL, FALSE, G_CALLBACK(change_sel_mode_radio), (gpointer)view,
                          FALSE, (i == view -> selection_mode) ? TRUE : FALSE, TRUE, j);
      g_free (str);
    }
    else
    {
      append_opengl_item (view, menu, _(smodes[i]), "sel-mode", popm, i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(change_sel_mode_radio), (gpointer)view,
                          FALSE, (i == view -> selection_mode) ? TRUE : FALSE, TRUE, j);
    }
  }
  return menu;
}

/*!
  \fn GMenu * modes_section (glwin * view, int popm)

  \brief create the 'Tools -> * Modes' submenus GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * modes_section (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, _("Mouse Mode"), mouse_mode_menu(view, popm));
  append_submenu (menu, _("Selection Mode"), selection_mode_menu(view, popm));
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_create_field (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief run MD input assistant callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_create_field (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  create_field (NULL, data);
}

/*!
  \fn GMenu * md_menu (glwin * view, int popm)

  \brief create the 'Molecular Dynamics' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * md_menu (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  int i;
  for (i=0; i<NINPUTS; i++)
  {
    append_opengl_item (view, menu, _(input_types[i]), "md", popm, i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_create_field), & view -> colorp[i][0],
                        FALSE, FALSE, FALSE, (i < 2) ? view -> adv_bonding[1] : (i > 3) ? FALSE : TRUE);
  }
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void to_invert_this (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief invert this callback GTK4

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_invert_this (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  invert_this (NULL, data);
}

/*!
  \fn GMenu * inv_menu (glwin * view, int popm)

  \brief create the 'Tools -> Invert' submenu GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * inv_menu (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  int i;
  for (i=0; i<2; i++)
  {
    append_opengl_item (view, menu, _(invl[i]), "inv", popm, i, NULL, IMG_NONE, NULL, FALSE, G_CALLBACK(to_invert_this), & view -> colorp[i][0], FALSE, FALSE, FALSE, TRUE);
  }
  return menu;
}

/*!
  \fn GMenu * add_section_item_with_menu (glwin * view, gchar * item_name, GMenu * men)

  \brief append a new menu item with a new submenu

  \param view the target glwin
  \param item_name the new menu item label
  \param men the menu item new submenu
*/
GMenu * add_section_item_with_menu (glwin * view, gchar * item_name, GMenu * men)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, item_name, men);
  return menu;
}

/*!
  \fn GMenu * menu_tools (glwin * view, int popm)

  \brief create the 'Tools' submenu - GTK4

  \param view the target glwin
  \param popm main app (0) or popup (1)
*/
GMenu * menu_tools (glwin * view, int popm)
{
  GMenu * menu = g_menu_new ();
  g_menu_append_section (menu, NULL, (GMenuModel*)measure_section(view, popm));
  g_menu_append_section (menu, NULL, (GMenuModel*)volume_section(view, popm));
  g_menu_append_section (menu, NULL, (GMenuModel*)edit_section(view, popm));
  g_menu_append_section (menu, NULL, (GMenuModel*)modes_section(view, popm));
  g_menu_append_section (menu, NULL, (GMenuModel*)add_section_item_with_menu(view, _("Molecular Dynamics"), md_menu(view, popm)));
  g_menu_append_section (menu, NULL, (GMenuModel*)add_section_item_with_menu(view, _("Invert"), inv_menu(view, popm)));
  return menu;
}
#endif
