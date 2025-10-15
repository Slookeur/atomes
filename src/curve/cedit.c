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
* @file cedit.c
* @short Initialization of the curve layout edition dialog
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cedit.c'
*
* Contains:
*

- The initialization of the curve layout edition dialog

*
* List of functions:

  void prepbox (gpointer data);
  void set_set (int a, int b, int c, gpointer data);
  void set_visible_curve_data (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void edit_curve (gpointer data);

  static void fill_proj_model (curve_edition * cedit, GtkTreeStore * store, gpointer data);

  G_MODULE_EXPORT void run_curve_edit (GtkDialog * dial, gint response_id, gpointer data);

  GtkWidget * create_projects_tree (gpointer data);

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <cairo-svg.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
// #include "cedit.h"

extern G_MODULE_EXPORT void set_data_aspect (GtkComboBox * box, gpointer data);
extern G_MODULE_EXPORT void update_axis (GtkComboBox * widg, gpointer data);
extern void action_to_plot (gpointer data);
extern gboolean was_not_added (ExtraSets * sets, int a, int b, int c);
extern G_MODULE_EXPORT void choose_set (GtkComboBox * box, gpointer data);
extern GtkWidget * create_org_list (gpointer data);
extern GtkWidget * create_tab_1 (curve_edition * cedit, gpointer data);
extern GtkWidget * create_tab_2 (curve_edition * cedit, gpointer data);
extern GtkWidget * create_tab_3 (curve_edition * cedit, gpointer data);
extern GtkWidget * create_tab_4 (curve_edition * cedit, gpointer data);

char * lapos[2]={"x: ", "y: "};

/*!
  \fn void prepbox (gpointer data)

  \brief prepare the curve selection combo box

  \param data the associate data pointer
*/
void prepbox (gpointer data)
{
  int i, n, o, p;
  gchar * str;
  project * extra_proj;
  project * this_proj = get_project_by_id(((tint *)data) -> a);
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  str = g_strdup_printf ("%s - %s", prepare_for_title(this_proj -> name), this_curve -> name);
  combo_text_append (cedit -> setcolorbox, str);
  g_free (str);
  CurveExtra * ctmp = this_curve -> extrac -> first;
  for ( i=0 ; i < this_curve -> extrac -> extras ; i++ )
  {
    n = ctmp -> id.a;
    o = ctmp -> id.b;
    p = ctmp -> id.c;
    extra_proj = get_project_by_id(n);
#ifdef NEW_ANA
    str = g_strdup_printf ("%s - %s", prepare_for_title(extra_proj -> name), extra_proj -> analysis[o]curves[p] -> name);
#else
    str = g_strdup_printf ("%s - %s", prepare_for_title(extra_proj -> name), extra_proj -> curves[o][p] -> name);
#endif
    combo_text_append (cedit -> setcolorbox, str);
    g_free (str);
    if (ctmp -> next != NULL) ctmp = ctmp -> next;
  }
  if (this_curve -> extrac -> extras > 0)
  {
    widget_set_sensitive (cedit -> setcolorbox, 1);
  }
  else
  {
    widget_set_sensitive (cedit -> setcolorbox, 0);
  }
  gtk_widget_set_size_request (cedit -> setcolorbox, -1, 30);
  combo_set_active (cedit -> setcolorbox, 0);
  g_signal_connect (G_OBJECT(cedit -> setcolorbox), "changed", G_CALLBACK(choose_set), data);
}

/*!
  \fn void set_set (int a, int b, int c, gpointer data)

  \brief addjust widgets to handle the new curve

  \param a project id
  \param b calculation id
  \param c curve id
  \param data the associated data pointer
*/
void set_set (int a, int b, int c, gpointer data)
{
  Curve * this_curve = get_curve_from_pointer (data);
  curve_edition * cedit = this_curve -> curve_edit;
  cedit -> setcolorbox = destroy_this_widget (cedit -> setcolorbox);
  cedit -> setcolorbox = create_combo ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, cedit -> thesetbox, cedit -> setcolorbox, FALSE, FALSE, 0);
  show_the_widgets (cedit -> setcolorbox);
  activeg = ((tint *)data) -> a;
#ifdef NEW_ANA
  action_to_plot (& get_project_by_id(a) -> analysis[b].idcc[c]);
#else
  action_to_plot (& get_project_by_id(a) -> idcc[b][c]);
#endif
  prepbox (data);
  choose_set (GTK_COMBO_BOX(cedit -> setcolorbox), data);
  cedit -> orgtree = destroy_this_widget (cedit -> orgtree);
  add_container_child (CONTAINER_SCR, cedit -> datascroll, create_org_list(data));
  show_the_widgets (cedit -> orgtree);
  widget_set_sensitive (cedit -> orgtree, this_curve -> extrac -> extras);
}

/*!
  \fn static void fill_proj_model (curve_edition * cedit, GtkTreeStore * store, gpointer data)

  \brief fill project(s) / curves tree store

  \param cedit the target curve_edition pointer
  \param store the GtkTreeStore to fill
  \param data the associated data pointer
*/
static void fill_proj_model (curve_edition * cedit, GtkTreeStore * store, gpointer data)
{
  GtkTreeIter projlevel;
  GtkTreeIter calclevel;
  GtkTreeIter curvelevel;
  project * this_proj;
  int i, j, k;
  int start, end, step;
  gboolean append;
  int pid = ((tint *)data) -> a;
  int rid = ((tint *)data) -> b;
  int cid = ((tint *)data) -> c;

   /* Append a top level row and leave it empty */
  if (cedit -> ppath != NULL) g_free (cedit -> ppath);
  if (cedit -> cpath != NULL) g_free (cedit -> cpath);
  cedit -> ppath = g_malloc0 (nprojects*sizeof*cedit -> ppath);
  cedit -> cpath = g_malloc0 (nprojects*sizeof*cedit -> cpath);

  for (i=0; i<nprojects; i++)
  {
    this_proj = get_project_by_id(i);
    gtk_tree_store_append (store, & projlevel, NULL);
    gtk_tree_store_set (store, & projlevel, 0, 0, 1, prepare_for_title(this_proj -> name), 2, TRUE, 3, -1, -1);
    cedit -> ppath[i] = gtk_tree_model_get_path ((GtkTreeModel *)store, & projlevel);
#ifdef NEW_ANA
    for (j=0; j<this_proj -> analysis[rid].c_sets; j++)
    {
      k = this_proj -> analysis[rid].compat_id[j];
#else
    if (rid == 0 || rid == 3)
    {
      start = 0;
      end = 4;
      step = 3;
    }
    else if (rid == 1 || rid == 2)
    {
      start = 1;
      end = 3;
      step = 1;
    }
    else
    {
      start = rid;
      end = start + 1;
      step = 1;
    }
    for (j=start; j<end; j=j+step)
    {
#endif
#ifdef NEW_ANA
      if (this_proj -> analysis[k].init_ok)
       {
        gtk_tree_store_append (store, & calclevel, & projlevel);
        gtk_tree_store_set (store, & calclevel, 0, 0, 1, graph_name[k], 2, TRUE, 3, -1, -1);
        if (j == 0)
        {
          cedit -> cpath[i] = gtk_tree_model_get_path ((GtkTreeModel *)store, & calclevel);
        }
        for (l = 0 ; l < this_proj -> analysis[k].numc ; l++)
        {
          append = FALSE;
          if (i != pid && this_proj -> analysis[k].curves[l] -> ndata != 0)
          {
            append = TRUE;
          }
          else if (((i != pid) || (j != rid || k != cid)) && this_proj -> analysis[k].curves[l] -> ndata != 0)
          {
            append = TRUE;
          }
          if (append)
          {
            gtk_tree_store_append (store, & curvelevel, & calclevel);
            gtk_tree_store_set (store, & curvelevel,
                                0, 1,
                                1, this_proj -> analysis[k].curves[l] -> name,
                                2, ! was_not_added (get_curve_from_pointer(data) -> extrac, i, k, l),
                                3, i,
                                4, k,
                                5, l, -1);
          }
        }
      }
#else
      if (this_proj -> initok[j])
      {
        gtk_tree_store_append (store, & calclevel, & projlevel);
        gtk_tree_store_set (store, & calclevel, 0, 0, 1, graph_name[j], 2, TRUE, 3, -1, -1);
        if (j == start)
        {
          cedit -> cpath[i] = gtk_tree_model_get_path ((GtkTreeModel *)store, & calclevel);
        }
        for (k = 0 ; k < this_proj -> numc[j] ; k++)
        {
          append = FALSE;
          if (i != pid && this_proj -> curves[j][k] -> ndata != 0)
          {
            append = TRUE;
          }
          else if (((i != pid) || (j != rid || k != cid)) && this_proj -> curves[j][k] -> ndata != 0)
          {
            append = TRUE;
          }
          if (append)
          {
            gtk_tree_store_append (store, & curvelevel, & calclevel);
            gtk_tree_store_set (store, & curvelevel,
                                0, 1,
                                1, this_proj -> curves[j][k] -> name,
                                2, ! was_not_added (get_curve_from_pointer(data) -> extrac, i, j, k),
                                3, i,
                                4, j,
                                5, k, -1);
          }
        }
      }
#endif
    }
  }
}

GtkTreeStore * projmodel;

/*!
  \fn void toggle_curve (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief show / hide curve cellrenderer toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void toggle_curve (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  int i, j, k;
  gboolean status;
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(projmodel), & iter, path);
  gtk_tree_model_get (GTK_TREE_MODEL(projmodel), & iter, 2, & status, 3, & i, 4, & j, 5, & k, -1);
  gtk_tree_store_set (projmodel, & iter, 2, ! status, -1);
  set_set (i, j, k, data);
}

/*!
  \fn void set_visible_curve_data (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer

  \param col the tree view column
  \param renderer the cell renderer
  \param mod the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
void set_visible_curve_data (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int m;
  gtk_tree_model_get (mod, iter, 0, & m, -1);
  gtk_cell_renderer_set_visible (renderer, m);
}

/*!
  \fn GtkWidget * create_projects_tree (curve_edition * cedit, gpointer data)

  \brief curve edition create the project(s) / curves tree model

  \param cedit the target curve_edition pointer
  \param data the associated data pointer
*/
GtkWidget * create_projects_tree (curve_edition * cedit, gpointer data)
{
  int i;
  GtkTreeViewColumn * projcol[6];
  GtkCellRenderer * projcell[6];
  gchar * col_title[6] = {" ", "Data sets", "Select", " ", " ", " "};
  gchar * ctype[6]={"text", "text", "active", "text", "text", "text"};
  GType col_type[6] = {G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT};
  projmodel = gtk_tree_store_newv (6, col_type);
  cedit -> projtree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(projmodel));
  for (i=0; i<6; i++)
  {
    if (i == 2)
    {
      projcell[i] = gtk_cell_renderer_toggle_new ();
      projcol[i] = gtk_tree_view_column_new_with_attributes(col_title[i],  projcell[i], ctype[i], i, NULL);
      g_signal_connect (G_OBJECT(projcell[i]), "toggled", G_CALLBACK(toggle_curve), data);
      gtk_tree_view_column_set_cell_data_func (projcol[i], projcell[i], set_visible_curve_data, NULL, NULL);
    }
    else
    {
      projcell[i] = gtk_cell_renderer_text_new();
      projcol[i] =  gtk_tree_view_column_new_with_attributes(col_title[i], projcell[i], ctype[i], i, NULL);
    }
    gtk_tree_view_append_column(GTK_TREE_VIEW(cedit -> projtree), projcol[i]);
    if (i == 0 || i > 2) gtk_tree_view_column_set_visible (projcol[i], FALSE);
  }
  fill_proj_model (cedit, projmodel, data);
  g_object_unref (projmodel);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(cedit -> projtree));
  return cedit -> projtree;
}

/*!
  \fn G_MODULE_EXPORT void run_curve_edit (GtkDialog * dial, gint response_id, gpointer data)

  \brief curve edition dialog callback

  \param dial the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_curve_edit (GtkDialog * dial, gint response_id, gpointer data)
{
  destroy_this_dialog (dial);
  if (get_curve_from_pointer(data) -> curve_edit)
  {
    g_free (get_curve_from_pointer(data) -> curve_edit);
    get_curve_from_pointer(data) -> curve_edit = NULL;
  }
}

/*!
  \fn void edit_curve (gpointer data)

  \brief create the curve edition dialog

  \param data the associated data pointer
*/
void edit_curve (gpointer data)
{
  GtkWidget * ebox;
  GtkWidget * enoote;
  GtkWidget * dbox;
  GtkWidget * scrollsets;

  activer = ((tint *)data) -> b;
  activec = ((tint *)data) -> c;
// Axis data
  Curve * this_curve = get_curve_from_pointer (data);
  this_curve -> curve_edit = g_malloc0(sizeof*this_curve -> curve_edit);
  curve_edition * this_cedit = this_curve -> curve_edit;

  this_cedit -> win = dialogmodal ("Edit curve", GTK_WINDOW(this_curve -> window));
  gtk_window_set_resizable (GTK_WINDOW (this_cedit -> win), FALSE);
#ifndef GTK4
  gtk_window_set_icon (GTK_WINDOW (this_cedit -> win), THETD);
#endif
  ebox = dialog_get_content_area (this_cedit -> win);
  enoote = gtk_notebook_new ();
  add_box_child_start (GTK_ORIENTATION_VERTICAL, ebox, enoote, FALSE, FALSE, 0);

// The first tab of the notebook
  gtk_notebook_append_page (GTK_NOTEBOOK(enoote), create_tab_1 (this_cedit, data), gtk_label_new ("Graph"));
//  gtk_notebook_set_tab_label (GTK_NOTEBOOK (enoote), gtk_notebook_get_nth_page (GTK_NOTEBOOK (enoote), 0), gtk_label_new ("Graph"));

// The second tab of the notebook
  gtk_notebook_append_page (GTK_NOTEBOOK(enoote), create_tab_2 (this_cedit, data), gtk_label_new ("Data"));
//  gtk_notebook_set_tab_label (GTK_NOTEBOOK (enoote), gtk_notebook_get_nth_page (GTK_NOTEBOOK (enoote), 1), gtk_label_new ("Data"));

// The third tab of the notebook
  gtk_notebook_append_page (GTK_NOTEBOOK(enoote), create_tab_3 (this_cedit, data), gtk_label_new ("Legend"));
//  gtk_notebook_set_tab_label (GTK_NOTEBOOK (enoote), gtk_notebook_get_nth_page (GTK_NOTEBOOK (enoote), 2), gtk_label_new ("Legend"));

// The fourth tab of the notebook
  gtk_notebook_append_page (GTK_NOTEBOOK(enoote), create_tab_4 (this_cedit, data), gtk_label_new ("Axis"));
//  gtk_notebook_set_tab_label (GTK_NOTEBOOK (enoote), gtk_notebook_get_nth_page (GTK_NOTEBOOK (enoote), 3), gtk_label_new ("Axis"));

// The fifth tab of the notebook
  dbox = create_vbox (BSEP);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dbox, markup_label("<b>Add data set(s) to the active window</b>", -1, 30, 0.5, 0.5), FALSE, FALSE, 0);
  scrollsets = create_scroll (dbox, 250, 525, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, create_projects_tree (this_cedit, data));
  gtk_notebook_append_page (GTK_NOTEBOOK(enoote), dbox, gtk_label_new ("Add data set"));
//  gtk_notebook_set_tab_label (GTK_NOTEBOOK (enoote), gtk_notebook_get_nth_page (GTK_NOTEBOOK (enoote), 4), gtk_label_new ("Add data set"));

  add_gtk_close_event (this_cedit -> win, G_CALLBACK(destroy_this_window), NULL);

  g_signal_connect (G_OBJECT(this_cedit -> win), "response", G_CALLBACK(run_curve_edit), data);
  show_the_widgets (this_cedit -> win);
  set_data_aspect (GTK_COMBO_BOX(this_cedit -> data_aspect), data);
  update_axis (GTK_COMBO_BOX(this_cedit -> axischoice), data);

  dialog_id ++;
  Event_loop[dialog_id] = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (Event_loop[dialog_id]);
}
