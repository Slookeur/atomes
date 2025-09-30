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
* @file w_measures.c
* @short Functions to create the measurement(s) window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_measures.c'
*
* Contains:
*

 - The functions to create the measurement(s) window

*
* List of functions:

  int get_selection_type (glwin * view);

  G_MODULE_EXPORT gboolean measure_tree_selection_event (GtkWidget * widget, GdkEventButton * event, gpointer data);
  G_MODULE_EXPORT gboolean close_measure_event (GtkWindow * widg, gpointer data);
  G_MODULE_EXPORT gboolean close_measure_event (GtkWidget * widg, GdkEvent * event, gpointer data);

  gchar * create_measure_label (glwin * view, int sid);

  void measure_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void measure_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void dihedral_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void measure_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void fill_bond_model_row (int p, int a, int b, GtkTreeStore * store);
  void fill_angle_model_row (int p, int a, int b, int c, GtkTreeStore * store);
  void fill_dihedral_model_row (int p, int a, int b, int c, int d, GtkTreeStore * store);
  void update_selection_tree (glwin * view, int sid, int mid);
  void update_label_selection (glwin * view, int sid);
  void measure_tree_button_event (GtkWidget * widget, double event_x, double event_y, guint event_button, gpointer data);

  G_MODULE_EXPORT void measure_tree_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data);
  G_MODULE_EXPORT void close_ml (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void measure_labels (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void window_measures (GtkWidget * widg, gpointer data);

  GtkWidget * create_selection_tree (glwin * view, int sid, int mid);
  GtkWidget * measurment_tab (glwin * view, int sid, int mid);

*/

#include "global.h"
#include "glview.h"

extern void bonds_loop (glwin * view, int id, int pi, GtkTreeStore * store);
extern void angles_loop (glwin * view, int id, int pi, GtkTreeStore * store);
extern void dihedrals_loop (glwin * view, int id, int pi, GtkTreeStore * store);
extern int num_bonds (int i);
extern int num_angles (int i);
extern int num_dihedrals (int i);

int bond_id;
int angle_id;
int dihedral_id;

/*!
  \fn int get_selection_type (glwin * view)

  \brief get selection type

  \param view the target glwin
*/
int get_selection_type (glwin * view)
{
  if (view -> mode == EDITION && view -> selection_mode == NSELECTION-1)
  {
    return 1;
  }
  else if (view -> atom_win)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/*!
  \fn void fill_bond_model_row (int p, int a, int b, GtkTreeStore * store)

  \brief fill bond tree store row

  \param p the project id
  \param a 1st atom id
  \param b 2nd atom id
  \param store the GtkTreeStore to fill
*/
void fill_bond_model_row (int p, int a, int b, GtkTreeStore * store)
{
  GtkTreeIter bond_level;
  gchar * str_a, * str_b, * str_c;
  project * this_proj = get_project_by_id(p);
  atom * at, * bt;
  gboolean pbc = this_proj -> cell.pbc;
  distance dist;
  int s = this_proj -> modelgl -> anim -> last -> img -> step;
  at = & this_proj -> atoms[s][a];
  bt = & this_proj -> atoms[s][b];
  bond_id ++;
  dist = distance_3d (& this_proj -> cell, (this_proj -> cell.npt) ? s : 0, at, bt);
  str_a = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[at -> sp], a+1);
  str_b = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[bt -> sp], b+1);
  str_c = g_strdup_printf("%.3lf", dist.length);
  if (pbc)
  {
    gtk_tree_store_append (store, & bond_level, NULL);
    gtk_tree_store_set (store, & bond_level, 0, bond_id,
                                             1, str_a,
                                             2, str_b,
                                             3, str_c,
                                             4, dist.pbc, -1);
  }
  else
  {
    gtk_tree_store_append (store, & bond_level, NULL);
    gtk_tree_store_set (store, & bond_level, 0, bond_id,
                                             1, str_a,
                                             2, str_b,
                                             3, str_c, -1);
  }
  g_free (str_a);
  g_free (str_b);
  g_free (str_c);
}

/*!
  \fn void fill_angle_model_row (int p, int a, int b, int c, GtkTreeStore * store)

  \brief fill angle tree store row

  \param p the project id
  \param a 1st atom id
  \param b 2nd atom id
  \param c 3rd atom id
  \param store the GtkTreeStore to fill
*/
void fill_angle_model_row (int p, int a, int b, int c, GtkTreeStore * store)
{
  GtkTreeIter angle_level;
  gchar * str_a, * str_b, * str_c, * str_d;
  project * this_proj = get_project_by_id(p);
  atom * at, * bt, * ct;
  gboolean pbc = this_proj -> cell.pbc;
  angle theta;
  int s = this_proj -> modelgl -> anim -> last -> img -> step;
  at = & this_proj -> atoms[s][a];
  bt = & this_proj -> atoms[s][b];
  ct = & this_proj -> atoms[s][c];
  angle_id ++;
  theta = angle_3d (& this_proj -> cell, (this_proj -> cell.npt) ? s : 0, at, bt, ct);
  str_a = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[at -> sp], a+1);
  str_b = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[bt -> sp], b+1);
  str_c = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[ct -> sp], c+1);
  str_d = g_strdup_printf("%.2lf", theta.angle);
  if (pbc)
  {
    gtk_tree_store_append (store, & angle_level, NULL);
    gtk_tree_store_set (store, & angle_level, 0, angle_id,
                                              1, str_a,
                                              2, str_b,
                                              3, str_c,
                                              4, str_d,
                                              5, theta.pbc, -1);
  }
  else
  {
    gtk_tree_store_append (store, & angle_level, NULL);
    gtk_tree_store_set (store, & angle_level, 0, angle_id,
                                              1, str_a,
                                              2, str_b,
                                              3, str_c,
                                              4, str_d, -1);
  }
  g_free (str_a);
  g_free (str_b);
  g_free (str_c);
  g_free (str_d);
}

/*!
  \fn void fill_dihedral_model_row (int p, int a, int b, int c, int d, GtkTreeStore * store)

  \brief fill dihedral tree store row

  \param p the project id
  \param a 1st atom id
  \param b 2nd atom id
  \param c 3rd atom id
  \param d 4th atom id
  \param store the GtkTreeStore to fill
*/
void fill_dihedral_model_row (int p, int a, int b, int c, int d, GtkTreeStore * store)
{
  GtkTreeIter dihedral_level;
  gchar * str_a, * str_b, * str_c, * str_d, * str_e;
  project * this_proj = get_project_by_id(p);
  atom * at, * bt, * ct, * dt;
  gboolean pbc = this_proj -> cell.pbc;
  angle phi;
  int s = this_proj -> modelgl -> anim -> last -> img -> step;
  at = & this_proj -> atoms[s][a];
  bt = & this_proj -> atoms[s][b];
  ct = & this_proj -> atoms[s][c];
  dt = & this_proj -> atoms[s][d];
  dihedral_id ++;
  phi = dihedral_3d (& this_proj -> cell, (this_proj -> cell.npt) ? s : 0, at, bt, ct, dt);
  str_a = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[at -> sp], a+1);
  str_b = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[bt -> sp], b+1);
  str_c = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[ct -> sp], c+1);
  str_d = g_strdup_printf("%s<sub>%d</sub>", this_proj -> chemistry -> label[dt -> sp], d+1);
  str_e = g_strdup_printf("%.2lf", phi.angle);
  if (pbc)
  {
    gtk_tree_store_append (store, & dihedral_level, NULL);
    gtk_tree_store_set (store, & dihedral_level, 0, dihedral_id,
                                                 1, str_a,
                                                 2, str_b,
                                                 3, str_c,
                                                 4, str_d,
                                                 5, str_e,
                                                 6, phi.pbc, -1);
  }
  else
  {
    gtk_tree_store_append (store, & dihedral_level, NULL);
    gtk_tree_store_set (store, & dihedral_level, 0, dihedral_id,
                                                 1, str_a,
                                                 2, str_b,
                                                 3, str_c,
                                                 4, str_d,
                                                 5, str_e, -1);
  }
  g_free (str_a);
  g_free (str_b);
  g_free (str_c);
  g_free (str_d);
  g_free (str_e);
}

extern ColRGBA init_color (int id, int numid);

/*!
  \fn void measure_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the measures tree store

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void measure_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i;
  tint * dat = (tint *)data;
  gtk_tree_model_get (mod, iter, dat -> b, & i, -1);
  gtk_cell_renderer_set_visible (renderer, i);
}

/*!
  \fn void measure_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the measures tree store

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void measure_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  j = 0;
  tint * dat = (tint *) data;
  gboolean docolor = FALSE;
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  project * this_proj = get_project_by_id(dat -> a);
  image * img = this_proj -> modelgl -> anim -> last -> img;
  int pi = get_selection_type (this_proj -> modelgl);
  if (img -> selected[pi] -> selected)
  {
    if ((dat -> c == 2 && img -> selected[pi] -> selected < MAX_IN_SELECTION-10) || (dat -> c < 2 && img -> selected[pi] -> selected < MAX_IN_SELECTION))
    {
      switch (dat -> c)
      {
        case 0:
          j = num_bonds (img -> selected[pi] -> selected);
          if (img -> selected[pi] -> selected_bonds[i]) docolor = TRUE;
          break;
        case 1:
          j = num_angles (img -> selected[pi] -> selected);
          if (img -> selected[pi] -> selected_angles[i]) docolor = TRUE;
          break;
        case 2:
          j = num_dihedrals (img -> selected[pi] -> selected);
          if (img -> selected[pi] -> selected_dihedrals[i]) docolor = TRUE;
          break;
      }
    }
  }
  set_renderer_color (docolor, renderer, init_color (i, j));
}

/*!
  \fn void dihedral_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void dihedral_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  tint * dat = (tint *)data;
  dat -> c = 2;
  measure_set_color (col, renderer, mod, iter, data);
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, dat -> b, & str, -1);
  g_object_set (renderer, "markup", str, NULL, NULL);
  g_free (str);
}

/*!
  \fn void measure_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void measure_set_color_and_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  tint * dat = (tint *)data;
  measure_set_color (col, renderer, mod, iter, dat);
  gchar * str = NULL;
  gtk_tree_model_get (mod, iter, dat -> b, & str, -1);
  g_object_set (renderer, "markup", str, NULL, NULL);
  g_free (str);
}

GtkWidget * create_selection_tree (glwin * view, int sid, int mid);

/*!
  \fn void update_selection_tree (glwin * view, int sid, int mid)

  \brief update measurements tree view

  \param view the target glwin
  \param sid the type of selection (0 = analysis mode, 1 = edition mode)
  \param mid the type of measurement (0 = bonds, 1 = angles, 2 = dihedrals)
*/
void update_selection_tree (glwin * view, int sid, int mid)
{
  GtkWidget * cont = gtk_widget_get_parent (view -> measure_win -> selection_tree[mid]);
  view -> measure_win -> selection_tree[mid] = destroy_this_widget (view -> measure_win -> selection_tree[mid]);
  view -> measure_win -> selection_tree[mid] = create_selection_tree (view, sid, mid);
  add_container_child (CONTAINER_SCR, cont, view -> measure_win -> selection_tree[mid]);
  show_the_widgets (cont);
}

/*!
  \fn void measure_tree_button_event (GtkWidget * widget, double event_x, double event_y, guint event_button, gpointer data)

  \brief measure tree button event

  \param widget the GtkWidget sending the signal
  \param event_x x position
  \param event_y y position
  \param event_button event buttton
  \param data the associated data pointer
*/
void measure_tree_button_event (GtkWidget * widget, double event_x, double event_y, guint event_button, gpointer data)
{
  if (event_button == 1)
  {
    tint * dat = (tint *)data;
    glwin * view = get_project_by_id(dat -> a) -> modelgl;
    int pi = get_selection_type (view);
    GtkTreeModel * measure_model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
    GtkTreePath * path;
    GtkTreeViewColumn * column;
    int i, j;
#ifdef GTK4
    int e_x, e_y;
    gtk_tree_view_convert_widget_to_bin_window_coords (GTK_TREE_VIEW(widget), event_x, event_y, & e_x, & e_y);
    if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(widget), e_x, e_y, & path, & column, & i, & j))
#else
    if (gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(widget), event_x, event_y, & path, & column, & i, & j))
#endif
    {
      GtkTreeIter row;
      if (gtk_tree_model_get_iter (measure_model, & row, path))
      {
        gtk_tree_model_get (measure_model, & row, 0, & j, -1);
        // select bonds, angles or dihedrals:
        switch (dat -> c)
        {
          case 0:
            view -> anim -> last -> img -> selected[pi] -> selected_bonds[j] = ! view -> anim -> last -> img -> selected[pi] -> selected_bonds[j];
            break;
          case 1:
            view -> anim -> last -> img -> selected[pi] -> selected_angles[j] = ! view -> anim -> last -> img -> selected[pi] -> selected_angles[j];
            break;
          case 2:
            view -> anim -> last -> img -> selected[pi] -> selected_dihedrals[j] = ! view -> anim -> last -> img -> selected[pi] -> selected_dihedrals[j];
            break;
        }
      }
    }
    view -> create_shaders[MEASU] = TRUE;
    update (view);
  }
}

#ifdef GTK3
/*!
  \fn G_MODULE_EXPORT gboolean measure_tree_selection_event (GtkWidget * widget, GdkEventButton * event, gpointer data)

  \brief measures tree view button press callback GTK3

  \param widget the GtkWidget sending the signal
  \param event the GtkEventButton triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean measure_tree_selection_event (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  if (event -> type == GDK_BUTTON_PRESS)
  {
    GdkEventButton * bevent = (GdkEventButton*)event;
    measure_tree_button_event (widget, bevent -> x, bevent -> y, bevent -> button, data);
  }
  return FALSE;
}
#else
/*!
  \fn G_MODULE_EXPORT void measure_tree_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief measures tree view button press callback GTK4

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void measure_tree_button_pressed (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  measure_tree_button_event (gtk_event_controller_get_widget ((GtkEventController*)gesture), x, y, gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture), data);
}
#endif

/*!
  \fn GtkWidget * create_selection_tree (glwin * view, int sid, int mid)

  \brief create the measurements selection tree

  \param view the target glwin
  \param sid the type of selection (0 = analysis mode, 1 = edition mode)
  \param mid the type of measurement (0 = bonds, 1 = angles, 2 = dihedrals)
*/
GtkWidget * create_selection_tree (glwin * view, int sid, int mid)
{
  int i, j;
  GtkTreeViewColumn * sel_col[7];
  GtkCellRenderer * sel_cell[7];
  GtkTreeStore * sel_model;
  gchar * ctitle[3][7]={{"Id", "Atom 1", "Atom 2", "Distance [Å]", "Using PBC", "NULL", "NULL"},
                        {"Id", "Atom 1", "Atom 2", "Atom 3", "θ [°]", "Using PBC", "NULL"},
                        {"Id", "Atom 1", "Atom 2", "Atom 3", "Atom 4", "ϕ [°]", "Using PBC"}};
  gchar * ctype[3][7]={{"text", "text", "text", "text", "active", "text", "text"},
                       {"text", "text", "text", "text", "text", "active", "text"},
                       {"text", "text", "text", "text", "text", "text", "active"}};
  int tree_dim[3]={4, 5, 6};
  GType col_type[3][7]= {{G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING},
                         {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING},
                         {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN}};
  if (get_project_by_id(view -> proj) -> cell.pbc)
  {
    sel_model = gtk_tree_store_newv (tree_dim[mid]+1, col_type[mid]);
    j = 1;
  }
  else
  {
    sel_model = gtk_tree_store_newv (tree_dim[mid], col_type[mid]);
    j = 0;
  }

  GtkWidget * selection_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(sel_model));
  for (i=0; i<4+j+mid; i++)
  {
    if ((mid == 0 && i < 4) || (mid == 1 && i < 5) || (mid == 2 && i < 6))
    {
      sel_cell[i] = gtk_cell_renderer_text_new();
      sel_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[mid][i], sel_cell[i], ctype[mid][i], i, NULL);
      if (i == 0) gtk_tree_view_column_set_visible (sel_col[i], FALSE);
    }
    else if ((mid == 0 && i == 4) || (mid == 1 && i == 5) || (mid == 2 && i == 6))
    {
      sel_cell[i] = gtk_cell_renderer_toggle_new ();
      sel_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[mid][i], sel_cell[i], ctype[mid][i], i, NULL);
    }
    gtk_tree_view_column_set_alignment (sel_col[i], 0.5);
    gtk_tree_view_append_column(GTK_TREE_VIEW(selection_tree), sel_col[i]);
    if (i > 0 && i < 4+mid)
    {
      if (mid < 2)
      {
        gtk_tree_view_column_set_cell_data_func (sel_col[i], sel_cell[i], measure_set_color_and_markup, & view -> colorp[i][mid], NULL);
      }
      else
      {
        gtk_tree_view_column_set_cell_data_func (sel_col[i], sel_cell[i], dihedral_set_color_and_markup, & view -> colorp[i][0], NULL);
      }
    }
    else if (i > 0)
    {
      gtk_tree_view_column_set_cell_data_func (sel_col[i], sel_cell[i], measure_set_visible, & view -> colorp[i][0], NULL);
    }
  }
  image * img = view -> anim -> last -> img;
  if (img -> selected[sid] -> selected < MAX_IN_SELECTION)
  {
    if ( (mid == 0 && img -> selected[sid] -> selected > 1)
      || (mid == 1 && img -> selected[sid] -> selected > 2)
      || (mid == 2 && img -> selected[sid] -> selected > 3 && img -> selected[sid] -> selected < MAX_IN_SELECTION-10))
    {
      if (mid == 0)
      {
        bond_id = -1;
        bonds_loop (view, 2, sid, GTK_TREE_STORE(sel_model));
#ifdef GTK4
        add_widget_gesture_and_key_action (selection_tree, "bond-tree-pressed", G_CALLBACK(measure_tree_button_pressed), & view -> colorp[0][mid],
                                           NULL, NULL, NULL, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL);
#else
        g_signal_connect (G_OBJECT(selection_tree), "button_press_event", G_CALLBACK(measure_tree_selection_event), & view -> colorp[0][mid]);
#endif
      }
      else if (mid == 1)
      {
        angle_id = -1;
        angles_loop (view, 2, sid, GTK_TREE_STORE(sel_model));
#ifdef GTK4
        add_widget_gesture_and_key_action (selection_tree, "angle-tree-pressed", G_CALLBACK(measure_tree_button_pressed), & view -> colorp[0][mid],
                                           NULL, NULL, NULL, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL);
#else
        g_signal_connect (G_OBJECT(selection_tree), "button_press_event", G_CALLBACK(measure_tree_selection_event), & view -> colorp[0][mid]);
#endif
      }
      else if (mid == 2)
      {
        dihedral_id = -1;
        dihedrals_loop (view, 2, sid, GTK_TREE_STORE(sel_model));
/*#ifdef GTK4
        add_widget_gesture_and_key_action (selection_tree, "dihedral-tree-pressed", G_CALLBACK(measure_tree_button_pressed), & view -> colorp[0][mid],
                                           NULL, NULL, NULL, NULL, NULL, NULL,
                                           NULL, NULL, NULL, NULL, NULL, NULL);
#else
        g_signal_connect (G_OBJECT(selection_tree), "button_press_event", G_CALLBACK(measure_tree_selection_event), & view -> colorp[0][mid]);
#endif*/
      }
    }
  }
  GtkTreeSelection * selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(selection_tree));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(selection_tree));
  return selection_tree;
}

/*!
  \fn GtkWidget * measurment_tab (glwin * view, int sid, mid)

  \brief create measurement tab

  \param view the target glwin
  \param sid the type of selection (0 = analysis mode, 1 = edition mode)
  \param mid the type of measurement (0 = bonds, 1 = angles, 2 = dihedrals)
*/
GtkWidget * measurment_tab (glwin * view, int sid, int mid)
{
  GtkWidget * scrollsets = create_scroll (NULL, 350+50*mid, 350, GTK_SHADOW_NONE);
  view -> measure_win -> selection_tree[mid] = create_selection_tree (view, sid, mid);
  add_container_child (CONTAINER_SCR, scrollsets, view -> measure_win -> selection_tree[mid]);
  return scrollsets;
}

/*!
  \fn gchar * create_measure_label (glwin * view, int sid)

  \brief create the text information for the number of atom(s) in selection

  \param view the target glwin
  \param sid the type of selection (0 = analysis mode, 1 = edition mode)
*/
gchar * create_measure_label (glwin * view, int sid)
{
  gchar * str;
  image * img = view -> anim -> last -> img;
  if (img -> selected[sid] -> selected < MAX_IN_SELECTION)
  {
    str = g_strdup_printf ("\tAtom(s) in selection<sup>*</sup> :\t<b>%d</b>", img -> selected[sid] -> selected);
  }
  else
  {
    str = g_strdup_printf ("\tSorry too many [%d] atoms in selection<sup>*</sup>", img -> selected[sid] -> selected);
  }
  return str;
}

/*!
  \fn void update_label_selection (glwin * view, int sid)

  \brief update the text information for the number of atoms/measures in selection

  \param view the target glwin
  \param sid the type of selection (0 = analysis mode, 1 = edition mode)
*/
void update_label_selection (glwin * view, int sid)
{
  gtk_label_set_text (GTK_LABEL(view -> measure_win -> label), create_measure_label(view, sid));
  gtk_label_set_use_markup (GTK_LABEL(view -> measure_win -> label), TRUE);
}

extern GtkWidget * labels_tab (glwin * view, int id);

/*!
  \fn G_MODULE_EXPORT void close_ml (GtkButton * but, gpointer data)

  \brief measurements style edition window close button

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void close_ml (GtkButton * but, gpointer data)
{
  destroy_this_widget (GTK_WIDGET(data));
}

/*!
  \fn G_MODULE_EXPORT void measure_labels (GtkButton * but, gpointer data)

  \brief measurements style edition window

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void measure_labels (GtkButton * but, gpointer data)
{
  glwin * view = (glwin *) data;
  gchar * str = g_strdup_printf ("%s - measures - style", get_project_by_id(view -> proj) -> name);
  GtkWidget * win = create_win (str, view -> measure_win -> win, TRUE, FALSE);
  //gtk_widget_set_size_request (win, -1, -1480);
  g_free (str);
  GtkWidget * vbox = create_vbox (BSEP);
  add_container_child (CONTAINER_WIN, win, vbox);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, labels_tab (view, (is_atom_win_active(view) || (view -> mode == EDITION && view -> selection_mode == NSELECTION-1)) ? 4 : 3), FALSE, FALSE, 0);
  GtkWidget * hbox = create_hbox (0);
  GtkWidget * close_but = create_button ("Close", IMG_NONE, NULL, 100, -1, GTK_RELIEF_NORMAL, G_CALLBACK(close_ml), win);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, close_but, FALSE, TRUE, 150);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 20);
  add_gtk_close_event (win, G_CALLBACK(hide_this_window), NULL);
  show_the_widgets (win);
  gtk_window_set_resizable (GTK_WINDOW(win), FALSE);
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT gboolean close_measure_event (GtkWindow * widg, gpointer data)

  \brief measurements window close event callback GTK4

  \param data the associated data pointer
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_measure_event (GtkWindow * widg, gpointer data)
#else
/*!
  \fn G_MODULE_EXPORT gboolean close_measure_event (GtkWidget * widg, GdkEvent * event, gpointer data)

  \brief measurements window close event callback GTK3

  \param widg the GtkWidget sending the signal
  \param event the GdkEvent triggering the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean close_measure_event (GtkWidget * widg, GdkEvent * event, gpointer data)
#endif
{
  glwin * view = (glwin *) data;
  view -> measure_win -> win = destroy_this_widget (view -> measure_win -> win);
  int i;
  for (i=0; i<3; i++)
  {
    view -> measure_win -> selection_tree[i] = NULL;
  }
  g_free (view -> measure_win);
  view -> measure_win = NULL;
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void window_measures (GtkWidget * widg, gpointer data)

  \brief create the measurements window

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void window_measures (GtkWidget * widg, gpointer data)
{
  glwin * view = (glwin *) data;
  if (view -> measure_win == NULL)
  {
    view -> measure_win = g_malloc0 (sizeof*view -> measure_win);
    gchar * str = g_strdup_printf ("%s - measures", get_project_by_id(view -> proj) -> name);
    int pi = get_selection_type (view);
    view -> measure_win -> win = create_win (str, view -> win, FALSE, FALSE);
    gtk_widget_set_size_request (view -> measure_win -> win, 450, 500);
    g_free (str);
    GtkWidget * vbox = create_vbox (BSEP);
    add_container_child (CONTAINER_WIN, view -> measure_win -> win, vbox);
    GtkWidget * notebook = gtk_notebook_new ();
    view -> measure_win -> label = markup_label(create_measure_label(view, pi), -1, 50, 0.0, 0.5);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, view -> measure_win -> label, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, notebook, FALSE, FALSE, 0);
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), measurment_tab (view, pi, 0), gtk_label_new ("Distances"));
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), measurment_tab (view, pi, 1), gtk_label_new ("Angles"));
    gtk_notebook_append_page (GTK_NOTEBOOK(notebook), measurment_tab (view, pi, 2), gtk_label_new ("Dihedrals"));

    str = g_strdup_printf ("You can select up to %d atoms for both inter-atomic distance(s) and angle(s),",  MAX_IN_SELECTION-1);
    append_comments (vbox, "<sup>*</sup>", str);
    g_free (str);
    str = g_strdup_printf ("and up to %d atoms for dihedral angle(s) measurement(s)", MAX_IN_SELECTION-11);
    append_comments (vbox, " ", str);
    g_free (str);
    GtkWidget * hbox = create_hbox (0);
    GtkWidget * but = create_button ("Font and style", IMG_NONE, NULL, 150, -1, GTK_RELIEF_NORMAL, G_CALLBACK(measure_labels), view);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, but, FALSE, TRUE, 150);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 10);
    add_gtk_close_event (view -> measure_win -> win,  G_CALLBACK(close_measure_event), view);
    show_the_widgets (view -> measure_win -> win);
  }
  else
  {
    show_the_widgets (view -> measure_win -> win);
  }
}
