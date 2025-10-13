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
* @file datab.c
* @short Curve data edition window and associated callbacks
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'datab.c'
*
* Contains:
*

 - The curve data edition window and associated callbacks

*
* List of functions:

  G_MODULE_EXPORT gboolean on_data_button_event (GtkWidget * widget, GdkEvent * event, gpointer data);
  G_MODULE_EXPORT gboolean cancel_win (GtkWindow * win, gpointer data);
  G_MODULE_EXPORT gboolean cancel_win (GtkWidget * win, GdkEvent * event, gpointer data);

  void get_tree_data (GtkWidget * tree);
  void save_row (gpointer data, gpointer user_data);
  void update_first_row (gpointer data, gpointer user_data);
  void update_first_col ();
  void add_to_last_row (gpointer data, gpointer user_data);
  void add_to_last_col (double cte, gpointer data);
  void multiply_last_row (gpointer data, gpointer user_data);
  void multiply_last_col (double cte, gpointer data);
  void select_row (gpointer data, gpointer user_data);
  void copy_row (gpointer data, gpointer user_data);
  void copy_content (gpointer data);
  void add_row (gpointer data, gpointer user_data);
  void delete_row (gpointer data, gpointer user_data);
  void insert_cell (gpointer data);
  void delete_cell (gpointer data);
  void add_to_column (gpointer data);
  void multiply_column (gpointer data);
  void data_popup_menu (GtkWidget * top_level, double x, double y, gpointer data);
  void data_popup_menu (GtkWidget * top_level, GdkEvent * event, gpointer data);
  void data_button_event (GtkWidget * data_tree, double event_x, double event_y, guint event_button, guint event_type, gpointer data);
  void data_button_event (GtkWidget * data_tree, GdkEvent * event, guint event_button, guint event_type, gpointer data);
  void cancel_changes (GtkWidget * widg, gpointer data);
  void edit_data (gpointer data);

  static void fill_data_model (GtkListStore * store, project * this_proj, int b, int c);

  G_MODULE_EXPORT void edit_cell (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer user_data);
  G_MODULE_EXPORT void adjust_value (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void run_add_to_column (GtkDialog * wind, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_multiply_column (GtkDialog * wind, gint response_id, gpointer data);
  G_MODULE_EXPORT void data_pop_action (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void on_data_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void on_data_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void cancel_but (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void validate_changes (GtkButton * but, gpointer data);

  GtkWidget * setview (project * this_proj, int b, int c);

  GMenu * insert_place ();
  GMenu * insert_data ();
  GMenu * delete_data ();
  GMenu* cell_actions ();
  GMenu * cell_title ();
  GMenu * column_actions ();
  GMenu * column_title ();
  GMenu * data_menu ();

*/

#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"

int nrows;
GtkTreeModel * curve_model;
GList * lrows;
GList * rows;
GtkTreeSelection * sel;
GtkTreePath * path;
GtkTreeIter row;
gchar * text;

/*!
  \fn void get_tree_data (GtkWidget * tree)

  \brief get information on location in a GtkTreeView

  \param tree the GtkTreeView
*/
void get_tree_data (GtkWidget * tree)
{
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(tree));
  curve_model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
  lrows = gtk_tree_selection_get_selected_rows (sel, & curve_model);
  nrows = gtk_tree_selection_count_selected_rows (sel);
}

/*!
  \fn void save_row (gpointer data, gpointer user_data)

  \brief save row data and udpate calculation result accordingly

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void save_row (gpointer data, gpointer user_data)
{
  path = data;
  tint * id = (tint *)user_data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
#ifdef NEW_ANA
    gtk_tree_model_get (curve_model, & row, 1, & get_project_by_id(id -> a) -> analysis[id -> b].curves[id -> c] -> data[0][nrows],
                                            2, & get_project_by_id(id -> a) -> analysis[id -> b].curves[id -> c] -> data[1][nrows], -1);
#else
    gtk_tree_model_get (curve_model, & row, 1, & get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> data[0][nrows],
                                            2, & get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> data[1][nrows], -1);
#endif
  }
  nrows = nrows + 1;
}

/*!
  \fn void update_first_row (gpointer data, gpointer user_data)

  \brief update row in the first column

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void update_first_row (gpointer data, gpointer user_data)
{
  path = data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    nrows = nrows + 1;
    gtk_list_store_set (GTK_LIST_STORE(curve_model), & row, 0, nrows, -1);
  }
}

/*!
  \fn void update_first_col ()

  \brief update all rows in the first column
*
*/
void update_first_col ()
{
  gtk_tree_selection_select_all (sel);
  lrows = gtk_tree_selection_get_selected_rows (sel, & curve_model);
  nrows = 0;
  g_list_foreach (lrows, (GFunc)update_first_row, NULL);
  gtk_tree_selection_unselect_all (sel);
}

/*!
  \fn void add_to_last_row (gpointer data, gpointer user_data)

  \brief add constant to last row

  \param data the path in the tree model
  \param user_data the constant
*/
void add_to_last_row (gpointer data, gpointer user_data)
{
  path = data;
  double vold;
  double * cte = (double *)user_data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    gtk_tree_model_get (curve_model, & row, 2, & vold, -1);
    gtk_list_store_set (GTK_LIST_STORE(curve_model), & row, 2, * cte + vold, -1);
  }
}

/*!
  \fn void add_to_last_col (double cte, gpointer data)

  \brief add constant value to selected rows

  \param cte the constant to add
  \param data the associated data pointer
*/
void add_to_last_col (double cte, gpointer data)
{
  qint * dat = (qint *)data;
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = get_project_by_id (((tint *)data) -> a) -> analysis[dat -> b].curves[dat -> c];
#else
  this_curve = get_project_by_id (((tint *)data) -> a) -> curves[dat -> b][dat -> c];
#endif
  curve_model = gtk_tree_view_get_model (GTK_TREE_VIEW(this_curve -> datatree));
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(this_curve -> datatree));
  gtk_tree_selection_select_all (sel);
  get_tree_data (this_curve -> datatree);
  g_list_foreach (lrows, (GFunc)add_to_last_row, & cte);
  gtk_tree_selection_unselect_all (sel);
}

/*!
  \fn void multiply_last_row (gpointer data, gpointer user_data)

  \brief multiply last row by constant

  \param data the path in the tree model
  \param user_data the constant
*/
void multiply_last_row (gpointer data, gpointer user_data)
{
  path = data;
  double vold;
  double * cte = (double *) user_data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    gtk_tree_model_get (curve_model, & row, 2, & vold, -1);
    gtk_list_store_set (GTK_LIST_STORE(curve_model), & row, 2, * cte * vold, -1);
  }
}

/*!
  \fn void multiply_last_col (double cte, gpointer data)

  \brief multiply last colum by constant value

  \param cte the constant value
  \param data the associated data pointer
*/
void multiply_last_col (double cte, gpointer data)
{
  qint * dat = (qint *)data;
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = get_project_by_id (dat -> a) -> analysis[dat -> b].curves[dat -> c];
#else
  this_curve = get_project_by_id (dat -> a) -> curves[dat -> b][dat -> c];
#endif
  curve_model = gtk_tree_view_get_model (GTK_TREE_VIEW(this_curve -> datatree));
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(this_curve -> datatree));
  gtk_tree_selection_select_all (sel);
  get_tree_data (this_curve -> datatree);
  g_list_foreach (lrows, (GFunc)multiply_last_row, & cte);
  gtk_tree_selection_unselect_all (sel);
}

/*!
  \fn void select_row (gpointer data, gpointer user_data)

  \brief select row

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void select_row (gpointer data, gpointer user_data)
{
  path = data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    gtk_tree_selection_select_iter (sel, & row);
  }
}

/*!
  \fn void copy_row (gpointer data, gpointer user_data)

  \brief copy row

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void copy_row (gpointer data, gpointer user_data)
{
  path = data;
  int  v1;
  double v2, v3;
  gchar * tmp;

  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    gtk_tree_model_get (curve_model, & row, 0, & v1, 1, & v2, 2, & v3, -1);
    if (text == NULL)
    {
      text = g_strdup_printf ("%d\t%f\t%f\n", v1, v2, v3);
    }
    else
    {
      tmp = g_strdup_printf ("%s%d\t%f\t%f\n", text, v1, v2, v3);
      g_free (text);
      text = g_strdup_printf ("%s", tmp);
      g_free (tmp);
    }
  }
}

/*!
  \fn void copy_content (gpointer data)

  \brief copy selection

  \param data the associated data pointer
*/
void copy_content (gpointer data)
{
  text = NULL;
  g_list_foreach (lrows, (GFunc)copy_row, NULL);
#ifdef GTK3
  gtk_clipboard_set_text (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), text, -1);
  gtk_clipboard_store (gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
#endif
  g_free (text);
}

/*!
  \fn void add_row (gpointer data, gpointer user_data)

  \brief add row

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void add_row (gpointer data, gpointer user_data)
{
  GtkTreeIter newrow;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    if (GPOINTER_TO_INT(user_data) == 0)
    {
      gtk_list_store_insert_before (GTK_LIST_STORE(curve_model), & newrow, & row);
    }
    else
    {
      gtk_list_store_insert_after (GTK_LIST_STORE(curve_model), & newrow, & row);
    }
    gtk_list_store_set (GTK_LIST_STORE(curve_model), & newrow, 0, 0, 1, 0.00000, 2, 0.00000, -1);
  }
}

/*!
  \fn void delete_row (gpointer data, gpointer user_data)

  \brief delete row

  \param data the path in the tree model
  \param user_data the associated data pointer
*/
void delete_row (gpointer data, gpointer user_data)
{
  path = data;
  if (gtk_tree_model_get_iter (curve_model, & row, path))
  {
    gtk_list_store_remove (GTK_LIST_STORE(curve_model), & row);
  }
}

/*!
  \fn void insert_cell (gpointer data)

  \brief insert row

  \param data the associated data pointer
*/
void insert_cell (gpointer data)
{
  if (GPOINTER_TO_INT(data) == 0)
  {
    path = (GtkTreePath *) g_list_nth_data (lrows, 0);
  }
  else
  {
    path = (GtkTreePath *) g_list_nth_data (lrows, nrows-1);
  }
  g_list_foreach (lrows, (GFunc)add_row, data);
  update_first_col ();
}

/*!
  \fn void delete_cell (gpointer data)

  \brief delete row

  \param data the associated data pointer
*/
void delete_cell (gpointer data)
{
  g_list_foreach (g_list_reverse (lrows), (GFunc)delete_row, NULL);
  update_first_col ();
}

/*!
  \fn G_MODULE_EXPORT void edit_cell (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer user_data)

  \brief edit cell in the curve data edition tree model

  \param cell the GtkCellRendererText sending the signal
  \param path_string the path in the tree model
  \param new_text the string describing the new value
  \param user_data the associated data pointer
*/
G_MODULE_EXPORT void edit_cell (GtkCellRendererText * cell, gchar * path_string, gchar * new_text, gpointer user_data)
{
  qint * id = (qint *)user_data;
  project * this_proj = get_project_by_id (id -> a);
#ifdef NEW_ANA
  curve_model = gtk_tree_view_get_model(GTK_TREE_VIEW(this_proj -> analysis[id -> b].curves[id -> c] -> datatree));
#else
  curve_model = gtk_tree_view_get_model(GTK_TREE_VIEW(this_proj -> curves[id -> b][id -> c] -> datatree));
#endif
  gtk_tree_model_get_iter_from_string (curve_model, & row, path_string);
  double val = string_to_double ((gpointer)new_text);
  gtk_list_store_set (GTK_LIST_STORE(curve_model), & row, id -> d, val, -1);
}

GtkWidget * col_entry;

/*!
  \fn G_MODULE_EXPORT void adjust_value (GtkEntry * res, gpointer data)

  \brief adjust constant value entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void adjust_value (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  update_entry_double (res, v);
}

/*!
  \fn G_MODULE_EXPORT void run_add_to_column (GtkDialog * wind, gint response_id, gpointer data)

  \brief  add constant to column - running the dialog

  \param wind the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_add_to_column (GtkDialog * wind, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    const gchar * p = entry_get_text (GTK_ENTRY(col_entry));
    double c = string_to_double ((gpointer)p);
    update_entry_double (GTK_ENTRY(col_entry), c);
    add_to_last_col (c, data);
  }
  destroy_this_dialog (wind);
}

/*!
  \fn void add_to_column (gpointer data)

  \brief add constant to column - creating the dialog

  \param data the associated data pointer
*/
void add_to_column (gpointer data)
{
  GtkWidget * wind;
  GtkWidget * box;
  GtkWidget * hbox;
  GtkWidget * lab;
  gchar * str;
  qint * dat = (qint *)data;
#ifdef NEW_ANA
  wind = dialogmodal ("Add constant to column", GTK_WINDOW(get_project_by_id(dat -> a) -> analysis[dat -> b].curves[dat -> c] -> window));
#else
  wind = dialogmodal ("Add constant to column", GTK_WINDOW(get_project_by_id(dat -> a) -> curves[dat -> b][dat -> c] -> window));
#endif
  gtk_dialog_add_button (GTK_DIALOG (wind), "Apply", GTK_RESPONSE_APPLY);
  box = dialog_get_content_area (wind);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, TRUE, TRUE, 0);

  str = g_strdup_printf ("Add Constant to Last Column");
  lab = gtk_label_new (str);
  gtk_widget_set_size_request (lab, 200, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, TRUE, FALSE, 0);
  col_entry = create_entry (G_CALLBACK(adjust_value), 100, 15, FALSE, NULL);
  gtk_entry_set_alignment (GTK_ENTRY(col_entry), 1.0);
  update_entry_double (GTK_ENTRY(col_entry), 1.0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, col_entry, FALSE, FALSE, 0);
  run_this_gtk_dialog (wind, G_CALLBACK(run_add_to_column), data);
}

/*!
  \fn G_MODULE_EXPORT void run_multiply_column (GtkDialog * wind, gint response_id, gpointer data)

  \brief multiply column by constant - running the dialog

  \param wind the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_multiply_column (GtkDialog * wind, gint response_id, gpointer data)
{
  if (response_id == GTK_RESPONSE_APPLY)
  {
    const gchar * p = entry_get_text (GTK_ENTRY(col_entry));
    double c = string_to_double ((gpointer)p);
    update_entry_double (GTK_ENTRY(col_entry), c);
    multiply_last_col (c, data);
  }
  destroy_this_dialog (wind);
}

/*!
  \fn void multiply_column (gpointer data)

  \brief multiply column by constant - creating dialog

  \param data the associated data pointer
*/
void multiply_column (gpointer data)
{
  GtkWidget * wind;
  GtkWidget * box;
  GtkWidget * hbox;
  GtkWidget * lab;
  gchar * str;
  qint * dat = (qint *)data;
  wind = dialogmodal ("Multiply Column by Constant", GTK_WINDOW(get_project_by_id(dat -> a) -> curves[dat -> b][dat -> c] -> window));
  gtk_dialog_add_button (GTK_DIALOG (wind), "Apply", GTK_RESPONSE_APPLY);
  box = dialog_get_content_area (wind);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, box, hbox, TRUE, TRUE, 0);

  str = g_strdup_printf ("Multiply Last Column by Constant");
  lab = gtk_label_new (str);
  gtk_widget_set_size_request (lab, 200, -1);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, TRUE, FALSE, 0);
  col_entry = create_entry (G_CALLBACK(adjust_value), 100, 15, FALSE, NULL);
  gtk_entry_set_alignment (GTK_ENTRY(col_entry), 1.0);
  update_entry_double (GTK_ENTRY(col_entry), 1.0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, col_entry, FALSE, FALSE, 0);
  run_this_gtk_dialog (wind, G_CALLBACK(run_multiply_column), data);
}

/*!
  \fn GMenu * insert_place ()

  \brief create the insertion submenu
*/
GMenu * insert_place ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Before", "data-pop.insert.before", NULL, NULL, IMG_STOCK, (gpointer)GO_UP, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "After", "data-pop.insert.after", NULL, NULL, IMG_STOCK, (gpointer)GO_DOWN, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * insert_data ()

  \brief create the insert data submenu
*/
GMenu * insert_data ()
{
  GMenu * menu = g_menu_new ();
  append_submenu (menu, "Insert Row(s)", insert_place());
  return menu;
}

/*!
  \fn GMenu * delete_data ()

  \brief create the delete data submenu
*/
GMenu * delete_data ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Delete Row(s)", "data-pop.delete", NULL, NULL, IMG_STOCK, (gpointer)LIST_REM, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu* cell_actions ()

  \brief create the cell actions submenu
*/
GMenu* cell_actions ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Copy Selected Row(s)", "data-pop.copy", NULL, NULL, IMG_STOCK, (gpointer)EDITC, FALSE, FALSE, FALSE, NULL);
  g_menu_append_section (menu, NULL, (GMenuModel*)insert_data());
  g_menu_append_section (menu, NULL, (GMenuModel*)delete_data());
  return menu;
}

/*!
  \fn GMenu * cell_title ()

  \brief create the cell based operations subemu
*/
GMenu * cell_title ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Cell Based Operations", "None", NULL, NULL, IMG_NONE, NULL, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * column_actions ()

  \brief create the column actions submenu
*/
GMenu * column_actions ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Add Constant to Last Column", "data-pop.add", NULL, NULL, IMG_STOCK, (gpointer)LIST_ADD, FALSE, FALSE, FALSE, NULL);
  append_menu_item (menu, "Multiply Last Column by Constant", "data-pop.mul", NULL, NULL, IMG_STOCK, (gpointer)FCLOSE, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * column_title ()

  \brief create the column based operations submenu
*/
GMenu * column_title ()
{
  GMenu * menu = g_menu_new ();
  append_menu_item (menu, "Column Based Operations", "None", NULL, NULL, IMG_NONE, NULL, FALSE, FALSE, FALSE, NULL);
  return menu;
}

/*!
  \fn GMenu * data_menu ()

  \brief create the curve data edition popup menu elements
*/
GMenu * data_menu ()
{
  GMenu * menu = g_menu_new ();
  g_menu_append_section (menu, NULL, (GMenuModel*)cell_title());
  g_menu_append_section (menu, NULL, (GMenuModel*)cell_actions());
  g_menu_append_section (menu, NULL, (GMenuModel*)column_title());
  g_menu_append_section (menu, NULL, (GMenuModel*)column_actions());
  return menu;
}

/*!
  \fn G_MODULE_EXPORT void data_pop_action (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief curve data edition popup menu actions callbacks

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void data_pop_action (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  gchar * name = g_strdup_printf ("%s", g_action_get_name(G_ACTION(action)));
  if (g_strcmp0 (name, "copy") == 0)
  {
    copy_content (data);
  }
  else if (g_strcmp0 (name, "insert.before") == 0)
  {
    insert_cell (data);
  }
  else if (g_strcmp0 (name, "insert.after") == 0)
  {
    insert_cell (data);
  }
  else if (g_strcmp0 (name, "delete") == 0)
  {
    delete_cell (data);
  }
  else if (g_strcmp0 (name, "add") == 0)
  {
    add_to_column (data);
  }
  else if (g_strcmp0 (name, "mul") == 0)
  {
    multiply_column (data);
  }
}

#ifdef GTK4
/*!
  \fn void data_popup_menu (GtkWidget * top_level, double x, double y, gpointer data)

  \brief create curve data edition popup menu GTK4

  \param top_level the top level widget
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
void data_popup_menu (GtkWidget * top_level, double x, double y, gpointer data)
#else
/*!
  \fn void data_popup_menu (GtkWidget * top_level, GdkEvent * event, gpointer data)

  \brief create curve data edition popup menu GTK3

  \param top_level the top level widget
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
void data_popup_menu (GtkWidget * top_level, GdkEvent * event, gpointer data)
#endif
{
  GSimpleActionGroup * data_popup = g_simple_action_group_new ();
  GSimpleAction * pop_data[6];
  pop_data[0]  = g_simple_action_new ("copy", NULL);
  pop_data[1]  = g_simple_action_new ("insert.before", NULL);
  pop_data[2]  = g_simple_action_new ("insert.after", NULL);
  pop_data[3]  = g_simple_action_new ("delete", NULL);
  pop_data[4]  = g_simple_action_new ("add", NULL);
  pop_data[5]  = g_simple_action_new ("mul", NULL);

  g_signal_connect (pop_data[0], "activate", G_CALLBACK(data_pop_action), NULL);
  g_signal_connect (pop_data[1], "activate", G_CALLBACK(data_pop_action), GINT_TO_POINTER(0));
  g_signal_connect (pop_data[2], "activate", G_CALLBACK(data_pop_action), GINT_TO_POINTER(1));
  g_signal_connect (pop_data[3], "activate", G_CALLBACK(data_pop_action), NULL);
  g_signal_connect (pop_data[4], "activate", G_CALLBACK(data_pop_action), data);
  g_signal_connect (pop_data[5], "activate", G_CALLBACK(data_pop_action), data);

  int i;
  for (i=0; i<6; i++) g_action_map_add_action (G_ACTION_MAP(data_popup), G_ACTION(pop_data[i]));

  GtkWidget * menu;
#ifdef GTK4
  menu = gtk_popover_menu_new_from_model_full ((GMenuModel *)data_menu(), GTK_POPOVER_MENU_NESTED);
  gtk_widget_set_parent  (menu, top_level);
#else
  menu = gtk_menu_new_from_model ((GMenuModel *)data_menu ());
#endif
  gtk_widget_insert_action_group (menu, "data-pop", G_ACTION_GROUP(data_popup));

#ifdef GTK4
  gtk_widget_set_size_request (menu, -1, 240);
  pop_menu_at_pointer (menu, x, y);
#else
  pop_menu_at_pointer (menu, event);
#endif
}

#ifdef GTK4
/*!
  \fn void data_button_event (GtkWidget * data_tree, double event_x, double event_y, guint event_button, guint event_type, gpointer data)

  \brief curve data edition mouse button event GTK4

  \param data_tree the GtkWidget sending the signal
  \param event_x x position
  \param event_y y position
  \param event_button event button
  \param event_type event type
  \param data the associated data pointer
*/
void data_button_event (GtkWidget * data_tree, double event_x, double event_y, guint event_button, guint event_type, gpointer data)
#else
/*!
  \fn void data_button_event (GtkWidget * data_tree, GdkEvent * event, guint event_button, guint event_type, gpointer data)

  \brief curve data edition mouse button event GTK3

  \param data_tree the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param event_button event button
  \param event_type event type
  \param data the associated data pointer
*/
void data_button_event (GtkWidget * data_tree, GdkEvent * event, guint event_button, guint event_type, gpointer data)
#endif
{
  if (event_type == GDK_BUTTON_PRESS)
  {
    get_tree_data (data_tree);
    if (event_button == 3)
    {
#ifdef GTK4
      data_popup_menu ((GtkWidget *)get_top_level(data_tree), event_x, event_y, data);
#else
      data_popup_menu (data_tree, event, data);
#endif
    }
  }
  else if (event_type == GDK_BUTTON_RELEASE)
  {
    g_list_foreach (lrows, (GFunc)gtk_tree_path_free, NULL);
    g_list_free (lrows);
  }
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void on_data_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief mouse button pressed callback GTK4

  \param gesture the GtkGesture sending the signal
  \param n_press the number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_data_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  data_button_event (gtk_event_controller_get_widget ((GtkEventController*)gesture), x, y,
                     gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_PRESS, data);
}

/*!
  \fn G_MODULE_EXPORT void on_data_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief mouse button released callback GTK4

  \param gesture the GtkGesture sending the signal
  \param n_press the number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_data_button_released (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  data_button_event (gtk_event_controller_get_widget ((GtkEventController*)gesture), x, y,
                     gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), GDK_BUTTON_RELEASE, data);
}
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_data_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)

  \brief on button event in the curve data edition window

  \param widget the GtkWidget sending the signal
  \param event
  \param data
*/
G_MODULE_EXPORT gboolean on_data_button_event (GtkWidget * widget, GdkEvent * event, gpointer data)
{
  GdkEventButton * bevent = (GdkEventButton*)event;
  data_button_event (widget, event, bevent -> button, bevent -> type, data);
  return FALSE;
}
#endif

/*!
  \fn static void fill_data_model (GtkListStore * store, project * this_proj, int b, int c)

  \brief fill curve data list store

  \param store the GtkListStore to fill
  \param this_proj the target project
  \param b the calculation id
  \param c the curve id
*/
static void fill_data_model (GtkListStore * store, project * this_proj, int b, int c)
{
  GtkTreeIter datalevel;
  int i;
#ifdef NEW_ANA
  for (i=0; i<this_proj -> analysis[b].curves[c] -> ndata; i++)
  {
    gtk_list_store_append (store, & datalevel);
    gtk_list_store_set (store, & datalevel,
                        0, i+1,
                        1, this_proj -> analysis[b].curves[c] -> data[0][i],
                        2, this_proj -> analysis[b].curves[c] -> data[1][i], -1);
  }
#else
  for (i=0; i<this_proj -> curves[b][c] -> ndata; i++)
  {
    gtk_list_store_append (store, & datalevel);
    gtk_list_store_set (store, & datalevel,
                        0, i+1,
                        1, this_proj -> curves[b][c] -> data[0][i],
                        2, this_proj -> curves[b][c] -> data[1][i], -1);
  }
#endif
}

/*!
  \fn GtkWidget * setview (project * this_proj, int b, int c)

  \brief create the curve data tree store

  \param this_proj the target project
  \param b the target calculation
  \param c the target curve
*/
GtkWidget * setview (project * this_proj, int b, int c)
{
  GtkWidget * dataview;
  GtkListStore * datamodel;
  GType type[3]={G_TYPE_INT, G_TYPE_DOUBLE, G_TYPE_DOUBLE};
  GtkTreeViewColumn * datacol[3];
  GtkCellRenderer * datacel[3];
  GtkTreeSelection * dataselect;
//  ColRGBA col[3];
  gchar * name[3];
  int i;
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = this_proj -> analysis[b].curves[c];
#else
  this_curve = this_proj -> curves[b][c];
#endif
  name[0]=g_strdup_printf (" ");
  name[1]=g_strdup_printf ("%s", this_curve -> axis_title[0]);
  name[2]=g_strdup_printf ("%s\n%s", prepare_for_title(this_proj -> name), this_curve -> name);
  datamodel = gtk_list_store_newv (3, type);
  fill_data_model (datamodel, this_proj, b, c);
  dataview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(datamodel));
  //gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(dataview), TRUE);
  for (i=0; i<3; i++)
  {
    datacel[i] = gtk_cell_renderer_text_new();
 /*   if (i == 1)
    {
      col[i].red = 62965;
      col[i].green = 62965;
      col[i].blue  = 62965;
    }
    else
    {
      col[i].red = 51914;
      col[i].green = 57825;
      col[i].blue = 65535;
    }
    g_object_set (datacel[i], "cell-background-gdk", & col[i], "cell-background-set", TRUE, NULL); */
    if (i > 0)
    {
      g_object_set (datacel[i], "editable", TRUE, NULL);
      gtk_cell_renderer_set_alignment (datacel[i], 0.5, 0.5);
      this_curve -> idcol[i-1].a = this_proj -> id;
      this_curve -> idcol[i-1].b = b;
      this_curve -> idcol[i-1].c = c;
      this_curve -> idcol[i-1].d = i;
      g_signal_connect (G_OBJECT(datacel[i]), "edited", G_CALLBACK(edit_cell), & this_curve -> idcol[i-1]);
    }
    datacol[i] = gtk_tree_view_column_new_with_attributes(name[i], datacel[i], "text", i, NULL);
    gtk_tree_view_column_set_alignment (datacol[i], 0.5);
    gtk_tree_view_column_set_resizable (datacol[i], TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(dataview), datacol[i]);
    if (i > 0)
    {
      gtk_tree_view_column_set_min_width (datacol[i], 100);
    }
  }
  g_object_unref (datamodel);
  dataselect = gtk_tree_view_get_selection (GTK_TREE_VIEW(dataview));
  gtk_tree_selection_set_mode (dataselect, GTK_SELECTION_MULTIPLE);
#ifdef GTK3
  g_signal_connect (G_OBJECT(dataview), "button_press_event", G_CALLBACK(on_data_button_event), & this_curve -> idcol[1]);
#else
  add_widget_gesture_and_key_action (dataview, "datab-context-click", G_CALLBACK(on_data_button_pressed), & this_curve -> idcol[1],
                                               "datab-context-release", G_CALLBACK(on_data_button_released), & this_curve -> idcol[1],
                                                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif
  gtk_tree_view_expand_all (GTK_TREE_VIEW(dataview));
  return dataview;
}

/*!
  \fn void cancel_changes (GtkWidget * widg, gpointer data)

  \brief cancel curve data edition

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
void cancel_changes (GtkWidget * widg, gpointer data)
{
  tint * id = (tint *)data;
  destroy_this_widget (widg);
#ifdef NEW_ANA
  get_project_by_id(id -> a) -> analysis[id -> b].curves[id -> c] -> datatree = NULL;
#else
  get_project_by_id(id -> a) -> curves[id -> b][id -> c] -> datatree = NULL;
#endif
}

/*!
  \fn G_MODULE_EXPORT void cancel_but (GtkButton * but, gpointer data)

  \brief cancel curve data edition button callback

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void cancel_but (GtkButton * but, gpointer data)
{
  cancel_changes (get_top_level(GTK_WIDGET(but)), data);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean cancel_win (GtkWindow * win, gpointer data)

  \brief cancel curve data edition callback GTK4

  \param win the GtkWindow sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean cancel_win (GtkWindow * win, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean cancel_win (GtkWidget * win, GdkEvent * event, gpointer data)

  \brief cancel curve data edition callback GTK3

  \param win the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean cancel_win (GtkWidget * win, GdkEvent * event, gpointer data)
#endif
{
  cancel_changes ((GtkWidget *)win, data);
  return TRUE;
}

/*!
  \fn G_MODULE_EXPORT void validate_changes (GtkButton * but, gpointer data)

  \brief apply data edition changes

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void validate_changes (GtkButton * but, gpointer data)
{
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id(id -> a);
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = this_proj -> analysis[id -> b].curves[id -> c];
#else
  this_curve = this_proj -> curves[id -> b][id -> c];
#endif
  curve_model = gtk_tree_view_get_model(GTK_TREE_VIEW(this_curve -> datatree));
  sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(this_curve -> datatree));
  gtk_tree_selection_select_all (sel);
  get_tree_data (this_curve -> datatree);
  if (nrows != this_curve -> ndata)
  {
    this_curve -> ndata = nrows;
    g_free (this_curve -> data[0]);
    this_curve -> data[0] = allocdouble (nrows);
    g_free (this_curve -> data[1]);
    this_curve -> data[1] = allocdouble (nrows);
  }
  nrows = 0;
  g_list_foreach (lrows, (GFunc)save_row, data);
  cancel_changes (get_top_level(GTK_WIDGET(but)), data);
  update_curves ();
}

/*!
  \fn void edit_data (gpointer data)

  \brief create edit curve data dialog

  \param data the associated data pointer
*/
void edit_data (gpointer data)
{
  GtkWidget * win;
  tint * id = (tint *)data;
  project * this_proj = get_project_by_id(id -> a);
  Curve * this_curve;
#ifdef NEW_ANA
  this_curve = this_proj -> analysis[id -> b].curves[id -> c];
#else
  this_curve = this_proj -> curves[id -> b][id -> c];
#endif
  if (this_curve -> datatree != NULL)
  {
    win = get_top_level(GTK_WIDGET(this_curve -> datatree));
  }
  else
  {
    gchar * str = g_strdup_printf ("%s - %s", prepare_for_title (this_proj -> name), this_curve -> name);
    win = create_win (str, MainWindow, FALSE, TRUE);
    g_free (str);
    gtk_window_set_default_size (GTK_WINDOW(win), 300, 600);
    GtkWidget * vbox = create_vbox (BSEP);
    this_curve -> datatree = setview (this_proj, id -> b, id -> c);
    GtkWidget * scrol = create_scroll (vbox, -1, 570, GTK_SHADOW_ETCHED_IN);
    add_container_child (CONTAINER_SCR, scrol, this_curve -> datatree);
    GtkWidget * hbox = create_hbox (0);
    GtkWidget * butc = create_button ("Cancel", IMG_STOCK, CANCEL, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(cancel_but), data);
    GtkWidget * butv = create_button ("Apply", IMG_STOCK, EXECUTE, -1, -1, GTK_RELIEF_NORMAL, G_CALLBACK(validate_changes), data);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, butc, TRUE, TRUE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, butv, TRUE, TRUE, 0);
    gtk_widget_set_size_request (hbox, -1, 40);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, TRUE, TRUE, 0);
    add_container_child (CONTAINER_WIN, win, vbox);
    add_gtk_close_event (win, G_CALLBACK(cancel_win), data);
  }
  show_the_widgets (win);
}
