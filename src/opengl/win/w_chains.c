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
* @file w_chains.c
* @short Functions to create the chain(s) tab for the advanced environments window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_chains.c'
*
* Contains:
*

 - The functions to create the chain(s) tab for the advanced environments window

*
* List of functions:

  gchar * chain_atoms (project * this_proj, int step, int size, int cid, gboolean to_file);

  int get_cmin (project * this_proj, int step);
  int get_cmax (project * this_proj, int step);

  void fill_chains_model (GtkTreeStore * store, project * this_proj);
  void chains_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void add_this_chain_to_search_tree (project * this_proj);

  G_MODULE_EXPORT void on_select_chains (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void update_chains_search (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void run_save_chains_to_file (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_save_chains_to_file (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void save_chains_to_file (GtkButton * but, gpointer data);

  GtkWidget * create_chains_tree (project * this_proj, gboolean fill_this);
  GtkWidget * create_chains_search (project * this_proj);
  GtkWidget * chains_tab (glwin * view);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"

extern const gchar * dfi[2];
extern void rings_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);

/*!
  \fn G_MODULE_EXPORT void on_select_chains (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select chain toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_select_chains (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  tint * dat = (tint * )data;
  gboolean saved_label[2];
  int i, j, c, s, v, u, a, b;
  opengl_project_changed(dat -> a);
  coord_edition * coord = opengl_project -> modelgl -> coord_win;
  b = (opengl_project -> steps > 1) ? 1: 0;
  c = dat -> b;
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    v = 0;
  }
  else
  {
    v = 1;
  }
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(coord -> chains_model), & iter, path);
  gtk_tree_store_set (coord -> chains_model, & iter, c, v, -1);
  if (b)
  {
    gtk_tree_model_get (GTK_TREE_MODEL(coord -> chains_model), & iter, 0, & s, -1);
    s = - s - 1;
  }
  else
  {
    s = 0;
  }
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> chains_model), & iter, b, & i, -1);
  i = -i;
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> chains_model), & iter, b+1, & j, -1);
  switch (c-b)
  {
    case 2:
      // Viz
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_chains[s][i-1][j-1][u];
#ifdef GTK4
        if (opengl_project -> atoms[s][a].show[0] != v) show_hide_this_atom (NULL, NULL, GINT_TO_POINTER(a));
#else
        if (opengl_project -> atoms[s][a].show[0] != v) show_hide_this_atom (NULL, GINT_TO_POINTER(a));
#endif // GTK4
      }
      break;
    case 3:
      // Label
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_chains[s][i-1][j-1][u];
        opengl_project -> atoms[s][a].label[0] = opengl_project -> atoms[s][a].label[1] = v;
      }
      opengl_project -> modelgl -> create_shaders[LABEL] = TRUE;
      break;
    case 4:
      // Pick
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_chains[s][i-1][j-1][u];
        saved_label[0] = opengl_project -> atoms[s][a].label[0];
        saved_label[1] = opengl_project -> atoms[s][a].label[1];
#ifdef GTK4
        if (opengl_project -> atoms[s][a].pick[0] != v) select_unselect_this_atom (NULL, NULL, GINT_TO_POINTER(a));
#else
        if (opengl_project -> atoms[s][a].pick[0] != v) select_unselect_this_atom (NULL, GINT_TO_POINTER(a));
#endif // GTK4
        opengl_project -> atoms[s][a].label[0] = saved_label[0];
        opengl_project -> atoms[s][a].label[1] = saved_label[1];
      }
      break;
  }
  update (opengl_project -> modelgl);
}

/*!
  \fn gchar * chain_atoms (project * this_proj, int step, int size, int cid, gboolean to_file)

  \brief create a string containing the list of atoms in a chain

  \param this_proj the target project
  \param step the configuration
  \param size the size of the ring
  \param cid the id number of the ring
  \param to_file output
*/
gchar * chain_atoms (project * this_proj, int step, int size, int cid, gboolean to_file)
{
  gchar * str;
  int aid, sid;
  aid = this_proj -> modelgl -> all_chains[step][size][cid][0];
  sid = this_proj -> atoms[step][aid].sp;
  int i;
  if (to_file)
  {
    str = g_strdup_printf ("%s%d", exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    for (i=1; i<size+1; i++)
    {
      aid = this_proj -> modelgl -> all_chains[step][size][cid][i];
      sid = this_proj -> atoms[step][aid].sp;
      str = g_strdup_printf ("%s - %s%d", str, exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    }
  }
  else
  {
    str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    for (i=1; i<size+1; i++)
    {
      aid = this_proj -> modelgl -> all_chains[step][size][cid][i];
      sid = this_proj -> atoms[step][aid].sp;
      str = g_strdup_printf ("%s-%s<sub>%d</sub>", str, exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    }
  }
  return str;
}

/*!
  \fn void fill_chains_model (GtkTreeStore * store, project * this_proj)

  \brief fill the entire chain(s) tree store

  \param store the GtkTreeStore to fill
  \param this_proj the target project
*/
void fill_chains_model (GtkTreeStore * store, project * this_proj)
{
  GtkTreeIter step_level, size_level, chain_level;
  int h, i, j, k, l;
  gchar * str;
  if (this_proj -> coord -> totcoord[9])
  {
    for (h=0; h < this_proj -> steps; h++)
    {
      if (this_proj -> steps > 1)
      {
        gtk_tree_store_append (store, & step_level, NULL);
        gtk_tree_store_set (store, & step_level, 0, h+1,
                                                 1, 0,
                                                 2, 0,
                                                 3, 0,
                                                 4, 0,
                                                 5, 0, -1);
      }
      for (i=0; i < this_proj -> coord -> totcoord[9]; i++)
      {
        j = this_proj -> coord -> geolist[9][0][i];
        k = this_proj -> modelgl -> num_chains[h][j-1];
        if (this_proj -> steps > 1 && k > 0)
        {
          gtk_tree_store_append (store, & size_level, & step_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, j,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
        }
        else if (this_proj -> steps == 1)
        {
          gtk_tree_store_append (store, & size_level, NULL);
          gtk_tree_store_set (store, & size_level, 0, j,
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0, -1);
        }
        for (l=0; l<k; l++)
        {
          gtk_tree_store_append (store, & chain_level, & size_level);
          str = chain_atoms (this_proj, h, j-1, l, FALSE);
          if (this_proj -> steps > 1)
          {
            gtk_tree_store_set (store, & chain_level, 0, -(h+1),
                                                      1, -j,
                                                      2, l+1,
                                                      3, FALSE,
                                                      4, FALSE,
                                                      5, FALSE,
                                                      6, str, -1);
          }
          else
          {
            gtk_tree_store_set (store, & chain_level, 0, -j,
                                                      1, l+1,
                                                      2, FALSE,
                                                      3, FALSE,
                                                      4, FALSE,
                                                      5, str, -1);
          }
          g_free (str);
        }
      }
    }
  }
}

/*!
  \fn void chains_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief Chains tree view set color and Pango markup in tree view

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void chains_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int steps = GPOINTER_TO_INT(data);
  set_renderer_markup (mod, iter, renderer, (steps) ? 6 : 5);
}

/*!
  \fn GtkWidget * create_chains_tree (project * this_proj, gboolean fill_this)

  \brief create the chain(s) search tree store

  \param this_proj the target project
  \param fill_this 1 = yes, 0 = no
*/
GtkWidget * create_chains_tree (project * this_proj, gboolean fill_this)
{
  int i, j, k;
  GtkTreeViewColumn * chains_col[7];
  GtkCellRenderer * chains_cell[7];
  gchar * ctitle[7]={"MD step", "Chain(s) size", "Id.", "Show", "Label", "Pick", "Atoms"};
  gchar * ctype[7]={"text", "text", "text", "active", "active", "active", "text"};
  GType col_type_ms[7]={G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING};
  GType col_type_ss[6]={G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING};

  coord_edition * coord = this_proj -> modelgl -> coord_win;
  j = (this_proj -> steps > 1) ? 1: 0;
  k = (this_proj -> steps > 1) ? 0: 1;
  coord -> chains_model = gtk_tree_store_newv (6+j, (j) ? col_type_ms : col_type_ss);
  if (fill_this) fill_chains_model (coord -> chains_model, this_proj);
  GtkWidget * chains_tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(coord -> chains_model));
  for (i=0; i<6+j; i++)
  {
    if (i < 2+j || i > 4+j)
    {
      chains_cell[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      chains_cell[i] = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(chains_cell[i]), "toggled", G_CALLBACK(on_select_chains), & this_proj -> modelgl -> colorp[i][0]);
    }
    gtk_cell_renderer_set_fixed_size (chains_cell[i], -1, 25);
    chains_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[i+k], chains_cell[i], ctype[i+k], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(chains_tree), chains_col[i]);
    gtk_tree_view_column_set_alignment (chains_col[i], 0.5);
    gtk_tree_view_column_set_cell_data_func (chains_col[i], chains_cell[i], rings_set_visible, & this_proj -> modelgl -> colorp[i][0], NULL);
    if (i > 4+j) gtk_tree_view_column_set_cell_data_func (chains_col[i], chains_cell[i], chains_set_markup, GINT_TO_POINTER(j), NULL);
  }
  return chains_tree;
}

/*!
  \fn void add_this_chain_to_search_tree (project * this_proj)

  \brief add chain in the search tree based on chain length and id

  \param this_proj the target project
*/
void add_this_chain_to_search_tree (project * this_proj)
{
  GtkTreeIter step_level, size_level, chain_level;
  GtkTreeIter new_level;
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  GtkTreeStore * store = (GtkTreeStore *) coord -> chains_model;
  GtkTreeModel * mod = GTK_TREE_MODEL(coord -> chains_model);
  gboolean valid;
  gboolean insert = TRUE;
  int g, h, i, j, k, l, m;
  gchar * str;
  int prepend = 0;
  if (this_proj -> steps > 1)
  {
    valid = gtk_tree_model_get_iter_first(mod, & step_level);
    while (valid)
    {
      gtk_tree_model_get (mod, & step_level, 0, & g, -1);
      if (g > coord -> cst)
      {
        prepend = 1;
        valid = FALSE;
      }
      else if (g == coord -> cst)
      {
        if (gtk_tree_model_iter_has_child (mod, &step_level))
        {
          h = gtk_tree_model_iter_n_children (mod, &step_level);
          for (i=0; i<h; i++)
          {
            if (gtk_tree_model_iter_nth_child (mod, &size_level, &step_level, i))
            {
              gtk_tree_model_get (mod, &size_level, 1, & j, -1);
              if (j > coord -> csz)
              {
                prepend = 3;
                valid = FALSE;
                break;
              }
              else if (j == coord -> csz)
              {
                if (gtk_tree_model_iter_has_child (mod, &size_level))
                {
                  k = gtk_tree_model_iter_n_children (mod, &size_level);
                  for (l=0; l<k; l++)
                  {
                    if (gtk_tree_model_iter_nth_child (mod, &chain_level, &size_level, l))
                    {
                      gtk_tree_model_get (mod, &chain_level, 2, & m, -1);
                      if (m > coord -> ch)
                      {
                        prepend = 5;
                        valid = FALSE;
                        break;
                      }
                      else if (m == coord -> ch)
                      {
                        insert = valid = FALSE;
                        break;
                      }
                    }
                  }
                  if (valid)
                  {
                    prepend = 4;
                    valid = FALSE;
                    break;
                  }
                }
              }
            }
          }
          if (valid)
          {
            prepend = 2;
            valid = FALSE;
          }
        }
      }
      else
      {
        valid = gtk_tree_model_iter_next(mod, & step_level);
      }
    }
    if (insert)
    {
      str = chain_atoms (this_proj, coord -> cst-1, coord -> csz-1, coord -> ch-1, FALSE);
      switch (prepend)
      {
        case 0:
          gtk_tree_store_append (store, & step_level, NULL);
          gtk_tree_store_set (store, & step_level, 0, coord -> cst,
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
          gtk_tree_store_append (store, & size_level, & step_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, coord -> csz,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
          gtk_tree_store_append (store, & chain_level, & size_level);
          gtk_tree_store_set (store, & chain_level, 0, -coord -> cst,
                                                    1, -coord -> csz,
                                                    2, coord -> ch,
                                                    3, FALSE,
                                                    4, FALSE,
                                                    5, FALSE,
                                                    6, str, -1);
          break;
        case 1:
          gtk_tree_store_insert_before (store, & new_level, NULL, & step_level);
          gtk_tree_store_set (store, & new_level, 0, coord -> cst,
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
          gtk_tree_store_append (store, & size_level, & new_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, coord -> csz,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
          gtk_tree_store_append (store, & chain_level, & size_level);
          gtk_tree_store_set (store, & chain_level, 0, -coord -> cst,
                                                    1, -coord -> csz,
                                                    2, coord -> ch,
                                                    3, FALSE,
                                                    4, FALSE,
                                                    5, FALSE,
                                                    6, str, -1);
          break;
        case 2:
          gtk_tree_store_insert_after (store, & new_level, & step_level, & size_level);
          gtk_tree_store_set (store, & new_level, 0, 0,
                                                  1, coord -> csz,
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0, -1);
          gtk_tree_store_append (store, & chain_level, & new_level);
          gtk_tree_store_set (store, & chain_level, 0, -coord -> cst,
                                                   1, -coord -> csz,
                                                   2, coord -> ch,
                                                   3, FALSE,
                                                   4, FALSE,
                                                   5, FALSE,
                                                   6, str, -1);
          break;
        case 3:
          gtk_tree_store_insert_before (store, & new_level, & step_level, & size_level);
          gtk_tree_store_set (store, & new_level, 0, 0,
                                                  1, coord -> csz,
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0, -1);
          gtk_tree_store_append (store, & chain_level, & new_level);
          gtk_tree_store_set (store, & chain_level, 0, -coord -> cst,
                                                    1, -coord -> csz,
                                                    2, coord -> ch,
                                                    3, FALSE,
                                                    4, FALSE,
                                                    5, FALSE,
                                                    6, str, -1);
          break;
        case 4:
          gtk_tree_store_insert_after (store, & new_level, & size_level, & chain_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> cst,
                                                  1, -coord -> csz,
                                                  2, coord -> ch,
                                                  3, FALSE,
                                                  4, FALSE,
                                                  5, FALSE,
                                                  6, str, -1);
          break;
        case 5:
          gtk_tree_store_insert_before (store, & new_level, & size_level, & chain_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> cst,
                                                  1, -coord -> csz,
                                                  2, coord -> ch,
                                                  3, FALSE,
                                                  4, FALSE,
                                                  5, FALSE,
                                                  6, str, -1);
          break;
      }
      g_free (str);
    }
  }
  else
  {
    valid = gtk_tree_model_get_iter_first(mod, & size_level);
    while (valid)
    {
      gtk_tree_model_get (mod, & size_level, 0, & i, -1);
      if (i > coord -> csz)
      {
        prepend = 1;
        valid = FALSE;
      }
      else if (i == coord -> csz)
      {
        if (gtk_tree_model_iter_has_child (mod, &size_level))
        {
          j = gtk_tree_model_iter_n_children (mod, &size_level);
          for (k=0; k<j; k++)
          {
            if (gtk_tree_model_iter_nth_child (mod, &chain_level, &size_level, k))
            {
              gtk_tree_model_get (mod, &chain_level, 1, & l, -1);
              if (l > coord -> ch)
              {
                prepend = 2;
                valid = FALSE;
                break;
              }
              else if (l == coord -> ch)
              {
                insert = valid = FALSE;
                break;
              }
            }
          }
          if (valid)
          {
            prepend = 3;
            valid = FALSE;
          }
        }
      }
      else
      {
        valid = gtk_tree_model_iter_next(mod, & step_level);
      }
    }
    if (insert)
    {
      str = chain_atoms  (this_proj, coord -> cst-1, coord -> csz-1, coord -> ch-1, FALSE);
      switch (prepend)
      {
        case 0:
          gtk_tree_store_append (store, & size_level, NULL);
          gtk_tree_store_set (store, & size_level, 0, coord -> csz,
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0, -1);
           gtk_tree_store_append (store, & chain_level, & size_level);
           gtk_tree_store_set (store, & chain_level, 0, -coord -> csz,
                                                     1, coord -> ch,
                                                     2, FALSE,
                                                     3, FALSE,
                                                     4, FALSE,
                                                     5, str, -1);
         break;
       case 1:
          gtk_tree_store_insert_before (store, & new_level, NULL, & size_level);
          gtk_tree_store_set (store, & new_level, 0, coord -> csz,
                                                  1, 0,
                                                  2, 0,
                                                  3, 0,
                                                  4, 0, -1);
           gtk_tree_store_append (store, & chain_level, & new_level);
           gtk_tree_store_set (store, & chain_level, 0, -coord -> csz,
                                                     1, coord -> ch,
                                                     2, FALSE,
                                                     3, FALSE,
                                                     4, FALSE,
                                                     5, str, -1);
          break;
        case 2:
          gtk_tree_store_insert_before (store, & new_level, & size_level, & chain_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> csz,
                                                  1, coord -> ch,
                                                  2, FALSE,
                                                  3, FALSE,
                                                  4, FALSE,
                                                  5, str, -1);
          break;
        case 3:
          gtk_tree_store_insert_after (store, & new_level, & size_level, & chain_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> csz,
                                                  1, coord -> ch,
                                                  2, FALSE,
                                                  3, FALSE,
                                                  4, FALSE,
                                                  5, str, -1);
          break;
      }
      g_free (str);
    }
  }
}

/*!
  \fn int get_cmin (project * this_proj, int step)

  \brief get chain(s) min size for the MD step

  \param this_proj the target project
  \param step the MD step
*/
int get_cmin (project * this_proj, int step)
{
  int i, j;
  for (i=0; i<this_proj -> coord -> totcoord[9]; i++)
  {
     j = this_proj -> coord -> geolist[9][0][i];
     if (this_proj -> modelgl -> num_chains[step-1][j-1]) break;
  }
  return j;
}

/*!
  \fn int get_cmax (project * this_proj, int step)

  \brief get chain(s) max size for the MD step

  \param this_proj the target project
  \param step the MD step
*/
int get_cmax (project * this_proj, int step)
{
  int i, j;
  for (i=this_proj -> coord -> totcoord[9]-1; i>-1; i--)
  {
     j = this_proj -> coord -> geolist[9][0][i];
     if (this_proj -> modelgl -> num_chains[step-1][j-1]) break;
  }
  return j;
}

/*!
  \fn int get_chain_size_index (project * this_proj, int s, int r)

  \brief NOT USED !

  \param this_proj the target project
  \param s
  \param r
*
int get_chain_size_index (project * this_proj, int s, int r)
{
  int i, j;
  for (i=0; i<this_proj -> coord -> totcoord[9]; i++)
  {
    j = this_proj -> coord -> geolist[9][0][i];
    if (j == r) break;
  }
  return i;
}
*/

/*!
  \fn G_MODULE_EXPORT void update_chains_search (GtkEntry * res, gpointer data)

  \brief update the chain(s) search widget

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_chains_search (GtkEntry * res, gpointer data)
{
  tint * dat = (tint * )data;
  gchar * str;
  const gchar * m = entry_get_text (res);
  int i, j;
  int v = (int)string_to_double ((gpointer)m);
  project * this_proj = get_project_by_id(dat -> a);
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  switch (dat -> b)
  {
    case 0:
      if (v > 0 && v < this_proj -> steps+1)
      {
        if (v != coord -> cst)
        {
          coord -> cst = v;
          coord -> csz = coord -> ch = -1;
          for (j=1; j<3; j++) gtk_label_set_text (GTK_LABEL(coord -> chlab[j]), "");
        }
      }
      else
      {
        coord -> cst = coord -> csz = coord -> ch = -1;
      }
      if (coord -> cst > 0)
      {
        update_entry_int(res, coord -> cst);
        str = g_strdup_printf ("in [%d - %d]", get_cmin(this_proj, coord -> cst), get_cmax(this_proj, coord -> cst));
        gtk_label_set_text (GTK_LABEL(coord -> chlab[1]), str);
        g_free (str);
      }
      else
      {
        update_entry_text (res, "");
        for (j=1; j<3; j++) gtk_label_set_text (GTK_LABEL(coord -> chlab[j]), "");
      }
      break;
    case 1:
      if (coord -> cst > -1)
      {
        i = get_cmin(this_proj, coord -> cst);
        j = get_cmax(this_proj, coord -> cst);
        if (v >= i && v <= j)
        {
          if (v != coord -> csz)
          {
            if (this_proj -> modelgl -> num_chains[coord -> cst-1][coord -> csz-1])
            {
              coord -> csz = v;
              coord -> ch = -1;
              gtk_label_set_text (GTK_LABEL(coord -> chlab[2]), "");
            }
            else
            {
              coord -> csz = coord -> ch = -1;
            }
          }
        }
      }
      else
      {
        coord -> csz = coord -> ch = -1;
      }
      if (coord -> csz > 0)
      {
        update_entry_int(res, coord -> csz);
        str = g_strdup_printf ("in [1 - %d]", this_proj -> modelgl -> num_chains[coord -> cst-1][coord -> csz-1]);
        gtk_label_set_text (GTK_LABEL(coord -> chlab[2]), str);
        g_free (str);
      }
      else
      {
        update_entry_text (res, "");
        gtk_label_set_text (GTK_LABEL(coord -> chlab[2]), "");
      }
      break;
    case 2:
      if (coord -> cst > -1 && coord -> csz > -1)
      {
        if (v > 0 && v < this_proj -> modelgl -> num_chains[coord -> cst-1][coord -> csz-1]+1)
        {
          coord -> ch = v;
        }
        else
        {
          coord -> ch = -1;
        }
      }
      else
      {
        coord -> ch = -1;
      }
      if (coord -> ch > 0)
      {
        update_entry_int(res, coord -> ch);
      }
      else
      {
        update_entry_text (res, "");
      }
      break;
  }
  if (coord -> cst > 0 && coord -> csz > 0 && coord -> ch > 0)
  {
    add_this_chain_to_search_tree (this_proj);
  }
}

/*!
  \fn GtkWidget * create_chains_search (project * this_proj)

  \brief create the chain(s) search widget

  \param this_proj the target project
*/
GtkWidget * create_chains_search (project * this_proj)
{
  GtkWidget * chains_search = create_vbox (BSEP);
  gchar * str = g_strdup_printf ("Too many chains in your model !\n"
                                 "  It is impossible to display the entire list ...\n"
                                 "... instead you can look for chain(s) 'manually':\n");
  add_box_child_start (GTK_ORIENTATION_VERTICAL, chains_search, markup_label(str, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  g_free (str);
  gchar * search_item[3]={"MD step:", "Chain size:", "Chain ID:"};
  int i, j;
  GtkWidget * hbox;
  GtkWidget * entry;
  j = (this_proj -> steps) > 1 ? 0 : 1;
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  coord -> cst = 1;
  for (i=j; i<3; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, chains_search, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(search_item[i], 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
    entry = create_entry (G_CALLBACK(update_chains_search), 100, 15, FALSE, & this_proj -> modelgl -> colorp[i][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,entry, FALSE, FALSE, 0);
    if (i==0)
    {
      str = g_strdup_printf ("in [1 - %d]", this_proj -> steps);
      coord -> chlab[i] = markup_label(str, 50, -1, 0.0, 0.5);
      g_free (str);
    }
    else if (i == 1)
    {
      str = g_strdup_printf ("in [%d - %d]", get_cmin(this_proj, coord -> cst), get_cmax(this_proj, coord -> cst));
      coord -> chlab[i] = markup_label(str, 50, -1, 0.0, 0.5);
      g_free (str);
    }
    else
    {
      coord -> chlab[i] = markup_label("", 50, -1, 0.0, 0.5);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, coord -> chlab[i], FALSE, FALSE, 5);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, chains_search, markup_label("<b>Search result(s)</b>", 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, chains_search, create_chains_tree (this_proj, FALSE), FALSE, FALSE, 0);
  return chains_search;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_save_chains_to_file (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief save chains data to file: run the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_chains_to_file (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_save_chains_to_file (GtkDialog * info, gint response_id, gpointer data)

  \brief save chains data to file: run the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_chains_to_file (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    gchar * chains_file = file_chooser_get_file_name (chooser);
    // Save data to file here !
    if (chains_file)
    {
      tint * dat = (tint *)data;
      project * this_proj = get_project_by_id (dat -> a);
      if (this_proj -> coord -> totcoord[9])
      {
        FILE * fp = fopen (chains_file, dfi[1]);
        gchar * str;
        if (fp)
        {
          fprintf (fp, "# This file contains the list of all chains in the %s model\n\n",  prepare_for_title(this_proj -> name));
          int h, i, j, k, l;
          for (h=0; h < this_proj -> steps; h++)
          {
            i = h+1;
            if (this_proj -> steps > 1) fprintf (fp, "Step N°%d\n", i);
            for (i=0; i < this_proj -> coord -> totcoord[9]; i++)
            {
              j = this_proj -> coord -> geolist[9][0][i];
              fprintf (fp, (this_proj -> steps > 1) ? "\n\tChain(s) of size %d atoms\n" : "\nChain(s) of size %d atoms\n", j);
              k = this_proj -> modelgl -> num_chains[h][j-1];
              // ring size
              for (l=0; l<k; l++)
              {
                str = chain_atoms (this_proj, h, j-1, l, TRUE);
                fprintf (fp, (this_proj -> steps > 1) ? "\t\tN°%d\t:\t%s\n" : "\tN°%d\t:\t%s\n", l+1, str);
                g_free (str);
              }
            }
          }
          fclose (fp);
        }
        else
        {
          str = g_strdup_printf ("Impossible to open file: %s", chains_file);
          show_error (str, 0, this_proj -> modelgl -> coord_win -> win);
          g_free (str);
        }
      }
    }
  }
#ifdef GTK4
  destroy_this_native_dialog (info);
#else
  destroy_this_dialog (info);
#endif
}

/*!
  \fn G_MODULE_EXPORT void save_chains_to_file (GtkButton * but, gpointer data)

  \brief save chains data to file

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void save_chains_to_file (GtkButton * but, gpointer data)
{
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  info = create_file_chooser ("Save atoms in chains to file",
                              GTK_WINDOW(MainWindow),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  file_chooser_set_current_folder (chooser);
  gtk_file_chooser_set_current_name (chooser, "chains.dat");
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_save_chains_to_file), data);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_save_chains_to_file), data);
#endif
}

/*!
  \fn GtkWidget * chains_tab (glwin * view)

  \brief create the chain(s) tab for the advanced environments window

  \param view the target glwin
*/
GtkWidget * chains_tab (glwin * view)
{
  GtkWidget * chains = create_vbox (0);
  GtkWidget * scroll = create_scroll (chains, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (chains, TRUE);
  gtk_widget_set_vexpand (chains, TRUE);
 int h, i, j, k;
  project * this_proj = get_project_by_id(view -> proj);
  k = 0;
  for (h=0; h < this_proj -> steps; h++)
  {
    for (i=0; i < this_proj -> coord -> totcoord[9]; i++)
    {
      j = this_proj -> coord -> geolist[9][0][i];
      k += this_proj -> modelgl -> num_chains[h][j-1];
    }
  }
  if (k < GTK_LIMIT)
  {
    add_container_child (CONTAINER_SCR, scroll, create_chains_tree (this_proj, TRUE));
  }
  else
  {
    add_container_child (CONTAINER_SCR, scroll, create_chains_search (this_proj));
  }

  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, chains, hbox, FALSE, FALSE, 2);
  GtkWidget * hhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, TRUE, TRUE, 100);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, create_button("Save to file", IMG_NONE, NULL, 50, -1, GTK_RELIEF_NORMAL, G_CALLBACK(save_chains_to_file), & view -> colorp[0][0]), TRUE, TRUE, 40);
  return chains;
}
