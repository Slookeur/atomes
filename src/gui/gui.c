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
* @file gui.c
* @short GUI of the main window \n
         Menu elements of the workspace menu \n
         Creation of menu items, actions, used in the entire atomes software
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'gui.c'
*
* Contains:
*

 - The GUI of the main window
 - The menu elements of the workspace menu
 - Creation of menu items, actions, of menu items, actions, used in the entire atomes software

*
* List of functions:

  G_MODULE_EXPORT gboolean pop_menu (GtkWidget * widget, GdkEventButton * event, gpointer data);

  void clean_view ();
  void view_buffer (GtkTextBuffer * buffer);
  void atomes_key_pressed (guint keyval, GdkModifierType state);
  void add_action (GSimpleAction * action);
  void remove_action (gchar * action_name);
  void remove_edition_actions ();
  void remove_edition_and_analyze_actions ();
  void widget_add_action (GSimpleActionGroup * action_group, const gchar * act, GCallback handler, gpointer data,
                          gboolean check, gboolean status, gboolean radio, const gchar * stat);
  void append_submenu (GMenu * menu, const gchar * label, GMenu * submenu);
  void append_menu_item (GMenu * menu, const gchar * label, const gchar * action, const gchar * accel,
                         const gchar * custom, int format, const gchar * icon,
                         gboolean check, gboolean status, gboolean radio, const gchar * rstatus);

  G_MODULE_EXPORT void show_periodic_table (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void atomes_popup_menu (GtkGesture * gesture, int n_press, double x, double y, gpointer data);

  GtkWidget * create_main_window (GApplication * atomes);

  GIcon * get_gicon_from_data (int format, const gchar * icon);

  GMenuItem * create_gmenu_item (const gchar * label, const gchar * action, const gchar * accel,
                                 const gchar * custom, int format, const gchar * icon,
                                 gboolean check, gboolean status, gboolean radio, const gchar * rstatus);

  GMenu * workspace_section (gchar * act, int pop);
  GMenu * port_section (gchar * act, int pop, int port);
  GMenu * project_section (gchar * act, int pop_up, int proj, int calc);
  GMenu * import_section (gchar * act);
  GMenu * quit_section (gchar * act);
  GMenu * workspace_title ();
  GMenu * project_title (int pop_up, int proj);
  GMenu * create_workspace_menu (gchar * act, int pop_up, int proj, int calc);
  GMenu * create_edit_menu ();
  GMenu * tool_box_section ();
  GMenu * create_analyze_menu ();
  GMenu * create_help_menu ();
  GMenu * atomes_menu_bar ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "workspace.h"

extern int objects[3];
extern int * object_was_selected[3];
extern int ** tmp_object_id[3];
extern GtkWidget * curvetbox ();
extern GtkWidget * work_menu (int p, int c);

extern G_MODULE_EXPORT void compute_this_prop (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_create_new_project (GtkWidget * widg, gpointer data);
extern int get_atom_id_from_periodic_table (atom_search * asearch);
extern G_MODULE_EXPORT void leaving_from_menu (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_edit_activate (GtkWidget * widg, gpointer data);
extern G_MODULE_EXPORT void on_calc_activate (GtkWidget * widg, gpointer data);
extern void create_user_preferences_dialog ();

#ifdef GTK3
GtkWidget * MainEvent;
#endif

gchar * dots[NDOTS];
gchar * calc_img[NCALCS-2];
gchar * graph_img[NGRAPHS];

atomes_action edition_acts[] = {{"edit.chemistry",   GINT_TO_POINTER(0)},
                                {"edit.periodicity", GINT_TO_POINTER(1)},
                                {"edit.cutoffs",     GINT_TO_POINTER(2)}};

atomes_action analyze_acts[] = {{"analyze.gr",     GINT_TO_POINTER(0)},
                                {"analyze.sq",     GINT_TO_POINTER(1)},
                                {"analyze.sk",     GINT_TO_POINTER(2)},
                                {"analyze.gk",     GINT_TO_POINTER(3)},
                                {"analyze.bonds",  GINT_TO_POINTER(4)},
                                {"analyze.rings",  GINT_TO_POINTER(5)},
                                {"analyze.chains", GINT_TO_POINTER(6)},
                                {"analyze.sp",     GINT_TO_POINTER(7)},
                                {"analyze.msd",    GINT_TO_POINTER(8)}};

char * calc_name[NCALCS-2] = {"g(r)/G(r)",
                              "S(q) from FFT[g(r)]",
                              "S(q) from Debye equation",
                              "g(r)/G(r) from FFT[S(q)]",
                              "Bonds and angles",
                              "Ring statistics",
                              "Chain statistics",
                              "Spherical harmonics",
                              "Mean Squared Displacement",
                              "Bond valence"};

char * graph_name[NGRAPHS] = {"g(r)/G(r)",
                              "S(q) from FFT[g(r)]",
                              "S(q) from Debye equation",
                              "g(r)/G(r) from FFT[S(q)]",
                              "Bonds properties",
                              "Angle distributions",
                              "Ring statistics",
                              "Chain statistics",
                              "Spherical harmonics",
                              "Mean Squared Displacement"};

tint cut_sel;
dint davect[9];
ColRGBA std[6];

shortcuts main_shortcuts[] = {
  { "About", "open about dialog",  GDK_KEY_a, "<Ctrl>a" },
  { "Periodic table", "open periodic table",  GDK_KEY_p, "<Ctrl>p" },
  { "Quit", "quit atomes",  GDK_KEY_q, "<Ctrl>q" },
//};
//{
  { "Open workspace", "open atomes workspace",  GDK_KEY_w, "<Ctrl>w" },
  { "Save workspace", "save atomes workspace",  GDK_KEY_s, "<Ctrl>s" },
  { "Close workspace", "close atomes workspace",  GDK_KEY_c, "<Ctrl>c" },
//};
//{
  { "New project", "create new atomes project",  GDK_KEY_n, "<Ctrl>n" },
  { "Open project", "open atomes project",  GDK_KEY_o, "<Ctrl>o" }
};

gchar * main_section_names[] = { "Main" };
int main_group_by_section[] = { 3 };
gchar * main_group_names[] = { "General", "Workspace", "Projects" };
int main_shortcut_by_group[] = { 3, 3, 2 };

/*!
  \fn GtkWidget * shortcuts_window (int sections, int group_by_section[sections], int groups, int shortcut_by_group[groups],
                                    gchar * section_names[sections], gchar * group_names[groups], shortcuts shortcs[])

  \brief Create the shortcuts information window

  \param sections number of shortcut sections
  \param group_by_section number of group by section
  \param groups number of groups
  \param shortcut_by_group number of shortcuts by group
  \param shortcs shortcuts information
*/
GtkWidget * shortcuts_window (int sections, int group_by_section[sections], int groups, int shortcut_by_group[groups],
                              gchar * section_names[sections], gchar * group_names[groups], shortcuts shortcs[])
{
  GtkShortcutsWindow * win = NULL;
#ifdef G_OS_WIN32
  win = g_object_new (GTK_TYPE_SHORTCUTS_WINDOW, "modal", FALSE, "resizable", TRUE, "deletable", TRUE, NULL);
#else
  win = g_object_new (GTK_TYPE_SHORTCUTS_WINDOW, "modal", FALSE, "resizable", TRUE, NULL);
#endif
  GtkShortcutsSection * shortcut_section[sections];
  GtkShortcutsGroup * shortcut_group[groups];
  GtkShortcutsShortcut * shortcut;
#ifdef GTK4
#if GTK_MINOR_VERSION < 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION < 4)
  GtkWidget * sections_book = gtk_notebook_new ();
  gtk_notebook_set_scrollable (GTK_NOTEBOOK(sections_book), TRUE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK(sections_book), GTK_POS_TOP);
#endif
#endif // GTK4
  int i, j, k, l, m;
  l = m = 0;
  for (i=0; i<sections; i++)
  {
    shortcut_section[i] = g_object_new (GTK_TYPE_SHORTCUTS_SECTION, "visible", TRUE, "title", section_names[i], "section-name", section_names[i], NULL);
#ifdef GTK4
#if GTK_MINOR_VERSION < 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION < 4)
  gtk_orientable_set_orientation ((GtkOrientable *)shortcut_section[i], GTK_ORIENTATION_HORIZONTAL);
#endif
#endif
    for (j=0; j<group_by_section[i]; j++, l++)
    {
      shortcut_group[l] = g_object_new (GTK_TYPE_SHORTCUTS_GROUP, "visible", TRUE, "title", group_names[l], NULL);
      for (k=0; k<shortcut_by_group[l]; k++, m++)
      {
        shortcut = g_object_new (GTK_TYPE_SHORTCUTS_SHORTCUT,
                                 "visible",       TRUE,
                                 "shortcut-type", GTK_SHORTCUT_ACCELERATOR,
                                 "accelerator",   shortcs[m].accelerator,
                                 "title",         shortcs[m].description,
                                 NULL );
#ifdef GTK4
#if GTK_MINOR_VERSION > 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION >= 4)
        gtk_shortcuts_group_add_shortcut (shortcut_group[l], shortcut);
#else
        add_box_child_start (GTK_ORIENTATION_VERTICAL, (GtkWidget *)shortcut_group[l], (GtkWidget *)shortcut, FALSE, FALSE, 0);
#endif
#else
        gtk_container_add (GTK_CONTAINER((GtkWidget *)shortcut_group[l]), (GtkWidget *)shortcut);
#endif
      }
#ifdef GTK4
#if GTK_MINOR_VERSION > 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION >= 4)
      gtk_shortcuts_section_add_group (shortcut_section[i], shortcut_group[l]);
#else
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, (GtkWidget *)shortcut_section[i], (GtkWidget *)shortcut_group[l], FALSE, FALSE, 0);
#endif
#else
      gtk_container_add (GTK_CONTAINER((GtkWidget *)shortcut_section[i]), (GtkWidget *)shortcut_group[l]);
#endif
    }
#ifdef GTK4
#if GTK_MINOR_VERSION > 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION >= 4)
    gtk_shortcuts_window_add_section (GTK_SHORTCUTS_WINDOW(win), shortcut_section[i]);
#else
    gtk_notebook_append_page (GTK_NOTEBOOK(sections_book), (GtkWidget *)shortcut_section[i], gtk_label_new (section_names[i]));
#endif
#else
    gtk_container_add (GTK_CONTAINER((GtkWidget *)win), (GtkWidget *)shortcut_section[i]);
#endif
  }
#ifdef GTK4
#if GTK_MINOR_VERSION < 14 || (GTK_MINOR_VERSION == 14 && GTK_MICRO_VERSION < 4)
   gtk_window_set_child ((GtkWindow*) win, sections_book);
#endif
#endif
  add_gtk_close_event ((GtkWidget *)win, G_CALLBACK(destroy_this_window), NULL);
  show_the_widgets ((GtkWidget *)win);
  return (GtkWidget *)win;
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean pop_menu (GtkWidget * widget, GdkEventButton * event, gpointer data)

  \brief popup a menu at an event position

  \param widget the GtkWidget sending the signal
  \param event the associated event
  \param data the associated pointer data
*/
G_MODULE_EXPORT gboolean pop_menu (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  if (event -> button == 3)
  {
    pop_menu_at_pointer (work_menu (-1, -1), (GdkEvent *)event);
  }
  return FALSE;
}
#endif

/*!
  \fn void clean_view ()

  \brief clean the main window
*/
void clean_view ()
{
  MainView = destroy_this_widget (MainView);
  atomes_logo = destroy_this_widget (atomes_logo);
#ifdef GTK4
  atomes_logo = gtk_picture_new_for_filename (PACKAGE_LOGO);
  gtk_widget_set_size_request (atomes_logo, 550, -1);
  add_container_child (CONTAINER_SCR, MainScrol[1], atomes_logo);
#else
  atomes_logo = gtk_image_new_from_file (PACKAGE_LOGO);
  MainScrol[1] = destroy_this_widget (MainScrol[1]);
  gtk_container_add (GTK_CONTAINER(MainEvent), atomes_logo);
  show_the_widgets (MainEvent);
#endif
}

/*!
  \fn void view_buffer (GtkTextBuffer * buffer)

  \brief set a text buffer in the main window or an image

  \param buffer the GtkTextBuffer to display
*/
void view_buffer (GtkTextBuffer * buffer)
{
  if (atomes_logo)
  {
    atomes_logo = destroy_this_widget (atomes_logo);
#ifdef GTK4
    gtk_scrolled_window_set_child ((GtkScrolledWindow *)MainScrol[1], NULL);
#endif
  }
  gboolean add = FALSE;
  if (! MainView)
  {
    MainView = gtk_text_view_new ();
    gtk_text_view_set_editable (GTK_TEXT_VIEW(MainView), 0);
    text_view_set_monospace (MainView);
    add = TRUE;
  }
  gtk_text_view_set_buffer (GTK_TEXT_VIEW(MainView), buffer);
#ifdef GTK3
  if (! MainScrol[1])
  {
    MainScrol[1] = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN);
    gtk_container_add (GTK_CONTAINER(MainEvent), MainScrol[1]);
    add = TRUE;
  }
  if (add) gtk_container_add (GTK_CONTAINER(MainScrol[1]), MainView);
#else
  if (add)
  {
     add_container_child (CONTAINER_SCR, MainScrol[1], MainView);
  }
#endif
  show_the_widgets (MainScrol[1]);
}

/*!
  \fn void atomes_key_pressed (guint keyval, GdkModifierType state)

  \brief main window key actions callbacks

  \param keyval key id
  \param state modifier (Ctrl, Alt ...)
*/
void atomes_key_pressed (guint keyval, GdkModifierType state)
{
  if (state & GDK_CONTROL_MASK)
  {
    switch (keyval)
    {
      case GDK_KEY_a:
        create_about_dialog (NULL, NULL);
        break;
      case GDK_KEY_c:
        on_close_workspace (NULL, GINT_TO_POINTER(1));
        break;
      case GDK_KEY_n:
        on_create_new_project (NULL, NULL);
        break;
      case GDK_KEY_o:
        on_open_save_activate (NULL, GINT_TO_POINTER(0));
        break;
      case GDK_KEY_p:
        get_atom_id_from_periodic_table (NULL);
        break;
      case GDK_KEY_q:
#ifdef GTK4
        leaving_question (NULL, NULL);
#else
        leaving_question (NULL, NULL, NULL);
#endif
        break;
      case GDK_KEY_s:
        on_save_as_activate (NULL, GINT_TO_POINTER(3));
        break;
      case GDK_KEY_t:
        on_show_curve_toolbox (NULL, NULL);
        break;
      case GDK_KEY_w:
        on_open_save_activate (NULL, GINT_TO_POINTER(2));
        break;
    }
  }
}

/*!
  \fn void add_action (GSimpleAction * action)

  \brief add action to the main window action map

  \param action the GAction sending the signal the action to add
*/
void add_action (GSimpleAction * action)
{
  g_action_map_add_action (G_ACTION_MAP(AtomesApp), G_ACTION(action));
}

/*!
  \fn void remove_action (gchar * action_name)

  \brief add action from the main window action map

  \param action_name the action to remove
*/
void remove_action (gchar * action_name)
{
  g_action_map_remove_action (G_ACTION_MAP(AtomesApp), (const gchar *)action_name);
}

/*!
  \fn void remove_edition_actions ()

  \brief remove all edition actions
*/
void remove_edition_actions ()
{
  int i;
  for (i=0; i<G_N_ELEMENTS(edition_acts); i++) remove_action (edition_acts[i].action_name);
}

/*!
  \fn void remove_edition_and_analyze_actions ()

  \brief remove all edition and analysis action
*/
void remove_edition_and_analyze_actions ()
{
  remove_edition_actions ();
  int i;
  for (i=0; i<G_N_ELEMENTS(analyze_acts); i++) remove_action (analyze_acts[i].action_name);
}

/*!
  \fn G_MODULE_EXPORT void show_periodic_table (GtkWidget * widg, gpointer data)

  \brief show the periodic table of the elements

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void show_periodic_table (GtkWidget * widg, gpointer data)
{
  get_atom_id_from_periodic_table (NULL);
}

/*!
  \fn G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief atomes menu bar actions

  \param action the GAction sending the signal the GSimpleAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atomes_menu_bar_action (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  gchar * name = g_strdup_printf ("%s", g_action_get_name(G_ACTION(action)));
  if (g_strcmp0 (name, "workspace.open") == 0)
  {
    on_open_save_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "workspace.save") == 0)
  {
    on_open_save_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "workspace.save-as") == 0)
  {
    on_save_as_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "workspace.close") == 0)
  {
    on_close_workspace (NULL, data);
  }
  else if (g_strcmp0 (name, "project.new") == 0)
  {
    on_create_new_project (NULL, data);
  }
  else if (g_strcmp0 (name, "project.edit") == 0)
  {
    change_project_name (NULL, data);
  }
  else if (g_strcmp0 (name, "project.active") == 0)
  {
    activate_project (NULL, data);
  }
  else if (g_strcmp0 (name, "project.compute") == 0)
  {
    compute_this_prop (NULL, data);
  }
  else if (g_strcmp0 (name, "project.open") == 0)
  {
    on_open_save_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "project.save") == 0)
  {
    on_open_save_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "project.save-as") == 0)
  {
    on_save_as_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "project.close") == 0)
  {
    on_close_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "export.isaacs") == 0)
  {
    on_isaacs_port (NULL, data);
  }
  else if (g_strcmp0 (name, "export.coordinates") == 0)
  {
    on_coord_port (NULL, data);
  }
  else if (g_strcmp0 (name, "import.isaacs") == 0)
  {
    on_isaacs_port (NULL, data);
  }
  else if (g_strcmp0 (name, "import.coordinates") == 0)
  {
    on_coord_port (NULL, data);
  }
  else if (g_strcmp0 (name, "program.quit") == 0)
  {
    leaving_from_menu (NULL, data);
  }
  else if (g_strcmp0 (name, "edit.chemistry") == 0)
  {
    on_edit_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "edit.periodicity") == 0)
  {
    on_edit_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "edit.cutoffs") == 0)
  {
    on_edit_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.gr") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.sq") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.sk") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.gk") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.bonds") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.rings") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.chains") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.sp") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.msd") == 0)
  {
    on_calc_activate (NULL, data);
  }
  else if (g_strcmp0 (name, "analyze.tool-box") == 0)
  {
    on_show_curve_toolbox (NULL, data);
  }
  else if (g_strcmp0 (name, "help.periodic") == 0)
  {
    show_periodic_table (NULL, data);
  }
  else if (g_strcmp0 (name, "help.about") == 0)
  {
    create_about_dialog (NULL, data);
  }
  else if (g_strcmp0 (name, "help.shortcuts") == 0)
  {
    atomes_shortcuts = destroy_this_widget (atomes_shortcuts);
    atomes_shortcuts = shortcuts_window (G_N_ELEMENTS(main_group_by_section), main_group_by_section, G_N_ELEMENTS(main_shortcut_by_group),
                                         main_shortcut_by_group, main_section_names, main_group_names, main_shortcuts);
  }
  else if (g_strcmp0 (name, "help.preferences") == 0)
  {
    create_user_preferences_dialog ();
  }
  g_free (name);
}

/*!
  \fn GIcon * get_gicon_from_data (int format, const gchar * icon)

  \brief create a new icon using data

  \param format for the data
  \param icon image name or file name
*/
GIcon * get_gicon_from_data (int format, const gchar * icon)
{
  switch (format)
  {
    case IMG_FILE:
      return g_file_icon_new (g_file_new_for_path (icon));
      break;
    case IMG_STOCK:
      return g_themed_icon_new (icon);
      break;
    default:
      return NULL;
      break;
  }
}

/*!
  \fn void widget_add_action (GSimpleActionGroup * action_group, const gchar * act, GCallback handler, gpointer data,
                              gboolean check, gboolean status, gboolean radio, const gchar * stat)

  \brief add an action to an action group

  \param action_group the action group to add an action to
  \param act the name of the action to add
  \param handler the associated callback
  \param data the associated data pointer
  \param check is the action a check (1/0)
  \param status if check or radio, status of the action (1/0)
  \param radio is the action a radio (1/0)
  \param stat if radio variant parameter of the action
*/
void widget_add_action (GSimpleActionGroup * action_group, const gchar * act, GCallback handler, gpointer data,
                        gboolean check, gboolean status, gboolean radio, const gchar * stat)
{
   GSimpleAction * action;
   GVariant * action_state;
   if (radio)
   {
     action_state = g_variant_new_string (stat);
     action = g_simple_action_new_stateful (act, G_VARIANT_TYPE_STRING, action_state);
   }
   else if (check)
   {
     action_state = g_variant_new_boolean (status);
     action = g_simple_action_new_stateful (act, NULL, action_state);
   }
   else
   {
     action = g_simple_action_new (act, NULL);
   }
   g_signal_connect (action, "activate", handler, data);
   g_action_map_add_action (G_ACTION_MAP(action_group), G_ACTION(action));
}

/*!
  \fn GMenuItem * create_gmenu_item (const gchar * label, const gchar * action, const gchar * accel,
                                     const gchar * custom, int format, const gchar * icon,
                                     gboolean check, gboolean status, gboolean radio, const gchar * rstatus)

  \brief create menu item

  \param label Label of the menu item
  \param action Action of the menu item
  \param accel Keyboard accelerator, if any
  \param custom Custom menu item ? NULL otherwise
  \param format Image format if not IMG_NONE
  \param icon Image data if not NULL
  \param check Check menu item ?
  \param status If check then status
  \param radio Radio menu item ?
  \param rstatus If radio then variant parameter
*/
GMenuItem * create_gmenu_item (const gchar * label, const gchar * action, const gchar * accel,
                               const gchar * custom, int format, const gchar * icon,
                               gboolean check, gboolean status, gboolean radio, const gchar * rstatus)
{
  GMenuItem * item;
  item = g_menu_item_new (label, action);
  /* GKT4 bug, normally mark-up should be provided using boolean:
  g_menu_item_set_attribute (item, "use-markup", "b", TRUE, NULL);
  But it does not work, however it does using a string: */
  g_menu_item_set_attribute (item, "use-markup", "s", "TRUE", NULL);
  if (custom)
  {
    g_menu_item_set_attribute (item, "custom", "s", custom, NULL);
    // g_menu_item_set_attribute_value (item, "custom", g_variant_new_string (custom));
    GVariant * cust = g_menu_item_get_attribute_value (item, "custom",  g_variant_type_new("s"));
    if (cust) g_print ("item :: %s, custom :: %s\n", label, g_variant_get_string  (cust, NULL));
  }
  else
  {
    if (accel) g_menu_item_set_attribute (item, "accel", "s", accel, NULL);
    // if (check) g_menu_item_set_attribute (item, "target", "b", status, NULL);
    if (radio) g_menu_item_set_attribute (item, "target", "s", rstatus, NULL);
#ifdef MENU_ICONS
    if (icon)
    {
      GIcon * gicon = get_gicon_from_data (format, icon);
      if (gicon)
      {
        g_menu_item_set_icon (item, gicon);
        g_object_unref (gicon);
      }
    }
#endif
  }
  return item;
}

/*!
  \fn void append_submenu (GMenu * menu, const gchar * label, GMenu * submenu)

  \brief append a GMenuItem with a subenu to a GMenu, and use markup for the GMenuItem

  \param menu the GMenu to insert the item with a submenu
  \param label the text for the menu item
  \param submenu the submenu to add to the menu item
*/
void append_submenu (GMenu * menu, const gchar * label, GMenu * submenu)
{
  GMenuItem * item = g_menu_item_new (label, NULL);
  /* GKT4 bug, normally mark-up should be provided using boolean:
  g_menu_item_set_attribute (item, "use-markup", "b", TRUE, NULL);
  But it does not work, however it does using a string: */
  g_menu_item_set_attribute (item, "use-markup", "s", "TRUE", NULL);
  g_menu_item_set_submenu (item, (GMenuModel *)submenu);
  g_menu_append_item (menu, item);
}

/*!
  \fn void append_menu_item (GMenu * menu, const gchar * label, const gchar * action, const gchar * accel,
                             const gchar * custom, int format, const gchar * icon,
                             gboolean check, gboolean status, gboolean radio, const gchar * rstatus)

  \brief create a menu item, then append it to a menu

  \param menu the menu to insert the item in
  \param label Label of the menu item
  \param action Action of the menu item
  \param accel Keyboard accelerator, if any
  \param custom Custom menu item ? NULL otherwise
  \param format Image format if not IMG_NONE
  \param icon Image data if not NULL
  \param check Check menu item ?
  \param status If check then status
  \param radio Radio menu item ?
  \param rstatus If radio then variant parameter
*/
void append_menu_item (GMenu * menu, const gchar * label, const gchar * action, const gchar * accel,
                                     const gchar * custom, int format, const gchar * icon,
                                     gboolean check, gboolean status, gboolean radio, const gchar * rstatus)
{
  GMenuItem * item = create_gmenu_item (label, action, accel, custom, format, icon, check, status, radio, rstatus);
  g_menu_append_item (menu, item);
  g_object_unref (item);
}

/*!
  \fn GMenu * workspace_section (gchar * act, int pop)

  \brief create the workspace section

  \param act app" or "pop" key for the GAction
  \param pop from main app (0) or contextual (1)
*/
GMenu * workspace_section (gchar * act, int pop)
{
  GMenu * menu = g_menu_new ();
  gchar * str;
  str =  g_strdup_printf ("%s.workspace.open", act);
  append_menu_item (menu, "Open", (const gchar *)str, "<CTRL>W", NULL, IMG_STOCK, FOPEN, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  if (! pop || nprojects)
  {
    str =  g_strdup_printf ("%s.workspace.save", act);
    append_menu_item (menu, "Save", (const gchar *)str, NULL, NULL, IMG_STOCK, FSAVE, FALSE, FALSE, FALSE, NULL);
    g_free (str);
    str =  g_strdup_printf ("%s.workspace.save-as", act);
    append_menu_item (menu, "Save As", (const gchar *)str, "<CTRL>S", NULL, IMG_STOCK, FSAVEAS, FALSE, FALSE, FALSE, NULL);
    g_free (str);
    str =  g_strdup_printf ("%s.workspace.close", act);
    append_menu_item (menu, "Close", (const gchar *)str, "<CTRL>C", NULL, IMG_STOCK, FCLOSE, FALSE, FALSE, FALSE, NULL);
    g_free (str);
  }
  return menu;
}

/*!
  \fn GMenu * port_section (gchar * act, int pop, int port)

  \brief create the import/export menu items

  \param act "app" or "pop" key for the GAction
  \param pop from main app (0) or contextual (1)
  \param port Import (1) or Export (0)
*/
GMenu * port_section (gchar * act, int pop, int port)
{
  gchar * port_action[2]={"export", "import"};
  GMenu * menu = g_menu_new ();
  gchar * str;
  str =  g_strdup_printf ("%s.%s.isaacs", act, port_action[port]);
  append_menu_item (menu, "ISAACS Project File (*.ipf)", (const gchar *)str, NULL, NULL, IMG_FILE, PACKAGE_MOL, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  str =  g_strdup_printf ("%s.%s.coordinates", act, port_action[port]);
  append_menu_item (menu, "Atomic Coordinates", (const gchar *)str, NULL, NULL, IMG_FILE, (port) ? PACKAGE_IMP : PACKAGE_CON, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  return menu;
}

/*!
  \fn GMenu * project_section (gchar * act, int pop_up, int proj, int calc)

  \brief create the project section

  \param act "app" or "pop" key for the GAction
  \param pop_up from main app (0) or contextual (1)
  \param proj project id, if any (or -1)
  \param calc calculation id, if any (or -1)
*/
GMenu * project_section (gchar * act, int pop_up, int proj, int calc)
{
  GMenu * menu = g_menu_new ();
  gchar * str, * str_n;
  if (pop_up && nprojects && proj > -1)
  {
    if (activep != proj)
    {
      str = g_strdup_printf ("%s.project.active", act);
      append_menu_item (menu, "Make Active", (const gchar *)str, NULL, NULL, IMG_STOCK, YES, FALSE, FALSE, FALSE, NULL);
      g_free (str);
    }
    if (calc > -1)
    {
      str = g_strdup_printf ("%s.project.compute", act);
      str_n = g_strdup_printf ("Analyze: %s", work_menu_items[calc+4]);
      append_menu_item (menu, str_n, (get_project_by_id(proj) -> runok[calc]) ? (const gchar *)str : "None", NULL, NULL, IMG_FILE, graph_img[calc], FALSE, FALSE, FALSE, NULL);
      g_free (str);
      g_free (str_n);
    }
    str = g_strdup_printf ("%s.project.edit", act);
    append_menu_item (menu, "Edit Name", (const gchar *)str, NULL, NULL, IMG_STOCK, EDITA, FALSE, FALSE, FALSE, NULL);
    g_free (str);
  }
  str = g_strdup_printf ("%s.project.new", act);
  append_menu_item (menu, "New", (const gchar *)str, "<CTRL>N", NULL, IMG_STOCK, FNEW, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  str = g_strdup_printf ("%s.project.open", act);
  append_menu_item (menu, "Open", (const gchar *)str, "<CTRL>O", NULL, IMG_STOCK, FOPEN, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  if (! pop_up || proj > -1)
  {
    str = g_strdup_printf ("%s.project.save", act);
    append_menu_item (menu, "Save", (const gchar *)str, NULL, NULL, IMG_STOCK, FSAVE, FALSE, FALSE, FALSE, NULL);
    g_free (str);
    str = g_strdup_printf ("%s.project.save-as", act);
    append_menu_item (menu, "Save As", (const gchar *)str, NULL, NULL, IMG_STOCK, FSAVEAS, FALSE, FALSE, FALSE, NULL);
    g_free (str);
    str = g_strdup_printf ("%s.project.close", act);
    append_menu_item (menu, "Close", (const gchar *)str, NULL, NULL, IMG_STOCK, FCLOSE, FALSE, FALSE, FALSE, NULL);
    g_free (str);
    append_submenu (menu, "Export", port_section(act, pop_up, 0));
  }
  return menu;
}

/*!
  \fn GMenu * import_section (gchar * act)

  \brief create the 'Import' submenu

  \param act "app" or "pop" key for the GAction
*/
GMenu * import_section (gchar * act)
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Import", port_section(act, 0, 1));
  return menu;
}

/*!
  \fn GMenu * quit_section (gchar * act)

  \brief create the 'Quit' menu item

  \param act "app" or "pop" key for the GAction
*/
GMenu * quit_section (gchar * act)
{
  GMenu * menu = g_menu_new ();
  gchar * str = g_strdup_printf ("%s.program.quit", act);
  append_menu_item (menu, "Quit", (const gchar *)str, "<CTRL>Q", NULL, IMG_STOCK, FEXIT, FALSE, FALSE, FALSE, NULL);
  g_free (str);
  return menu;
}

/*!
  \fn GMenu * workspace_title ()

  \brief create the 'Workspace' menu item
*/
GMenu * workspace_title ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Workspace", "None", NULL, NULL, IMG_FILE, PACKAGE_TD, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * project_title (int pop_up, int proj)

  \brief create project title menu item

  \param pop_up from main app (0) or contextual (1)
  \param proj project id, if any (or -1)
*/
GMenu * project_title (int pop_up, int proj)
{
  GMenu * menu = g_menu_new ();
  if (pop_up && nprojects && proj > -1)
  {
    gchar * str = g_strdup_printf ("<b>%s</b>", get_project_by_id(proj) -> name);
    append_menu_item (menu, str, "None", NULL, NULL, IMG_FILE, PACKAGE_TD, FALSE, FALSE, FALSE, NULL);
    g_free (str);
  }
  else
  {
    append_menu_item (menu, "Project(s)", "None", NULL, NULL, IMG_FILE, PACKAGE_TD, FALSE, FALSE, FALSE, NULL);
  }
  return menu;
}

/*!
  \fn GMenu * create_workspace_menu (gchar * act, int pop_up, int proj, int calc)

  \brief create atomes 'workspace' menu

  \param act "app" or "pop" key for the GAction
  \param pop_up from main app (0) or contextual (1)
  \param proj project id, if any (or -1)
  \param calc calculation id, if any (or -1)
*/
GMenu * create_workspace_menu (gchar * act, int pop_up, int proj, int calc)
{
  GMenu * menu = g_menu_new ();
  g_menu_append_section (menu, NULL, (GMenuModel*)workspace_title());
  g_menu_append_section (menu, NULL, (GMenuModel*)workspace_section(act, pop_up));
  g_menu_append_section (menu, NULL, (GMenuModel*)project_title(pop_up, proj));
  g_menu_append_section (menu, NULL, (GMenuModel*)project_section(act, pop_up, proj, calc));
  g_menu_append_section (menu, NULL, (GMenuModel*)import_section(act));
  g_menu_append_section (menu, NULL, (GMenuModel*)quit_section(act));
  return menu;
}

/*!
  \fn GMenu * create_edit_menu ()

  \brief create atomes 'edit' menu
*/
GMenu * create_edit_menu ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Chemistry and Physics", "app.edit.chemistry", NULL, NULL, IMG_STOCK, DPROPERTIES, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Box and Periodicity", "app.edit.periodicity", NULL, NULL, IMG_STOCK, DPROPERTIES, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Bond Cutoffs", "app.edit.cutoffs", NULL, NULL, IMG_STOCK, DPROPERTIES, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * tool_box_section ()

  \brief create toolboxes menu item
*/
GMenu * tool_box_section ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Toolboxes", "app.analyze.tool-box", "<CTRL>T", NULL, IMG_STOCK, PAGE_SETUP, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * create_analyze_menu ()

  \brief create atomes 'analyze' menu
*/
GMenu * create_analyze_menu ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "g(r) / G(r)", "app.analyze.gr", NULL, NULL, IMG_FILE, PACKAGE_GR, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "S(q) from FFT[g(r)]", "app.analyze.sq", NULL, NULL, IMG_FILE, PACKAGE_SQ, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "S(q) from Debye Eq.", "app.analyze.sk", NULL, NULL, IMG_FILE, PACKAGE_SQ, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "g(r) / G(r) from FFT[S(q)]", "app.analyze.gk", NULL, NULL, IMG_FILE, PACKAGE_GR, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Bonds and Angles", "app.analyze.bonds", NULL, NULL, IMG_FILE, PACKAGE_BD, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Ring statistics", "app.analyze.rings", NULL, NULL, IMG_FILE, PACKAGE_RI, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Chain statistics", "app.analyze.chains", NULL, NULL, IMG_FILE, PACKAGE_CH, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Spherical Harmonics", "app.analyze.sp", NULL, NULL, IMG_FILE, PACKAGE_SP, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Mean Squared Displacement", "app.analyze.msd", NULL, NULL, IMG_FILE, PACKAGE_MS, FALSE, FALSE, FALSE, NULL);
  g_menu_append_section (menu, NULL, (GMenuModel*)tool_box_section());
  return menu;
}

/*!
  \fn GMenu * create_help_menu ()

  \brief create atomes 'help' menu
*/
GMenu * create_help_menu ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Periodic Table", "app.help.periodic", "<CTRL>P", NULL, IMG_STOCK, ABOUT, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Preferences", "app.help.preferences", NULL, NULL, IMG_STOCK, ABOUT, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Shortcuts", "app.help.shortcuts", NULL, NULL, IMG_STOCK, ABOUT, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "About", "app.help.about", "<CTRL>A", NULL, IMG_STOCK, ABOUT, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * atomes_menu_bar ()

  \brief create atomes menu bar
*/
GMenu * atomes_menu_bar ()
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Workspace", create_workspace_menu("app", 0, -1, -1));
  append_submenu (menu, "Edit", create_edit_menu());
  append_submenu (menu, "Analyze", create_analyze_menu());
  append_submenu (menu, "Help", create_help_menu());
  return menu;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void atomes_popup_menu (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief mouse right event to popup the main application 'workspace' menu

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void atomes_popup_menu (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  if (gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture) == GDK_BUTTON_SECONDARY)
  {
    GtkWidget * popover = work_menu (-1, -1);
    gtk_widget_set_parent (popover, MainWindow);
    pop_menu_at_pointer (popover, x, y);
  }
}

/*!
  \fn G_MODULE_EXPORT gboolean on_atomes_pressed (GtkEventControllerKey * self, guint keyval, guint keycode, GdkModifierType state, gpointer data)

  \brief the GtkEventController for the keyboard button press event

  \param self the GtkEventController sending the signal
  \param keyval the
  \param keycode the key pressed
  \param state the keyboard modifier (Ctrl, Alt ... if any)
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_atomes_pressed (GtkEventControllerKey * self, guint keyval, guint keycode, GdkModifierType state, gpointer data)
{
  atomes_key_pressed (keyval, state);
  return TRUE;
}
#endif

/*!
  \fn GtkWidget * create_main_window (GApplication * atomes)

  \brief create the main application window

  \param atomes the initial GtkApplication
*/
GtkWidget * create_main_window (GApplication * atomes)
{
  int i;
  THETD = gdk_pixbuf_new_from_file (PACKAGE_TD, NULL);
#ifdef GTK3
  gtk_window_set_default_icon (THETD);
#endif
  THEMO = gdk_pixbuf_new_from_file (PACKAGE_MOL, NULL);
  THEBD = gdk_pixbuf_new_from_file (PACKAGE_BD, NULL);
  SETTING = gdk_pixbuf_new_from_file (PACKAGE_PRO, NULL);
  SETTINGS = gdk_pixbuf_new_from_file (PACKAGE_SET, NULL);
  OGL = gdk_pixbuf_new_from_file (PACKAGE_OGL, NULL);
  OGLM = gdk_pixbuf_new_from_file (PACKAGE_OGLM, NULL);
  OGLC = gdk_pixbuf_new_from_file (PACKAGE_OGLC, NULL);

  GtkWidget * window = gtk_application_window_new (GTK_APPLICATION(atomes));
  gtk_window_set_title (GTK_WINDOW(window), PACKAGE);
  gtk_window_set_resizable (GTK_WINDOW(window), TRUE);
  gtk_widget_set_size_request (window, 900, 450);

  atomes_action main_actions[] = {{ "workspace.open", GINT_TO_POINTER(2)},
                                  { "workspace.save",  GINT_TO_POINTER(3)},
                                  { "workspace.save-as",  GINT_TO_POINTER(3)},
                                  { "workspace.close", GINT_TO_POINTER(1)},
                                  { "project.new", NULL},
                                  { "project.open", GINT_TO_POINTER(0)},
                                  { "project.save", GINT_TO_POINTER(1) },
                                  { "project.save-as", GINT_TO_POINTER(1)},
                                  { "project.close", NULL},
                                  { "export.isaacs", GINT_TO_POINTER(1)},
                                  { "export.coordinates", GINT_TO_POINTER(1)},
                                  { "import.isaacs", GINT_TO_POINTER(0)},
                                  { "import.coordinates", GINT_TO_POINTER(0)},
                                  { "program.quit",  NULL},
                                  { "analyze.tool-box", NULL},
                                  { "help.periodic", NULL},
                                  { "help.about", NULL},
                                  { "help.shortcuts", NULL},
                                  { "help.preferences", NULL}};

  GSimpleAction ** main_act = g_malloc0 (G_N_ELEMENTS(main_actions)*sizeof*main_act);
  for (i=0; i<G_N_ELEMENTS(main_actions); i++)
  {
    main_act[i] = g_simple_action_new (main_actions[i].action_name, NULL);
    add_action (main_act[i]);
    g_signal_connect (main_act[i], "activate", G_CALLBACK(atomes_menu_bar_action), main_actions[i].action_data);
  }
  for (i=0; i<G_N_ELEMENTS(edition_acts); i++)
  {
    edition_actions[i] = g_simple_action_new (edition_acts[i].action_name, NULL);
    g_signal_connect (edition_actions[i], "activate", G_CALLBACK(atomes_menu_bar_action), edition_acts[i].action_data);
  }
  for (i=0; i<G_N_ELEMENTS(analyze_acts); i++)
  {
    analyze_actions[i] = g_simple_action_new (analyze_acts[i].action_name, NULL);
    g_signal_connect (analyze_actions[i], "activate", G_CALLBACK(atomes_menu_bar_action), analyze_acts[i].action_data);
  }

  /*GtkBuilder * builder = gtk_builder_new_from_file ("menus/main.ui");
  GMenuModel * model = G_MENU_MODEL (gtk_builder_get_object (builder, "atomes_menu_bar"));
  gtk_application_set_menubar (GTK_APPLICATION(atomes), G_MENU_MODEL(model));
  g_object_unref (model);*/
  gtk_application_set_menubar (GTK_APPLICATION(atomes), G_MENU_MODEL(atomes_menu_bar()));
  gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW(window), TRUE);

#ifdef GTK3
  MainEvent = gtk_event_box_new ();
  gtk_widget_add_events (MainEvent,
                         GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK |
                         GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  g_signal_connect(G_OBJECT(MainEvent), "button_press_event", G_CALLBACK(pop_menu), NULL);
  g_signal_connect(G_OBJECT(MainEvent), "button_release_event", G_CALLBACK(pop_menu), NULL);
#else
  add_widget_gesture_and_key_action (window, "atomes-context-click", G_CALLBACK(atomes_popup_menu), NULL,
                                             NULL, NULL, NULL,
                                             "atomes-key-pressed", G_CALLBACK(on_atomes_pressed), NULL,
                                             NULL, NULL, NULL, NULL, NULL, NULL);
#endif
  add_gtk_close_event (window, G_CALLBACK(leaving_question), NULL);
  GtkWidget * hpaned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
  int frame_size[2]={200, 550};
  for (i=0; i<2; i++)
  {
    MainFrame[i] = gtk_frame_new (NULL);
    gtk_widget_set_size_request (MainFrame[i], frame_size[i], -1);
    MainScrol[i] = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN);
#ifdef GTK3
    if (! i)
    {
      add_container_child (CONTAINER_FRA, MainFrame[i], MainScrol[i]);
    }
    else
    {
      add_container_child (CONTAINER_FRA, MainFrame[i], MainEvent);
      gtk_container_add (GTK_CONTAINER(MainEvent), MainScrol[i]);
    }
#else
    gtk_scrolled_window_set_propagate_natural_height ((GtkScrolledWindow *)MainScrol[i], TRUE);
    gtk_scrolled_window_set_propagate_natural_width ((GtkScrolledWindow *)MainScrol[i], TRUE);
    add_container_child (CONTAINER_FRA, MainFrame[i], MainScrol[i]);
#endif
  }
#ifdef GTK4
  gtk_paned_set_start_child (GTK_PANED (hpaned), MainFrame[0]);
  gtk_paned_set_resize_start_child (GTK_PANED (hpaned), FALSE);
  gtk_paned_set_shrink_start_child (GTK_PANED (hpaned), FALSE);
  gtk_paned_set_end_child (GTK_PANED (hpaned), MainFrame[1]);
  gtk_paned_set_resize_end_child (GTK_PANED (hpaned), TRUE);
  gtk_paned_set_shrink_end_child (GTK_PANED (hpaned), FALSE);
#else
  gtk_paned_pack1 (GTK_PANED (hpaned), MainFrame[0], FALSE, FALSE);
  gtk_paned_pack2 (GTK_PANED (hpaned), MainFrame[1], FALSE, FALSE);
#endif
  curvetoolbox = curvetbox ();
  add_project_to_workspace ();
  add_container_child (CONTAINER_WIN, window, hpaned);
  clean_view ();
  show_the_widgets (window);

  for (i=0; i<9; i++)
  {
    davect[i].a = i/3;
    davect[i].b = i-3*davect[i].a;
  }

  calc_img[0] = g_strdup_printf ("%s", PACKAGE_GR);
  calc_img[1] = g_strdup_printf ("%s", PACKAGE_SQ);
  calc_img[2] = g_strdup_printf ("%s", PACKAGE_SQ);
  calc_img[3] = g_strdup_printf ("%s", PACKAGE_GR);
  calc_img[4] = g_strdup_printf ("%s", PACKAGE_AN);
  calc_img[5] = g_strdup_printf ("%s", PACKAGE_RI);
  calc_img[6] = g_strdup_printf ("%s", PACKAGE_CH);
  calc_img[7] = g_strdup_printf ("%s", PACKAGE_SP);
  calc_img[8] = g_strdup_printf ("%s", PACKAGE_MS);
  calc_img[9] = g_strdup_printf ("%s", PACKAGE_BD);

  dots[0] = g_strdup_printf ("%s", PACKAGE_DOTA);
  dots[1] = g_strdup_printf ("%s", PACKAGE_DOTB);
  dots[2] = g_strdup_printf ("%s", PACKAGE_DOTC);
  dots[3] = g_strdup_printf ("%s", PACKAGE_DOTD);
  dots[4] = g_strdup_printf ("%s", PACKAGE_DOTE);
  dots[5] = g_strdup_printf ("%s", PACKAGE_DOTF);
  dots[6] = g_strdup_printf ("%s", PACKAGE_DOTG);
  dots[7] = g_strdup_printf ("%s", PACKAGE_DOTH);

  ifield[0] = g_strdup_printf ("%s", PACKAGE_DFBD);
  ifield[1] = g_strdup_printf ("%s", PACKAGE_DFBD);
  ifield[2] = g_strdup_printf ("%s", PACKAGE_DFAN);
  ifield[3] = g_strdup_printf ("%s", PACKAGE_DFAN);
  ifield[4] = g_strdup_printf ("%s", PACKAGE_DFDI);
  ifield[5] = g_strdup_printf ("%s", PACKAGE_DFDI);
  ifield[6] = g_strdup_printf ("%s", PACKAGE_DFTD);
  ifield[7] = g_strdup_printf ("%s", PACKAGE_DFIN);

  bravais_img[0] = g_strdup_printf ("%s", PACKAGE_SGTC);
  bravais_img[1] = g_strdup_printf ("%s", PACKAGE_SGMP);
  bravais_img[2] = g_strdup_printf ("%s", PACKAGE_SGMI);
  bravais_img[3] = g_strdup_printf ("%s", PACKAGE_SGOP);
  bravais_img[4] = g_strdup_printf ("%s", PACKAGE_SGOI);
  bravais_img[5] = g_strdup_printf ("%s", PACKAGE_SGOC);
  bravais_img[6] = g_strdup_printf ("%s", PACKAGE_SGOF);
  bravais_img[7] = g_strdup_printf ("%s", PACKAGE_SGTP);
  bravais_img[8] = g_strdup_printf ("%s", PACKAGE_SGTI);
  bravais_img[9] = g_strdup_printf ("%s", PACKAGE_SGTR);
  bravais_img[10] = g_strdup_printf ("%s", PACKAGE_SGHP);
  bravais_img[11] = g_strdup_printf ("%s", PACKAGE_SGCP);
  bravais_img[12] = g_strdup_printf ("%s", PACKAGE_SGCI);
  bravais_img[13] = g_strdup_printf ("%s", PACKAGE_SGCF);

  // Rouge
  std[0].red = 1.0;
  std[0].green = 0.0;
  std[0].blue = 0.0;
  // Jaune
  std[1].red = 1.0;
  std[1].green = 1.0;
  std[1].blue = 0.0;
  // Vert
  std[2].red = 0.0;
  std[2].green = 1.0;
  std[2].blue = 0.0;
  // Cyan
  std[3].red = 0.0;
  std[3].green = 1.0;
  std[3].blue = 1.0;
  // Bleu
  std[4].red = 0.0;
  std[4].green = 0.0;
  std[4].blue = 1.0;
  // Rose
  std[5].red = 1.0;
  std[5].green = 0.0;
  std[5].blue = 1.0;

  for (i=0; i<3; i++)
  {
    objects[i] = 0;
    object_was_selected[i] = NULL;
    tmp_object_id[i] = NULL;
  }
  cut_sel.a = cut_sel.b = 0;
  cut_sel.c = 1;

  return window;
}
