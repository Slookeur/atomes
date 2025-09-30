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
* @file workspace.c
* @short Implementation of the workspace tree view \n
         Associated callbacks
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'workspace.c'
*
* Contains:
*
 - Implementation of the workspace tree view
 - Associated callbacks

*
* List of functions:

  int find_calc_by_path (GtkTreeView * treeview, GtkTreePath * path);
  int find_proj_by_path (GtkTreePath * path);

  G_MODULE_EXPORT gboolean on_workspace_button_event (GtkWidget * widget, GdkEventButton * event, gpointer data);

  void add_project (GtkTreeStore * store, int i);
  void correct_this_window_title (GtkWidget * win, gchar * str);
  void workspace_menu (GtkWidget * tree, gpointer event, double x, double y);
  void create_workspace ();
  void add_project_to_workspace ();
  void remove_project_from_workspace (int id);

  static void fill_workspace (GtkTreeStore * store);

  G_MODULE_EXPORT void activate_project (GtkWidget * widg, gpointer data);
  G_MODULE_EXPORT void workspace_ondc (GtkTreeView * treeview,
                                       GtkTreePath * path,
                                       GtkTreeViewColumn  * col,
                                       gpointer data);
  G_MODULE_EXPORT void change_project_name (GtkWidget * wid, gpointer edata);
  G_MODULE_EXPORT void workspace_popup (GtkGesture * gesture, int n_press, double x, double y, gpointer data);

  GtkWidget * create_workspace_tree ();

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"

extern void workinfo (project * this_proj, int i);
extern GtkWidget * work_menu (int p, int c);

GdkPixbuf * pix = NULL;
GtkWidget * img = NULL;
GtkWidget * hbox = NULL;
GtkWidget * vbox = NULL;
GtkWidget * lab = NULL;
GtkWidget * lap = NULL;
GtkWidget * eap = NULL;
gchar * labtmp = NULL;
gchar * laptmp = NULL;
GtkWidget * worktree = NULL;
GtkTreeStore * workstore = NULL;
GtkTreeIter worklevel;
GtkTreeIter * piter = NULL;
GtkTreeIter * witer = NULL;
GtkTreePath ** prpath = NULL;
GtkTreePath * wpath = NULL;
GdkPixbuf * wpix = NULL;
gchar * wchar = NULL;
int projects_in_workspace = 0;

char * work_menu_items[NITEMS-2] = {"Workspace                ",
                                    "Settings                 ",
                                   //"OpenGL - 3D              ",
                                    "Visualization            ",
                                   //"Calc. visualization      ",
                                    "Analysis                 ",
                                    "g(r)/G(r)                ",
                                    "S(q) from g(r)           ",
                                    "S(q) from Debye equation ",
                                    "g(r) from FFT[S(q) Debye]",
                                    "Bonding information      ",
                                    "Angle distribution       ",
                                    "Ring statistics          ",
                                    "Chain statistics         ",
                                    "Spherical harmonics      ",
                                    "Mean Square Displacement "};

/*!
  \fn void add_project (GtkTreeStore * store, int i)

  \brief add project to the GtkTreeStore of the workspace

  \param store the GtkTreeStore
  \param i the id of the project to add
*/
void add_project (GtkTreeStore * store, int i)
{
  int j;
  gchar * tmp;
  GtkTreeIter steplevel;
  GtkTreeIter optslevel;
  gtk_tree_store_append (store, & piter[i], & worklevel);
  if (i == activep)
  {
    tmp = g_strdup_printf ("<b>%s</b>", active_project -> name);
  }
  else
  {
    tmp = g_strdup_printf ("%s", get_project_by_id(i) -> name);
  }
  gtk_tree_store_set (store, & piter[i], 0, THETD, 1, tmp, 2, -1, -1);
  prpath[i] = gtk_tree_model_get_path (GTK_TREE_MODEL(store), & piter[i]);
  g_free (tmp);
  gtk_tree_store_append (store, & steplevel, & piter[i]);
  gtk_tree_store_set (store, & steplevel, 0, SETTING, 1, work_menu_items[1], 2, -3, -1);
  // OpenGL
  gtk_tree_store_append (store, & steplevel, & piter[i]);
  gtk_tree_store_set (store, & steplevel, 0, OGLM, 1, work_menu_items[2], 2, -2, -1);
  //gtk_tree_store_append (store, & optslevel, & steplevel);
  //gtk_tree_store_set (store, & optslevel, 0, OGLM, 1, work_menu_items[3], 2, -1, -1);
  //gtk_tree_store_append (store, & optslevel, & steplevel);
  //gtk_tree_store_set (store, & optslevel, 0, OGLC, 1, work_menu_items[4], 2, -1, -1);
  // Calculations
  gtk_tree_store_append (store, & steplevel, & piter[i]);
  gtk_tree_store_set (store, & steplevel, 0, RUN, 1, work_menu_items[3], 2, -1, -1);
  for (j=0; j<NCALCS-2; j++)
  {
    if (j < NCALCS-3 || get_project_by_id(i) -> steps > 1)
    {
      gtk_tree_store_append (store, & optslevel, & steplevel);
      gtk_tree_store_set (store, & optslevel, 0, gdk_pixbuf_new_from_file(graph_img[j], NULL), 1, work_menu_items[4+j], 2, j, -1);
    }
  }
  gtk_tree_view_expand_to_path (GTK_TREE_VIEW(worktree), gtk_tree_model_get_path(GTK_TREE_MODEL(store), & worklevel));
  projects_in_workspace ++;
}

/*!
  \fn static void fill_workspace (GtkTreeStore * store)

  \brief fill the workspace tree store

  \param store the GtkTreeStore
*/
static void fill_workspace (GtkTreeStore * store)
{
  int i;

  gtk_tree_store_append (store, & worklevel, NULL);
  gtk_tree_store_set (store, & worklevel, 0, THETD, 1, work_menu_items[0], 2, -4, -1);

  if (piter != NULL) g_free (piter);
  if (prpath != NULL) g_free (prpath);
  if (nprojects > 0)
  {
    piter = g_malloc (nprojects*sizeof*piter);
    prpath = g_malloc (nprojects*sizeof*prpath);
  }
  for (i=0; i<nprojects; i++)
  {
    add_project (store, i);
  }
}

/*!
  \fn G_MODULE_EXPORT void activate_project (GtkWidget * widg, gpointer data)

  \brief activate a project

  \param widg the GtkWidget sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void activate_project (GtkWidget * widg, gpointer data)
{
  int id = GPOINTER_TO_INT (data);

  if (activep < nprojects)
  {
    gtk_tree_store_set (workstore, & piter[activep], 1, active_project -> name, -1);
  }
  active_project_changed (id);

  gchar * tmp = g_strdup_printf ("<b>%s</b>", active_project -> name);
  gtk_tree_store_set (workstore, & piter[id], 1, tmp, -1);
  g_free (tmp);
}

/*!
  \fn int find_calc_by_path (GtkTreeView * treeview, GtkTreePath * path)

  \brief find the calculation using the GtkTreePath in the GtkTreeView

  \param treeview the GtkTreeView
  \param path the GtkTreePath
*/
int find_calc_by_path (GtkTreeView * treeview, GtkTreePath * path)
{
  int i;
  GtkTreeIter iter;
  GtkTreeModel * model = gtk_tree_view_get_model (treeview);
  if (gtk_tree_model_get_iter (model, & iter, path))
  {
    gtk_tree_model_get (model, & iter, 2, & i, -1);
    return i;
  }
  return -1;
}


/*!
  \fn int find_proj_by_path (GtkTreePath * path)

  \brief find the project id using the GtkTreePath

  \param path the GtkTreePath
*/
int find_proj_by_path (GtkTreePath * path)
{
  int i, j;

  i = -1;
  for (j=0; j<nprojects; j++)
  {
    if (gtk_tree_path_compare (prpath[j], path) <= 0) i = j;
  }
  return i;
}

/*!
  \fn G_MODULE_EXPORT void workspace_ondc (GtkTreeView * treeview,
                                           GtkTreePath * path,
                                           GtkTreeViewColumn  * col,
                                           gpointer data)

  \brief do something after Double Click in the workspace tree
    - DC on a project name: activate this project
    - DC elsewhere: display related information

  \param treeview the GtkTreeView
  \param path the GtkTreePath
  \param col the GtkTreeViewColumn
  \param data the associated data pointer
*/
G_MODULE_EXPORT void workspace_ondc (GtkTreeView * treeview,
                                     GtkTreePath * path,
                                     GtkTreeViewColumn  * col,
                                     gpointer data)
{
  int i, j;
  GtkTreeModel * model;
  GtkTreeIter iter;
  model = gtk_tree_view_get_model (treeview);
  gboolean was = FALSE;
#ifdef DEBUG
  g_debug ("WORKSPACE_ONDC: activep= %d", activep);
#endif
  if (gtk_tree_model_get_iter (model, & iter, path))
  {
#ifdef DEBUG
  g_debug ("WORKSPACE_ONDC: in workspace");
#endif
    for (i=0; i<nprojects; i++)
    {
      if (gtk_tree_path_compare (path, prpath[i]) == 0)
      {
        if (i != activep) activate_project (NULL, GINT_TO_POINTER(i));
        was = TRUE;
      }
    }
    if (! was)
    {
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: not an activation");
#endif
      if (wpath != NULL && witer != NULL && wchar != NULL)
      {
        gtk_tree_store_set (GTK_TREE_STORE(model), witer, 1, wchar, -1);
      }
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: saving old data");
#endif
      wpath = gtk_tree_path_copy (path);
      witer = gtk_tree_iter_copy (& iter);
      gtk_tree_model_get (model, & iter, 1, & wchar, -1);
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: modifying data");
#endif
      gchar * tmp = g_strdup_printf ("<b>%s</b>", wchar);
      gtk_tree_store_set (GTK_TREE_STORE(model), & iter, 1, tmp, -1);
      g_free (tmp);
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: cleaning view");
      g_debug ("WORKSPACE_ONDC: wchar = %s", wchar);
#endif
      gtk_tree_model_get (model, & iter, 2, & i, -1);
      i += 4;
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: creating view id= %d", i);
#endif
      j = find_proj_by_path (path);
#ifdef DEBUG
      g_debug ("WORKSPACE_ONDC: for project= %d", j);
#endif
      if (j < 0) j = activep;
      if (i == 2) prep_model (j);
      workinfo (get_project_by_id(j), i);
    }
  }
}

/*!
  \fn void correct_this_window_title (GtkWidget * win, gchar * str)

  \brief use new title for GtkWindow, providing it exists

  \param win the GtkWindow
  \param str the new title
*/
void correct_this_window_title (GtkWidget * win, gchar * str)
{
  if (win)
  {
    if (GTK_IS_WIDGET(win))
    {
      gtk_window_set_title (GTK_WINDOW (win), str);
    }
  }
  if (str) g_free (str);
}

/*!
  \fn G_MODULE_EXPORT void change_project_name (GtkWidget * wid, gpointer edata)

  \brief change project name

  \param wid the GtkWidgent sending the signal
  \param edata the associated data pointer
*/
G_MODULE_EXPORT void change_project_name (GtkWidget * wid, gpointer edata)
{
  int i, j, k;
  gchar * tmp;
  i = GPOINTER_TO_INT (edata);
  tmp = g_strdup_printf ("Please enter a new name for project N°%d", i);
  project * this_proj = get_project_by_id(i);
  tmp = cask(tmp, "Project name", i, this_proj -> name, MainWindow);
  if (tmp != NULL)
  {
    this_proj -> name = g_strdup_printf ("%s", tmp);
    if (i == activep)
    {
      g_free (tmp);
      tmp = g_strdup_printf ("<b>%s</b>", this_proj -> name);
    }
    gtk_tree_store_set (workstore, & piter[i], 1, tmp, -1);
    g_free (tmp);
    // Need to update other project windows names !!!
    gchar * tmp_title = prepare_for_title (this_proj -> name);
    if (this_proj -> modelgl != NULL)
    {
      if (this_proj -> modelgl -> win  != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> win, g_strdup_printf ("%s - 3D view - [%s mode]", tmp_title, mode_name[this_proj -> modelgl -> mode]));
      }
      gchar * win_title[2]={"Atom(s) configuration", "Clone(s) configuration"};
      for (j=0; j<2; j++)
      {
        if (this_proj -> modelgl -> model_win[j] != NULL)
        {
          correct_this_window_title (this_proj -> modelgl -> model_win[j] -> win, g_strdup_printf ("%s - %s", win_title[j], tmp_title));
        }
      }
      if (this_proj -> modelgl -> coord_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> coord_win -> win, g_strdup_printf ("Environments configuration - %s", tmp_title));
      }
      if (this_proj -> modelgl -> atom_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> atom_win -> win, g_strdup_printf ("Model edition - %s", tmp_title));
      }
      if (this_proj -> modelgl -> cell_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> cell_win -> win, g_strdup_printf ("Cell edition - %s", tmp_title));
      }
      if (this_proj -> modelgl -> opengl_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> opengl_win -> win, g_strdup_printf ("OpenGL material aspect and light settings - %s", tmp_title));
      }
      if (this_proj -> modelgl -> spiner != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> spiner -> win, g_strdup_printf ("%s - spin", tmp_title));
      }
      if (this_proj -> modelgl -> player != NULL)
      {
        j = this_proj -> modelgl -> anim -> last -> img -> step;
        correct_this_window_title (this_proj -> modelgl -> player -> win, g_strdup_printf ("%s - player - step %d", tmp_title, j));
      }
      if (this_proj -> modelgl -> measure_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> measure_win -> win, g_strdup_printf ("%s - measures", tmp_title));
      }
      if (this_proj -> modelgl -> gradient_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> gradient_win -> win, g_strdup_printf ("%s - background settings", tmp_title));
      }
      if (this_proj -> modelgl -> box_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> box_win -> win, g_strdup_printf ("%s - box settings", tmp_title));
      }
      if (this_proj -> modelgl -> axis_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> axis_win -> win, g_strdup_printf ("%s - axis settings", tmp_title));
      }
      if (this_proj -> modelgl -> rep_win != NULL)
      {
        correct_this_window_title (this_proj -> modelgl -> rep_win -> win, g_strdup_printf ("%s - OpenGL camera set-up", tmp_title));
      }
    }
    g_free (tmp_title);
    for (j=0; j<NGRAPHS; j++)
    {
      if (this_proj -> initok[j])
      {
        for (k=0; k<this_proj -> numc[j]; k++)
        {
          if (this_proj -> curves[j][k] -> window != NULL)
          {
            correct_this_window_title (this_proj -> curves[j][k] -> window, g_strdup_printf ("%s - %s", prepare_for_title(this_proj -> name), this_proj -> curves[j][k] -> name));
          }
        }
      }
    }
    if (activep == i)
    {
      correct_this_window_title (MainWindow, g_strdup_printf ("%s - %s", PACKAGE, prepare_for_title(active_project -> name)));
      correct_this_window_title (curvetoolbox, g_strdup_printf ("Toolboxes - %s", prepare_for_title(active_project -> name)));
    }
  }
}

/*!
  \fn void workspace_menu (GtkWidget * tree, gpointer event, double x, double y)

  \brief popup the workspace contextual menu in workspace tree view

  \param tree the GtkTreeView
  \param event the event
  \param x x position
  \param y y position
*/
void workspace_menu (GtkWidget * tree, gpointer event, double x, double y)
{
  GtkWidget * menu;
  GtkTreePath * mpath;
  int this_calc = -1;
  gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(tree), (gint)x, (gint)y, & mpath, NULL, NULL, NULL);
  if (mpath != NULL)
  {
    activew = find_proj_by_path (mpath);
    this_calc = find_calc_by_path (GTK_TREE_VIEW(tree), mpath);
  }
  else
  {
    activew = activep;
  }
  menu = work_menu (activew, this_calc);
#ifdef GTK3
  pop_menu_at_pointer (menu, (GdkEvent *)event);
#else
  gtk_widget_set_parent (menu, MainWindow);
  pop_menu_at_pointer (menu, x, y);
#endif
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void workspace_popup (GtkGesture * gesture, int n_press, double x, double y, gpointer data)

  \brief GTK4 popover workspace menu at current location

  \param gesture the GtkGesture sending the signal
  \param n_press number of times it was pressed
  \param x x position
  \param y y position
  \param data the associated data pointer
*/
G_MODULE_EXPORT void workspace_popup (GtkGesture * gesture, int n_press, double x, double y, gpointer data)
{
  if (gtk_gesture_single_get_current_button ((GtkGestureSingle * )gesture) == GDK_BUTTON_SECONDARY)
  {
    workspace_menu ((GtkWidget *)data, NULL, x, y);
  }
}
#else
/*!
  \fn G_MODULE_EXPORT gboolean on_workspace_button_event (GtkWidget * widget, GdkEventButton * event, gpointer data)

  \brief GTK3 button event on workspace to display contextual menu

  \param widget the GtkWidget sending the signal
  \param event the associated GtkButton
  \param data the associated data pointer
*/
G_MODULE_EXPORT gboolean on_workspace_button_event (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  if (event -> type == GDK_BUTTON_PRESS && event -> button == 3)
  {
    workspace_menu (widget, (gpointer)event, event -> x, event -> y);
  }
  return FALSE;
}
#endif

void workspace_set_visible (GtkTreeViewColumn * col,
                            GtkCellRenderer   * renderer,
                            GtkTreeModel      * mod,
                            GtkTreeIter       * iter,
                            gpointer          data)
{
  gtk_cell_renderer_set_visible (renderer, FALSE);
}

/*!
  \fn GtkWidget * create_workspace_tree ()

  \brief create the workspace tree store
*/
GtkWidget * create_workspace_tree ()
{
  GtkTreeViewColumn * col;
  GtkCellRenderer * renderer;
  GtkTreeSelection * workselect;

  worktree = destroy_this_widget (worktree);
  workstore = gtk_tree_store_new (3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
  fill_workspace (workstore);
  worktree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(workstore));
  g_object_unref (workstore);


  col = gtk_tree_view_column_new ();

  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_tree_view_column_pack_start (col, renderer, FALSE);
  gtk_tree_view_column_set_attributes (col, renderer, "pixbuf", 0, NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start (col, renderer, TRUE);
  gtk_tree_view_column_set_attributes (col, renderer, "markup", 1, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (col, renderer, TRUE);
  gtk_tree_view_column_set_attributes (col, renderer, "markup", 2, NULL);
  gtk_tree_view_column_set_cell_data_func (col, renderer, workspace_set_visible, NULL, NULL);

  gtk_tree_view_append_column(GTK_TREE_VIEW(worktree), col);

  workselect = gtk_tree_view_get_selection (GTK_TREE_VIEW(worktree));
  gtk_tree_selection_set_mode (workselect,  GTK_SELECTION_SINGLE);
  gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(worktree), FALSE);
  //g_signal_connect (G_OBJECT(workselect), "changed", G_CALLBACK(selework), (gpointer)worktree);
  g_signal_connect (G_OBJECT(worktree), "row-activated", G_CALLBACK(workspace_ondc), NULL);
#ifdef GTK3
  g_signal_connect (G_OBJECT(worktree), "button-press-event", G_CALLBACK(on_workspace_button_event), NULL);
#else
  add_widget_gesture_and_key_action (worktree, "workspace-context-click", G_CALLBACK(workspace_popup), (gpointer)worktree,
                                               NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif
  gtk_tree_view_expand_all (GTK_TREE_VIEW(worktree));
  return worktree;
}

/*!
  \fn void create_workspace ()

  \brief create the workspace
*/
void create_workspace ()
{
  add_container_child (CONTAINER_SCR, MainScrol[0], create_workspace_tree ());
  lab = NULL;
  witer = NULL;
  wpath = NULL;
  show_the_widgets (MainScrol[0]);
}

/*!
  \fn void add_project_to_workspace ()

  \brief add project(s) to the workspace tree
*/
void add_project_to_workspace ()
{
  if (worktree != NULL && workstore != NULL && nprojects > 0)
  {
    int i;
    if (projects_in_workspace > 0)
    {
      GtkTreeIter ** tmpiter;
      tmpiter = g_malloc (projects_in_workspace*sizeof*tmpiter);
      for (i=0; i<projects_in_workspace; i++)
      {
        tmpiter[i] = gtk_tree_iter_copy (& piter[i]);
      }
      if (piter != NULL) g_free (piter);
      if (prpath != NULL) g_free (prpath);
      piter = g_malloc ((projects_in_workspace+1)*sizeof*piter);
      prpath = g_malloc ((projects_in_workspace+1)*sizeof*prpath);
      for (i=0; i<projects_in_workspace; i++)
      {
        piter[i] = * gtk_tree_iter_copy (tmpiter[i]);
        prpath[i] = gtk_tree_model_get_path (GTK_TREE_MODEL(workstore), & piter[i]);
        gtk_tree_store_set (workstore, & piter[i], 1, get_project_by_id(i) -> name, -1);
      }
      if (tmpiter != NULL) g_free (tmpiter);
    }
    else
    {
      if (prpath != NULL) g_free (prpath);
      if (piter != NULL) g_free (piter);
      piter = g_malloc ((projects_in_workspace+1)*sizeof*piter);
      prpath = g_malloc ((projects_in_workspace+1)*sizeof*prpath);
    }
    add_project (workstore, activep);
    newspace = FALSE;
  }
  else
  {
    create_workspace ();
  }
  if (nprojects == 0)
  {
    widget_set_sensitive (worktree, 0);
  }
  else
  {
    widget_set_sensitive (worktree, 1);
  }
}

/*!
  \fn void remove_project_from_workspace (int id)

  \brief remove project from workspace

  \param id the id of the project to be remove
*/
void remove_project_from_workspace (int id)
{
  int i, j;
  GtkTreeIter ** tmpiter;

  if (worktree != NULL && workstore != NULL && nprojects > 0)
  {
    if (wpath != NULL)
    {
      i = find_proj_by_path (wpath);
      if (i == id)
      {
         wpath = NULL;
         wchar = NULL;
      }
    }
    if (projects_in_workspace > 1)
    {
      tmpiter = g_malloc ((projects_in_workspace-1)*sizeof*tmpiter);
      j = -1;
      for (i=0; i<projects_in_workspace; i++)
      {
        if (i != id)
        {
          j++;
          tmpiter[j] = gtk_tree_iter_copy (& piter[i]);
        }
      }
      gtk_tree_store_remove (workstore, & piter[id]);
      g_free (piter);
      g_free (prpath);
      projects_in_workspace --;
      piter = g_malloc (projects_in_workspace*sizeof*piter);
      prpath = g_malloc (projects_in_workspace*sizeof*prpath);
      j = -1;
      for (i=0; i<projects_in_workspace+1; i++)
      {
        if (i != id)
        {
          j ++;
          piter[j] = * gtk_tree_iter_copy (tmpiter[j]);
          prpath[j] = gtk_tree_model_get_path (GTK_TREE_MODEL(workstore), & piter[j]);
          gtk_tree_store_set (workstore, & piter[j], 1, get_project_by_id(i) -> name, -1);
        }
      }
      g_free (tmpiter);
    }
    else
    {
      gtk_tree_store_remove (workstore, & piter[id]);
      if (prpath != NULL) g_free (prpath);
      prpath = NULL;
      if (piter != NULL) g_free (piter);
      piter = NULL;
      projects_in_workspace --;
      widget_set_sensitive (worktree, 0);
      witer = NULL;
      wchar = NULL;
    }
  }
}
