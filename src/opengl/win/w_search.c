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
* @file w_search.c
* @short Functions to create the object search widgets, for:
   - The atom(s) / clone(s) advanced window \n
   - The model edition window (move, remove, replace, insert, random move) \n
   - The crystal builder window
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'w_search.c'
*
* Contains:
*

 - The functions to create the object search widgets, for:
   - The atom(s) / clone(s) advanced window
   - The model edition window (move, remove, replace, insert, random move)
   - The crystal builder window

*
* List of functions:

  int get_asearch_num_objects (atom_search * asearch);
  int get_asearch_object (atom_search * asearch);
  int get_asearch_filter (atom_search * asearch);
  int get_selected_object_id (gboolean visible, int p, gchar * str, atom_search * asearch);
  int get_todo_size (atom_search * asearch);

  gboolean get_asearch_is_object (atom_search * asearch);
  gboolean fill_for_action (atom_search * asearch, int i, int j, project * this_proj);
  gboolean append (atom_search * asearch, project * this_proj, int i, int j);
  gboolean update_this_search (atom_search * asearch);
  gboolean remove_from_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  gboolean atom_is_in_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);
  gboolean update_search_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data);

  gchar * adjust_picked (gchar * picked, atomic_object * object, gboolean init);
  gchar * get_node_name (int node, atom_search * asearch, project * this_proj);

  void check_tree_for_this_search (project * this_proj, atom_search * asearch);
  void check_all_trees (project * this_proj);
  void motion_to_zero (atom_search * asearch);
  void adjust_object_to_move (project * this_proj, atom_search * asearch, int mv, int id);
  void append_to_model (GtkTreeIter * atom_level, atom_search * asearch, gboolean is_object, int h, int i, project * this_proj);
  void fill_atom_model (atom_search * asearch, project * this_proj);
  void update_search_tree (atom_search * asearch);
  void clear_fields (atom_search * asearch);
  void re_populate_tree_search (atom_search * asearch);
  void adjust_search_param (atom_search * asearch, project * this_proj, int a, int s, int c, gboolean status);
  void adjust_this_tree_branch (atom_search * asearch, project * this_proj, int oid, int sid, GtkTreeIter iter);
  void adjust_this_tree_leaf (atom_search * asearch, project * this_proj, int oid, int aid, int new_status, GtkTreeIter iter);
  void adjust_data_model_to_replace (project * this_proj, atom_search * asearch, int sid, int vid);
  void search_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void get_coord_iter_and_edit (gchar * path_string, gpointer data, GtkWidget * widg);
  void allocate_todo (atom_search * asearch, int tsize);
  void clean_todo (atom_search * asearch);
  void clean_picked_and_labelled (atom_search * asearch);
  void add_random_column (atom_search * asearch);
  void prep_search_box (GtkWidget * vbox, GtkWidget * lab, GtkWidget * combo);

  G_MODULE_EXPORT void set_atom (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void remove_atom (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void add_atom (GtkButton * but, gpointer data);
  G_MODULE_EXPORT void set_id (GtkEntry * entry, gpointer data);
  G_MODULE_EXPORT void select_atom (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void changed_action_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data);
  G_MODULE_EXPORT void set_occupancy (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_i_coord (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_max_msd (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_max_action (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void set_max_msd_for_all (GtkEntry * res, gpointer data);
  G_MODULE_EXPORT void to_edit_coords (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data);
  G_MODULE_EXPORT void markup_action_renderer (GtkCellRendererCombo * cell, GtkCellEditable * editable, gchar * path_string, gpointer data);
  G_MODULE_EXPORT void select_all_atoms (GtkTreeViewColumn * col, gpointer data);
  G_MODULE_EXPORT void move_up_down (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data);
  G_MODULE_EXPORT void set_spec_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_object_changed (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_search_mode (GtkComboBox * box, gpointer data);
  G_MODULE_EXPORT void set_search_digit (GtkEntry * res, gpointer data);

  GtkWidget * create_atoms_tree (atom_search * asearch, project * this_proj, int nats);
  GtkWidget * prepare_box_too_much (atom_search * asearch);
  GtkWidget * selection_tab (atom_search * asearch, int nats);

  GtkTreeModel * replace_combo_tree (gboolean insert, int proj);

  atomic_object * get_atomic_object_by_origin (atomic_object * first, int oid, int aid);

*/

#include "atom_edit.h"

extern int check_label_numbers (project * this_proj, int types);
extern int selected_aspec;
extern int select_from_library (gboolean visible, project * this_proj, atom_search * asearch);
extern atom_selection * preserve_ogl_selection (glwin * view);
extern void create_slab_lists (project * this_proj);
extern int get_to_be_selected (glwin * view);
extern void restore_ogl_selection (glwin * view);

/*!
  \fn int get_asearch_num_objects (atom_search * asearch)

  \brief the number of type of object(s) in this atom search

  \param asearch the target atom search
*/
int get_asearch_num_objects (atom_search * asearch)
{
  int filter = get_asearch_filter (asearch);
  switch (filter)
  {
    case 0:
      return get_project_by_id (asearch -> proj) -> nspec;
      break;
    case 1:
      return get_project_by_id (asearch -> proj) -> coord -> cmax+1;
      break;
    default:
      return get_project_by_id (asearch -> proj) -> coord -> totcoord[filter -1];
      break;
  }
}

/*!
  \fn int get_asearch_object (atom_search * asearch)

  \brief get the number of object(s) in this atom search

  \param asearch the target atom search
*/
int get_asearch_object (atom_search * asearch)
{
  if (asearch -> object < 0) return 0;
  if (asearch -> mode) return asearch -> object;
  if (get_project_by_id(asearch -> proj) -> natomes >= 10000)
  {
    return (asearch -> object > 1) ? 2 : asearch -> object;
  }
  else
  {
    return asearch -> object;
  }
}

/*!
  \fn int get_asearch_filter (atom_search * asearch)

  \brief get asearch filter

  \param asearch the target atom search
*/
int get_asearch_filter (atom_search * asearch)
{
  if (asearch -> object < 0 || asearch -> filter < 0) return 0;
  if (asearch -> mode)
  {
    return asearch -> object + asearch -> filter;
  }
  else if (get_project_by_id(asearch -> proj) -> natomes >= 10000)
  {
    return (asearch -> object > 1) ? 1 + asearch -> filter : asearch -> filter;
  }
  else
  {
    return asearch -> object + asearch -> filter;
  }
}

/*!
  \fn gboolean get_asearch_is_object (atom_search * asearch)

  \brief get asearch object

  \param asearch the target atom search
*/
gboolean get_asearch_is_object (atom_search * asearch)
{
  if (asearch -> passivating)
  {
    return (asearch -> object > 1) ? TRUE : FALSE;
  }
  else
  {
    return (asearch -> object) ? TRUE : FALSE;
  }
}

/*!
  \fn void check_tree_for_this_search (project * this_proj, atom_search * asearch)

  \brief check, and adjust if needed, the tree construction for this atom search

  \param this_proj the target project
  \param asearch the target atom search
*/
void check_tree_for_this_search (project * this_proj, atom_search * asearch)
{
  int j, k, l, m, n, o, p;
  double u, v;
  gchar * str;
  GtkTreeIter iter;
  GtkTreeIter child;
  GtkTreeModel * atom_model;
  gboolean dothis, dothat, is_first;

  int obj = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  int is_clone = (asearch -> action == 1) ? 1 : 0;
  int status = (asearch -> action < 2) ? 2 : asearch -> status;
  int val = get_asearch_num_objects (asearch);
  int step = this_proj -> modelgl -> anim -> last -> img -> step;
  atom_model = GTK_TREE_MODEL(asearch -> atom_model);
  if (gtk_tree_model_get_iter_first (atom_model, & iter))
  {
    dothis = TRUE;
    if (! filter && status == 2)
    {
      while (dothis)
      {
        gtk_tree_model_get (atom_model, & iter, IDCOL, & j, -1);
        j = abs(j) -1;
        if (j > -1)
        {
          gtk_tree_store_set (asearch -> atom_model, & iter, TOLAB, this_proj -> modelgl -> anim -> last -> img -> show_label[is_clone][j], -1);
          if (gtk_tree_model_iter_children (atom_model, & child, & iter))
          {
            dothat = is_first = TRUE;
            u = v = 0.0;
            l = 0;
            m = 1;
            while (dothat)
            {
              gtk_tree_model_get (atom_model, & child, IDCOL, & k, TOPIC, & l, -1);
              if (k > 0)
              {
                k --;
                if (asearch -> action == RANMOVE)
                {
                  v = this_proj -> modelgl -> atom_win -> msd[k];
                  if (! is_first && u != v) v = 0.0;
                  is_first = FALSE;
                  u = v;
                }
                m = (l && m) ? 1 : 0;
                gtk_tree_store_set (asearch -> atom_model, & child, TOLAB, this_proj -> atoms[step][k].label[is_clone], -1);
                if (asearch -> action == RANMOVE)
                {
                  if (this_proj -> modelgl -> atom_win -> msd[k] > 0.0)
                  {
                    str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd[k]);
                  }
                  else
                  {
                    str = g_strdup_printf ("Set MSD<sub>max</sub> for ...");
                  }
                  gtk_tree_store_set (asearch -> atom_model, & child, TOPIC+2, str, -1);
                  g_free (str);
                }
                is_first = FALSE;
              }
              dothat = gtk_tree_model_iter_next (atom_model, & child);
            }
            asearch -> pick[j] = m;
            if (asearch -> action == RANMOVE)
            {
              if (v > 0.0)
              {
                str = g_strdup_printf ("For all: MSD<sub>max</sub>= %f", v);
              }
              else
              {
                str = g_strdup_printf ("Set MSD<sub>max</sub> for all ...");
              }
              gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC+2, str, -1);
              g_free (str);
            }
          }
          gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC, asearch -> pick[j], -1);
        }
        dothis = gtk_tree_model_iter_next (atom_model, & iter);
      }
    }
    else
    {
      while (dothis)
      {
        gtk_tree_model_get (atom_model, & iter, IDCOL, & j, -1);
        j = abs(j) - 1;
        if (j > -1 && j < val)
        {
          k = l = 0;
          switch (filter)
          {
            case 0:
              for (m=0; m<this_proj -> natomes; m++)
              {
                n = this_proj -> atoms[step][m].sp;
                if (n == j && (this_proj -> atoms[step][m].pick[is_clone] == status || status == 2))
                {
                  k ++;
                  l += (this_proj -> atoms[step][m].label[is_clone]) ? 1 : 0;
                }
              }
              break;
            case 1:
              for (m=0; m<this_proj -> natomes; m++)
              {
                n = this_proj -> atoms[step][m].sp;
                o = this_proj -> atoms[step][m].coord[filter-1];
                if (this_proj -> atoms[step][m].numv == j && (this_proj -> atoms[step][m].pick[is_clone] == status || status == 2))
                {
                  k ++;
                  l += (this_proj -> atoms[step][m].label[is_clone]) ? 1 : 0;
                }
              }
              break;
            default:
              for (m=0; m<this_proj -> natomes; m++)
              {
                n = this_proj -> atoms[step][m].sp;
                o = this_proj -> atoms[step][m].coord[filter-1];
                if (filter == 2)
                {
                  for (p=0; p<n; p++)
                  {
                    o += this_proj -> coord -> ntg[1][p];
                  }
                }
                if (o == j && (this_proj -> atoms[step][m].pick[is_clone] == status || status == 2))
                {
                  k ++;
                  l += (this_proj -> atoms[step][m].label[is_clone]) ? 1 : 0;
                }
              }
              break;
          }
          gtk_tree_store_set (asearch -> atom_model, & iter, TOLAB, (k == l && k != 0) ? 1 : 0, -1);
          if (gtk_tree_model_iter_children (atom_model, & child, & iter))
          {
            dothat = TRUE;
            u = v = 0.0;
            l = n = 0;
            m = o = 1;
            while (dothat)
            {
              gtk_tree_model_get (atom_model, & child, IDCOL, & k, TOPIC, & l, -1);
              if (asearch -> action == RANMOVE && obj)
              {
                gtk_tree_model_get (atom_model, & child, TOPIC+1, & n, -1);
              }
              if (k > 0)
              {
                k --;
                if (asearch -> action == RANMOVE)
                {
                  v = this_proj -> modelgl -> atom_win -> msd[k];
                  if (! is_first && u != v) v = 0.0;
                  is_first = FALSE;
                  u = v;
                }
                m = (m && l) ? 1 : 0;
                o = (o && n) ? 1 : 0;
                gtk_tree_store_set (asearch -> atom_model, & child, TOLAB, this_proj -> atoms[step][k].label[is_clone], -1);
                if (asearch -> action == RANMOVE)
                {
                  if (this_proj -> modelgl -> atom_win -> msd[k] > 0.0)
                  {
                    str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd[k]);
                  }
                  else
                  {
                    str = g_strdup_printf ("Set MSD<sub>max</sub> for ...");
                  }
                  gtk_tree_store_set (asearch -> atom_model, & child, TOPIC+2, str, -1);
                  g_free (str);
                }
              }
              dothat = gtk_tree_model_iter_next (atom_model, & child);
            }
            if (asearch -> action == RANMOVE)
            {
              asearch -> pick[j] = (m && o) ? 3 : (o) ? 2 : (m) ? 1 : 0;
            }
            else
            {
              asearch -> pick[j] = m;
            }
            gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC, m, -1);
            if (asearch -> action == RANMOVE && v != 0)
            {
              if (v > 0.0)
              {
                if (filter > 2)
                {
                  str = g_strdup_printf ("For all: MSD<sub>max</sub>= %f", v);
                }
                else
                {
                  str = g_strdup_printf ("MSD<sub>max</sub>= %f", v);
                }
              }
              else
              {
                if (obj && filter > 2)
                {
                  str = g_strdup_printf ("Set MSD<sub>max</sub> for ...");
                }
                else
                {
                  str = g_strdup_printf ("Set MSD<sub>max</sub> for all ...");
                }
              }
              gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC+2, str, -1);
              g_free (str);
              if (obj) gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC+1, o, -1);
            }
          }
        }
        dothis = gtk_tree_model_iter_next (atom_model, & iter);
      }
    }
  }
}

/*!
  \fn void check_all_trees (project * this_proj)

  \brief check all search trees

  \param this_proj the target project
*/
void check_all_trees (project * this_proj)
{
  int i;
  for (i=0; i<7; i++)
  {
    if (i != 5 && this_proj -> modelgl -> search_widg[i] != NULL)
    {
      if (this_proj -> modelgl -> search_widg[i] -> atom_model && this_proj -> modelgl -> search_widg[i] -> todo_size < 10000)
      {
        check_tree_for_this_search (this_proj, this_proj -> modelgl -> search_widg[i]);
      }
    }
  }
}

/*!
  \fn atomic_object * get_atomic_object_by_origin (atomic_object * first, int oid, int aid)

  \brief get insert object from a list by id

  \param first the first insert object of the list
  \param oid object origin id (type of origin)
  \param aid object id
*/
atomic_object * get_atomic_object_by_origin (atomic_object * first, int oid, int aid)
{
  atomic_object * object = first;
  while (object)
  {
    if (! aid && object -> origin == oid) return object;
    if (aid && object -> id == aid-1) return object;
    object = object -> next;
  }
  return NULL;
}

/*!
  \fn void motion_to_zero (atom_search * asearch)

  \brief reset motion to 0.0

  \param asearch the target atom search
*/
void motion_to_zero (atom_search * asearch)
{
  project * this_proj = get_project_by_id (asearch -> proj);
  int i, j;
  for (i=0; i<2; i++)
  {
    for (j=0; j<6; j++)
    {
      this_proj -> modelgl -> atom_win -> old_param[asearch -> status][i][j] = this_proj -> modelgl -> atom_win -> new_param[asearch -> status][i][j] = 0.0;
    }
  }
  for (i=0; i<2; i++)
  {
    for (j=0; j<6; j++)
    {
      update_range_and_entry (this_proj, asearch -> status, i, j);
    }
  }
}

/*!
  \fn void adjust_object_to_move (project * this_proj, atom_search * asearch, int mv, int id)

  \brief adjust atom search parameters for motion cases

  \param this_proj the target project
  \param asearch the target atom search
  \param mv motion id (0 = standard, 1 = random)
  \param id object id
*/
void adjust_object_to_move (project * this_proj, atom_search * asearch, int mv, int id)
{
  gboolean adjust_mv = FALSE;
  int filter = get_asearch_filter (asearch);
  if (this_proj -> modelgl -> atom_win -> to_be_moved[mv])
  {
    atomic_object * object = this_proj -> modelgl -> atom_win -> to_be_moved[mv];
    gboolean check_it = TRUE;
    gboolean remove_it = FALSE;
    gboolean add_it = FALSE;
    while (check_it)
    {
      if (object -> id == id)
      {
        if (! asearch -> todo[id]) remove_it = TRUE;
        check_it = FALSE;
      }
      else
      {
        if (object -> next)
        {
          object = object -> next;
        }
        else
        {
          check_it = FALSE;
          add_it = TRUE;
        }
      }
    }
    if (remove_it)
    {
      adjust_mv = TRUE;
      if (object -> ibonds) g_free (object -> ibonds);
      if (object -> baryc) g_free (object -> baryc);
      if (object -> at_list) g_free (object -> at_list);
      if (object -> coord) g_free (object -> coord);
      if (object -> bcid) g_free (object -> bcid);
      object -> atoms = object -> bonds = 0;
      if (object -> next && object -> prev)
      {
        object -> prev -> next = object -> next;
        object -> next -> prev = object -> prev;
        g_free (object);
      }
      else if (! object -> prev && object -> next)
      {
        object = object -> next;
        object -> prev = NULL;
        g_free (this_proj -> modelgl -> atom_win -> to_be_moved[mv]);
        this_proj -> modelgl -> atom_win -> to_be_moved[mv] = object;
      }
      else if (object -> prev)
      {
        object -> prev -> next = NULL;
        g_free (object);
      }
      else
      {
        g_free (this_proj -> modelgl -> atom_win -> to_be_moved[mv]);
        this_proj -> modelgl -> atom_win -> to_be_moved[mv] = NULL;
      }
    }
    else if (add_it)
    {
      adjust_mv = TRUE;
      object_motion = TRUE;
      if (! filter)
      {
        object -> next = create_object_from_species (this_proj, id, NULL);
      }
      else if (filter < 3)
      {
        object -> next = create_object_from_atom_coordination (this_proj, filter-1, id, NULL);
      }
      else
      {
        object -> next = create_object_from_frag_mol (this_proj, filter-1, id, NULL);
      }
      object -> next -> id = id;
      object_motion = FALSE;
      this_proj -> modelgl -> atom_win -> rebuilt[mv] = FALSE;
      object -> next -> prev = object;
    }
  }
  else
  {
    adjust_mv = TRUE;
    object_motion = TRUE;
    if (! filter)
    {
      this_proj -> modelgl -> atom_win -> to_be_moved[mv] = create_object_from_species (this_proj, id, NULL);
    }
    else if (filter < 3)
    {
      this_proj -> modelgl -> atom_win -> to_be_moved[mv] = create_object_from_atom_coordination (this_proj, filter-1, id, NULL);
    }
    else
    {
      this_proj -> modelgl -> atom_win -> to_be_moved[mv] = create_object_from_frag_mol (this_proj, filter-1, id, NULL);
    }
    this_proj -> modelgl -> atom_win -> to_be_moved[mv] -> id = id;
    this_proj -> modelgl -> atom_win -> rebuilt[mv] = FALSE;
    object_motion = FALSE;
  }
  if (adjust_mv && ! mv && this_proj -> modelgl -> atom_win) motion_to_zero (asearch);
}

/*!
  \fn void append_to_model (GtkTreeIter * atom_level, atom_search * asearch, gboolean is_object, int h, int i, project * this_proj)

  \brief append data to the search tree store

  \param atom_level the iter to insert the data to
  \param asearch the target atom search
  \param is_object the atom search object (atom or group of atoms)
  \param h the object type id
  \param i the object id
  \param this_proj the target project
*/
void append_to_model (GtkTreeIter * atom_level, atom_search * asearch, gboolean is_object, int h, int i, project * this_proj)
{
  int j, k, l;
  int step = this_proj -> modelgl -> anim -> last -> img -> step;
  gchar * str;
  j = this_proj -> atoms[0][i].sp;
  str = g_strdup_printf ("%s<sub>%d</sub>", this_proj -> chemistry -> label[j], i+1);
  int aobj = get_asearch_object (asearch);
  if (aobj == 2) str = g_strdup_printf ("%s + neighbor(s)", str);
  gboolean is_clone = (asearch -> pointer[0].c == 1) ? 1 : 0;
  gtk_tree_store_set (asearch -> atom_model, atom_level, IDCOL, i+1, 1, " ", 2, str, TOLAB, this_proj -> atoms[step][i].label[is_clone], -1);
  g_free (str);
  atomic_object * object;
  switch (asearch -> action)
  {
    case 0:
      gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC, this_proj -> atoms[step][i].pick[0], -1);
      break;
    case 1:
      gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC, this_proj -> atoms[step][i].pick[0], -1);
      break;
    case RANMOVE:
      if (asearch -> set_for_all > 0)
      {
        asearch -> todo[i] = asearch -> set_for_all;
        if (aobj)
        {
          adjust_object_to_move (this_proj, asearch, 1, i);
        }
      }
      k = (asearch -> todo[i] == 1 || asearch -> todo[i] == 3) ? 1 : 0;
      l = (asearch -> todo[i] == 2 || asearch -> todo[i] == 3) ? 1 : 0;
      if (this_proj -> modelgl -> atom_win -> msd[i] > 0.0)
      {
        str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd[i]);
      }
      else
      {
        str = g_strdup_printf ("Set MSD<sub>max</sub> for ...");
      }
      gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC, k, TOPIC+2, str, -1);
      if (is_object)
      {
        gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC+1, l, -1);
      }
      g_free (str);
      break;
    default:
      if (asearch -> set_for_all > 0)
      {
        asearch -> todo[i] = asearch -> set_for_all;
        if (asearch -> action == DISPL)
        {
          if (aobj)
          {
            adjust_object_to_move (this_proj, asearch, 0, i);
          }
        }
      }
      gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC, asearch -> todo[i], -1);
      if (asearch -> action == REPLACE)
      {
        object = get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[0], i, 0);
        gtk_tree_store_set (asearch -> atom_model, atom_level, TOPIC+1, (object) ? object -> name : "Select ...", TOPIC+3, h, -1);
      }
      break;
  }
}

/*!
  \fn gboolean fill_for_action (atom_search * asearch, int i, int j, project * this_proj)

  \brief test if the atom 'i' of species 'j' matches requirement(s) to be in the tree store

  \param asearch the target atom search
  \param i the atom id
  \param j the chemical species id (or type id)
  \param this_proj the target project
*/
gboolean fill_for_action (atom_search * asearch, int i, int j, project * this_proj)
{
  gboolean append = FALSE;
  if (asearch -> spec == 0 || asearch -> spec == j+1)
  {
    if (asearch -> action > 1)
    {
      if (asearch -> status == 2)
      {
        append = TRUE;
      }
      else
      {
        append = (this_proj -> atoms[0][i].pick[0] == asearch -> status) ? TRUE : FALSE;
      }
    }
    else
    {
       append = TRUE;
    }
  }
  return append;
}

/*!
  \fn gboolean append (atom_search * asearch, project * this_proj, int i, int j)

  \brief test if the atom 'i' of species 'j' must be added to the tree store or not

  \param asearch the target atom search
  \param this_proj the target project
  \param i the atom id
  \param j the chemical species id
*/
gboolean append (atom_search * asearch, project * this_proj, int i, int j)
{
  int k;
  gchar * str_a, * str_b;
  k = this_proj -> modelgl -> anim -> last -> img -> step;
  gboolean append = FALSE;
  if (fill_for_action (asearch, i, j, this_proj))
  {
    if (asearch -> search_digit == -1)
    {
      if (asearch -> pointer[0].c == 1)
      {
        append = this_proj -> atoms[k][i].cloned;
      }
      else
      {
        append = TRUE;
      }
    }
    else
    {
      str_a = g_strdup_printf ("%d", i+1);
      str_b = g_strdup_printf ("%d", asearch -> search_digit);
      if (strlen (str_b) > strlen (str_a))
      {
        append = FALSE;
      }
      else
      {
        k = 0;
        append = TRUE;
        while (k < strlen (str_b))
        {
          if (str_a[k] != str_b[k])
          {
            append = FALSE;
            break;
          }
          k++;
        }
      }
      g_free (str_a);
      g_free (str_b);
    }
  }
  return append;
}

/*!
  \fn gchar * adjust_picked (gchar * picked, atomic_object * object, gboolean init)

  \brief adjust the name of the replacement object to use for a type of object if any

  \param picked type name (species, coordination, fragment, molecule name)
  \param object the target insert object
  \param init 1 / 0 depending on the atom search filter
*/
gchar * adjust_picked (gchar * picked, atomic_object * object, gboolean init)
{
  if (object)
  {
    if (init && ! picked)
    {
      return g_strdup_printf ("%s", object -> name);
    }
    else
    {
      if (g_strcmp0(picked, object -> name) == 0)
      {
        return g_strdup_printf ("%s", picked);
      }
      else
      {
        return NULL;
      }
    }
  }
  else
  {
    return NULL;
  }
}

/*!
  \fn gchar * get_node_name (int node, atom_search * asearch, project * this_proj)

  \brief get node label in the atom search tree store

  \param node the node id
  \param asearch the target atom search
  \param this_proj the target project
*/
gchar * get_node_name (int node, atom_search * asearch, project * this_proj)
{
  int i, j, k, l;
  int filter = get_asearch_filter (asearch);
  gchar * str;
  if (! filter)
  {
    str = g_strdup_printf ("%s", this_proj -> chemistry -> label[node]);
  }
  else if (filter == 1)
  {
    str = (node) ? g_strdup_printf ("%d-fold", node) : g_strdup_printf ("Isolated");
  }
  else if (filter == 2)
  {
    i = 0;
    for (j=0; j<this_proj -> nspec; j++)
    {
      i += this_proj -> coord -> ntg[1][j];
      if (i > node) break;
    }
    k = 0;
    for (l=0; l<j; l++) k += this_proj -> coord -> ntg[filter - 1][l];
    str = g_strdup_printf ("%s", env_name(this_proj, node-k, j, 1, NULL));
  }
  else if (filter == 3)
  {
    str = g_strdup_printf ("Fragment N°%d", node+1);
  }
  else
  {
    str = g_strdup_printf ("Molecule N°%d", node+1);
  }
  if (filter < 3 && asearch -> pointer[0].c == 1) str = g_strdup_printf ("%s<sup>*</sup>", str);
  return str;
}

/*!
  \fn void fill_atom_model (atom_search * asearch, project * this_proj)

  \brief fill atom search tree store

  \param asearch the target atom search
  \param this_proj the target project
*/
void fill_atom_model (atom_search * asearch, project * this_proj)
{
  GtkTreeIter spec_level, atom_level;
  int g, h, i, j, k, l, m, n;
  gchar * str;
  gboolean do_append;
  gboolean doit;
  gboolean * to_insert;
  gboolean is_object = get_asearch_is_object (asearch);
  gchar ** picked_names;
  int obj = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  int step = this_proj -> modelgl -> anim -> last -> img -> step;
  if (asearch -> action != INSERT)
  {
    if (asearch -> todo_size >= 10000 && ! asearch -> passivating && ! asearch -> mode)
    {
      if (! is_the_widget_visible(asearch -> info[1]))
      {
        show_the_widgets (asearch -> info[1]);
      }
      if (is_the_widget_visible (asearch -> id_box))
      {
        hide_the_widgets (asearch -> id_box);
      }
    }
    else
    {
      if (is_the_widget_visible(asearch -> info[1]))
      {
        hide_the_widgets (asearch -> info[1]);
      }
      if (asearch -> mode)
      {
        if (is_the_widget_visible(asearch -> id_box))
        {
          hide_the_widgets (asearch -> id_box);
        }
      }
      else
      {
        if (asearch -> passivating || (filter > 2 && obj == 2))
        {
          if (is_the_widget_visible (asearch -> id_box))
          {
            hide_the_widgets (asearch -> id_box);
          }
        }
        else
        {
          if (! is_the_widget_visible (asearch -> id_box))
          {
            show_the_widgets (asearch -> id_box);
          }
        }
      }
      int val = get_asearch_num_objects (asearch);
      to_insert = allocbool(val);
      n = 0;
      if (asearch -> action == REPLACE)
      {
        picked_names = g_malloc0(val*sizeof*picked_names);
        n = (asearch -> pointer[0].c == 3) ? 0 : 3;
      }
      switch (filter)
      {
        case 0:
          for (h=0; h<val; h++)
          {
            doit = TRUE;
            if (! asearch -> spec || asearch -> spec == h+1)
            {
              for (i=0; i<this_proj -> natomes; i++)
              {
                j = this_proj -> atoms[0][i].sp;
                if (j == h)
                {
                  to_insert[h] = append (asearch, this_proj, i, j);
                  if (to_insert[h])
                  {
                    if (asearch -> action == REPLACE && asearch -> in_selection)
                    {
                      picked_names[h] = adjust_picked (picked_names[h],
                                                       get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[n], (asearch -> mode) ? -(h+3) : (asearch -> passivating) ? h : i, 0),
                                                       doit);
                      doit = FALSE;
                    }
                    if (asearch -> action != REPLACE || n || ! picked_names[h] || asearch -> mode) break;
                  }
                }
              }
            }
          }
          break;
        case 1:
          for (h=0; h<val; h++)
          {
            doit = TRUE;
            for (i=0; i<this_proj -> natomes; i++)
            {
              if (this_proj -> atoms[0][i].numv == h)
              {
                j = this_proj -> atoms[0][i].sp;
                to_insert[h] = append (asearch, this_proj, i, j);
                if (to_insert[h])
                {
                  if (asearch -> action == REPLACE && asearch -> in_selection)
                  {
                    picked_names[h] = adjust_picked (picked_names[h],
                                                     get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[n], (asearch -> mode) ? -(h+3) : (asearch -> passivating) ? h : i, 0),
                                                     doit);
                  }
                  if (asearch -> action != REPLACE || n || ! picked_names[h] || asearch -> mode) break;
                }
              }
            }
          }
          break;
        default:
          if (val >= 10000 && (obj == 1 || asearch -> object == 3))
          {
            // Improbable: more than 10 000 fragments or molecules
            // Note: so far the selection and the test case functions are not ready yet
            to_insert[0] = TRUE;
            picked_names[0] = g_strdup_printf ("All %s(s)", (filter == 2) ? "fragment" : "molecule");
            val = 1;
          }
          else
          {
            for (h=0; h<val; h++)
            {
              doit = TRUE;
              for (i=0; i<this_proj -> natomes; i++)
              {
                if (asearch -> spec == 0 || asearch -> spec == this_proj -> atoms[0][i].sp + 1)
                {
                  j = this_proj -> atoms[step][i].coord[filter - 1];
                  if (filter == 2)
                  {
                    for (k=0; k<this_proj -> atoms[0][i].sp; k++) j += this_proj -> coord -> ntg[filter - 1][k];
                  }
                  if (j == h)
                  {
                    to_insert[h] = append (asearch, this_proj, i, this_proj -> atoms[0][i].sp);
                    if (to_insert[h])
                    {
                      if (asearch -> action == REPLACE && asearch -> in_selection)
                      {
                        picked_names[h] = adjust_picked (picked_names[h],
                                                         get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[n], (asearch -> mode) ? -(h+3) : (asearch -> passivating) ? h : i, 0),
                                                         doit);
                      }
                      if (asearch -> action != REPLACE || n || ! picked_names[h] || asearch -> mode) break;
                    }
                  }
                }
              }
            }
          }
          break;
      }
      if ((asearch -> action == REPLACE || asearch -> action == REMOVE) && asearch -> mode && obj && filter > 2)
      {
        if (filter == 3 && this_proj -> coord -> totcoord[2] > 1)
        {
          str = g_strdup_printf ("Fragments");
          gtk_tree_store_append (asearch -> atom_model, & spec_level, NULL);
          if (asearch -> set_for_all > 0) asearch -> pick[0] = asearch -> set_for_all;
          gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, -1, 1, str, TOPIC, asearch -> pick[0], -1);
          g_free (str);
          str = g_strdup_printf ("%d", asearch -> todo[0]);
          j = (asearch -> action == REMOVE) ? 1 : 2;
          gtk_tree_store_set (asearch -> atom_model, & spec_level,  TOPIC+j, str, -1);
          g_free (str);
          if (asearch -> action == REPLACE)
          {
            if (picked_names[0])
            {
              str = g_strdup_printf ("For all: %s", picked_names[0]);
              gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, str, -1);
              g_free (str);
            }
            else
            {
              gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, "Select for all ...", -1);
            }
          }
        }
        else if (filter == 4 && this_proj -> modelfc)
        {
          for (k=0; k<this_proj -> modelfc -> mol_by_step[0]; k++)
          {
            if (this_proj -> modelfc -> mols[0][k].multiplicity > 1)
            {
              str = g_strdup_printf ("Molecule N°%d", k+1);
              gtk_tree_store_append (asearch -> atom_model, & spec_level, NULL);
              if (asearch -> set_for_all > 0) asearch -> pick[k] = asearch -> set_for_all;
              gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, -(k+1), 1, str, TOPIC, asearch -> pick[k], -1);
              g_free (str);
              str = g_strdup_printf ("%d", asearch -> todo[k]);
              gtk_tree_store_set (asearch -> atom_model, & spec_level,  TOPIC+2, str, -1);
              g_free (str);
              if (asearch -> action == REPLACE)
              {
                if (picked_names[k])
                {
                  str = g_strdup_printf ("For all: %s", picked_names[k]);
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, str, -1);
                  g_free (str);
                }
                else
                {
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, "Select for all ...", -1);
                }
              }
            }
          }
        }
      }
      else
      {
        for (h=0; h<val; h++)
        {
          if (to_insert[h])
          {
            gtk_tree_store_append (asearch -> atom_model, & spec_level, NULL);
            if (asearch -> action == REPLACE && ! asearch -> mode)
            {
              g = (asearch -> pointer[0].c == 3) ? 0 : 1;
              this_proj -> modelgl -> atom_win -> replace_nodes[g][h] = gtk_tree_model_get_path (GTK_TREE_MODEL(asearch -> atom_model), & spec_level);
            }
            str = get_node_name (h, asearch, this_proj);
            if (asearch -> set_for_all > 0) asearch -> pick[h] = asearch -> set_for_all;
            if (asearch -> action < RANMOVE || asearch -> action > RANMOVE)
            {
              gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, -(h+1), 1, str, TOPIC, asearch -> pick[h], -1);
              if (asearch -> passivating)
              {
                gtk_tree_store_set (asearch -> atom_model, & spec_level, TOLAB, asearch -> lab[h], -1);
              }

              if (asearch -> set_for_all > 0 && asearch -> action == DISPL && filter > 2)
              {
                  asearch -> todo[h] = asearch -> set_for_all;
                  adjust_object_to_move (this_proj, asearch, 0, h);
              }
              if (asearch -> mode)
              {
                g_free (str);
                str = g_strdup_printf ("%d", asearch -> todo[h]);
                gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+5-asearch -> action, str, -1);
              }
            }
            else if (asearch -> action == RANMOVE)
            {
              i = (asearch -> pick[h] == 1 || asearch -> pick[h] == 3) ? 1 : 0;
              gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, -(h+1), 1, str, TOPIC, i, -1);
              if (is_object)
              {
                i = (asearch -> pick[h] == 2 || asearch -> pick[h] == 3) ? 1 : 0;
                gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, i, -1);
                if (asearch -> set_for_all > 0)
                {
                  if (filter > 2)
                  {
                    asearch -> todo[h] = asearch -> set_for_all;
                    adjust_object_to_move (this_proj, asearch, 1, h);
                  }
                }
              }
            }
            g_free (str);
            if (asearch -> action == REPLACE)
            {
              if (picked_names[h])
              {
                if (asearch -> mode || (obj && filter > 2))
                {
                  str = g_strdup_printf ("%s", picked_names[h]);
                }
                else
                {
                  str = g_strdup_printf ("For all: %s", picked_names[h]);
                }
                gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, str, -1);
                g_free (str);
              }
              else
              {
                if (asearch -> mode || (obj && filter > 2))
                {
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, "Select ...", -1);
                }
                else
                {
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, "Select for all ...", -1);
                }
              }
            }
            else if (asearch -> action == RANMOVE)
            {
              if (this_proj -> modelgl -> atom_win -> msd_all[h] > 0.0)
              {
                if (obj && filter > 2)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd_all[h]);
                }
                else
                {
                  str = g_strdup_printf ("For all: MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd_all[h]);
                }
                gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+2, str, -1);
                g_free (str);
              }
              else
              {
                if (obj && filter > 2)
                {
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+2, "Set MSD<sub>max</sub> for ...", -1);
                }
                else
                {
                  gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+2, "Set MSD<sub>max</sub> for all ...", -1);
                }
              }
            }
            doit = FALSE;
            if (asearch -> action > REMOVE && asearch -> action != RANMOVE)
            {
              doit = TRUE;
            }
            else
            {
              doit = (asearch -> passivating) ? FALSE : ! asearch -> mode;
            }
            if (doit && (! obj || (filter > 0 && filter < 3)))
            {
              for (i=0; i<this_proj -> natomes; i++)
              {
                do_append = FALSE;
                j = this_proj -> atoms[0][i].sp;
                if (! filter)
                {
                  if (j == h && append (asearch, this_proj, i, j)) do_append=TRUE;
                }
                else
                {
                  k = filter - 1;
                  l = this_proj -> atoms[step][i].coord[k];
                  if (filter == 1)
                  {
                    m = this_proj -> coord -> geolist[0][j][l];
                    if (m == h && append (asearch, this_proj, i, j)) do_append=TRUE;
                  }
                  else
                  {
                    if (filter == 2) for (m=0; m<j; m++) l += this_proj -> coord -> ntg[1][m];
                    if (l == h && append (asearch, this_proj, i, j)) do_append=TRUE;
                  }
                }
                if (do_append)
                {
                  gtk_tree_store_append (asearch -> atom_model, & atom_level, & spec_level);
                  append_to_model (& atom_level, asearch, is_object, h, i, this_proj);
                }
              }
            }
          }
        }
        g_free (to_insert);
        if (asearch -> action == DISPL) check_motion_interactors (this_proj, asearch);
        if (asearch -> action == REPLACE) g_free (picked_names);
      }
      //if (asearch -> passivating)
      check_tree_for_this_search (this_proj, asearch);
    }
  }
  else
  {
    i = (asearch -> pointer[0].c == 7) ? 2 : 1;
    atomic_object * iobj = this_proj -> modelgl -> atom_win -> to_be_inserted[i];
    j = 0;
    while (iobj)
    {
      gtk_list_store_append (asearch -> obj_model, & atom_level);
      str = g_strdup_printf ("x= %f, y= %f, z= %f", iobj -> baryc[0], iobj -> baryc[1], iobj -> baryc[2]);
      gtk_list_store_set (asearch -> obj_model, & atom_level, IDCOL, j+1, 1, g_strdup_printf ("%d", j+1), 2, iobj -> name,
                          TOLAB, asearch -> todo[j], TOPIC, str, -1);
      g_free (str);
      if (i == 2)
      {
        str = g_strdup_printf ("%f", iobj -> occ);
        gtk_list_store_set (asearch -> obj_model, & atom_level, TOPIC+1, str, -1);
        g_free (str);
      }
      j ++;
      iobj = iobj -> next;
    }
  }
  if (asearch -> set_for_all > 0) asearch -> set_for_all = -asearch -> set_for_all;
}

G_MODULE_EXPORT void move_up_down (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data);
void add_random_column (atom_search * asearch);

/*!
  \fn void update_search_tree (atom_search * asearch)

  \brief update search tree

  \param asearch the target atom search
*/
void update_search_tree (atom_search * asearch)
{
  if (asearch -> pointer[0].c == 7 && asearch -> obj_model)
  {
    g_signal_handler_disconnect (G_OBJECT(GTK_TREE_MODEL(asearch -> obj_model)), asearch -> filter);
    gtk_list_store_clear (asearch -> obj_model);
    asearch -> filter =  g_signal_connect_data (G_OBJECT(GTK_TREE_MODEL(asearch -> obj_model)), "row-deleted", G_CALLBACK(move_up_down), asearch, NULL, (GConnectFlags) 0);
  }
  else
  {
    if (asearch -> atom_model) gtk_tree_store_clear (asearch -> atom_model);
    if (asearch -> obj_model) gtk_list_store_clear (asearch -> obj_model);
  }
  fill_atom_model (asearch, get_project_by_id(asearch -> proj));
  //if (asearch -> action) gtk_tree_view_expand_all (GTK_TREE_VIEW(asearch -> atom_tree));
}

/*!
  \fn gboolean update_this_search (atom_search * asearch)

  \brief test if it is required to update the search tree or not

  \param asearch the target atom search
*/
gboolean update_this_search (atom_search * asearch)
{
  int object = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  if (asearch -> passivating)
  {
    return FALSE;
  }
  else if (! asearch -> passivating && asearch -> todo_size >= 10000)
  {
    return FALSE;
  }
  else if (! (object && filter > 2))
  {
    return TRUE;
  }
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void set_atom (GtkEntry * entry, gpointer data)

  \brief adjust atom search parameters if >= 10 000 atoms

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atom (GtkEntry * entry, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  int i;
  project * this_proj = get_project_by_id(asearch -> proj);
  gchar * str_a, * str_b;
  str_a = g_strdup_printf ("%s", entry_get_text (GTK_ENTRY(asearch -> entry_a)));
  asearch -> spec_to_add = -1;
  for (i = 0; i < this_proj -> nspec; i++)
  {
    str_b = g_strdup_printf ("%s", exact_name(this_proj -> chemistry -> label[i]));
    if (g_strcmp0 (str_a, str_b) == 0)
    {
      asearch -> spec_to_add = i;
    }
    g_free (str_b);
  }
  g_free (str_a);
  if (asearch -> spec_to_add != -1)
  {
    widget_set_sensitive (asearch -> entry_b, 1);
    set_image_from_icon_name (asearch -> img_a, APPLY);
  }
  else
  {
    widget_set_sensitive (asearch -> entry_b, 0);
    widget_set_sensitive (asearch -> but_a, 0);
    widget_set_sensitive (asearch -> but_b, 0);
    update_entry_text (GTK_ENTRY(asearch -> entry_b), "");
    set_image_from_icon_name (asearch -> img_a, DIAL_ERROR);
  }
}

/*!
  \fn void clear_fields (atom_search * asearch)

  \brief clear all search fields if >= 10 000 atoms

  \param asearch the target atom search
*/
void clear_fields (atom_search * asearch)
{
  int i = combo_get_active (asearch -> atom_box);
  if (i)
  {
    update_entry_text (GTK_ENTRY(asearch -> entry_a), exact_name(get_project_by_id(asearch -> proj) -> chemistry -> label[i-1]));
  }
  else
  {
    update_entry_text (GTK_ENTRY(asearch -> entry_a), "");
  }
  set_atom (GTK_ENTRY(asearch -> entry_a), asearch);
  update_entry_text (GTK_ENTRY(asearch -> entry_b), "");
  set_image_from_icon_name (asearch -> img_b, DIAL_ERROR);
}

/*!
  \fn gboolean remove_from_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)

  \brief remove from the tree model

  \param model the tree model
  \param path the path in the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
gboolean remove_from_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  GtkTreeIter parent;
  atom_search * asearch = (atom_search *)data;
  project * this_proj;
  if (gtk_tree_model_get_iter (model, iter, path))
  {
    gtk_tree_model_get (model,  iter, IDCOL, & i, -1);
    if (i == asearch -> int_b)
    {
      j = (i > 0) ? i -- : abs(i) - 1;
      asearch -> todo[j] = 0;
      if (asearch -> action == REPLACE)
      {
        j = (i > 0) ? i -- : i - 2;
        this_proj = get_project_by_id(asearch -> proj);
        clean_this_object (j, 0, this_proj, asearch);
        asearch -> in_selection --;
      }
      if (gtk_tree_model_iter_parent (model, & parent, iter))
      {
        gtk_tree_store_remove (GTK_TREE_STORE(model), iter);
        if (! gtk_tree_model_iter_has_child(model, & parent))
        {
          gtk_tree_store_remove (GTK_TREE_STORE(model), & parent);
          if (asearch -> action == REPLACE)
          {
            j = abs(i) - 1;
            gtk_tree_path_free (this_proj -> modelgl -> atom_win -> replace_nodes[0][j]);
            this_proj -> modelgl -> atom_win -> replace_nodes[0][j] = NULL;
          }
        }
      }
      else
      {
        gtk_tree_store_remove (GTK_TREE_STORE(model), iter);
      }
      return TRUE;
    }
  }
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void remove_atom (GtkButton * but, gpointer data)

  \brief remove atom (or object) from the search tree if >= 10 000 atoms

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void remove_atom (GtkButton * but, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  asearch -> int_b = (int )string_to_double ((gpointer)entry_get_text (GTK_ENTRY(asearch -> entry_b)));
  gtk_tree_model_foreach (GTK_TREE_MODEL(asearch -> atom_model), remove_from_model, asearch);
  clear_fields (asearch);
}

/*!
  \fn gboolean atom_is_in_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)

  \brief is this atom already in the tree model if >= 10 000 atoms

  \param model the target tree model
  \param path the path in the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
gboolean atom_is_in_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  int b;
  atom_search * asearch = (atom_search *)data;
  if (gtk_tree_model_get_iter (model, iter, path))
  {
    gtk_tree_model_get (model,  iter, IDCOL, & b, -1);
    if (b == asearch -> int_b)
    {
      asearch -> was_selected = TRUE;
      asearch -> path = gtk_tree_model_get_path (model, iter);
      return TRUE;
    }
  }
  return FALSE;
}

/*!
  \fn G_MODULE_EXPORT void add_atom (GtkButton * but, gpointer data)

  \brief add atom (or object) to the search tree if >= 10 000 atoms

  \param but the GtkButton sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_atom (GtkButton * but, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  project * this_proj = get_project_by_id(asearch -> proj);
  GtkTreeIter spec_level, atom_level, new_level;
  int i, j, k, l, m;
  i = this_proj -> modelgl -> anim -> last -> img -> step;
  j = asearch -> spec_to_add;
  int filter = get_asearch_filter (asearch);
  gboolean is_object = get_asearch_is_object (asearch);
  if (! asearch -> mode)
  {
    if (append (asearch, this_proj, -1, j))
    {
      asearch -> was_selected = FALSE;
      m = asearch -> int_b;
      asearch -> int_b = j+1;
      gtk_tree_model_foreach (GTK_TREE_MODEL(asearch -> atom_model), atom_is_in_model, asearch);
      if (! asearch -> was_selected)
      {
        switch (filter)
        {
          case 0:
            l = j;
            break;
          case 1:
            l = this_proj -> atoms[i][asearch -> num_to_add].numv;
            break;
          case 2:
            l = this_proj -> atoms[i][asearch -> num_to_add].coord[filter - 1];
            for (k=0; k<j; k++) l += this_proj -> coord -> ntg[filter - 1][k];
            break;
          default:
            l = this_proj -> atoms[i][asearch -> num_to_add].coord[filter - 1];
            break;
        }
        asearch -> int_b = -(l+1);
        gchar * str = get_node_name (l, asearch, this_proj);
        asearch -> was_selected = FALSE;
        gtk_tree_model_foreach (GTK_TREE_MODEL(asearch -> atom_model), atom_is_in_model, asearch);
        if (! asearch -> was_selected)
        {
          gtk_tree_store_append (asearch -> atom_model, & spec_level, NULL);
          if (asearch -> action == REPLACE)
          {
            this_proj -> modelgl -> atom_win -> replace_nodes[0][l] = gtk_tree_model_get_path (GTK_TREE_MODEL(asearch -> atom_model), & spec_level);
          }
          gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, -(l+1), 1, str, -1);
          gtk_tree_store_append (asearch -> atom_model, & atom_level, & spec_level);
          append_to_model (& atom_level, asearch, is_object, l, asearch -> num_to_add, this_proj);
        }
        else
        {
          // Find out where to insert that node !
          gtk_tree_model_get_iter (GTK_TREE_MODEL(asearch -> atom_model), & spec_level, asearch -> path);
          if (gtk_tree_model_iter_children (GTK_TREE_MODEL(asearch -> atom_model), & atom_level, & spec_level))
          {
            gboolean append = FALSE;
            gboolean dothat = TRUE;
            k = 0;
            while (dothat)
            {
              gtk_tree_model_get (GTK_TREE_MODEL(asearch -> atom_model), & atom_level, IDCOL, & k, -1);
              if (k > asearch -> num_to_add)
              {
                dothat = FALSE;
              }
              else
              {
                dothat = gtk_tree_model_iter_next (GTK_TREE_MODEL(asearch -> atom_model), & atom_level);
                append = TRUE;
              }
            }
            gtk_tree_store_insert_before (asearch -> atom_model, & new_level, & spec_level, (append) ? NULL : & atom_level);
            append_to_model (& new_level, asearch, l, is_object, asearch -> num_to_add, this_proj);
          }
        }
        g_free (str);
        asearch -> int_b = m;
        if (asearch -> action == REPLACE)
        {
          gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+1, "Select for all ...", -1);
        }
        else if (asearch -> action == RANMOVE)
        {
          str = g_strdup_printf  ("Set MSD<sub>max</sub> for all ...");
          gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+2, str, -1);
          g_free (str);
        }
      }
    }
  }
  clear_fields (asearch);
}

/*!
  \fn void re_populate_tree_search (atom_search * asearch)

  \brief re populate search tree after atom action if >= 10 000 atoms

  \param asearch the target atom search
*/
void re_populate_tree_search (atom_search * asearch)
{
  project * this_proj = get_project_by_id (asearch -> proj);
  GtkTreeIter spec_level, atom_level, new_level;
  int i, j, k, l, m;
  i = this_proj -> modelgl -> anim -> last -> img -> step;
  int filter = get_asearch_filter (asearch);
  gboolean is_object = get_asearch_is_object (asearch);
  int s_int_b = asearch -> int_b;
  for (j=0; j<asearch -> todo_size; j++)
  {
    if (asearch -> todo[j])
    {
      k = this_proj -> atoms[i][j].sp;
      if (append (asearch, this_proj, -1, k))
      {
        asearch -> was_selected = FALSE;
        switch (filter)
        {
          case 0:
            l = k;
            break;
          case 1:
            l = this_proj -> atoms[i][j].numv;
            break;
          case 2:
            l = this_proj -> atoms[i][j].coord[filter - 1];
            for (m=0; m<k; m++) l += this_proj -> coord -> ntg[filter - 1][m];
            break;
          default:
            l = this_proj -> atoms[i][j].coord[filter - 1];
            break;
        }
        asearch -> int_b = -(l+1);
        gtk_tree_model_foreach (GTK_TREE_MODEL(asearch -> atom_model), atom_is_in_model, asearch);
        if (! asearch -> was_selected)
        {
          gchar * str = get_node_name (l, asearch, this_proj);
          gtk_tree_store_append (asearch -> atom_model, & spec_level, NULL);
          gtk_tree_store_set (asearch -> atom_model, & spec_level, IDCOL, asearch -> int_b, 1, str, -1);
          if (asearch -> action == RANMOVE)
          {
            str = g_strdup_printf  ("Set MSD<sub>max</sub> for all ...");
            gtk_tree_store_set (asearch -> atom_model, & spec_level, TOPIC+2, str, -1);
            g_free (str);
          }
          gtk_tree_store_append (asearch -> atom_model, & atom_level, & spec_level);
          append_to_model (& atom_level, asearch, is_object, l, j, this_proj);
        }
        else
        {
          // Find out where to insert that node !
          gtk_tree_model_get_iter (GTK_TREE_MODEL(asearch -> atom_model), & spec_level, asearch -> path);
          if (gtk_tree_model_iter_children (GTK_TREE_MODEL(asearch -> atom_model), & atom_level, & spec_level))
          {
            gboolean append = FALSE;
            gboolean dothat = TRUE;
            k = 0;
            while (dothat)
            {
              gtk_tree_model_get (GTK_TREE_MODEL(asearch -> atom_model), & atom_level, IDCOL, & k, -1);
              if (k > j)
              {
                dothat = FALSE;
              }
              else
              {
                dothat = gtk_tree_model_iter_next (GTK_TREE_MODEL(asearch -> atom_model), & atom_level);
                append = TRUE;
              }
            }
            gtk_tree_store_insert_before (asearch -> atom_model, & new_level, & spec_level, (append) ? NULL : & atom_level);
            append_to_model (& new_level, asearch, is_object, l, j, this_proj);
          }
        }
      }
    }
  }
  asearch -> int_b = s_int_b;
}

/*!
  \fn G_MODULE_EXPORT void set_id (GtkEntry * entry, gpointer data)

  \brief set search id if >= 10 000 atoms

  \param entry the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_id (GtkEntry * entry, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  project * this_proj = get_project_by_id(asearch -> proj);
  asearch -> int_b = (int )string_to_double ((gpointer)entry_get_text (GTK_ENTRY(asearch -> entry_b)));
  if (asearch -> int_b > 0 && asearch -> int_b < this_proj -> natomes+1)
  {
    asearch -> was_selected = FALSE;
    gtk_tree_model_foreach (GTK_TREE_MODEL(asearch -> atom_model), atom_is_in_model, asearch);
    int s = this_proj -> modelgl -> anim -> last -> img -> step;
    asearch -> int_b --;
    if (asearch -> was_selected && this_proj -> atoms[s][asearch -> int_b].sp == asearch -> spec_to_add)
    {
      widget_set_sensitive (asearch -> but_a, 0);
      widget_set_sensitive (asearch -> but_b, 1);
      set_image_from_icon_name (asearch -> img_b, APPLY);
    }
    else
    {
      if (this_proj -> atoms[s][asearch -> int_b].sp == asearch -> spec_to_add)
      {
        asearch -> num_to_add = asearch -> int_b;
        widget_set_sensitive (asearch -> but_a, 1);
        widget_set_sensitive (asearch -> but_b, 0);
        set_image_from_icon_name (asearch -> img_b, APPLY);
      }
      else
      {
        widget_set_sensitive (asearch -> but_a, 0);
        widget_set_sensitive (asearch -> but_b, 0);
        set_image_from_icon_name (asearch -> img_b, DIAL_ERROR);
      }
    }
  }
}

/*!
  \fn void adjust_search_param (atom_search * asearch, project * this_proj, int a, int s, int c, gboolean status)

  \brief adjust parameters for this atom search

  \param asearch the target atom search
  \param this_proj the target project
  \param a the atom id
  \param s the chemical species, or object id
  \param c the column id
  \param status the new toogle status
*/
void adjust_search_param (atom_search * asearch, project * this_proj, int a, int s, int c, gboolean status)
{
  int i, j, k, l, m;
  opengl_project_changed (asearch -> proj);
  i = (a < 0) ? 0 : a;
  for (j=i; j<this_proj -> natomes; j++)
  {
    k = this_proj -> atoms[0][j].pick[0];
    l = this_proj -> atoms[0][j].sp;
    m = this_proj -> atoms[0][j].label[0];
    if (s < 0 || l == s)
    {
      if (asearch -> status == 2 || asearch -> status == k)
      {
        if (a == j || a < 0)
        {
          switch (c)
          {
            case TOLAB:
#ifdef GTK4
              if (m != status) label_unlabel_this_atom (NULL, NULL, GINT_TO_POINTER(j));
#else
              if (m != status) label_unlabel_this_atom (NULL, GINT_TO_POINTER(j));
#endif
              break;
            default:
              if (asearch -> action != RANMOVE)
              {
                asearch -> todo[j] = status;
                if (asearch -> action == DISPL && asearch -> object) adjust_object_to_move (this_proj, asearch, 0, j);
              }
              else
              {
                switch (asearch -> todo[j])
                {
                  case 0:
                    asearch -> todo[j] += (c-TOLAB);
                    break;
                  case 1:
                    if (c == TOPIC)
                    {
                       asearch -> todo[j] -= (c-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[j] += (c-TOLAB);
                    }
                    break;
                  case 2:
                    if (c == TOPIC)
                    {
                      asearch -> todo[j] += (c-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[j] -= (c-TOLAB);
                    }
                    break;
                  case 3:
                    asearch -> todo[j] -= (c-TOLAB);
                    break;
                }
                if (asearch -> object) adjust_object_to_move (this_proj, asearch, 1, j);
              }
              break;
          }
        }
        if (a == j) break;
      }
    }
  }
}

G_MODULE_EXPORT void set_spec_changed (GtkComboBox * box, gpointer data);

/*!
  \fn void adjust_this_tree_branch (atom_search * asearch, project * this_proj, int oid, int sid, GtkTreeIter iter)

  \brief adjust tree branch (for an entire type of object)

  \param asearch the target atom search
  \param this_proj the target project
  \param oid the column id
  \param sid the chemical species or object type id
  \param iter the tree iter
*/
void adjust_this_tree_branch (atom_search * asearch, project * this_proj, int oid, int sid, GtkTreeIter iter)
{
  int k, l, m, n, o, p, q;
  int status;
  int * to_label = NULL;
  int is_clone = (asearch -> action == 1) ? 1 : 0;
  int object = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  gboolean doit;
  GtkTreeModel * model = (asearch -> action == INSERT) ? GTK_TREE_MODEL(asearch -> obj_model) : GTK_TREE_MODEL(asearch -> atom_model);
  opengl_project_changed (this_proj -> id);
  gtk_tree_model_get (model, & iter, oid, & status, -1);
  atom_in_selection * selected;
  if (asearch -> action < 2 && oid == TOPIC)
  {
    selected_status = ! status;
    switch (filter)
    {
      case 0:
        selected_aspec = sid;
#ifdef GTK4
        select_unselect_atoms (NULL, NULL, & this_proj -> modelgl -> colorp[0][! status]);
#else
        select_unselect_atoms (NULL, & this_proj -> modelgl -> colorp[0][! status]);
#endif
        break;
      default:
        if (filter < 3)
        {
          k = this_proj -> modelgl -> selection_mode;
          this_proj -> modelgl -> selection_mode = object;
        }
        p = this_proj -> modelgl -> anim -> last -> img -> step;
        for (l=0; l<this_proj -> natomes; l++)
        {
          n = this_proj -> atoms[p][l].sp;
          o = this_proj -> atoms[p][l].coord[filter-1];
          doit = FALSE;
          if (filter == 1)
          {
            if (this_proj -> atoms[p][l].numv == sid && append (asearch, this_proj, l, n)) doit = TRUE;
          }
          else
          {
            if (filter == 2)
            {
              for (m=0; m<n; m++)
              {
                o += this_proj -> coord -> ntg[1][m];
              }
            }
            if (o == sid  && append (asearch, this_proj, l, n)) doit = TRUE;
          }
          if (doit)
          {
            o = get_to_be_selected (this_proj -> modelgl);
#ifdef GTK4
            if (this_proj -> atoms[p][l].pick[o] != status)
            {
              select_unselect_this_atom (NULL, NULL, GINT_TO_POINTER(l));
            }
            select_unselect_this_atom (NULL, NULL, GINT_TO_POINTER(l));
#else
            if (this_proj -> atoms[p][l].pick[o] != status)
            {
              select_unselect_this_atom (NULL, GINT_TO_POINTER(l));
            }
            select_unselect_this_atom (NULL, GINT_TO_POINTER(l));
#endif
          }
        }
        if (filter < 3)  this_proj -> modelgl -> selection_mode = k;
        break;
    }
  }
  else
  {
    if (asearch -> action != INSERT)
    {
      if (oid == TOLAB)
      {
        if (asearch -> pointer[0].c == 8)
        {
          preserve_ogl_selection (this_proj -> modelgl);
          this_proj -> modelgl -> cell_win -> cut_this_slab = TRUE;
          save_all_selections (this_proj -> modelgl, 0);
          create_slab_lists (this_proj);
          update_all_selections (this_proj -> modelgl, 0);
          q = 0;
          if (this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected)
          {
            to_label = allocint (this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected);
          }
        }
        p = this_proj -> modelgl -> anim -> last -> img -> step;
        switch (filter)
        {
          case 0:
            if (asearch -> pointer[0].c == 8)
            {
              if (this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected)
              {
                selected = this_proj -> modelgl -> anim -> last -> img -> selected[0] -> first;
                while (selected)
                {
                  if (selected -> sp == sid)
                  {
                    to_label[q] = selected -> id;
                    q ++;
                  }
                  selected = selected -> next;
                }
              }
            }
            else
            {
#ifdef GTK4
              show_hide_labels (NULL, NULL, & this_proj -> modelgl -> colorp[is_clone][sid]);
#else
              // GTK3 Menu Action To Check
              gtk_check_menu_item_set_active ((GtkCheckMenuItem *)this_proj -> modelgl -> ogl_lab[is_clone][sid], ! status);
              show_hide_labels (this_proj -> modelgl -> ogl_lab[is_clone][sid], & this_proj -> modelgl -> colorp[is_clone][sid]);
#endif
            }
            break;
          default:
            if (filter < 3)
            {
              k = this_proj -> modelgl -> selection_mode;
              this_proj -> modelgl -> selection_mode = object;
            }
            for (l=0; l<this_proj -> natomes; l++)
            {
              n = this_proj -> atoms[p][l].sp;
              o = this_proj -> atoms[p][l].coord[filter-1];
              doit = FALSE;
              if (filter == 1)
              {
                if (this_proj -> atoms[p][l].numv == sid && append (asearch, this_proj, l, n)) doit = TRUE;
              }
              else if (filter == 2 || filter == 4)
              {
                if (filter == 2)
                {
                  for (m=0; m<n; m++)
                  {
                    o += this_proj -> coord -> ntg[1][m];
                  }
                }
                if (o == sid && append (asearch, this_proj, l, n)) doit = TRUE;
              }
              else if (filter == 3)
              {
                if (asearch -> mode && object)
                {
                  doit = TRUE;
                }
                else
                {
                  if (o == sid && append (asearch, this_proj, l, n)) doit = TRUE;
                }
              }
              if (doit)
              {
                if (asearch -> pointer[0].c == 8)
                {
                  if (this_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected)
                  {
                    selected = this_proj -> modelgl -> anim -> last -> img -> selected[0] -> first;
                    while (selected)
                    {
                      if (selected -> id == l)
                      {
                        to_label[q] = l;
                        q ++;
                      }
                      selected = selected -> next;
                    }
                  }
                }
                else
                {
                  this_proj -> atoms[p][l].label[0] = status;
#ifdef GTK4
                  label_unlabel_this_atom (NULL, NULL, GINT_TO_POINTER(l));
#else
                  label_unlabel_this_atom (NULL, GINT_TO_POINTER(l));
#endif
                }
              }
            }
            if (filter < 3) this_proj -> modelgl -> selection_mode = k;
            break;
        }

        if (asearch -> pointer[0].c == 8)
        {
          restore_ogl_selection (this_proj -> modelgl);
          if (q)
          {
            for (l=0; l<q; l++)
            {
              m = to_label[l];
              this_proj -> atoms[0][m].label[0] = ! status;
              this_proj -> atoms[0][m].label[1] = ! status;
            }
            g_free (to_label);
          }
          this_proj -> modelgl -> cell_win -> cut_this_slab = FALSE;
          this_proj -> modelgl -> create_shaders[SLABS] = TRUE;
          gtk_tree_store_set (asearch -> atom_model, & iter, TOLAB, ! status, -1);
        }
      }
      else
      {
        if (asearch -> passivating || (object && filter > 2))
        {
          if (asearch -> action == RANMOVE)
          {
            if (! status)
            {
              asearch -> todo[sid] += (oid-TOLAB);
            }
            else
            {
              asearch -> todo[sid] -= (oid-TOLAB);
            }
            if (object && ! asearch -> passivating) adjust_object_to_move (this_proj, asearch, 1, sid);
          }
          else
          {
            asearch -> todo[sid] = ! status;
            if (asearch -> action == DISPL)
            {
              if (object && ! asearch -> passivating) adjust_object_to_move (this_proj, asearch, 0, sid);
              motion_to_zero (asearch);
            }
          }
        }
        else if (! asearch -> mode)
        {
          switch (filter)
          {
            case 0:
              for (l=0; l<this_proj -> natomes; l++)
              {
                n = this_proj -> atoms[0][l].sp;
                if (n == sid && append (asearch, this_proj, l, n))
                {
                  if (asearch -> action == RANMOVE)
                  {
                    if (! status)
                    {
                      asearch -> todo[l] += (oid-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[l] -= (oid-TOLAB);
                    }
                    if (object) adjust_object_to_move (this_proj, asearch, 1, l);
                  }
                  else
                  {
                    asearch -> todo[l] = ! status;
                    if (asearch -> action == DISPL && object) adjust_object_to_move (this_proj, asearch, 0, l);
                  }
                }
              }
              break;
            case 1:
              for (l=0; l<this_proj -> natomes; l++)
              {
                n = this_proj -> atoms[0][l].sp;
                if (this_proj -> atoms[0][l].numv == sid && append (asearch, this_proj, l, n))
                {
                  if (asearch -> action == RANMOVE)
                  {
                    if (! status)
                    {
                      asearch -> todo[l] += (oid-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[l] -= (oid-TOLAB);
                    }
                    if (object) adjust_object_to_move (this_proj, asearch, 1, l);
                  }
                  else
                  {
                    asearch -> todo[l] = ! status;
                    if (asearch -> action == DISPL && object) adjust_object_to_move (this_proj, asearch, 0, l);
                  }
                }
              }
              break;
            case 2:
              for (l=0; l<this_proj -> natomes; l++)
              {
                n = this_proj -> atoms[0][l].sp;
                o = this_proj -> atoms[0][l].coord[filter-1];
                for (m=0; m<n; m++)
                {
                  o += this_proj -> coord -> ntg[1][m];
                }
                 if (o == sid  && append (asearch, this_proj, l, n))
                {
                  if (asearch -> action == RANMOVE)
                  {
                    if (! status)
                    {
                      asearch -> todo[l] += (oid-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[l] -= (oid-TOLAB);
                    }
                    if (object) adjust_object_to_move (this_proj, asearch, 1, l);
                  }
                  else
                  {
                    asearch -> todo[l] = ! status;
                    if (asearch -> action == DISPL && object) adjust_object_to_move (this_proj, asearch, 0, l);
                  }
                }
              }
              break;
            default:
              for (l=0; l<this_proj -> natomes; l++)
              {
                n = this_proj -> atoms[0][l].sp;
                o = this_proj -> atoms[0][l].coord[filter-1];
                if (o == sid  && append (asearch, this_proj, l, n))
                {
                  if (asearch -> action == RANMOVE)
                  {
                    if (! status)
                    {
                      asearch -> todo[l] += (oid-TOLAB);
                    }
                    else
                    {
                      asearch -> todo[l] -= (oid-TOLAB);
                    }
                    if (object) adjust_object_to_move (this_proj, asearch, 1, l);
                  }
                  else
                  {
                    asearch -> todo[l] = ! status;
                    if (asearch -> action == DISPL && object) adjust_object_to_move (this_proj, asearch, 0, l);
                  }
                }
              }
              break;
          }
        }
      }
    }
    else
    {
      for (k=0; k<asearch -> in_selection; k++)
      {
        asearch -> todo[k] = ! status;
      }
    }
  }
  switch (oid)
  {
    case TOLAB:
      if (asearch -> action == INSERT) asearch -> pick[sid] = ! status;
      asearch -> lab[sid] = ! status;
      break;
    default:
      if (asearch -> action == RANMOVE)
      {
        if (! status)
        {
          asearch -> pick[sid] += (oid-TOLAB);
        }
        else
        {
          asearch -> pick[sid] -= (oid-TOLAB);
        }
      }
      else
      {
        asearch -> pick[sid] = ! status;
      }
      break;
  }
  if (asearch -> todo_size < 10000 || asearch -> passivating || (object == 2 && filter > 2)) update_search_tree (asearch);
}

/*!
  \fn void adjust_this_tree_leaf (atom_search * asearch, project * this_proj, int oid, int aid, int new_status, GtkTreeIter iter)

  \brief adjust tree leaf (for a single object)

  \param asearch the target atom search
  \param this_proj the target project
  \param oid the column id
  \param aid the atom id
  \param new_status the new toggle status
  \param iter the tree iter
*/
void adjust_this_tree_leaf (atom_search * asearch, project * this_proj, int oid, int aid, int new_status, GtkTreeIter iter)
{
  int status;
  int k, l, p;
  opengl_project_changed (this_proj -> id);
  int sel = this_proj -> modelgl -> selection_mode;
  int is_clone = (asearch -> action == 1) ? 1 : 0;
  int object = (asearch -> object) ? 1 : 0;
  this_proj -> modelgl -> selection_mode = object;
  p = this_proj -> modelgl -> anim -> last -> img -> step;
  // For atom i
  if (oid == TOLAB)
  {
    if (asearch -> action == INSERT)
    {
      if (asearch -> todo[aid] != new_status || new_status < 0)
      {
        status = ! asearch -> todo[aid];
        asearch -> todo[aid] = status;
      }
    }
    else if (this_proj -> atoms[p][aid].label[is_clone] != new_status || new_status < 0)
    {
#ifdef GTK4
      label_unlabel_this_atom (NULL, NULL, GINT_TO_POINTER(aid));
#else
      label_unlabel_this_atom (NULL, GINT_TO_POINTER(aid));
#endif
      status = this_proj -> atoms[p][aid].label[is_clone];
    }
  }
  else
  {
    if (asearch -> action < 2)
    {
      if (this_proj -> atoms[p][aid].pick[0] != new_status || new_status < 0)
      {
        // selected_status = ! this_proj -> atoms[p][aid].pick[get_to_be_selected (this_proj -> modelgl)];
        selected_status = ! this_proj -> atoms[p][aid].pick[0];
#ifdef GTK4
        select_unselect_this_atom (NULL, NULL, GINT_TO_POINTER(aid));
#else
        select_unselect_this_atom (NULL, GINT_TO_POINTER(aid));
#endif
        status = this_proj -> atoms[p][aid].pick[0];
      }
    }
    else
    {
      if (asearch -> todo[aid] != new_status || new_status < 0)
      {
        status = ! asearch -> todo[aid];
        adjust_search_param (asearch, this_proj, aid, this_proj -> atoms[p][aid].sp, oid, status);
      }
    }
  }
  status = (new_status < 0) ? status : new_status;
  if (asearch -> action == RANMOVE && oid != TOLAB)
  {
    k = (asearch -> todo[aid] == 1 || asearch -> todo[aid] == 3) ? 1 : 0;
    l = (asearch -> todo[aid] == 2 || asearch -> todo[aid] == 3) ? 1 : 0;
    // Check 'for all'
    gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC, k, -1);
    if (object) gtk_tree_store_set (asearch -> atom_model, & iter, TOPIC+1, l, -1);
  }
  else if (asearch -> action == INSERT)
  {
    gtk_list_store_set (asearch -> obj_model, & iter, oid, status, -1);
  }
  else
  {
    if (asearch -> action == DISPL && oid != TOLAB) motion_to_zero (asearch);
    gtk_tree_store_set (asearch -> atom_model, & iter, oid, status, -1);
  }
  this_proj -> modelgl -> selection_mode = sel;
  check_all_trees (this_proj);
}

/*!
  \fn G_MODULE_EXPORT void select_atom (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief toggle select / unselect object in the atom search tree model

  \param cell_renderer the cell renderer toggle button
  \param string_path the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_atom (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  int i, j, k;
  GtkTreeIter iter;
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id(dat -> a);
  atom_search * asearch = this_proj -> modelgl -> search_widg[dat -> c];
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  GtkTreeModel * model = (asearch -> action == INSERT) ? GTK_TREE_MODEL(asearch -> obj_model) : GTK_TREE_MODEL(asearch -> atom_model);
  gtk_tree_model_get_iter (model, & iter, path);
  gtk_tree_model_get (model, & iter, IDCOL, & i, -1);
  check_label = FALSE;
  if (this_proj -> modelgl -> atom_win)
  {
    this_proj -> modelgl -> atom_win -> rebuilt[(asearch -> action == RANMOVE) ? 1 : 0] = FALSE;
  }
  if (i > 0)
  {
    i --;
    // For atom i
    adjust_this_tree_leaf (asearch, this_proj, dat -> b, i, -1, iter);
  }
  else
  {
    // For spec i
    if (asearch -> todo_size >= 10000 && ! asearch -> passivating)
    {
      GtkTreeIter child;
      gtk_tree_model_get (GTK_TREE_MODEL(asearch -> atom_model), & iter, dat -> b, & j, -1);
      gtk_tree_store_set (asearch -> atom_model, & iter, dat -> b, ! j, -1);
      if (gtk_tree_model_iter_children (GTK_TREE_MODEL(asearch -> atom_model), & child, & iter))
      {
        gboolean dothis = TRUE;
        while (dothis)
        {
          gtk_tree_model_get (GTK_TREE_MODEL(asearch -> atom_model), & child, IDCOL, & k, -1);
          k --;
          adjust_this_tree_leaf (asearch, this_proj, dat -> b, k, ! j, child);
          dothis =  gtk_tree_model_iter_next (GTK_TREE_MODEL(asearch -> atom_model), & child);
        }
      }
    }
    else
    {
      adjust_this_tree_branch (asearch, this_proj, dat -> b, abs(i) - 1, iter);
    }
  }
  if (asearch -> action == DISPL) check_motion_interactors (this_proj, asearch);
  check_label =  TRUE;
  this_proj -> modelgl -> labelled = check_label_numbers (this_proj, 2);
  update (this_proj -> modelgl);
}

/*!
  \fn int get_selected_object_id (gboolean visible, int p, gchar * str, atom_search * asearch)

  \brief get the id of the object selected (in contextual menu, or in combo box)

  \param visible is the 'model edition' window visible (1/0)
  \param p the target project id
  \param str the string that describing the selection
  \param asearch the target atom search
*/
int get_selected_object_id (gboolean visible, int p, gchar * str, atom_search * asearch)
{
  int i, j;
  gchar * word, * name;
  for (i = 1; mol[i].type || mol[i].object; i++)
  {
    if (mol[i].object != NULL)
    {
      if (g_strcmp0 (mol[i].object, str) == 0)
      {
        if (i < 9)
        {
          return (int) mol[i].Z;
        }
        else if (i == 9)
        {
          return get_atom_id_from_periodic_table (asearch);
        }
        else if (i > 10 && i < 17)
        {
          return insert_this_project_from_lib (i-11, visible, get_project_by_id(p), asearch);
        }
        else if (i == 17)
        {
          return select_from_library (visible, get_project_by_id(p), asearch);
        }
      }
    }
  }
  for (i=0; i<nprojects; i++)
  {
    name = g_strdup_printf ("%s (%d)", get_project_by_id(i) -> name, i+1);
    for (j=0; j<3; j++)
    {
      word = g_strdup_printf ("%s in %s", action_atoms[j], name);
      if (g_strcmp0 (word, str) == 0)
      {
        g_free (word);
        g_free (name);
        get_project_by_id(p) -> modelgl -> other_status = j;
        return create_object_from_open_project (get_project_by_id(p), i);
      }
      else
      {
        g_free (word);
      }
    }
    g_free (name);
  }
  if (g_strcmp0 ("Copied data", str) == 0)
  {
    return FROM_DATA;
  }
  if (g_strcmp0 ("Empty position", str) == 0) return 120;
  return 0;
}

/*!
  \fn void adjust_data_model_to_replace (project * this_proj, atom_search * asearch, int sid, int vid)

  \brief adjust the data of the tree model to insert 'vid'

  \param this_proj the target project
  \param asearch the target atom search
  \param sid the chemical species
  \param vid id of the object to insert
*/
void adjust_data_model_to_replace (project * this_proj, atom_search * asearch, int sid, int vid)
{
  int i, j, k, l;
  int filter = get_asearch_filter (asearch);
  switch (filter)
  {
    case 0:
      for (i=0; i<this_proj -> natomes; i++)
      {
        if (this_proj -> atoms[0][i].sp == sid && append (asearch, this_proj, i, this_proj -> atoms[0][i].sp))
        {
          to_insert_in_project (vid, i, this_proj, asearch, TRUE);
        }
      }
      break;
    case 1:
      for (i=0; i<this_proj -> natomes; i++)
      {
        if (this_proj -> atoms[0][i].numv == sid)
        {
          j = this_proj -> atoms[0][i].sp;
          if (append (asearch, this_proj, i, j))
          {
            to_insert_in_project (vid, i, this_proj, asearch, TRUE);
          }
        }
      }
      break;
    case 2:
      for (i=0; i<this_proj -> natomes; i++)
      {
        j = this_proj -> atoms[0][i].sp;
        k = this_proj -> atoms[0][i].coord[1];
        for (l=0; l<j; l++) k += this_proj -> coord -> ntg[1][l];
        if (k == sid && append (asearch, this_proj, i, j))
        {
          to_insert_in_project (vid, i, this_proj, asearch, TRUE);
        }
      }
      break;
    default:
      for (i=0; i<this_proj -> natomes; i++)
      {
        j = this_proj -> atoms[0][i].sp;
        k = this_proj -> atoms[0][i].coord[filter-1];
        if (k == sid && append (asearch, this_proj, i, j))
        {
          to_insert_in_project (vid, i, this_proj, asearch, TRUE);
        }
      }
      break;
  }
}

/*!
  \fn G_MODULE_EXPORT void changed_action_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)

  \brief change combo box in atom search tree model

  \param combo the cell renderer combo box
  \param path_string the path in the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_action_renderer (GtkCellRendererCombo * combo, gchar * path_string, GtkTreeIter * iter, gpointer data)
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id(dat -> a);
  int h, i, j, k;
  atom_search * asearch = this_proj -> modelgl -> search_widg[dat -> c];
  GValue val = {0, };
  GObject * cmodel;
  g_object_get (combo, "model", & cmodel, NULL);
  gtk_tree_model_get_value ((GtkTreeModel *)cmodel, iter, 0, & val);
  GtkTreeIter child;
  gboolean dothis;
  GtkTreeModel * model = (asearch  -> action == INSERT) ? GTK_TREE_MODEL(asearch -> obj_model) : GTK_TREE_MODEL(asearch -> atom_model);
  if (gtk_tree_model_get_iter_from_string (model, iter, path_string))
  {
    gtk_tree_model_get (model, iter, IDCOL, & h, -1);
    gchar * str = g_strdup_printf ("%s", (char *)g_value_get_string (& val));
    i = get_selected_object_id (TRUE, this_proj -> id, str, asearch);
    g_free (str);
    int object = get_asearch_object (asearch);
    int filter = get_asearch_filter (asearch);
    k = (asearch -> pointer[0].c == 3) ? 0 : (asearch -> pointer[0].c == 5) ? 1 : 3;
    if (h > 0 || (object && ! asearch -> passivating && filter > 2) || asearch -> passivating || asearch -> mode)
    {
      // Single atom or object
      if (i)
      {
        j = (asearch -> mode) ? h - 2 : abs(h) - 1;
        to_insert_in_project (i, j, this_proj, asearch, TRUE);
        str = (asearch -> mode) ? g_strdup_printf ("%s", get_atomic_object_by_origin(this_proj -> modelgl -> atom_win -> to_be_inserted[k], j, 0) -> name) : g_strdup_printf ("For all: %s", get_atomic_object_by_origin(this_proj -> modelgl -> atom_win -> to_be_inserted[k], j, 0) -> name);
        gtk_tree_store_set (asearch -> atom_model, iter, TOPIC+1, str, -1);
        g_free (str);
      }
      else
      {
        gtk_tree_store_set (asearch -> atom_model, iter, TOPIC+1, (h > 0) ? "Select ..." : "Select for all ...", -1);
      }
    }
    else
    {
      // The entire species
      if (i)
      {
        if (! asearch -> passivating && this_proj -> natomes >= 10000)
        {
          if (gtk_tree_model_iter_children (GTK_TREE_MODEL(asearch -> atom_model), & child, iter))
          {
            dothis = TRUE;
            while (dothis)
            {
              gtk_tree_model_get (GTK_TREE_MODEL(asearch -> atom_model), & child, IDCOL, & j, -1);
              j --;
              to_insert_in_project (i, j, this_proj, asearch, TRUE);
              gtk_tree_store_set (asearch -> atom_model, & child, TOPIC+1, get_atomic_object_by_origin(this_proj -> modelgl -> atom_win -> to_be_inserted[k], j, 0) -> name, -1);
              dothis =  gtk_tree_model_iter_next (GTK_TREE_MODEL(asearch -> atom_model), & child);
            }
          }
          str = g_strdup_printf ("For all: %s", get_atomic_object_by_origin(this_proj -> modelgl -> atom_win -> to_be_inserted[k], j, 0) -> name);
          gtk_tree_store_set (asearch -> atom_model, iter, TOPIC+1, str, -1);
          g_free (str);
        }
        else
        {
          adjust_data_model_to_replace (this_proj, asearch, abs(h)-1, i);
        }
      }
      else
      {
        if (! asearch -> passivating && this_proj -> natomes >= 10000)
        {
          gtk_tree_store_set (asearch -> atom_model, iter, 5, "Select for all ...", -1);
          if (gtk_tree_model_iter_children (GTK_TREE_MODEL(asearch -> atom_model), & child, iter))
          {
            dothis = TRUE;
            while (dothis)
            {
              gtk_tree_store_set (asearch -> atom_model, & child, TOPIC+1, "Select ...", -1);
              dothis =  gtk_tree_model_iter_next (GTK_TREE_MODEL(asearch -> atom_model), & child);
            }
          }
        }
        else
        {
          gtk_tree_store_set (asearch -> atom_model, iter, 5, "Select ...", -1);
        }
      }
    }
    if (update_this_search(asearch)) update_search_tree (asearch);
  }
}

/*!
  \fn GtkTreeModel * replace_combo_tree (gboolean insert, int proj)

  \brief replace combo box in the tree view

  \param insert add 'Select ...' combo box item
  \param proj target crystal builder project id
*/
GtkTreeModel * replace_combo_tree (gboolean insert, int proj)
{
  GtkTreeIter iter, iter2, iter3;
  GtkTreeStore *store;
  int i, j;
  gchar * name, * word;

  store = gtk_tree_store_new (1, G_TYPE_STRING);

  if (insert)
  {
    gtk_tree_store_append (store, & iter, NULL);
    gtk_tree_store_set (store, & iter, 0, "Select ...", -1);
  }
  for (i=0; mol[i].type || mol[i].object; i++)
  {
    if (mol[i].type)
    {
      gtk_tree_store_append (store, & iter, NULL);
      gtk_tree_store_set (store, & iter, 0, mol[i].type, -1);
    }
    else if (mol[i].object)
    {
      gtk_tree_store_append (store, & iter2, & iter);
      gtk_tree_store_set (store, & iter2, 0, mol[i].object, -1);
    }
  }
  gboolean doit = FALSE;
  for (i=0; i<nprojects; i++)
  {
    if (get_project_by_id(i) -> steps == 1 && get_project_by_id(i) -> natomes)
    {
      doit = TRUE;
      break;
    }
  }
  if (doit)
  {
    gtk_tree_store_append (store, & iter, NULL);
    gtk_tree_store_set (store, & iter, 0, "Import from project", -1);
    for (i=0; i<nprojects; i++)
    {
      if (get_project_by_id(i) -> steps == 1 && get_project_by_id(i) -> natomes)
      {
        gtk_tree_store_append (store, & iter2, & iter);
        name = g_strdup_printf ("%s (%d)", get_project_by_id(i) -> name, i+1);
        gtk_tree_store_set (store, & iter2, 0, name, -1);
        for (j=0; j<3; j++)
        {
          gtk_tree_store_append (store, & iter3, & iter2);
          word = g_strdup_printf ("%s in %s", action_atoms[j], name);
          gtk_tree_store_set (store, & iter3, 0, word, -1);
          g_free (word);
        }
        g_free (name);
      }
    }
  }
  if (copied_object)
  {
    gtk_tree_store_append (store, &iter, NULL);
    gtk_tree_store_set (store, & iter, 0, "Copied data", -1);
  }
  if (get_project_by_id(proj) -> modelgl)
  {
    if (get_project_by_id(proj) -> modelgl -> builder_win)
    {
      gtk_tree_store_append (store, &iter, NULL);
      gtk_tree_store_set (store, & iter, 0, "Empty position", -1);
    }
  }
  return GTK_TREE_MODEL (store);
}

/*!
  \fn void search_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief show / hide cell renderer, if visible then add or not pango markup

  \param col the tree view column
  \param renderer the cell renderer
  \param mod the tree model
  \param iter the tree iter
  \param data the associated data pointer
*/
void search_set_visible (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  i = GPOINTER_TO_INT(data);
  i ++;
  gtk_tree_model_get (mod, iter, IDCOL, & j, -1);
  gboolean vis = (j < 0 && i == 2) ? FALSE : TRUE;
  gtk_cell_renderer_set_visible (renderer, vis);
  if (vis && (i < TOLAB || i > TOPIC))
  {
    gchar * str = NULL;
    gtk_tree_model_get (mod, iter, i, & str, -1);
    g_object_set (renderer, "markup", str, NULL, NULL);
    g_free (str);
  }
}

int atom_to_edit;
atom_search * csearch;

/*!
  \fn G_MODULE_EXPORT void set_occupancy (GtkEntry * res, gpointer data)

  \brief set occupancy entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_occupancy (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  atomic_object * object = (atomic_object *)data;
  if (v > 0.0 && v<= 1.0)
  {
    object -> occ = v;
  }
  update_entry_double (res, object -> occ);
}

/*!
  \fn G_MODULE_EXPORT void set_i_coord (GtkEntry * res, gpointer data)

  \brief set insertion coordinate(s) for the object to be inserted entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_i_coord (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  int ax = GPOINTER_TO_INT (data);
  project * this_proj = get_project_by_id (csearch -> proj);
  int oid = (csearch -> pointer[0].c == 5) ? 1 : 2;
  atomic_object * object = get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[oid], 0, atom_to_edit+1);
  object -> baryc[ax] = v;
  update_entry_double (res, object -> baryc[ax]);
}

/*!
  \fn G_MODULE_EXPORT void set_max_msd (GtkEntry * res, gpointer data)

  \brief set MSD max entry callback

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_max_msd (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  int ax = GPOINTER_TO_INT (data);
  project * this_proj = get_project_by_id (csearch -> proj);
  if (! (v < 0.0))
  {
    if (ax > 0)
    {
      if (this_proj -> modelgl -> atom_win -> msd[ax-1] != v) csearch -> pick[this_proj -> atoms[0][ax-1].sp] = 0;
      this_proj -> modelgl -> atom_win -> msd[ax-1] = v;
    }
    else
    {
      ax = -ax-1;
      this_proj -> modelgl -> atom_win -> msd_all[ax] = v;
      if (! csearch -> passivating)
      {
        int object = get_asearch_object (csearch);
        int filter = get_asearch_filter (csearch);
        int i, j, k, l;
        switch (filter)
        {
          case 0:
            for (i=0; i<this_proj -> natomes; i++)
            {
              if (this_proj -> atoms[0][i].sp == ax) this_proj -> modelgl -> atom_win -> msd[i] = v;
            }
            break;
          case 1:
            for (i=0; i<this_proj -> natomes; i++)
            {
              j = this_proj -> atoms[0][i].sp;
              if (this_proj -> atoms[0][i].numv == ax) this_proj -> modelgl -> atom_win -> msd[i] = v;
            }
            break;
          case 2:
            for (i=0; i<this_proj -> natomes; i++)
            {
              j = this_proj -> atoms[0][i].sp;
              k = this_proj -> atoms[0][i].coord[filter-1];
              for (l=0; l<j; l++) k += this_proj -> coord -> ntg[1][l];
              if (k == ax) this_proj -> modelgl -> atom_win -> msd[i] = v;
            }
            break;
          default:
            if (! object)
            {
              for (i=0; i<this_proj -> natomes; i++)
              {
                j = this_proj -> atoms[0][i].coord[filter-1];
                if (j == ax) this_proj -> modelgl -> atom_win -> msd[i] = v;
              }
            }
            break;
        }
      }
    }
    update_entry_double (res, v);
  }
  else
  {
    //show_error ("MSD<sub>max</sub> must be > 0.0 !", 1, this_proj -> modelgl -> atom_win -> win);
    if (ax > 0)
    {
      v = this_proj -> modelgl -> atom_win -> msd[ax-1];
    }
    else
    {
      v = this_proj -> modelgl -> atom_win -> msd_all[-ax-1];
    }
    update_entry_double (res, v);
  }
}

int max_random;

/*!
  \fn G_MODULE_EXPORT void set_max_action (GtkEntry * res, gpointer data)

  \brief set how many time(s) to re-do the action

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_max_action (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  int v = (int)string_to_double ((gpointer)m);
  int ax = GPOINTER_TO_INT (data);
  if (v > -1 && v <= max_random)
  {
    csearch -> todo[-ax-1] = v;
  }
  update_entry_int (res, csearch -> todo[-ax-1]);
}

/*!
  \fn void get_coord_iter_and_edit (gchar * path_string, gpointer data, GtkWidget * widg)

  \brief find iter in the tree model using path string, then edit the data

  \param path_string the path in the tree model
  \param data the associated data pointer
  \param widg the GtkWidget sending the signal
*/
void get_coord_iter_and_edit (gchar * path_string, gpointer data, GtkWidget * widg)
{
  tint * cid = (tint *)data;
  project * this_proj = get_project_by_id (cid -> a);
  csearch = this_proj -> modelgl -> search_widg[cid -> c];
  GtkTreeIter iter;
  GtkTreeModel * model = (csearch -> action == INSERT) ? GTK_TREE_MODEL(csearch -> obj_model) : GTK_TREE_MODEL(csearch -> atom_model);
  if (gtk_tree_model_get_iter (model, & iter, gtk_tree_path_new_from_string (path_string)))
  {
    int h, i, j, k, l;
    gtk_tree_model_get (model, & iter, IDCOL, & h, -1);
    if (h > 0 || csearch -> action == REPLACE || csearch -> action == REMOVE || csearch -> action == RANMOVE)
    {

      atom_to_edit = h-1;
      GtkWidget * win = gtk_dialog_new ();
      if (this_proj -> modelgl -> builder_win)
      {
        gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW(this_proj -> modelgl -> builder_win -> win));
      }
      else
      {
        gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW(this_proj -> modelgl -> atom_win -> win));
      }
      gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
      gchar * str;
      atomic_object * iobj;
      gchar * obj[5]={"atom(s)", "total coordination(s)", "partial coordination(s)", "in fragment", "in molecule"};
      gchar * act[2]={"Replace", "Remove"};
      int object = get_asearch_object (csearch);
      int filter = get_asearch_filter (csearch);
      switch (csearch -> action)
      {
        case REPLACE:
          str = g_strdup_printf ("Replace %s randomly", obj[(object) ? filter : 0]);
          break;
        case REMOVE:
          str = g_strdup_printf ("Remove %s randomly", obj[(object) ? filter : 0]);
          break;
        case RANMOVE:
          str = g_strdup_printf ("Maximum Mean Square Displacement");
          break;
        default:
          i = (csearch -> pointer[0].c == 5) ? 1 : 2;
          iobj = get_atomic_object_by_origin (this_proj -> modelgl -> atom_win -> to_be_inserted[i], 0, atom_to_edit+1);
          if (cid -> b - TOLAB)
          {
            str = g_strdup_printf ("Site occupancy for %s", prepare_for_title(iobj -> name));
          }
          else
          {
            str = g_strdup_printf ("Insert %s at", prepare_for_title(iobj -> name));
          }
          break;
      }
      gtk_window_set_title (GTK_WINDOW(win), str);
      g_free (str);
      gtk_window_set_modal (GTK_WINDOW (win), TRUE);
      GtkWidget * vbox = dialog_get_content_area (win);
      GtkWidget * hbox;
      GtkWidget * entry;
      gchar * axis[3]={"x", "y", "z"};
      gchar * nran = NULL;
      switch (csearch -> action)
      {
        case INSERT:
          if (cid -> b - TOLAB && csearch -> pointer[0].c == 7)
          {
            hbox = create_hbox (5);
            str = g_strdup_printf ("Occupancy= ");
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
            g_free (str);
            entry = create_entry (G_CALLBACK(set_occupancy), 100, 15, FALSE, iobj);
            update_entry_double (GTK_ENTRY(entry), iobj -> occ);
            add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
            add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
          }
          else
          {
            for (i=0; i<3; i++)
            {
              hbox = create_hbox (5);
              str = g_strdup_printf ("%s =", axis[i]);
              add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 0);
              g_free (str);
              entry = create_entry (G_CALLBACK(set_i_coord), 100, 15, FALSE, GINT_TO_POINTER(i));
              update_entry_double (GTK_ENTRY(entry), iobj -> baryc[i]);
              add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
              add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
            }
          }
          break;
        case RANMOVE:
          hbox = create_hbox (5);
          entry = create_entry (G_CALLBACK(set_max_msd), 100, 15, FALSE, GINT_TO_POINTER(h));
          if (h > 0)
          {
            j = this_proj -> atoms[0][atom_to_edit].sp;
            if (object)
            {
              str = g_strdup_printf ("MSD<sub>max</sub> for %s<sub>%d</sub> + neighbors = ", this_proj -> chemistry -> label[j], atom_to_edit+1);
            }
            else
            {
              str = g_strdup_printf ("MSD<sub>max</sub> for %s<sub>%d</sub> = ", this_proj -> chemistry -> label[j], atom_to_edit+1);
            }
            update_entry_double (GTK_ENTRY(entry), this_proj -> modelgl -> atom_win -> msd[atom_to_edit]);
          }
          else
          {
            switch (filter)
            {
              case 0:
                str = g_strdup_printf ("MSD<sub>max</sub> for all %s atom(s) = ", this_proj -> chemistry -> label[-h-1]);
                break;
              case 1:
                if (-h-1 == 0)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for all isolated atom(s) = ");
                }
                else if (object)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for all %d-fold atom(s) + neighbors = ", -h-1);
                }
                else
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for all %d-fold atom(s) = ", -h-1);
                }
                break;
              case 2:
                i = 0;
                for (j=0; j<this_proj -> nspec; j++)
                {
                  i += this_proj -> coord -> ntg[1][j];
                  if (i > -h-1) break;
                }
                k = 0;
                for (l=0; l<j; l++) k += this_proj -> coord -> ntg[1][l];
                i = (-h-1) - k;
                nran = env_name(this_proj, i, j, 1, NULL);
                if (object)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for all %s + neighbors = ", nran);
                }
                else
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for all %s = ", nran);
                }
                break;
              case 3:
                if (object)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for fragment N°%d = ", -h);
                }
                else
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for the atom(s) in fragment N°%d = ", -h);
                }
                break;
              case 4:
                if (object)
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for molecule N°%d = ", -h);
                }
                else
                {
                  str = g_strdup_printf ("MSD<sub>max</sub> for the atom(s) in molecule N°%d = ", -h);
                }
                break;
            }
            update_entry_double (GTK_ENTRY(entry), this_proj -> modelgl -> atom_win -> msd_all[-h-1]);
          }
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 15);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("[&#xC5;<sup>2</sup>]", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
          break;
        default:
          max_random = 0;
          switch (filter)
          {
            case 0:
              if (csearch -> status == 2)
              {
                max_random = this_proj -> chemistry -> nsps[-h-1];
                nran = g_strdup_printf ("%s", this_proj -> chemistry -> label[-h-1]);
              }
              else
              {
                max_random = 0;
                for (i=0; i<this_proj -> natomes; i++)
                {
                  if (this_proj -> atoms[0][i].sp == -h-1 && this_proj -> atoms[0][i].pick[0] == csearch -> status) max_random ++;
                }
              }
              break;
            case 1:
              for (i=0; i<this_proj -> natomes; i++)
              {
                if (this_proj -> atoms[0][i].numv == -h-1 && (this_proj -> atoms[0][i].pick[0] == csearch -> status || csearch -> status == 2)) max_random ++;
              }
              if (-h-1 > 0)
              {
                nran = g_strdup_printf ("%d-fold", -h-1);
              }
              else
              {
                nran = g_strdup_printf ("isolated");
              }
              break;
            case 2:
              i = 0;
              for (j=0; j<this_proj -> nspec; j++)
              {
                i += this_proj -> coord -> ntg[1][j];
                if (i > -h-1) break;
              }
              k = 0;
              for (l=0; l<j; l++) k += this_proj -> coord -> ntg[1][l];
              i = (-h-1) - k;
              for (k=0; k<this_proj -> natomes; k++)
              {
                if (this_proj -> atoms[0][k].sp == j && this_proj -> atoms[0][k].coord[1] == i)
                {
                  if (this_proj -> atoms[0][k].pick[0] == csearch -> status || csearch -> status == 2) max_random ++;
                }
              }
              if (csearch -> action != RANMOVE) nran = env_name(this_proj, i, j, 1, NULL);
              break;
            default:
              if (object)
              {
                i = (-h-1);
                if (filter == 3)
                {
                  max_random = this_proj -> coord -> totcoord[2];
                }
                else if (filter == 4)
                {
                  max_random = this_proj -> modelfc -> mols[0][i].multiplicity;
                }
              }
              else
              {
                i = (-h-1);
                for (k=0; k<this_proj -> natomes; k++)
                {
                  if (this_proj -> atoms[0][k].coord[filter-1] == i)
                  {
                    if (this_proj -> atoms[0][k].pick[0] == csearch -> status || csearch -> status == 2) max_random ++;
                  }
                }
                nran = g_strdup_printf ("atom(s)");
              }
              break;
          }

          hbox = create_hbox (5);
          entry = create_entry (G_CALLBACK(set_max_action), 100, 15, FALSE, GINT_TO_POINTER(h));
          if (object)
          {
            if (filter == 3)
            {
              str = g_strdup_printf ("%s randomly <i>n</i> <b>fragment(s)</b> in all fragments, <i>n</i>=  ", act[csearch -> action - 3]);
            }
            else if (filter == 4)
            {
               str = g_strdup_printf ("%s randomly <i>n</i> <b>fragment(s)</b> in molecule N°%d, <i>n</i>=  ", act[csearch -> action - 3], -h);
            }
            else
            {
              str = g_strdup_printf ("%s randomly <i>n</i> <b>%s</b> coordinations, <i>n</i>=  ", act[csearch -> action - 3], nran);
            }
          }
          else
          {
            if (filter < 3)
            {
              str = g_strdup_printf ("%s randomly <i>n</i> <b>%s</b> %s, <i>n</i>=  ", act[csearch -> action - 3], nran, obj[0]);
            }
            else
            {
              str = g_strdup_printf ("%s randomly <i>n</i> <b>%s</b> %s N°%d, <i>n</i>=  ", act[csearch -> action - 3], nran, obj[filter], -h);
            }
          }
          if (nran) g_free (nran);
          update_entry_int (GTK_ENTRY(entry), csearch -> todo[-h-1]);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 15);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
          str = g_strdup_printf (" with <i>n</i><sub>max</sub> = <b>%d</b>", max_random);
          add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 50, -1, 0.0, 0.5), FALSE, FALSE, 15);
          g_free (str);
          add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
          break;
      }
      run_this_gtk_dialog (win, G_CALLBACK(run_destroy_dialog), NULL);
      if (csearch -> action != RANMOVE)
      {
        update_search_tree (csearch);
      }
      else
      {
        if (h > 0)
        {
          if (this_proj -> modelgl -> atom_win -> msd[atom_to_edit] > 0.0)
          {
            str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd[atom_to_edit]);
          }
          else
          {
            str = g_strdup_printf ("Set MSD<sub>max</sub> for ...");
          }
          gtk_tree_store_set (csearch -> atom_model, & iter, TOPIC+2, str, -1);
          g_free (str);
        }
        else
        {
          if (this_proj -> modelgl -> atom_win -> msd_all[-h-1] > 0.0)
          {
            if (object && filter > 2)
            {
              str = g_strdup_printf ("MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd_all[-h-1]);
            }
            else
            {
              str = g_strdup_printf ("For all: MSD<sub>max</sub>= %f", this_proj -> modelgl -> atom_win -> msd_all[-h-1]);
            }
            gtk_tree_store_set (csearch -> atom_model, & iter, TOPIC+2, str, -1);
            g_free (str);
          }
          else
          {
            if (object && filter > 2)
            {
              gtk_tree_store_set (csearch -> atom_model, &iter, TOPIC+2, "Set MSD<sub>max</sub> for ...", -1);
            }
            else
            {
              gtk_tree_store_set (csearch -> atom_model, & iter, TOPIC+2, "Set MSD<sub>max</sub> for all ...", -1);
            }
          }
        }
        check_all_trees (this_proj);
      }
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void to_edit_coords (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)

  \brief to edit data in the atom search tree model

  \param cell the GtkCellRenderer
  \param editable the editable
  \param path_string the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void to_edit_coords (GtkCellRenderer * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)
{
  destroy_this_widget (GTK_WIDGET(editable));
  get_coord_iter_and_edit (path_string, data, NULL);
}

/*!
  \fn G_MODULE_EXPORT void markup_action_renderer (GtkCellRendererCombo * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)

  \brief add pango markup to a combo box inserted in the cell of a tree view

  \param cell the GtkCellRendererCombo
  \param editable the editable
  \param path_string the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void markup_action_renderer (GtkCellRendererCombo * cell, GtkCellEditable * editable, gchar * path_string, gpointer data)
{
  combo_set_markup ((GtkWidget *)editable);
}

/*!
  \fn G_MODULE_EXPORT void set_max_msd_for_all (GtkEntry * res, gpointer data)

  \brief set the maximum MSD for all atom(s) / object(s) of the search

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_max_msd_for_all (GtkEntry * res, gpointer data)
{
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  project * this_proj = get_project_by_id (csearch -> proj);
  int i, j;
  if (v <= 0.0)
  {
    // show_error ("MSD<sub>max</sub> must be > 0.0 !", 1, this_proj -> modelgl -> atom_win -> win);
    v = this_proj -> modelgl -> atom_win -> msd[0];
  }
  i = csearch -> todo_size;
  for (j=0; j<i; j++)
  {
    this_proj -> modelgl -> atom_win -> msd[j] = v;
  }
  i = get_asearch_num_objects (csearch);
  for (j=0; j<i; j++)
  {
    this_proj -> modelgl -> atom_win -> msd_all[j] = v;
  }
  update_entry_double (res, v);
}

/*!
  \fn gboolean update_search_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)

  \brief update tree model for each iter

  \param model the tree model
  \param path the path in the tree model
  \param iter the tree it
  \param data the associated data pointer
*/
gboolean update_search_model (GtkTreeModel * model, GtkTreePath * path, GtkTreeIter * iter, gpointer data)
{
  int i;
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id(dat -> a);
  atom_search * asearch = this_proj -> modelgl -> search_widg[dat -> c];
  if (gtk_tree_model_get_iter (model, iter, path))
  {
    gtk_tree_model_get (model, iter, IDCOL, & i, -1);
    gtk_tree_store_set (GTK_TREE_STORE(model), iter, dat -> b, asearch -> int_b, -1);
    if (i > 0)
    {
      i --;
      adjust_this_tree_leaf (asearch, this_proj, dat -> b, i, asearch -> int_b, * iter);
    }
    else if (asearch -> object)
    {
      adjust_this_tree_branch (asearch, this_proj, dat -> b, abs(i) - 1, * iter);
    }
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn G_MODULE_EXPORT void select_all_atoms (GtkTreeViewColumn * col, gpointer data)

  \brief select all element(s) in the tree view column

  \param col the target tree view column
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_all_atoms (GtkTreeViewColumn * col, gpointer data)
{
  tint * dat = (tint *)data;
  project * this_proj = get_project_by_id(dat -> a);
  csearch = this_proj -> modelgl -> search_widg[dat -> c];
  int i, j, k;
  opengl_project_changed (dat -> a);
  if (dat -> b == TOPIC+2)
  {
    GtkWidget * win = gtk_dialog_new ();
    gtk_window_set_transient_for (GTK_WINDOW (win), GTK_WINDOW(this_proj -> modelgl -> atom_win -> win));
    gtk_window_set_resizable (GTK_WINDOW (win), FALSE);
    gchar * str = g_strdup_printf ("Maximum Mean Square Displacement");
    gtk_window_set_title (GTK_WINDOW(win), str);
    g_free (str);
    gtk_window_set_modal (GTK_WINDOW (win), TRUE);
    GtkWidget * vbox = dialog_get_content_area (win);
    GtkWidget * hbox = create_hbox (5);
    GtkWidget * entry = create_entry (G_CALLBACK(set_max_msd_for_all), 100, 15, FALSE, NULL);
    update_entry_double (GTK_ENTRY(entry), 0.0);
    str = g_strdup_printf ("MSD<sub>max</sub> for all object(s) = ");
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 100, -1, 0.0, 0.5), FALSE, FALSE, 15);
    g_free (str);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, entry, FALSE, FALSE, 0);
    str = g_strdup_printf ("[&#xC5;<sup>2</sup>]");
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label(str, 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
    run_this_gtk_dialog (win, G_CALLBACK(run_destroy_dialog), NULL);
    update_search_tree (csearch);
  }
  else
  {
    if (dat -> b == TOLAB)
    {
      if (csearch -> action == INSERT)
      {
        k = (csearch -> pointer[0].c == 5) ? 1 : 2;
        if (this_proj -> modelgl -> atom_win -> to_be_inserted[k])
        {
          j = ! csearch -> todo[0];
          for (i=0; i<csearch -> in_selection; i++) csearch -> todo[i] = j;
        }
      }
      else
      {
        k = this_proj -> modelgl -> anim -> last -> img -> step;
        if (csearch -> action < 2)
        {
          is_selected = -1;
          i = 0;
        }
        else
        {
          if (csearch -> status < 2)
          {
            is_selected = csearch -> status;
            for (i=0; i<this_proj -> natomes; i++)
            {
              if (this_proj -> atoms[k][i].pick[0] == is_selected) break;
            }
          }
          else
          {
            is_selected = -1;
            i = 0;
          }
        }
        j = ! this_proj -> atoms[k][i].label[0];
        selected_aspec = -1;
        column_label = TRUE;
#ifdef GTK4
        label_unlabel_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][j]);
#else
        label_unlabel_atoms (NULL, & opengl_project -> modelgl -> colorp[0][j]);
#endif
        column_label = FALSE;
      }
    }
    else
    {
      k = this_proj -> modelgl -> anim -> last -> img -> step;
      if (csearch -> action < 2)
      {
        j = ! this_proj -> atoms[k][0].pick[0];
        selected_aspec = is_selected = -1;
#ifdef GTK4
        select_unselect_atoms (NULL, NULL, & opengl_project -> modelgl -> colorp[0][j]);
#else
        select_unselect_atoms (NULL, & opengl_project -> modelgl -> colorp[0][j]);
#endif
      }
      else
      {
        if (csearch -> action != RANMOVE)
        {
          j = (csearch -> set_for_all < 0) ? 0 : 1;
        }
        else
        {
          // The value depends on both translation (1), rotation (2), or both (3)
          switch (abs(csearch -> set_for_all))
          {
            case 0:
              if (dat -> b == TOPIC)
              {
                j = 1;
              }
              else
              {
                j = 2;
              }
              break;
            case 1:
              if (dat -> b == TOPIC)
              {
                j = 0;
              }
              else
              {
                j = 3;
              }
              break;
            case 2:
              if (dat -> b == TOPIC)
              {
                j = 3;
              }
              else
              {
                j = 0;
              }
              break;
            case 3:
              if (dat -> b == TOPIC)
              {
                j = 2;
              }
              else
              {
                j = 1;
              }
              break;
          }
        }
      }
    }
    if (csearch -> action > 1 && dat -> c != 7)
    {
      clean_picked_and_labelled (csearch, FALSE);
      if (dat -> b != TOLAB) clean_todo (csearch);
    }
    csearch -> set_for_all = (dat -> b == TOLAB) ? 0 : j;
    update_search_tree (csearch);
  }
  /*else
  {
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(csearch -> atom_model), & iter))
    {
      gtk_tree_model_get (GTK_TREE_MODEL(csearch -> atom_model), & iter, dat -> b, & j, -1);
      g_debug ("j= %d,", j);
      g_debug ("a= %d, b= %d, c= %d, aa= %d, bb= %d, cc= %d", dat -> a, dat -> b, dat -> c,
                                                              csearch -> pointer[dat -> b-TOLAB].a, csearch -> pointer[dat -> b-TOLAB].b, csearch -> pointer[dat -> b-TOLAB].c);
      csearch -> int_b = ! j;
      gtk_tree_model_foreach (GTK_TREE_MODEL(csearch -> atom_model), update_search_model, & csearch -> pointer[dat -> b-TOLAB]);
    }
  }*/
}

/*!
  \fn G_MODULE_EXPORT void move_up_down (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data)

  \brief Move row up or down in the tree model using the mouse

  \param tree_model the target tree model
  \param path the path in the tree model
  \param data the associated data pointer
*/
G_MODULE_EXPORT void move_up_down (GtkTreeModel * tree_model, GtkTreePath * path, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  project * this_proj = get_project_by_id(asearch -> proj);
  atomic_object * obja, * objb, * objc;
  GtkTreeIter iter;
  gboolean valid;
  int i, j;
  int * old_todo = duplicate_int (asearch -> in_selection, asearch -> todo);
  obja = duplicate_atomic_object (this_proj -> modelgl -> atom_win -> to_be_inserted[2]);
  obja -> next = this_proj -> modelgl -> atom_win -> to_be_inserted[2] -> next;
  g_free (this_proj -> modelgl -> atom_win -> to_be_inserted[2]);
  objb = NULL;
  valid = gtk_tree_model_get_iter_first (tree_model, & iter);
  j = 0;
  while (valid)
  {
    gtk_tree_model_get (tree_model, & iter, 0, & i, -1);
    i --;
    objc = obja;
    while (objc -> id != i) objc = objc -> next;
    if (objc)
    {
      if (! objb)
      {
        this_proj -> modelgl -> atom_win -> to_be_inserted[2] = duplicate_atomic_object (objc);
        objb = this_proj -> modelgl -> atom_win -> to_be_inserted[2];
      }
      else
      {
        objb -> next = duplicate_atomic_object (objc);
        objb -> next -> prev = objb;
        objb = objb -> next;
      }
      objb -> id = j;
      asearch -> todo[j] = old_todo[i];
      j ++;
    }
    valid = gtk_tree_model_iter_next (tree_model, & iter);
  }
  g_free (old_todo);
  i = 0;
  valid = gtk_tree_model_get_iter_first (tree_model, & iter);
  while (valid)
  {
    gtk_list_store_set (GTK_LIST_STORE(tree_model), & iter, IDCOL, i+1, 1, g_strdup_printf ("%d", i+1), -1);
    i ++;
    valid = gtk_tree_model_iter_next (tree_model, & iter);
  }
}

/*!
  \fn GtkWidget * create_atoms_tree (atom_search * asearch, project * this_proj, int nats)

  \brief create atom search tree view

  \param asearch the target atom search
  \param this_proj the target project
  \param nats the total number of atom(s)
*/
GtkWidget * create_atoms_tree (atom_search * asearch, project * this_proj, int nats)
{
  int i, j, k, l;
  GtkTreeViewColumn * atom_col[5];
  GtkCellRenderer * atom_cell[5];
  gchar * ctitle[6][5]={{"Object", "Name", "Label", "Pick", " "},
                        {"Object", "Name", "Label", "Move", " "},
                        {"Object", "Name", "Label", "Replace", "By"},
                        {"Object", "Name", "Label", "Remove", " "},
                        {"Object", "Name", "Insert", "Position", "Occupancy"},
                        {"Object", "Name", "Label", "Translate", "Max. MSD"}};
  gchar * ctype[3][5]={{"text", "text", "active", "active", "text"},
                       {"text", "text", "active", "text", "text"},
                       {"text", "text", "active", "active", "text"}};
  j = (asearch -> action == REPLACE || asearch -> action == RANMOVE) ? 1 : 0;
  k = (asearch -> action == RANMOVE) ? 2 : (asearch -> action == INSERT) ? 1 : 0;
  if (asearch -> action == INSERT && asearch -> pointer[0].c == 7) j ++;
  gboolean toggle;
  GType coltype[3][8] = {{G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT},
                         {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT},
                         {G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_STRING, G_TYPE_INT}};
  if (asearch -> action == INSERT)
  {
    asearch -> obj_model = gtk_list_store_newv (8, coltype[k]);
  }
  else
  {
    asearch -> atom_model = gtk_tree_store_newv (8, coltype[k]);
  }
  if (! (nats  > 10000) || asearch -> action == INSERT)
  {
    fill_atom_model (asearch, this_proj);
  }
  if (asearch -> action == INSERT)
  {
    asearch -> atom_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(asearch -> obj_model));
  }
  else
  {
    asearch -> atom_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(asearch -> atom_model));
  }

  for (i=0; i<4+j; i++)
  {
    toggle = FALSE;
    if (i < 2 || (asearch -> action == INSERT && i > 2) ||  (asearch -> action == RANMOVE && i == 4))
    {
      atom_cell[i] = gtk_cell_renderer_text_new ();
      if ((asearch -> action == INSERT && i > 2) ||  (asearch -> action == RANMOVE && i == 4))
      {
        g_object_set (atom_cell[i], "editable", TRUE, NULL);
        g_signal_connect (G_OBJECT(atom_cell[i]), "editing-started", G_CALLBACK(to_edit_coords), & asearch -> pointer[i-3]);
        if (asearch -> action == RANMOVE) toggle = TRUE;
      }
    }
    else if (i < 4)
    {
      atom_cell[i] = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(atom_cell[i]), "toggled", G_CALLBACK(select_atom), & asearch -> pointer[i-2]);
      toggle = TRUE;
    }
    else
    {
      atom_cell[i] = gtk_cell_renderer_combo_new ();
      GtkTreeModel * model = replace_combo_tree (FALSE, this_proj -> id);
      g_object_set (atom_cell[i], "model", model, "text-column", 0, "has-entry", FALSE, "editable", TRUE, NULL);
      g_object_unref (model);
      g_signal_connect (G_OBJECT(atom_cell[i]), "editing-started", G_CALLBACK(markup_action_renderer), & asearch -> pointer[i-4]);
      g_signal_connect (G_OBJECT(atom_cell[i]), "changed", G_CALLBACK(changed_action_renderer), & asearch -> pointer[i-4]);
    }
    gtk_cell_renderer_set_fixed_size (atom_cell[i], -1, 25);
    l = (asearch -> action == RANMOVE && i == 4) ? i+1 : i;
    atom_col[i] = gtk_tree_view_column_new_with_attributes (ctitle[(asearch -> action < 2) ? 0 : asearch -> action - 1][i], atom_cell[i], ctype[k][i], l+1, NULL);
    if (toggle && ! asearch -> passivating)
    {
      gtk_tree_view_column_set_clickable (atom_col[i], TRUE);
      g_signal_connect (G_OBJECT(atom_col[i]), "clicked", G_CALLBACK(select_all_atoms), & asearch -> pointer[l-2]);
    }
    gtk_tree_view_append_column(GTK_TREE_VIEW(asearch -> atom_tree), atom_col[i]);
    gtk_tree_view_column_set_alignment (atom_col[i], 0.5);
    gtk_tree_view_column_set_cell_data_func (atom_col[i], atom_cell[i], search_set_visible, GINT_TO_POINTER(l), NULL);
  }
  if (asearch -> pointer[0].c == 7)
  {
    asearch -> filter =  g_signal_connect_data (G_OBJECT(GTK_TREE_MODEL(asearch -> obj_model)), "row-deleted", G_CALLBACK(move_up_down), asearch, NULL, (GConnectFlags) 0);
    gtk_tree_view_set_reorderable (GTK_TREE_VIEW(asearch -> atom_tree), TRUE);
  }
  return asearch -> atom_tree;
}

/*!
  \fn int get_todo_size (atom_search * asearch)

  \brief get the size of the atom search selection list

  \param asearch the target atom search
*/
int get_todo_size (atom_search * asearch)
{
  int object = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  project * this_proj = get_project_by_id (asearch -> proj);
  int tsize = 0;
  switch (filter)
  {
    case 0:
      tsize = (asearch -> mode || asearch -> passivating) ? this_proj -> nspec : this_proj -> natomes;
      break;
    case 1:
      tsize = (asearch -> mode || asearch -> passivating) ? this_proj -> coord -> cmax+1 : this_proj -> natomes;
      break;
    default:
      if (asearch -> mode || asearch -> passivating)
      {
        tsize = this_proj -> coord -> totcoord[filter -1];
      }
      else
      {
        tsize = (object && filter > 2) ? this_proj -> coord -> totcoord[filter -1] : this_proj -> natomes;
      }
      break;
  }
  return tsize;
}

/*!
  \fn void allocate_todo (atom_search * asearch, int tsize)

  \brief allocate the selection list data buffer

  \param asearch the target atom search
  \param tsize the target size
*/
void allocate_todo (atom_search * asearch, int tsize)
{
  asearch -> todo = allocint (tsize);
  asearch -> todo_size = tsize;
}

/*!
  \fn void clean_todo (atom_search * asearch)

  \brief clean atom search selection list

  \param asearch the target atom search
*/
void clean_todo (atom_search * asearch)
{
  project * this_proj = get_project_by_id (asearch -> proj);
  if (asearch -> todo) g_free (asearch -> todo);
  allocate_todo (asearch, get_todo_size(asearch));
  atomic_object * object;
  switch (asearch -> action)
  {
    case DISPL:
      if (this_proj -> modelgl -> atom_win -> to_be_moved[0])
      {
        object = this_proj -> modelgl -> atom_win -> to_be_moved[0];
        while (object -> next)
        {
          object = object -> next;
          g_free (object -> prev);
        }
        g_free (object);
        this_proj -> modelgl -> atom_win -> to_be_moved[0] = NULL;
      }
      break;
    case RANMOVE:
      if (this_proj -> modelgl -> atom_win -> to_be_moved[1])
      {
        object = this_proj -> modelgl -> atom_win -> to_be_moved[1];
        while (object -> next)
        {
          object = object -> next;
          g_free (object -> prev);
        }
        g_free (object);
        this_proj -> modelgl -> atom_win -> to_be_moved[1] = NULL;
      }
      g_free (this_proj -> modelgl -> atom_win -> msd);
      this_proj -> modelgl -> atom_win -> msd = allocfloat (asearch -> todo_size);
      break;
    case REPLACE:
      if (this_proj -> modelgl -> atom_win -> to_be_inserted[0])
      {
        object = this_proj -> modelgl -> atom_win -> to_be_inserted[0];
        while (object -> next)
        {
          object = object -> next;
          g_free (object -> prev);
        }
        g_free (object);
        this_proj -> modelgl -> atom_win -> to_be_inserted[0] = NULL;
      }
      asearch -> in_selection = 0;
      break;
  }
}

/*!
  \fn void clean_picked_and_labelled (atom_search * asearch, gboolean clean_msd)

  \brief initialize atom search data buffers

  \param asearch the target atom search
  \param clean_msd clean msd all data (1) or not (0)
*/
void clean_picked_and_labelled (atom_search * asearch, gboolean clean_msd)
{
  project * this_proj;
  int val = get_asearch_num_objects (asearch);
  asearch -> lab = allocint(val);
  asearch -> pick = allocint(val);
  if (asearch -> action == RANMOVE)
  {
    this_proj = get_project_by_id (asearch -> proj);
    if (this_proj -> modelgl -> atom_win && clean_msd)
    {
      g_free (this_proj -> modelgl -> atom_win -> msd_all);
      this_proj -> modelgl -> atom_win -> msd_all = allocfloat (val);
    }
  }
  else if (asearch -> action == REPLACE)
  {
    this_proj = get_project_by_id (asearch -> proj);
    int i = (asearch -> pointer[0].c == 3) ? 0 : 1;
    if (this_proj -> modelgl -> atom_win)
    {
      if (this_proj -> modelgl -> atom_win -> win && this_proj -> modelgl -> atom_win -> replace_nodes[i])
      {
        g_free (this_proj -> modelgl -> atom_win -> replace_nodes[i]);
      }
      this_proj -> modelgl -> atom_win -> replace_nodes[i] = g_malloc0(val*sizeof*this_proj -> modelgl -> atom_win -> replace_nodes[i]);
    }
  }
}

/*!
  \fn G_MODULE_EXPORT void set_spec_changed (GtkComboBox * box, gpointer data)

  \brief change the search chemical species

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_spec_changed (GtkComboBox * box, gpointer data)
{
  atom_search * asearch = (atom_search *) data;
  int i = combo_get_active ((GtkWidget *)box);
  asearch -> spec = i;
  update_search_tree (asearch);
  if (get_project_by_id(asearch -> proj) -> natomes >= 10000)
  {
    if (i > 0)
    {
      update_entry_text (GTK_ENTRY(asearch -> entry_a), exact_name(get_project_by_id(asearch -> proj) -> chemistry -> label[i-1]));
    }
    else
    {
      update_entry_text (GTK_ENTRY(asearch -> entry_a), "");
    }
    set_atom (GTK_ENTRY(asearch -> entry_a), asearch);
    widget_set_sensitive (asearch -> entry_a, ! i);
  }
}

/*!
  \fn G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data)

  \brief change the search filter

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_filter_changed (GtkComboBox * box, gpointer data)
{
  atom_search * asearch = (atom_search *) data;
  asearch -> filter = combo_get_active ((GtkWidget *)box);
  int object = get_asearch_object (asearch);
  int filter = get_asearch_filter (asearch);
  widget_set_sensitive (asearch -> atom_box, (object && filter > 2) ? 0 : 1);
  widget_set_sensitive (asearch -> id_box, (object && filter > 2) ? 0 : 1);
  if (object && filter > 2)
  {
    combo_set_active (asearch -> atom_box, 0);
  }
  if (asearch -> action == DISPL) motion_to_zero (asearch);
  clean_todo (asearch);
  clean_picked_and_labelled (asearch, TRUE);
  set_spec_changed (GTK_COMBO_BOX(asearch -> atom_box), asearch);
}

/*!
  \fn G_MODULE_EXPORT void set_object_changed (GtkComboBox * box, gpointer data)

  \brief change the search object

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_object_changed (GtkComboBox * box, gpointer data)
{
  atom_search * asearch = (atom_search *) data;
  GtkTreeViewColumn * rot_c;
  int was_object;
  int object;
  int filter;
  if (get_project_by_id(asearch -> proj) -> natomes >= 10000)
  {
    was_object = ((! asearch -> mode && asearch -> object > 1) || (asearch -> mode && asearch -> object)) ? 1 : 0;
    asearch -> object = combo_get_active ((GtkWidget *)box);
    filter = get_asearch_filter (asearch);
    object =  ((! asearch -> mode && asearch -> object > 1) || (asearch -> mode && asearch -> object)) ? 1 : 0;
    if ((! asearch -> mode && (asearch -> object == 1 || asearch -> object == 3)) || (asearch -> mode && asearch -> object))
    {
      if (! asearch -> mode) asearch -> passivating = TRUE;
      if (is_the_widget_visible(asearch -> id_box)) hide_the_widgets (asearch -> id_box);
      if (is_the_widget_visible(asearch -> info[1])) hide_the_widgets (asearch -> info[1]);
    }
    else
    {
      asearch -> passivating = FALSE;
      if (((! asearch -> mode && asearch -> object == 2) || (asearch -> mode && asearch -> object)) && filter > 2)
      {
        if (is_the_widget_visible(asearch -> id_box)) hide_the_widgets (asearch -> id_box);
        if (is_the_widget_visible(asearch -> info[1])) hide_the_widgets (asearch -> info[1]);
      }
      else
      {
        if (! is_the_widget_visible(asearch -> id_box)) show_the_widgets (asearch -> id_box);
        if (! is_the_widget_visible(asearch -> info[1])) show_the_widgets (asearch -> info[1]);
      }
    }
  }
  else
  {
    was_object = (asearch -> object) ? 1 : 0;
    asearch -> object = combo_get_active ((GtkWidget *)box);
    filter = get_asearch_filter (asearch);
    object = (asearch -> object) ? 1 : 0;
    if (is_the_widget_visible(asearch -> id_box)) hide_the_widgets (asearch -> id_box);
    if (is_the_widget_visible(asearch -> info[1])) hide_the_widgets (asearch -> info[1]);
  }
  if (was_object)
  {
    combo_text_prepend (asearch -> filter_box, "Chemical species");
    if (asearch -> action == RANMOVE)
    {
      rot_c = gtk_tree_view_get_column (GTK_TREE_VIEW(asearch -> atom_tree), 4);
      gtk_tree_view_remove_column (GTK_TREE_VIEW(asearch -> atom_tree), rot_c);
    }
  }
  if (object)
  {
    gtk_combo_box_text_remove ((GtkComboBoxText *)asearch -> filter_box, 0);
    if (asearch -> action == RANMOVE)
    {
      GtkCellRenderer * rot = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(rot), "toggled", G_CALLBACK(select_atom), & asearch -> pointer[2]);
      int i = 5;
      rot_c = gtk_tree_view_column_new_with_attributes ("Rotate", rot, "active", i, NULL);
      gtk_tree_view_column_set_clickable (rot_c, TRUE);
      g_signal_connect (G_OBJECT(rot_c), "clicked", G_CALLBACK(select_all_atoms), & asearch -> pointer[2]);
      gtk_tree_view_column_set_alignment (rot_c, 0.5);
      gtk_tree_view_insert_column (GTK_TREE_VIEW(asearch -> atom_tree), rot_c, i-1);
    }
  }

  if (asearch -> action == DISPL) widget_set_sensitive (get_project_by_id(asearch -> proj) -> modelgl -> atom_win -> at_expand[2], object);
  combo_set_active (asearch -> filter_box, 0);
  set_filter_changed (GTK_COMBO_BOX(asearch -> filter_box), asearch);
}

/*!
  \fn void add_random_column (atom_search * asearch)

  \brief add a column to the search tree model for the case of random search

  \param asearch the target atom search
*/
void add_random_column (atom_search * asearch)
{
  int i = 8 - asearch -> action;
  GtkCellRenderer * num = gtk_cell_renderer_text_new ();
  g_object_set (num, "editable", TRUE, NULL);
  g_signal_connect (G_OBJECT(num), "editing-started", G_CALLBACK(to_edit_coords), & asearch -> pointer[0]);
  GtkTreeViewColumn * num_c = gtk_tree_view_column_new_with_attributes ("Number", num, "text", i, NULL);
  gtk_tree_view_column_set_cell_data_func (num_c, num, search_set_visible, GINT_TO_POINTER(i), NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(asearch -> atom_tree), num_c);
}

/*!
  \fn G_MODULE_EXPORT void set_search_mode (GtkComboBox * box, gpointer data)

  \brief change search mode

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_search_mode (GtkComboBox * box, gpointer data)
{
  atom_search * asearch = (atom_search *)data;
  project * this_proj = get_project_by_id(asearch -> proj);
  asearch -> mode = combo_get_active ((GtkWidget *)box);
  if ((asearch -> action == REPLACE || asearch -> action == REMOVE) && asearch -> atom_tree)
  {
    if (! asearch -> mode)
    {
      GtkTreeViewColumn * num_c;
      num_c = gtk_tree_view_get_column (GTK_TREE_VIEW(asearch -> atom_tree), 8 - asearch -> action);
      gtk_tree_view_remove_column (GTK_TREE_VIEW(asearch -> atom_tree), num_c);
    }
    else
    {
      add_random_column (asearch);
    }
  }
  if (this_proj -> natomes >= 10000)
  {
    if (asearch -> object_box)
    {
      if (asearch -> mode)
      {
        gtk_combo_box_text_remove ((GtkComboBoxText *)asearch -> object_box, 3);
        gtk_combo_box_text_remove ((GtkComboBoxText *)asearch -> object_box, 1);
      }
      else
      {
        gtk_combo_box_text_insert ((GtkComboBoxText *)asearch -> object_box, 1, NULL, "Atom(s): all");
        combo_text_append (asearch -> object_box, "Group of atoms: all");
      }
    }
  }
  else
  {
    gtk_combo_box_text_remove_all ((GtkComboBoxText *)asearch -> object_box);
    combo_text_append (asearch -> object_box, "Atom(s)");
    combo_text_append (asearch -> object_box, "Group of atoms");
  }

  /*if (asearch -> filter_box)
  {
    if (! this_proj -> modelgl -> adv_bonding[1] && this_proj -> modelgl -> atom_win -> adv_bonding[1])
    {
      gtk_combo_box_text_remove ((GtkComboBoxText *)asearch -> filter_box, 3+i);
    }
    if (! this_proj -> modelgl -> adv_bonding[0] && this_proj -> modelgl -> atom_win -> adv_bonding[0])
    {
      gtk_combo_box_text_remove ((GtkComboBoxText *)asearch -> filter_box, 2+i);
    }
    combo_set_active (asearch -> filter_box, 0);
    set_filter_changed (GTK_COMBO_BOX(asearch -> filter_box), asearch);
  }*/
  combo_set_active (asearch -> object_box, 0);
  set_object_changed (GTK_COMBO_BOX(asearch -> object_box), asearch);
}

/*!
  \fn G_MODULE_EXPORT void set_search_digit (GtkEntry * res, gpointer data)

  \brief update search entry

  \param res the GtkEntry sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_search_digit (GtkEntry * res, gpointer data)
{
  atom_search * asearch = (atom_search *) data;
  const gchar * m = entry_get_text (res);
  double v = string_to_double ((gpointer)m);
  asearch -> search_digit = (int)v;
  if (asearch -> search_digit <= 0)
  {
    asearch -> search_digit --;
    update_entry_text (res, "");
  }
  else
  {
    update_entry_int (res, asearch -> search_digit);
  }
  set_spec_changed (GTK_COMBO_BOX(asearch -> atom_box), data);
}

/*!
  \fn void prep_search_box (GtkWidget * vbox, GtkWidget * lab, GtkWidget * combo)

  \brief prepare some search widgets

  \param vbox the box to insert the search box in
  \param lab the GtkWidget sending the signal
  \param combo the GtkWidget sending the signal
*/
void prep_search_box (GtkWidget * vbox, GtkWidget * lab, GtkWidget * combo)
{
  GtkWidget * hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 2);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, lab, FALSE, FALSE, 30);
  GtkWidget * fixed = gtk_fixed_new ();
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, fixed, FALSE, FALSE, 10);
  gtk_widget_set_size_request (combo, 175, -1);
  gtk_fixed_put (GTK_FIXED(fixed), combo, 0, 0);
}

/*!
  \fn GtkWidget * prepare_box_too_much (atom_search * asearch)

  \brief if too many atoms, then individual search

  \param asearch the target atom search
*/
GtkWidget * prepare_box_too_much (atom_search * asearch)
{
  GtkWidget * box;
  GtkWidget * widg;
  GtkWidget * too_box = create_vbox (BSEP);
  widg = bbox (too_box, "\t Atom species: ");
  gtk_widget_set_size_request (widg, 200, -1);
  asearch -> entry_a = create_entry (G_CALLBACK(set_atom), 90, 15, TRUE, asearch);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, widg, asearch -> entry_a, FALSE, FALSE, 0);
  asearch -> img_a = stock_image (DIAL_ERROR);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, widg, asearch -> img_a, FALSE, FALSE, 5);
  widg = bbox (too_box, "\t Atom Id: ");
  gtk_widget_set_size_request (widg, 200, -1);
  asearch -> entry_b = create_entry (G_CALLBACK(set_id), 90, 15, TRUE, asearch);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, widg, asearch -> entry_b, FALSE, FALSE, 0);
  asearch -> img_b = stock_image (DIAL_ERROR);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, widg, asearch -> img_b, FALSE, FALSE, 5);
  box = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, too_box, box, FALSE, FALSE, 5);
  asearch -> but_a = create_button ("Add", IMG_NONE, NULL, 75, -1, GTK_RELIEF_NORMAL, G_CALLBACK(add_atom), asearch);
  widget_set_sensitive (asearch -> but_a, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, asearch -> but_a, FALSE, FALSE, 75);
  asearch -> but_b = create_button ("Remove", IMG_NONE, NULL, 75, -1, GTK_RELIEF_NORMAL, G_CALLBACK(remove_atom), asearch);
  widget_set_sensitive (asearch -> but_b, 0);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, box, asearch -> but_b, FALSE, FALSE, 0);
  return too_box;
}

/*!
  \fn GtkWidget * selection_tab (atom_search * asearch, int nats)

  \brief create the search widget

  \param asearch the target atom search
  \param nats the total number of atoms
*/
GtkWidget * selection_tab (atom_search * asearch, int nats)
{
  project * this_proj = get_project_by_id (asearch -> proj);
  int i, j;
  i = (nats < 10000) ? 1 : 0;
  j = (asearch -> action == 5) ? 300 : -1;
  GtkWidget * selection = create_layout (j, 390 + (! i)*60 - i*100);
  GtkWidget * vbox = add_vbox_to_layout (selection, 0, (asearch -> action > 1) ? 0 : 10);
  GtkWidget * hbox, * vvbox;
  GtkWidget * lab;
  if (asearch -> action != 5)
  {
    // Later on simply hide info[1] (to much atoms info) if needed
    for (j=0; j<2; j++)
    {
      asearch -> info[j] = create_vbox (BSEP);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, asearch -> info[j], FALSE, FALSE, 0);
    }
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[0], hbox, FALSE, FALSE, 2);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("<b>.</b>", 5, -1, 0.0, 0.5), FALSE, FALSE, 10);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Search ", 50, -1, 0.0, 0.5), FALSE, FALSE, 0);
    asearch -> object_box  = create_combo ();
    combo_text_append (asearch -> object_box, "Atom(s)");
    if (! i) combo_text_append (asearch -> object_box, "Atom(s): all");
    combo_text_append (asearch -> object_box, "Group of atoms");
    if (! i) combo_text_append (asearch -> object_box, "Group of atoms: all");
    combo_set_active (asearch -> object_box, asearch -> object);
    asearch -> filter_box  = create_combo ();
    gchar * filters[5]={"Chemical species", "Total coordination", "Partial coordination", "Fragment", "Molecule"};
    for (j=0; j<3; j++) combo_text_append (asearch -> filter_box, filters[j]);
    if (this_proj -> modelgl -> adv_bonding[0]) combo_text_append (asearch -> filter_box, filters[3]);
    if (this_proj -> modelgl -> adv_bonding[1]) combo_text_append (asearch -> filter_box, filters[4]);
    combo_set_active (asearch -> filter_box, asearch -> filter);

    asearch -> atom_box  = create_combo ();
    combo_text_append (asearch -> atom_box, "All");
    for (j=0; j<this_proj -> nspec; j++) combo_text_append (asearch -> atom_box, this_proj -> chemistry -> label[j]);
    combo_set_active (asearch -> atom_box, asearch -> spec);

    g_signal_connect (G_OBJECT (asearch -> object_box), "changed", G_CALLBACK(set_object_changed), asearch);
    g_signal_connect (G_OBJECT (asearch -> filter_box), "changed", G_CALLBACK(set_filter_changed), asearch);
    g_signal_connect (G_OBJECT (asearch -> atom_box), "changed", G_CALLBACK(set_spec_changed), asearch);
    GtkWidget * entry = create_entry (G_CALLBACK(set_search_digit), 90, 15, TRUE, asearch);
    if (asearch -> action < 2)
    {
      prep_search_box (asearch -> info[0], markup_label("For ", 100, -1, 0.0, 0.5), asearch -> object_box);
      prep_search_box (asearch -> info[0], markup_label("Filter by ", 100, -1, 0.0, 0.5), asearch -> filter_box);
      prep_search_box (asearch -> info[0], markup_label("Species ", 100, -1, 0.0, 0.5), asearch -> atom_box);
      asearch -> id_box = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[0], asearch -> id_box, FALSE, FALSE, 0);
      prep_search_box (asearch -> id_box, markup_label("Atom Id ", 100, -1, 0.0, 0.5), entry);
    }
    else
    {
      hbox = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[0], hbox, FALSE, FALSE, 5);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("For ", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, asearch -> object_box, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Filter by ", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, asearch -> filter_box, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, markup_label("Species ", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, asearch -> atom_box, FALSE, FALSE, 0);
      asearch -> id_box = create_hbox (0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, asearch -> id_box, FALSE, FALSE, 0);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, asearch -> id_box, markup_label("Atom Id ", -1, -1, 0.0, 0.5), FALSE, FALSE, 10);
      add_box_child_start (GTK_ORIENTATION_HORIZONTAL, asearch -> id_box, entry, FALSE, FALSE, 0);
    }

    // If more than 10 000 atoms:
    if (asearch -> action < 2)
    {
      lab = markup_label("\t<b>The number of atoms in the model is too large</b>\n"
                         "\t\t<b>to display and browse the entire list !</b>\n"
                         "\t<b>You need to search for object(s) manually:</b>", -1, -1, 0.5, 0.5);
    }
    else
    {
      lab = markup_label("<b>The number of atoms in the model is too large to display and browse the entire list !</b>\n"
                         "\t\t\t\t<b>You need to search for object(s) manually:</b>", -1, -1, 0.5, 0.5);
    }
    add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[1], lab, FALSE, FALSE, 10);
    hbox = create_hbox (0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[1], hbox, FALSE, FALSE, 0);
    /* vvbox = create_vbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vvbox, FALSE, FALSE, 80);
    cbox = create_combo ();
    combo_text_append (cbox, "All objects");
    combo_text_append (cbox, "Selection");
    g_signal_connect (G_OBJECT(cbox), "changed", G_CALLBACK(set_too_much_type), asearch);
    combo_set_active (cbox, asearch -> too_much);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, cbox, FALSE, FALSE, 30);
    // asearch -> big_box = // Combo box with "All objects", "Selection" */
    vvbox = create_vbox (0);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, vvbox, FALSE, FALSE, 120);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, vvbox, prepare_box_too_much (asearch), FALSE, FALSE, 0);
    add_box_child_start (GTK_ORIENTATION_VERTICAL, asearch -> info[1],
                         markup_label("The search must be performed using species and/or ID.\n"
                                      "Use the filters above to define the object(s) of the search", -1, -1, 0.5, 0.5), FALSE, FALSE, 5);
  }

  abox (vbox, "Selection ", 2);
  hbox = create_hbox (0);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, vbox, hbox, FALSE, FALSE, 5);
  GtkWidget * scrollsets = create_scroll (NULL, -1, -1, GTK_SHADOW_ETCHED_IN);
  i = (asearch -> action < 2) ? 0 : 1;
  j = (asearch -> action < 2) ? 1 : 0;
  if (asearch -> action < 2)
  {
    gtk_widget_set_size_request (scrollsets, 400+i*100, 270);
  }
  else
  {
    gtk_widget_set_size_request (scrollsets, 400+i*100, 290-i*120);
  }
  gtk_widget_set_hexpand (scrollsets, TRUE);
  add_container_child (CONTAINER_SCR, scrollsets, create_atoms_tree (asearch, this_proj, nats));
  check_all_trees (this_proj);
  add_box_child_start (GTK_ORIENTATION_HORIZONTAL, hbox, scrollsets, FALSE, FALSE, 100-j*75);
  return selection;
}
