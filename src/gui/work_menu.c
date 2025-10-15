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
* @file work_menu.c
* @short Workspace menu GTK3 version \n
         Callbacks for the workspace menu
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'work_menu.c'
*
* Contains:
*

 - The workspace menu GTK3 version
 - Callbacks for the workspace menu

*
* List of functions:

  G_MODULE_EXPORT void on_create_new_project (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void leaving_from_menu (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void compute_this_prop (GtkWidget * widg, gpointer data);

  GtkWidget * this_work_menu (int p, int c);
  GtkWidget * work_menu (int p, int c);

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"
#include "glview.h"

extern G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void set_mode (GtkWidget * widg, gpointer data);

/*!
  \fn G_MODULE_EXPORT void on_create_new_project (GtkWidget * widg, gpointer data)

  \brief create a new project

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_create_new_project (GtkWidget * widg, gpointer data)
{
  init_project (TRUE);
  add_project_to_workspace ();
  frag_update = mol_update = 0;
  apply_project (FALSE);
#ifdef GTK3
  // GTK3 Menu Action To Check
  gtk_check_menu_item_set_active ((GtkCheckMenuItem *)active_project -> modelgl -> ogl_mode[1], TRUE);
  set_mode (active_project -> modelgl -> ogl_mode[1], & active_project -> modelgl -> colorp[1][0]);
#else
  set_mode (NULL, & active_project -> modelgl -> colorp[1][0]);
#endif
  prep_calc_actions ();
}

/*!
  \fn G_MODULE_EXPORT void leaving_from_menu (GtkWidget * widg, gpointer data)

  \brief leaving atomes ?

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void leaving_from_menu (GtkWidget * widg, gpointer data)
{
#ifdef GTK4
  leaving_question (NULL, NULL);
#else
  leaving_question (NULL, NULL, NULL);
#endif
}

int calc_to_compute;

/*!
  \fn G_MODULE_EXPORT void compute_this_prop (GtkWidget * widg, gpointer data)

  \brief to compute the 'calc_to_compute'

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void compute_this_prop (GtkWidget * widg, gpointer data)
{
  activate_project (NULL, data);
  on_calc_activate (NULL, GINT_TO_POINTER(calc_to_compute));
}

#ifdef GTK3
/*!
  \fn GtkWidget * this_work_menu (int p, int c)

  \brief create the workspace menu GTK3 version with icons

  \param p project id, or -1
  \param c calculation id, or -1
*/
GtkWidget * this_work_menu (int p, int c)
{
  GtkWidget * menu;
  GtkWidget * port;
  GtkWidget * dmenu;
  gchar * imp_str[2] = {"Import", "Export"};
  menu = gtk_menu_new ();
  GtkAccelGroup * accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (MainWindow), accel_group);
  widget_set_sensitive (gtk3_menu_item (menu, "Workspace", IMG_FILE, (gpointer)PACKAGE_TD, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE), 0);
  add_menu_separator (menu);
  gtk3_menu_item (menu, "Open", IMG_STOCK, (gpointer)FOPEN, G_CALLBACK(on_open_save_activate), GINT_TO_POINTER(2), TRUE, GDK_KEY_w, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  gtk3_menu_item (menu, "Save", IMG_STOCK, (gpointer)FSAVE, G_CALLBACK(on_open_save_activate), GINT_TO_POINTER(3), FALSE, 0, 0, FALSE, FALSE, FALSE);
  gtk3_menu_item (menu, "Save As", IMG_STOCK, (gpointer)FSAVEAS, G_CALLBACK(on_save_as_activate), GINT_TO_POINTER(3), TRUE, GDK_KEY_s, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  gtk3_menu_item (menu, "Close", IMG_STOCK, (gpointer)FCLOSE, G_CALLBACK(on_close_workspace), GINT_TO_POINTER(1), TRUE, GDK_KEY_c, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  add_menu_separator (menu);
  if (p == -1)
  {
    widget_set_sensitive (gtk3_menu_item (menu, "Project(s)", IMG_FILE, (gpointer)PACKAGE_TD, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE), FALSE);
    add_menu_separator (menu);
    gtk3_menu_item (menu, "New", IMG_STOCK, (gpointer)FNEW, G_CALLBACK(on_create_new_project), NULL, TRUE, GDK_KEY_n, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Open", IMG_STOCK, (gpointer)FSAVE, G_CALLBACK(on_open_save_activate), GINT_TO_POINTER(0), TRUE, GDK_KEY_o, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Save", IMG_STOCK, (gpointer)FSAVE, G_CALLBACK(on_open_save_activate), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Save As", IMG_STOCK, (gpointer)FSAVEAS, G_CALLBACK(on_save_as_activate), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Close", IMG_STOCK, (gpointer)FCLOSE, G_CALLBACK(on_close_activate), GINT_TO_POINTER(activep), FALSE, 0, 0, FALSE, FALSE, FALSE);
    port = gtk3_menu_item (menu, imp_str[1], IMG_FILE, (gpointer)PACKAGE_CON, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE);
    dmenu = gtk_menu_new ();
    gtk3_menu_item (dmenu, "ISAACS Project File '*.ipf'", IMG_FILE, (gpointer)PACKAGE_MOL, G_CALLBACK(on_isaacs_port), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (dmenu, "Atomic Coordinates", IMG_FILE, (gpointer)PACKAGE_CON, G_CALLBACK(on_coord_port), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk_menu_item_set_submenu ((GtkMenuItem *)port, dmenu);
  }
  else
  {
    gchar * pname = g_strdup_printf ("<b>%s</b>", get_project_by_id(p) -> name);
    widget_set_sensitive (gtk3_menu_item (menu, pname, IMG_FILE, (gpointer)PACKAGE_TD, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE), 0);
    g_free (pname);
    add_menu_separator (menu);
    if (p != activep)
    {
      gtk3_menu_item (menu, "Make Active", IMG_STOCK, (gpointer)YES, G_CALLBACK(activate_project), GINT_TO_POINTER(p), FALSE, 0, 0, FALSE, FALSE, FALSE);
    }
    if (c > -1)
    {
      gchar * str = g_strdup_printf ("Analyze: %s", graph_name[c]);
#ifdef NEW_ANA
      widget_set_sensitive (gtk3_menu_item (menu, str, IMG_FILE, (gpointer)graph_img[c], G_CALLBACK(compute_this_prop), GINT_TO_POINTER(p), FALSE, 0, 0, FALSE, FALSE, FALSE), get_project_by_id(p) -> analysis[c].avail_ok);
#else
      widget_set_sensitive (gtk3_menu_item (menu, str, IMG_FILE, (gpointer)graph_img[c], G_CALLBACK(compute_this_prop), GINT_TO_POINTER(p), FALSE, 0, 0, FALSE, FALSE, FALSE), get_project_by_id(p) -> runok[c]);
#endif
      g_free (str);
    }
    gtk3_menu_item (menu, "Edit Name", IMG_STOCK, (gpointer)EDITA, G_CALLBACK(change_project_name), GINT_TO_POINTER(p), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Save", IMG_STOCK, (gpointer)FSAVE, G_CALLBACK(on_open_save_activate), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Save As", IMG_STOCK, (gpointer)FSAVEAS, G_CALLBACK(on_save_as_activate), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (menu, "Close", IMG_STOCK, (gpointer)FCLOSE, G_CALLBACK(on_close_activate), GINT_TO_POINTER(p), FALSE, 0, 0, FALSE, FALSE, FALSE);
    port = gtk3_menu_item (menu, imp_str[1], IMG_FILE, (gpointer)PACKAGE_CON, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE);
    dmenu = gtk_menu_new ();
    gtk3_menu_item (dmenu, "ISAACS Project File '*.ipf'", IMG_FILE, (gpointer)PACKAGE_MOL, G_CALLBACK(on_isaacs_port), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk3_menu_item (dmenu, "Atomic Coordinates", IMG_FILE, (gpointer)PACKAGE_CON, G_CALLBACK(on_coord_port), GINT_TO_POINTER(1), FALSE, 0, 0, FALSE, FALSE, FALSE);
    gtk_menu_item_set_submenu ((GtkMenuItem *)port, dmenu);
  }
  add_menu_separator (menu);
  port = gtk3_menu_item (menu, imp_str[0], IMG_FILE, (gpointer)PACKAGE_IMP, NULL, NULL, FALSE, 0, 0, FALSE, FALSE, FALSE);
  dmenu = gtk_menu_new ();
  gtk_menu_item_set_submenu ((GtkMenuItem *)port, dmenu);
  gtk3_menu_item (dmenu, "ISAACS Project File '*.ipf'", IMG_FILE, (gpointer)PACKAGE_MOL, G_CALLBACK(on_isaacs_port), GINT_TO_POINTER(0), FALSE, 0, 0, FALSE, FALSE, FALSE);
  gtk3_menu_item (dmenu, "Atomic Coordinates", IMG_FILE, (gpointer)PACKAGE_IMP, G_CALLBACK(on_coord_port), GINT_TO_POINTER(0), FALSE, 0, 0, FALSE, FALSE, FALSE);
  add_menu_separator (menu);
  gtk3_menu_item (menu, "Quit", IMG_STOCK, (gpointer)FEXIT, G_CALLBACK(leaving_from_menu), NULL, TRUE, GDK_KEY_q, GDK_CONTROL_MASK, FALSE, FALSE, FALSE);
  show_the_widgets (menu);
  return menu;
}
#endif

extern void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data);
extern GMenu * create_workspace_menu (gchar * act, int pop_up, int proj, int calc);
extern GSimpleAction * pop_act[7];

/*!
  \fn GtkWidget * work_menu (int p, int c)

  \brief create the workspace popup menu

  \param p project id, or -1
  \param c calculation id, or -1
*/
GtkWidget * work_menu (int p, int c)
{
  GtkWidget * menu;

  GSimpleActionGroup * action_popup = g_simple_action_group_new ();
  GSimpleAction * pop_act[17];
  pop_act[0]  = g_simple_action_new ("workspace.open", NULL);
  pop_act[1]  = g_simple_action_new ("workspace.save", NULL);
  pop_act[2]  = g_simple_action_new ("workspace.save-as", NULL);
  pop_act[3]  = g_simple_action_new ("workspace.close", NULL);
  pop_act[4]  = g_simple_action_new ("project.active", NULL);
  pop_act[5]  = g_simple_action_new ("project.compute", NULL);
  pop_act[6]  = g_simple_action_new ("project.edit", NULL);
  pop_act[7]  = g_simple_action_new ("project.new", NULL);
  pop_act[8]  = g_simple_action_new ("project.open", NULL);
  pop_act[9]  = g_simple_action_new ("project.save", NULL);
  pop_act[10] = g_simple_action_new ("project.save-as", NULL);
  pop_act[11] = g_simple_action_new ("project.close", NULL);
  pop_act[12] = g_simple_action_new ("export.isaacs", NULL);
  pop_act[13] = g_simple_action_new ("export.coordinates", NULL);
  pop_act[14] = g_simple_action_new ("import.isaacs", NULL);
  pop_act[15] = g_simple_action_new ("import.coordinates", NULL);
  pop_act[16] = g_simple_action_new ("program.quit", NULL);

  int i;
  for (i=0; i<17; i++) g_action_map_add_action (G_ACTION_MAP(action_popup), G_ACTION(pop_act[i]));
  g_signal_connect (pop_act[0], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(2));
  g_signal_connect (pop_act[1], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(3));
  g_signal_connect (pop_act[2], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(3));
  g_signal_connect (pop_act[3], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_act[4], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(p));
  g_signal_connect (pop_act[5], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(p));
  g_signal_connect (pop_act[6], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(p));
  g_signal_connect (pop_act[7], "activate", G_CALLBACK(atomes_menu_bar_action), NULL);
  g_signal_connect (pop_act[8], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(0));
  g_signal_connect (pop_act[9], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_act[10], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_act[11], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(p));
  g_signal_connect (pop_act[12], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_act[13], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_act[14], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(0));
  g_signal_connect (pop_act[15], "activate", G_CALLBACK(atomes_menu_bar_action), GINT_TO_POINTER(0));
  g_signal_connect (pop_act[16], "activate", G_CALLBACK(atomes_menu_bar_action), NULL);

  calc_to_compute = (c < AN) ? c : c - 1;
#ifdef GTK3
  menu = this_work_menu (p, c);
  // GMenu * popup = create_workspace_menu ("pop", 1, p, c);
  // gtk_menu_new_from_model (G_MENU_MODEL(popup));
#else
  GMenu * popup = create_workspace_menu ("pop", 1, p, c);
  menu = gtk_popover_menu_new_from_model_full (G_MENU_MODEL(popup), GTK_POPOVER_MENU_NESTED);
  i = (nprojects) ? ((p > -1) ? 250 : 95) : 0;
  gtk_widget_set_size_request (menu, -1, 310 + i);
/* I need to use the ' gtk_popover_menu_new_from_model_full' command here,
   the menu created using the 'gtk_popover_menu_new_from_model' does not behave properly,
   that might be a bug, some items of the menu having the same name,
   It is not possible to navigate the menu properly and some elements remains inaccessible,
   this was solved with this  '_full' command.
    menu = gtk_popover_menu_new_from_model (G_MENU_MODEL(popup)); */
#endif
  gtk_widget_insert_action_group (menu, "pop", G_ACTION_GROUP(action_popup));
  return menu;
}
