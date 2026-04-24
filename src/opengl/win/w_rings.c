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
* @file w_rings.c
* @short Functions to create the ring(s) tab for the advanced environments window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_rings.c'
*
* Contains:
*

 - The functions to create the ring(s) tab for the advanced environments window

*
* List of functions:

  gchar * ring_atoms (project * this_proj, int ring, int step, int size, int rid, gboolean to_file);

  int get_rmin (project * this_proj, int rid, int step);
  int get_rmax (project * this_proj, int rid, int step);

  void rings_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void fill_rings_model (GtkTreeStore * store, project * this_proj, int rid);
  void rings_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void add_this_ring_to_search_tree (project * this_proj, int rid);

  G_MODULE_EXPORT void on_select_rings (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void update_rings_search (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void run_save_rings_to_file (GtkNativeDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void run_save_rings_to_file (GtkDialog * info, gint response_id, gpointer data);
  G_MODULE_EXPORT void save_rings_to_file (GtkButton * but, gpointer data);

  GtkWidget * create_rings_tree (project * this_proj, int rid, gboolean fill_this);
  GtkWidget * create_rings_search (project * this_proj, int rid);
  GtkWidget * rings_tab (glwin * view, int rid);

*/

#include "global.h"
#include "interface.h"
#include "glview.h"
#include "glwindow.h"

extern const gchar * dfi[2];
gchar * rtype[5] = {"all", "King", "Guttman", "primtive", "strong"};

/*!
  \fn G_MODULE_EXPORT void on_select_rings (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select ring toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_select_rings (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  tint * dat = (tint * )data;
  gboolean saved_label[2];
  int i, j, g, c, s, v, u, a, b;
  opengl_project_changed(dat -> a);
  coord_edition * coord = opengl_project -> modelgl -> coord_win;
  b = (opengl_project -> steps > 1) ? 1: 0;
  for (i=0; i<5; i++)
  {
    if (i*10 > dat -> b) break;
  }
  g = i - 1;
  for (i=2+b; i<5+b; i++)
  {
    if (10*g+i == dat -> b) break;
  }
  c = i;
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
  gtk_tree_model_get_iter (GTK_TREE_MODEL(coord -> rings_model[g]), & iter, path);
  gtk_tree_store_set (coord -> rings_model[g], & iter, c, v, -1);
  if (b)
  {
    gtk_tree_model_get (GTK_TREE_MODEL(coord -> rings_model[g]), & iter, 0, & s, -1);
    s = - s - 1;
  }
  else
  {
    s = 0;
  }
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> rings_model[g]), & iter, b, & i, -1);
  i = -i;
  gtk_tree_model_get (GTK_TREE_MODEL(coord -> rings_model[g]), & iter, b+1, & j, -1);
  switch (c-b)
  {
    case 2:
      // Viz
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_rings[g][s][i-1][j-1][u];
#ifdef GTK4
        if (opengl_project -> atoms[s][a].show[0] != v) show_hide_this_atom (NULL, NULL, GINT_TO_POINTER(a));
#else
        if (opengl_project -> atoms[s][a].show[0] != v) show_hide_this_atom (NULL, GINT_TO_POINTER(a));
#endif // GTK4
      }
      break;
    case 3:
      // Show poly
      opengl_project -> modelgl -> show_rpoly[g][s][i-1][j-1] = v;
      int shaders[1] = {RINGS};
      re_create_md_shaders (1, shaders, opengl_project);
      break;
    case 4:
      // Label
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_rings[g][s][i-1][j-1][u];
        opengl_project -> atoms[s][a].label[0] = opengl_project -> atoms[s][a].label[1] = v;
      }
      opengl_project -> modelgl -> create_shaders[LABEL] = TRUE;
      break;
    case 5:
      // Pick
      for (u=0; u<i; u++)
      {
        a = opengl_project -> modelgl -> all_rings[g][s][i-1][j-1][u];
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
  \fn void rings_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer in the ring search tree store

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void rings_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  tint * id = (tint *)data;
  int i, j, k;
  project * this_proj = get_project_by_id(id -> a);
  i = id -> b;
  j = (this_proj -> steps > 1) ? 1: 0;
  gtk_tree_model_get (mod, iter, j, & k, -1);
  if (j)
  {
    gtk_tree_model_get (mod, iter, 0, & k, -1);
    if (i == 0)
    {
      if (k > 0)
      {
        gtk_cell_renderer_set_visible (renderer, 1);
      }
      else
      {
        gtk_cell_renderer_set_visible (renderer, 0);
      }
    }
    else if (k > 0)
    {
      gtk_cell_renderer_set_visible (renderer, 0);
    }
    else
    {
      gtk_tree_model_get (mod, iter, j, & k, -1);
      if (i == 1)
      {
        if (k > 0)
        {
          gtk_cell_renderer_set_visible (renderer, 1);
        }
        else
        {
          gtk_cell_renderer_set_visible (renderer, 0);
        }
      }
      else
      {
        if (k > 0)
        {
          gtk_cell_renderer_set_visible (renderer, 0);
        }
        else
        {
          gtk_cell_renderer_set_visible (renderer, 1);
        }
      }
    }
  }
  else
  {
    if (i == 0)
    {
      if (k > 0)
      {
        gtk_cell_renderer_set_visible (renderer, 1);
      }
      else
      {
        gtk_cell_renderer_set_visible (renderer, 0);
      }
    }
    else
    {
      if (k > 0)
      {
        gtk_cell_renderer_set_visible (renderer, 0);
      }
      else
      {
        gtk_cell_renderer_set_visible (renderer, 1);
      }
    }
  }
}

/*!
  \fn gchar * ring_atoms (project * this_proj, int ring, int step, int size, int rid)

  \brief create a string containing the list of atoms in a ring

  \param this_proj the target project
  \param ring the type of ring
  \param step the configuration
  \param size the size of the ring
  \param rid the id number of the ring
  \param to_file type of output
*/
gchar * ring_atoms (project * this_proj, int ring, int step, int size, int rid, gboolean to_file)
{
  gchar * str;
  int aid, sid;
  aid = this_proj -> modelgl -> all_rings[ring][step][size][rid][0];
  sid = this_proj -> atoms[step][aid].sp;
  int i;
  if (to_file)
  {
    str = g_strdup_printf ("%s%d", exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    for (i=1; i<size+1; i++)
    {
      aid = this_proj -> modelgl -> all_rings[ring][step][size][rid][i];
      sid = this_proj -> atoms[step][aid].sp;
      str = g_strdup_printf ("%s - %s%d", str, exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    }
  }
  else
  {
    str = g_strdup_printf ("%s<sub>%d</sub>", exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    for (i=1; i<size+1; i++)
    {
      aid = this_proj -> modelgl -> all_rings[ring][step][size][rid][i];
      sid = this_proj -> atoms[step][aid].sp;
      str = g_strdup_printf ("%s-%s<sub>%d</sub>", str, exact_name(this_proj -> chemistry -> label[sid]), aid+1);
    }
  }
  return str;
}

/*!
  \fn void fill_rings_model (GtkTreeStore * store, project * this_proj, int rid)

  \brief fill the entire ring(s) tree store

  \param store the GtkTreeStore to fill
  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
*/
void fill_rings_model (GtkTreeStore * store, project * this_proj, int rid)
{
  GtkTreeIter step_level, size_level, ring_level;
  gchar * str;
  int h, i, j, k, l;
  if (this_proj -> coord -> totcoord[rid+4])
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
                                                 5, 0,
                                                 6, 0, -1);
      }
      for (i=0; i < this_proj -> coord -> totcoord[rid+4]; i++)
      {
        j = this_proj -> coord -> geolist[rid+4][0][i];
        k = this_proj -> modelgl -> num_rings[rid][h][j-1];
        if (this_proj -> steps > 1 && k > 0)
        {
          gtk_tree_store_append (store, & size_level, & step_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, j,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0,
                                                   6, 0, -1);
        }
        else if (this_proj -> steps == 1)
        {
          gtk_tree_store_append (store, & size_level, NULL);
          gtk_tree_store_set (store, & size_level, 0, j,
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
        }
        for (l=0; l<k; l++)
        {
          gtk_tree_store_append (store, & ring_level, & size_level);
          str = ring_atoms (this_proj, rid, h, j-1, l, FALSE);
          if (this_proj -> steps > 1)
          {
            gtk_tree_store_set (store, & ring_level, 0, -(h+1),
                                                     1, -j,
                                                     2, l+1,
                                                     3, FALSE,
                                                     4, this_proj -> modelgl -> show_rpoly[rid][h][j-1][l],
                                                     5, FALSE,
                                                     6, FALSE,
                                                     7, str, -1);
          }
          else
          {
            gtk_tree_store_set (store, & ring_level, 0, -j,
                                                     1, l+1,
                                                     2, FALSE,
                                                     3, this_proj -> modelgl -> show_rpoly[rid][h][j-1][l],
                                                     4, FALSE,
                                                     5, FALSE,
                                                     6, str, -1);
          }
          g_free (str);
        }
      }
    }
  }
}

/*!
  \fn void rings_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief Rings tree view set color and Pango markup in tree view

  \param col the tree view column
  \param renderer the column renderer
  \param mod the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
void rings_set_markup (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int steps = GPOINTER_TO_INT(data);
  set_renderer_markup (mod, iter, renderer, (steps) ? 7 : 6);
}

/*!
  \fn GtkWidget * create_rings_tree (project * this_proj, int rid, gboolean fill_this)

  \brief create the ring(s) search tree store

  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
  \param fill_this 1 = yes, 0 = no
*/
GtkWidget * create_rings_tree (project * this_proj, int rid, gboolean fill_this)
{
  int i, j, k;
  GtkTreeViewColumn * rings_col[7];
  GtkCellRenderer * rings_cell[7];
  gchar * ctitle[8]={"MD step", "Ring(s) size", "Id.", "Show", "Poly.", "Label", "Pick", "Atoms"};
  gchar * ctype[8]={"text", "text", "text", "active", "active", "active", "active", "text"};
  GType col_type_ms[8]={G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING};
  GType col_type_ss[7]={G_TYPE_INT, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING};

  coord_edition * coord = this_proj -> modelgl -> coord_win;

  j = (this_proj -> steps > 1) ? 1: 0;
  k = (this_proj -> steps > 1) ? 0: 1;
  coord -> rings_model[rid] = gtk_tree_store_newv (7+j, (j) ? col_type_ms : col_type_ss);
  if (fill_this) fill_rings_model (coord -> rings_model[rid], this_proj, rid);
  GtkWidget * rings_tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(coord -> rings_model[rid]));
  for (i=0; i<7+j; i++)
  {
    if (i < 2+j || i > 5+j)
    {
      rings_cell[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      rings_cell[i] = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(rings_cell[i]), "toggled", G_CALLBACK(on_select_rings), & this_proj -> modelgl -> colorp[rid*10+i][0]);
    }
    gtk_cell_renderer_set_fixed_size (rings_cell[i], -1, 25);
    rings_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[i+k], rings_cell[i], ctype[i+k], i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(rings_tree), rings_col[i]);
    gtk_tree_view_column_set_alignment (rings_col[i], 0.5);
    gtk_tree_view_column_set_cell_data_func (rings_col[i], rings_cell[i], rings_set_visible, & this_proj -> modelgl -> colorp[i][0], NULL);
    if (i > 5+j) gtk_tree_view_column_set_cell_data_func (rings_col[i], rings_cell[i], rings_set_markup, GINT_TO_POINTER(j), NULL);
  }

  return rings_tree;
}

/*!
  \fn void add_this_ring_to_search_tree (project * this_proj, int rid)

  \brief add ring in the search tree based on ring size and id

  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
*/
void add_this_ring_to_search_tree (project * this_proj, int rid)
{
  GtkTreeIter step_level, size_level, ring_level;
  GtkTreeIter new_level;
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  GtkTreeStore * store = (GtkTreeStore *) coord -> rings_model[rid];
  GtkTreeModel * mod = GTK_TREE_MODEL(coord -> rings_model[rid]);
  gboolean valid;
  gboolean insert = TRUE;
  gchar * str;
  int f, h, i, j, k, l, m;
  int prepend = 0;
  if (this_proj -> steps > 1)
  {
    valid = gtk_tree_model_get_iter_first(mod, & step_level);
    while (valid)
    {
      gtk_tree_model_get (mod, & step_level, 0, & f, -1);
      if (f > coord -> rst[rid])
      {
        prepend = 1;
        valid = FALSE;
      }
      else if (f == coord -> rst[rid])
      {
        if (gtk_tree_model_iter_has_child (mod, &step_level))
        {
          h = gtk_tree_model_iter_n_children (mod, &step_level);
          for (i=0; i<h; i++)
          {
            if (gtk_tree_model_iter_nth_child (mod, &size_level, &step_level, i))
            {
              gtk_tree_model_get (mod, &size_level, 1, & j, -1);
              if (j > coord -> rsz[rid])
              {
                prepend = 3;
                valid = FALSE;
                break;
              }
              else if (j == coord -> rsz[rid])
              {
                if (gtk_tree_model_iter_has_child (mod, &size_level))
                {
                  k = gtk_tree_model_iter_n_children (mod, &size_level);
                  for (l=0; l<k; l++)
                  {
                    if (gtk_tree_model_iter_nth_child (mod, &ring_level, &size_level, l))
                    {
                      gtk_tree_model_get (mod, &ring_level, 2, & m, -1);
                      if (m > coord -> ri[rid])
                      {
                        prepend = 5;
                        valid = FALSE;
                        break;
                      }
                      else if (m == coord -> ri[rid])
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
      str = ring_atoms (this_proj, rid, coord -> rst[rid]-1, coord -> rsz[rid]-1, coord -> ri[rid]-1, FALSE);
      switch (prepend)
      {
        case 0:
          gtk_tree_store_append (store, & step_level, NULL);
          gtk_tree_store_set (store, & step_level, 0, coord -> rst[rid],
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0,
                                                   6, 0, -1);
          gtk_tree_store_append (store, & size_level, & step_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, coord -> rsz[rid],
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0,
                                                   6, 0, -1);
          gtk_tree_store_append (store, & ring_level, & size_level);
          gtk_tree_store_set (store, & ring_level, 0, -coord -> rst[rid],
                                                   1, -coord -> rsz[rid],
                                                   2, coord -> ri[rid],
                                                   3, FALSE,
                                                   4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                   5, FALSE,
                                                   6, FALSE,
                                                   7, str, -1);
          break;
        case 1:
          gtk_tree_store_insert_before (store, & new_level, NULL, & step_level);
          gtk_tree_store_set (store, & new_level, 0, coord -> rst[rid],
                                                  1, 0,
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0,
                                                  6, 0, -1);
          gtk_tree_store_append (store, & size_level, & new_level);
          gtk_tree_store_set (store, & size_level, 0, 0,
                                                   1, coord -> rsz[rid],
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0,
                                                   6, 0, -1);
          gtk_tree_store_append (store, & ring_level, & size_level);
          gtk_tree_store_set (store, & ring_level, 0, -coord -> rst[rid],
                                                   1, -coord -> rsz[rid],
                                                   2, coord -> ri[rid],
                                                   3, FALSE,
                                                   4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                   5, FALSE,
                                                   6, FALSE,
                                                   7, str, -1);
          break;
        case 2:
          gtk_tree_store_insert_after (store, & new_level, & step_level, & size_level);
          gtk_tree_store_set (store, & new_level, 0, 0,
                                                  1, coord -> rsz[rid],
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0,
                                                  6, 0, -1);
          gtk_tree_store_append (store, & ring_level, & new_level);
          gtk_tree_store_set (store, & ring_level, 0, -coord -> rst[rid],
                                                   1, -coord -> rsz[rid],
                                                   2, coord -> ri[rid],
                                                   3, FALSE,
                                                   4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                   5, FALSE,
                                                   6, FALSE,
                                                   7, str, -1);
          break;
        case 3:
          gtk_tree_store_insert_before (store, & new_level, & step_level, & size_level);
          gtk_tree_store_set (store, & new_level, 0, 0,
                                                  1, coord -> rsz[rid],
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0,
                                                  6, 0, -1);
          gtk_tree_store_append (store, & ring_level, & new_level);
          gtk_tree_store_set (store, & ring_level, 0, -coord -> rst[rid],
                                                   1, -coord -> rsz[rid],
                                                   2, coord -> ri[rid],
                                                   3, FALSE,
                                                   4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                   5, FALSE,
                                                   6, FALSE,
                                                   7, str, -1);
          break;
        case 4:
          gtk_tree_store_insert_after (store, & new_level, & size_level, & ring_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> rst[rid],
                                                  1, -coord -> rsz[rid],
                                                  2, coord -> ri[rid],
                                                  3, FALSE,
                                                  4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                  5, FALSE,
                                                  6, FALSE,
                                                  7, str, -1);
          break;
        case 5:
          gtk_tree_store_insert_before (store, & new_level, & size_level, & ring_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> rst[rid],
                                                  1, -coord -> rsz[rid],
                                                  2, coord -> ri[rid],
                                                  3, FALSE,
                                                  4, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                  5, FALSE,
                                                  6, FALSE,
                                                  7, str, -1);
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
      if (i > coord -> rsz[rid])
      {
        prepend = 1;
        valid = FALSE;
      }
      else if (i == coord -> rsz[rid])
      {
        if (gtk_tree_model_iter_has_child (mod, &size_level))
        {
          j = gtk_tree_model_iter_n_children (mod, &size_level);
          for (k=0; k<j; k++)
          {
            if (gtk_tree_model_iter_nth_child (mod, &ring_level, &size_level, k))
            {
              gtk_tree_model_get (mod, &ring_level, 1, & l, -1);
              if (l > coord -> ri[rid])
              {
                prepend = 2;
                valid = FALSE;
                break;
              }
              else if (l == coord -> ri[rid])
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
      str = ring_atoms (this_proj, rid, coord -> rst[rid]-1, coord -> rsz[rid]-1, coord -> ri[rid]-1, FALSE);
      switch (prepend)
      {
        case 0:
          gtk_tree_store_append (store, & size_level, NULL);
          gtk_tree_store_set (store, & size_level, 0, coord -> rsz[rid],
                                                   1, 0,
                                                   2, 0,
                                                   3, 0,
                                                   4, 0,
                                                   5, 0, -1);
           gtk_tree_store_append (store, & ring_level, & size_level);
           gtk_tree_store_set (store, & ring_level, 0, -coord -> rsz[rid],
                                                    1, coord -> ri[rid],
                                                    2, FALSE,
                                                    3, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                    4, FALSE,
                                                    5, FALSE,
                                                    6, str, -1);
         break;
       case 1:
          gtk_tree_store_insert_before (store, & new_level, NULL, & size_level);
          gtk_tree_store_set (store, & new_level, 0, coord -> rsz[rid],
                                                  1, 0,
                                                  2, 0,
                                                  3, 0,
                                                  4, 0,
                                                  5, 0, -1);
           gtk_tree_store_append (store, & ring_level, & new_level);
           gtk_tree_store_set (store, & ring_level, 0, -coord -> rsz[rid],
                                                    1, coord -> ri[rid],
                                                    2, FALSE,
                                                    3, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                    4, FALSE,
                                                    5, FALSE,
                                                    6, str, -1);
          break;
        case 2:
          gtk_tree_store_insert_before (store, & new_level, & size_level, & ring_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> rsz[rid],
                                                  1, coord -> ri[rid],
                                                  2, FALSE,
                                                  3, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                  4, FALSE,
                                                  5, FALSE,
                                                  6, str, -1);
          break;
        case 3:
          gtk_tree_store_insert_after (store, & new_level, & size_level, & ring_level);
          gtk_tree_store_set (store, & new_level, 0, -coord -> rsz[rid],
                                                  1, coord -> ri[rid],
                                                  2, FALSE,
                                                  3, this_proj -> modelgl -> show_rpoly[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1][coord -> ri[rid]-1],
                                                  4, FALSE,
                                                  5, FALSE,
                                                  6, str, -1);
          break;
      }
      g_free (str);
    }
  }
}

/*!
  \fn int get_rmin (project * this_proj, int rid, int step)

  \brief get ring(s) max size for the MD step

  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
  \param step the MD step
*/
int get_rmin (project * this_proj, int rid, int step)
{
  int i, j;
  for (i=0; i<this_proj -> coord -> totcoord[rid+4]; i++)
  {
    j = this_proj -> coord -> geolist[rid+4][0][i];
    if (this_proj -> modelgl -> num_rings[rid][step-1][j-1]) break;
  }
  return j;
}

/*!
  \fn int get_rmax (project * this_proj, int rid, int step)

  \brief get ring(s) min size for the MD step

  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
  \param step the MD step
*/
int get_rmax (project * this_proj, int rid, int step)
{
  int i, j;
  for (i=this_proj -> coord -> totcoord[rid+4]-1; i>-1; i--)
  {
    j = this_proj -> coord -> geolist[rid+4][0][i];
    if (this_proj -> modelgl -> num_rings[rid][step-1][j-1]) break;
  }
  return j;
}

/*!
  \fn G_MODULE_EXPORT void update_rings_search (GtkEntry * res, gpointer data)

  \brief update the ring(s) search widget

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void update_rings_search (GtkEntry * res, gpointer data)
{
  tint * dat = (tint * )data;
  gchar * str;
  int i, j, k, v;
  int rid;
  const gchar * m = entry_get_text (res);
  v = (int)string_to_double ((gpointer)m);
  project * this_proj = get_project_by_id(dat -> a);
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  for (i=0; i<5; i++)
  {
    if (i*10 > dat -> b) break;
  }
  rid = i - 1;
  for (i=0; i<3; i++)
  {
    if (10*rid+i == dat -> b) break;
  }
  switch (i)
  {
    case 0:
      if (v > 0 && v < this_proj -> steps+1)
      {
        if (v != coord -> rst[rid])
        {
          coord -> rst[rid] = v;
          coord -> rsz[rid] = coord -> ri[rid] = -1;
          for (j=1; j<3; j++) gtk_label_set_text (GTK_LABEL(coord -> rilab[rid][j]), "");
        }
      }
      else
      {
        coord -> rst[rid] = coord -> rsz[rid] = coord -> ri[rid] = -1;
      }
      if (coord -> rst[rid] > 0)
      {
        update_entry_int(res, coord -> rst[rid]);
        str = g_strdup_printf ("&#x2208; [%d - %d]", get_rmin(this_proj, rid, coord -> rst[rid]),  get_rmax(this_proj, rid, coord -> rst[rid]));
        gtk_label_set_text (GTK_LABEL(coord -> rilab[rid][1]), str);
        g_free (str);
      }
      else
      {
        update_entry_text (res, "");
      }
      break;
    case 1:
      if (coord -> rst[rid] > -1)
      {
        j = get_rmin(this_proj, rid, coord -> rst[rid]);
        k = get_rmax(this_proj, rid, coord -> rst[rid]);
        if (v >= j && v <= k)
        {
          if (v != coord -> rsz[rid])
          {
            if (this_proj -> modelgl -> num_rings[rid][coord -> rst[rid]-1][v-1])
            {
              coord -> rsz[rid] = v;
              coord -> ri[rid] = -1;
            }
            else
            {
              coord -> rsz[rid] = -1;
            }
          }
        }
      }
      else
      {
        coord -> rsz[rid] = coord -> ri[rid] = -1;
      }
      if (coord -> rsz[rid] > 0)
      {
        update_entry_int(res, coord -> rsz[rid]);
        str = g_strdup_printf ("&#x2208; [1 - %d]", this_proj -> modelgl -> num_rings[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1]);
        gtk_label_set_text (GTK_LABEL(coord -> rilab[rid][2]), str);
        g_free (str);
      }
      else
      {
        update_entry_text (res, "");
        gtk_label_set_text (GTK_LABEL(coord -> rilab[rid][2]), "");
      }
      break;
    case 2:
      if (coord -> rst[rid] > -1 && coord -> rsz[rid] > -1)
      {
        if (v > 0 && v < this_proj -> modelgl -> num_rings[rid][coord -> rst[rid]-1][coord -> rsz[rid]-1]+1)
        {
          coord -> ri[rid] = v;
        }
        else
        {
          coord -> ri[rid] = -1;
        }
      }
      else
      {
        coord -> ri[rid] = -1;
      }
      if (coord -> ri[rid] > 0)
      {
        update_entry_int(res, coord -> ri[rid]);
      }
      else
      {
        update_entry_text (res, "");
      }
      break;
  }
  if (coord -> rst[rid] > 0 && coord -> rsz[rid] > 0 && coord -> ri[rid] > 0)
  {
    add_this_ring_to_search_tree (this_proj, rid);
  }
}

/*!
  \fn GtkWidget * create_rings_search (project * this_proj, int rid)

  \brief create the ring(s) search widget

  \param this_proj the target project
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
*/
GtkWidget * create_rings_search (project * this_proj, int rid)
{
  GtkWidget * rings_search = create_vbox (BSEP);
  gchar * str = g_strdup_printf ("Too many <b>%s</b> rings in your model !\n"
                                 "  It is impossible to display the entire list ...\n"
                                 "... instead you can look for ring(s) 'manually':\n", rings_type[rid]);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, rings_search, markup_label(str, 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  g_free (str);
  gchar * search_item[3]={"MD step:", "Ring size:", "Ring ID:"};
  int i, j;
  GtkWidget * hbox;
  GtkWidget * entry;
  j = (this_proj -> steps) > 1 ? 0 : 1;
  coord_edition * coord = this_proj -> modelgl -> coord_win;
  coord -> rst[rid] = 1;
  for (i=j; i<3; i++)
  {
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, rings_search, hbox, FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(search_item[i], 100, -1, 0.0, 0.5), FALSE, FALSE, 20);
    entry = create_entry (G_CALLBACK(update_rings_search), 100, 15, FALSE, & this_proj -> modelgl -> colorp[rid*10+i][0]);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox,entry, FALSE, FALSE, 0);
    if (i==0)
    {
      str = g_strdup_printf ("&#x2208; [1 - %d]", this_proj -> steps);
      coord -> rilab[rid][i] = markup_label(str, 50, -1, 0.0, 0.5);
      g_free (str);
    }
    else if (i == 1)
    {
      str = g_strdup_printf ("&#x2208; [%d - %d]", get_rmin(this_proj, rid, coord -> rst[rid]),  get_rmax(this_proj, rid, coord -> rst[rid]));
      coord -> rilab[rid][i] = markup_label(str, 50, -1, 0.0, 0.5);
      g_free (str);
    }
    else
    {
     coord -> rilab[rid][i] = markup_label("", 50, -1, 0.0, 0.5);
    }
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, coord -> rilab[rid][i], FALSE, FALSE, 5);
  }
  add_box_child_start (GTK_ORIENTATION_VERTICAL, rings_search, markup_label("<b>Search result(s)</b>", 200, -1, 0.5, 0.5), FALSE, FALSE, 10);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, rings_search, create_rings_tree (this_proj, rid, FALSE), FALSE, FALSE, 0);
  return rings_search;
}

#ifdef GTK4
/*!
  \fn G_MODULE_EXPORT void run_save_rings_to_file (GtkNativeDialog * info, gint response_id, gpointer data)

  \brief save rings data to file: run the dialog

  \param info the GtkNativeDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_rings_to_file (GtkNativeDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkFileChooserNative *)info);
#else
/*!
  \fn G_MODULE_EXPORT void run_save_rings_to_file (GtkDialog * info, gint response_id, gpointer data)

  \brief save rings data to file: run the dialog

  \param info the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_save_rings_to_file (GtkDialog * info, gint response_id, gpointer data)
{
  GtkFileChooser * chooser = GTK_FILE_CHOOSER((GtkWidget *)info);
#endif
  if (response_id == GTK_RESPONSE_ACCEPT)
  {
    gchar * rings_file = file_chooser_get_file_name (chooser);
    // Save data to file here !
    if (rings_file)
    {
      tint * dat = (tint *)data;
      project * this_proj = get_project_by_id (dat -> a);
      int rid = dat -> b;
      if (this_proj -> coord -> totcoord[rid+4])
      {
        FILE * fp = fopen (rings_file, dfi[1]);
        gchar * str;
        if (fp)
        {
          if (! rid)
          {
            fprintf (fp, "# This file contains the list of all rings in the %s model\n\n",  prepare_for_title(this_proj -> name));
          }
          else
          {
            fprintf (fp, "# This file contains the list of all %s rings in the %s model\n\n", rtype[rid],  prepare_for_title(this_proj -> name));
          }
          int h, i, j, k, l;
          for (h=0; h < this_proj -> steps; h++)
          {
            i = h+1;
            if (this_proj -> steps > 1) fprintf (fp, "Step N°%d\n", i);
            for (i=0; i < this_proj -> coord -> totcoord[rid+4]; i++)
            {
              j = this_proj -> coord -> geolist[rid+4][0][i];
              fprintf (fp, (this_proj -> steps > 1) ? "\n\tRing(s) of size %d atoms\n" : "\nRing(s) of size %d atoms\n", j);
              k = this_proj -> modelgl -> num_rings[rid][h][j-1];
              // ring size
              for (l=0; l<k; l++)
              {
                str = ring_atoms (this_proj, rid, h, j-1, l, TRUE);
                fprintf (fp, (this_proj -> steps > 1) ? "\t\tN°%d\t:\t%s\n" : "\tN°%d\t:\t%s\n", l+1, str);
                g_free (str);
              }
            }
          }
          fclose (fp);
        }
        else
        {
          str = g_strdup_printf ("Impossible to open file: %s", rings_file);
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
  \fn G_MODULE_EXPORT void save_rings_to_file (GtkButton * but, gpointer data)

  \brief save rings data to file

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void save_rings_to_file (GtkButton * but, gpointer data)
{
  tint * dat = (tint *) data;
#ifdef GTK4
  GtkFileChooserNative * info;
#else
  GtkWidget * info;
#endif
  gchar * crname = g_strdup_printf ("Save atoms in %s rings to file", rtype[dat -> b]);
  info = create_file_chooser (crname,
                              GTK_WINDOW(MainWindow),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Save");
  g_free (crname);
  GtkFileChooser * chooser = GTK_FILE_CHOOSER(info);
#ifdef GTK3
  gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
#endif
  file_chooser_set_current_folder (chooser);

  gchar * rings_file =  g_strdup_printf ("%s-rings.dat", rtype[dat -> b]);
  gtk_file_chooser_set_current_name (chooser, rings_file);
  // g_free (rings_file);
#ifdef GTK4
  run_this_gtk_native_dialog ((GtkNativeDialog *)info, G_CALLBACK(run_save_rings_to_file), data);
#else
  run_this_gtk_dialog (info, G_CALLBACK(run_save_rings_to_file), data);
#endif
}

/*!
  \fn GtkWidget * rings_tab (glwin * view, int rid)

  \brief create the ring(s) tab for the advanced environments window

  \param view the target glwin
  \param rid the type of ring(s), 0 = All, 1 = King, 2 = Guttman, 3 = Primtive, 4 = Strong
*/
GtkWidget * rings_tab (glwin * view, int rid)
{
  GtkWidget * rings = create_vbox (0);
  GtkWidget * scroll = create_scroll (rings, -1, -1, GTK_SHADOW_NONE);
  gtk_widget_set_hexpand (rings, TRUE);
  gtk_widget_set_vexpand (rings, TRUE);
  int h, i, j, k;
  project * this_proj = get_project_by_id(view -> proj);
  k = 0;
  for (h=0; h < this_proj -> steps; h++)
  {
    for (i=0; i < this_proj -> coord -> totcoord[rid+4]; i++)
    {
      j = this_proj -> coord -> geolist[rid+4][0][i];
      k += this_proj -> modelgl -> num_rings[rid][h][j-1];
    }
  }
  if (k < GTK_LIMIT)
  {
    add_container_child (CONTAINER_SCR, scroll, create_rings_tree (this_proj, rid, TRUE));
  }
  else
  {
    add_container_child (CONTAINER_SCR, scroll, create_rings_search (this_proj, rid));
  }
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, rings, hbox, FALSE, FALSE, 2);
  GtkWidget * hhbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, hhbox, TRUE, TRUE, 100);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hhbox, create_button("Save to file", IMG_NONE, NULL, 50, -1, GTK_RELIEF_NORMAL, G_CALLBACK(save_rings_to_file), & view -> colorp[rid][0]), TRUE, TRUE, 40);
  return rings;
}
