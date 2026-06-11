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
* @file dlp_mol.c
* @short Functions to add / remove molecule(s) to / from the force field
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_mol.c'
*
* Contains:
*

 - The functions to add / remove molecule(s) to / from the force field

*
* List of functions:

  gchar * remove_text (int i, int j, gchar * str);

  void clean_up_molecules_info (gboolean usel);
  void set_sensitive_mol (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void clear_field_atoms (field_molecule * fmol, field_atom* at, int mols, int * mol);
  void molecule_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data);
  void merge_all_atoms_to_mol (field_molecule * new_mol, int mstart);
  void prepare_atoms_to_merge (field_atom* at, field_molecule * new_mol, field_molecule * old_mol);

  G_MODULE_EXPORT void select_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void run_add_molecule_to_field (GtkDialog * dialog, gint response_id, gpointer data);
  G_MODULE_EXPORT void add_molecule_to_field (GSimpleAction * action, GVariant * parameter, gpointer data);
  G_MODULE_EXPORT void run_remove_molecule_from_field (GtkDialog * rmol, gint response_id, gpointer data);
  G_MODULE_EXPORT void remove_molecule_from_field (GSimpleAction * action, GVariant * parameter, gpointer data);

  field_atom* new_atom_to_merge (int id, field_molecule * fmol);

*/

#include "dlp_field.h"
#include "calc.h"
#include "interface.h"
#include "glview.h"

int active_col;
int a_mol, b_mol;
int * new_mol;
extern ColRGBA init_color (int id, int numid);
field_molecule * to_remove;
GtkWidget * remove_label;
GtkCellRenderer * remove_renderer[5];
GtkTreeViewColumn * remove_col[5];
gboolean removing = FALSE;
extern void field_unselect_all ();
extern gchar * set_field_atom_name (field_atom* ato, field_molecule * mol);
extern void find_atom_id_in_field_molecule (field_molecule * fmol);
extern void viz_fragment (field_molecule * fmol, int id, int viz);
extern void check_to_visualize_properties (int id);

/*!
  \fn gchar * remove_text (int i, int j, gchar * str)

  \brief get removal information text

  \param i -2 = remove field atom, -1 = remove field molecule
  \param j -1 = remove field molecule, else remove field atom
  \param str the name of the object to remove
*/
gchar * remove_text (int i, int j, gchar * str)
{
  switch (i)
  {
    case -2:
      return g_strdup_printf (_("The description of atom <b>%s</b> will be deleted\n"
                                "and merged with the one of the selected field atom.\n"
                                "Field object(s) using atom <b>%s</b> will also be deleted\n"
                                "and the force field parameter(s) will be updated accordingly."),
                              str, str);
      break;
    case -1:
      return g_strdup_printf (_("The description of molecule <b>%s</b> will be deleted\nand merged with the one of the selected molecule."), str);
      break;
    default:
      switch (j)
      {
        case -1:
          return g_strdup_printf (_("The description of molecule <b>%s</b> will be deleted\nand merged with the one of molecule <b>%s</b>"), str, get_active_field_molecule(i) -> name);
          break;
        default:
          return g_strdup_printf (_("The description of atom <b>%s</b> will be deleted\n"
                                    "and merged with the one of field atom <b>%s</b>\n"
                                    "Field object(s) using atom <b>%s</b> will also be deleted\n"
                                    "and the force field parameter(s) will be updated accordingly."),
                                  str, get_active_atom(i,j) -> name, str);
          break;
      }
      break;
  }
}

/*!
  \fn void clean_up_molecules_info (gboolean usel)

  \brief prepare molecule related widgets in the assistant

  \param usel unselect all atom(s) (yes / no)
*/
void clean_up_molecules_info (gboolean usel)
{
  int i;

  for (i=0; i<MOLIMIT-1; i++)
  {
    combo_mol[i] = destroy_this_widget (combo_mol[i]);
    combo_mol[i] = create_combo_mol (i);
    add_box_child_start (GTK_ORIENTATION_HORIZONTAL, mol_box[i], combo_mol[i], FALSE, FALSE, 10);
    show_the_widgets (combo_mol[i]);
    changed_mol_box (GTK_COMBO_BOX(combo_mol[i]), GINT_TO_POINTER(i+1));
  }
  set_mol_num_label ();
  if (usel) field_unselect_all ();
  gtk_tree_store_clear (field_model[0]);
  fill_field_model (field_model[0], 0, -1);
}

/*!
  \fn void set_sensitive_mol (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer sensitivity in the force field molecule selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void set_sensitive_mol (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int m;
  gtk_tree_model_get (mod, iter, 0, & m, -1);
  if (a_mol && m-1 != new_mol[0])
  {
    gtk_cell_renderer_set_visible (renderer, FALSE);
  }
  else
  {
    gtk_cell_renderer_set_visible (renderer, TRUE);
  }
}

int old_viz_mol;

/*!
  \fn G_MODULE_EXPORT void select_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on select field molecule toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void select_mol (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  GtkTreeStore ** model = (GtkTreeStore **)data;
  GtkTreeIter iter;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(* model), & iter, path);
  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    new_mol[a_mol-1] = -1;
    a_mol --;
    gtk_tree_store_set (* model, & iter, active_col, 0, -1);
    toviz.c = 0;
  }
  else
  {
    a_mol ++;
    gtk_tree_store_set (* model, & iter, active_col, 1, -1);
    gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & new_mol[a_mol-1], -1);
    new_mol[a_mol-1] --;
    toviz.c = 1;
  }
  if (active_col == 1)
  {
    toviz.a = 2;
    gtk_tree_model_get (GTK_TREE_MODEL(* model), & iter, 0, & toviz.b, -1);
    viz_fragment (tmp_fmol, toviz.b-1, toviz.c);
  }
  else if (active_col == 3)
  {
    if (toviz.c) old_viz_mol = new_mol[0];
    visualize_object (0, old_viz_mol, -1);
    check_to_visualize_properties (0);
    gtk_label_set_text (GTK_LABEL(remove_label), remove_text(new_mol[0], -1, to_remove -> name));
    gtk_label_set_use_markup (GTK_LABEL(remove_label), TRUE);
  }
}

dint ** atomd_id_save;

/*!
  \fn void clear_field_atoms (field_molecule * fmol, field_atom* at, int mols, int * mol)

  \brief clean the field atom from a list of atom(s)

  \param fmol the target fied molecule
  \param at the field atom to clean, remove specific atom(s) from it
  \param mols the number of fragment(s) for this molecule
  \param mol the list of fragment id
*/
void clear_field_atoms (field_molecule * fmol, field_atom* at, int mols, int * mol)
{
  int i, j, k, l;
  l = 0;
  for (i=0; i < at -> num; i++)
  {
    k = at -> list[i];
    for (j=0; j < mols; j++)
    {
      if (tmp_proj -> atoms[0][k].coord[2] == mol[j])
      {
        l ++;
      }
    }
  }
  int * r_list = allocint (l);
  int * s_list = allocint (l);
  int * t_list = allocint (l);
  l = -1;
  at -> frozen = 0;
  for (i=0; i < at -> num; i++)
  {
    k = at -> list[i];
    for (j=0; j < mols; j++)
    {
      if (tmp_proj -> atoms[0][k].coord[2] == mol[j])
      {
        l ++;
        r_list[l] = k;
        s_list[l] = at -> list_id[i];
        t_list[l] = at -> frozen_id[i];
        if (t_list[l]) at -> frozen ++;
      }
    }
  }
  at -> num = l+1;
  at -> list = NULL;
  at -> list = allocint (at -> num);
  at -> list_id = NULL;
  at -> list_id = allocint (at -> num);
  at -> frozen_id = NULL;
  at -> frozen_id = allocbool (at -> num);
  for (i=0; i < at -> num; i++)
  {
    at -> list[i] = r_list[i];
    at -> list_id[i] = s_list[i];
    at -> frozen_id[i] = t_list[i];
    j = at -> list[i];
    k = at -> list_id[i];
    for (l=0; l<mols; l++)
    {
      if (tmp_proj -> atoms[0][j].coord[2] == mol[l])
      {
        atomd_id_save[k][l].a = at -> id;
        atomd_id_save[k][l].b = i;
      }
    }
  }
  g_free (r_list);
  g_free (s_list);
  g_free (t_list);
}

/*!
  \fn void molecule_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)

  \brief set renderer color in the force field molecule selection tree store

  \param col the target GtkTreeViewColumn
  \param renderer the target cell renderer
  \param mod the target tree model
  \param iter the target tree iter
  \param data the associated data pointer
*/
void molecule_set_color (GtkTreeViewColumn * col, GtkCellRenderer * renderer, GtkTreeModel * mod, GtkTreeIter * iter, gpointer data)
{
  int i, j;
  gtk_tree_model_get (mod, iter, active_col, & j, -1);
  gtk_tree_model_get (mod, iter, 0, & i, -1);
  set_renderer_color (j, renderer, init_color (i-1, num_field_objects));
}

/*!
  \fn G_MODULE_EXPORT void run_add_molecule_to_field (GtkDialog * dialog, gint response_id, gpointer data)

  \brief add molecule to force field - running the dialog

  \param dialog the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_add_molecule_to_field (GtkDialog * dialog, gint response_id, gpointer data)
{
  int i, j, k;
  int * old_mol = NULL;
  gchar * str;
  gboolean done = FALSE;
  gboolean saveit = FALSE;
  // Id - Name - Multiplicity - Select
  // While selected < m apply inactif
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (a_mol > 0 && a_mol < tmp_fmol -> multi)
      {
        if (a_mol > 1)
        {
          str = g_strdup_printf (_("Fragments N°%d"), new_mol[0]+1);
          if (a_mol > 2)
          {
            for (i=1; i<a_mol-1; i++)
            {
              str = g_strdup_printf ("%s, %d", str, new_mol[i]+1);
            }
          }
          str = g_strdup_printf (_("%s and %d have been selected !"), str, new_mol[a_mol-1]+1);
          str = g_strdup_printf (_("%s\nConfirm this choice and create a new field molecule to describe these fragments ?"), str);
        }
        else
        {
          str = g_strdup_printf (_("Fragment N°%d has been selected !"), new_mol[0]+1);
          str = g_strdup_printf (_("%s\nConfirm this choice and create a new field molecule to describe this fragment ?"), str);
        }
        selection_confirmed = FALSE;
        field_question (str, G_CALLBACK(confirm_selection), NULL);
        g_free (str);
        if (selection_confirmed)
        {
          done = TRUE;
          field_molecule * old_fmol = get_active_field_molecule(row_id);
          field_molecule * next_fmol = duplicate_field_molecule (old_fmol);
          get_active_field_molecule(tmp_field -> molecules-1) -> next = next_fmol;
          next_fmol -> prev = get_active_field_molecule(tmp_field -> molecules-1);
          next_fmol -> id = tmp_field -> molecules;
          next_fmol -> multi = a_mol;
          next_fmol -> fragments = NULL;
          next_fmol -> fragments = allocint (a_mol);
          for (i=0; i<a_mol; i++) next_fmol -> fragments[i] = new_mol[i];
          // Atoms_id and field atoms
          atomd_id_save = g_malloc0(next_fmol-> mol -> natoms*sizeof*atomd_id_save);
          for (i=0; i<next_fmol-> mol -> natoms; i++)
          {
            atomd_id_save[i] = g_malloc0(next_fmol -> multi*sizeof*atomd_id_save[i]);
          }
          tmp_fat = next_fmol -> first_atom;
          for (i=0; i < next_fmol -> atoms; i++)
          {
            clear_field_atoms (next_fmol, tmp_fat, a_mol, new_mol);
            tmp_fat -> name = g_strdup_printf ("%s", set_field_atom_name (tmp_fat, next_fmol));
            if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
          }
          g_free (next_fmol -> atoms_id);
          next_fmol -> atoms_id = NULL;
          next_fmol -> atoms_id = g_malloc0(next_fmol-> mol -> natoms*sizeof*next_fmol -> atoms_id);
          for (i=0; i<next_fmol-> mol -> natoms; i++)
          {
            next_fmol -> atoms_id[i] = g_malloc0(next_fmol -> multi*sizeof*next_fmol -> atoms_id[i]);
            for (j=0; j<next_fmol -> multi; j++)
            {
              next_fmol -> atoms_id[i][j].a = atomd_id_save[i][j].a;
              next_fmol -> atoms_id[i][j].b = atomd_id_save[i][j].b;
            }
          }
          g_free (atomd_id_save);
          old_mol = allocint (old_fmol -> multi - a_mol);
          k = -1;
          for (i=0; i<old_fmol -> multi; i++)
          {
            saveit = TRUE;
            for (j=0; j<a_mol; j++)
            {
              if (new_mol[j] == old_fmol -> fragments[i])
              {
                saveit = FALSE;
                break;
              }
            }
            if (saveit)
            {
              k++;
              old_mol[k] = old_fmol -> fragments[i];
            }
          }
          new_mol = NULL;
          // Now we deal with atoms_id and the field atoms
          atomd_id_save = g_malloc0(old_fmol-> mol -> natoms*sizeof*atomd_id_save);
          for (i=0; i<old_fmol -> mol -> natoms; i++)
          {
            atomd_id_save[i] = g_malloc0((old_fmol -> multi - a_mol)*sizeof*atomd_id_save[i]);
          }
          tmp_fat = old_fmol -> first_atom;
          for (i=0; i < old_fmol -> atoms; i++)
          {
            clear_field_atoms (old_fmol, tmp_fat, old_fmol -> multi - a_mol, old_mol);
            if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
          }
          old_fmol -> multi -= a_mol;
          g_free (old_fmol -> atoms_id);
          old_fmol -> atoms_id = NULL;
          old_fmol -> atoms_id = g_malloc0(old_fmol-> mol -> natoms*sizeof*old_fmol -> atoms_id);
          for (i=0; i<old_fmol-> mol -> natoms; i++)
          {
            old_fmol -> atoms_id[i] = g_malloc0(old_fmol -> multi*sizeof*old_fmol -> atoms_id[i]);
            for (j=0; j<old_fmol -> multi; j++)
            {
              old_fmol -> atoms_id[i][j].a = atomd_id_save[i][j].a;
              old_fmol -> atoms_id[i][j].b = atomd_id_save[i][j].b;
            }
          }
          g_free (atomd_id_save);
          old_fmol -> fragments = NULL;
          old_fmol -> fragments = allocint (old_fmol -> multi);
          for (i=0; i<old_fmol -> multi; i++) old_fmol -> fragments[i] = old_mol[i];
          old_mol = NULL;
          row_id = tmp_field -> molecules;
          tmp_field -> molecules ++;
          edit_field_prop (NULL, NULL, GINT_TO_POINTER(0));
          clean_up_molecules_info (TRUE);
        }
      }
      break;
    default:
      field_unselect_all ();
      done = TRUE;
      break;
  }
  if (done)
  {
    g_free (new_mol);
    new_mol = NULL;
    destroy_this_dialog (dialog);
  }
}

/*!
  \fn G_MODULE_EXPORT void add_molecule_to_field (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief add molecule to force field - creating the dialog

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void add_molecule_to_field (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  int i;
  field_object = 0;
  gchar * str = g_strdup_printf (_("Please select the fragment(s) of the new molecule"));
  GtkWidget * amol = dialogmodal (str, GTK_WINDOW(field_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(amol), _("Apply"), GTK_RESPONSE_APPLY);
  GtkWidget * add_tree =  NULL;
  GtkTreeIter iter;
  GtkTreeViewColumn * mol_col[2];
  GtkCellRenderer * mol_cell[2];
  gchar * mol_title[2] = {i18n("Fragment"), i18n("Viz.3D & Select")};
  gchar * ctype[2]={"text", "active"};
  GType col_type[2] = {G_TYPE_INT, G_TYPE_BOOLEAN};

  active_col = 1;
  a_mol = 0;
  GtkTreeStore * add_model = gtk_tree_store_newv (2, col_type);
  add_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(add_model));
  tmp_fmol = get_active_field_molecule (row_id);
  for (i=0; i<2; i++)
  {
    if (i == 0)
    {
      mol_cell[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      mol_cell[i] = gtk_cell_renderer_toggle_new ();
      g_signal_connect (G_OBJECT(mol_cell[i]), "toggled", G_CALLBACK(select_mol), & add_model);
    }
    mol_col[i] = gtk_tree_view_column_new_with_attributes (_(mol_title[i]), mol_cell[i], ctype[i], i, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(add_tree), mol_col[i]);
    gtk_tree_view_column_set_alignment (mol_col[i], 0.5);
    if (i == 0)
    {
      gtk_tree_view_column_set_cell_data_func (mol_col[i], mol_cell[i], molecule_set_color, NULL, NULL);
    }
  }
  // fill model
  num_field_objects = tmp_coord-> totcoord[2];
  new_mol = NULL;
  new_mol = allocint (tmp_fmol -> multi);
  for (i=0; i<tmp_fmol -> multi; i++)
  {
    gtk_tree_store_append (add_model, & iter, NULL);
    gtk_tree_store_set (add_model, & iter, 0, tmp_fmol -> fragments[i] + 1, 1, 0, -1);
    new_mol[i] = -1;
  }
  g_object_unref (add_model);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(add_tree));

  i = ((tmp_fmol -> multi+1)*35 < 500) ? (tmp_fmol -> multi+1)*35 : 500;
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (amol), 220, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, add_tree);
  run_this_gtk_dialog (amol, G_CALLBACK(run_add_molecule_to_field), NULL);
}

/*!
  \fn void merge_all_atoms_to_mol (field_molecule * new_mol, int mstart)

  \brief merge all field atoms to another field molecule

  \param new_mol the field molecule to merge the atom(s) with
  \param mstart the fragment id to start with
*/
void merge_all_atoms_to_mol (field_molecule * new_mol, int mstart)
{
  field_atom* fat, * fbt;
  fat = get_active_atom (new_mol -> id, new_mol -> atoms-1);
  int i, j, k, l, m, n;
  for (i=0; i<fat -> num; i++)
  {
    j = fat -> list[i];
    k = fat -> list_id[i];
    l = new_mol -> atoms_id[k][0].a;
    fbt = get_active_atom (new_mol -> id, l);
    fbt -> list[fbt -> num] = j;
    fbt -> list_id[fbt -> num] = k;
    m = tmp_proj -> atoms[0][j].coord[2];
    for (n=mstart; n<new_mol -> multi; n++)
    {
      if (new_mol -> fragments[n] == m)
      {
        new_mol -> atoms_id[k][n].a = fbt -> id;
        new_mol -> atoms_id[k][n].b = fbt -> num;
        break;
      }
    }
    fbt -> num ++;
  }
}

/*!
  \fn void prepare_atoms_to_merge (field_atom* at, field_molecule * new_mol, field_molecule * old_mol)

  \brief update the data for the newly 'merged' field atom

  \param at the target field atom
  \param new_mol the target molecule to merge with
  \param old_mol the molecule to remove
*/
void prepare_atoms_to_merge (field_atom* at, field_molecule * new_mol, field_molecule * old_mol)
{
  int i, j;
  int * saved_list = allocint (at -> num);
  int * saved_list_id = allocint (at -> num);
  int * saved_frozen_id = allocbool (at -> num);
  for (j=0; j < at -> num; j++)
  {
    saved_list[j] = at -> list[j];
    saved_list_id[j] = at -> list_id[j];
    saved_frozen_id[j] = at -> frozen_id[j];
  }
  i = at -> num;
  i /= (new_mol -> multi - old_mol -> multi);
  i *= old_mol -> multi;
  g_free (at -> list);
  at -> list = allocint (at -> num + i);
  g_free (at -> list_id);
  at -> list_id = allocint (at -> num + i);
  g_free (at -> frozen_id);
  at -> frozen_id = allocbool (at -> num + i);
  for (j=0; j< at -> num; j++)
  {
    at -> list[j] = saved_list[j];
    at -> list_id[j] = saved_list_id[j];
    at -> frozen_id[j] = saved_frozen_id[j];
  }
  g_free (saved_list);
  g_free (saved_list_id);
  g_free (saved_frozen_id);
}

/*!
  \fn field_atom* new_atom_to_merge (int id, field_molecule * fmol)

  \brief merge field atoms from a field molecule

  \param id the id of the new field atom
  \param fmol the field molecule for of the atoms
*/
field_atom* new_atom_to_merge (int id, field_molecule * fmol)
{
  field_atom* fat, * fbt;
  fat = g_malloc0(sizeof*fat);
  fat -> id = id;
  fat -> sp = -1;
  fat -> num = fmol -> mol -> natoms * fmol -> multi;
  fat -> list = allocint (fmol -> mol -> natoms * fmol -> multi);
  fat -> list_id = allocint (fmol -> mol -> natoms * fmol -> multi);
  fbt = fmol -> first_atom;
  int i, j;
  i = 0;
  while (fbt != NULL)
  {
    for (j=0; j<fbt -> num; j++)
    {
      fat -> list[i] = fbt -> list[j];
      fat -> list_id[i] = fbt -> list_id[j];
      i ++;
    }
    fbt = fbt -> next;
  }
  return fat;
}

/*!
  \fn G_MODULE_EXPORT void run_remove_molecule_from_field (GtkDialog * rmol, gint response_id, gpointer data)

  \brief remove molecule from force field - running the dialog

  \param rmol the GtkDialog sending the signal
  \param response_id the response id
  \param data the associated data pointer
*/
G_MODULE_EXPORT void run_remove_molecule_from_field (GtkDialog * rmol, gint response_id, gpointer data)
{
  int i, j;
  gboolean done = FALSE;
  switch (response_id)
  {
    case GTK_RESPONSE_APPLY:
      if (a_mol)
      {
        done = TRUE;
        field_molecule * to_merge = get_active_field_molecule(new_mol[0]);
        int * old_mol = allocint (to_merge -> multi);
        for (i=0; i<to_merge -> multi; i++) old_mol[i] = to_merge -> fragments[i];
        g_free (to_merge -> fragments);
        to_merge -> fragments = allocint (to_merge -> multi + to_remove -> multi);
        for (i=0; i<to_merge -> multi; i++) to_merge -> fragments[i] = old_mol[i];
        old_mol = NULL;
        for (i=to_merge -> multi; i < to_merge -> multi + to_remove -> multi; i++)
        {
          to_merge -> fragments[i] = to_remove -> fragments[i - to_merge -> multi];
        }
        atomd_id_save = g_malloc0(to_merge -> mol -> natoms*sizeof*atomd_id_save);
        for (i=0; i<to_merge -> mol -> natoms; i++)
        {
          atomd_id_save[i] = g_malloc0((to_merge -> multi+to_remove -> multi)*sizeof*atomd_id_save[i]);
          for (j=0; j<to_merge -> multi; j++)
          {
            atomd_id_save[i][j].a = to_merge -> atoms_id[i][j].a;
            atomd_id_save[i][j].b = to_merge -> atoms_id[i][j].b;
          }
          for (j=to_merge -> multi; j<to_merge -> multi+to_remove -> multi; j++)
          {
            atomd_id_save[i][j].a = -1;
            atomd_id_save[i][j].b = -1;
          }
        }
        to_merge -> multi += to_remove -> multi;
        g_free (to_merge -> atoms_id);
        to_merge -> atoms_id = g_malloc0(to_merge -> mol -> natoms*sizeof*to_merge -> atoms_id);
        for (i=0; i<to_merge -> mol -> natoms; i++)
        {
          to_merge -> atoms_id[i] = g_malloc0(to_merge -> multi*sizeof*to_merge -> atoms_id[i]);
          for (j=0; j<to_merge -> multi; j++)
          {
            to_merge -> atoms_id[i][j].a = atomd_id_save[i][j].a;
            to_merge -> atoms_id[i][j].b = atomd_id_save[i][j].b;
          }
        }
        g_free (atomd_id_save);
        tmp_fat = get_active_atom (to_merge -> id, to_merge -> atoms -1);
        tmp_fat -> next = new_atom_to_merge (to_merge -> atoms, to_remove);
        to_merge -> atoms ++;
        tmp_fat = to_merge -> first_atom;
        for (i=0; i < to_merge -> atoms-1; i++)
        {
          prepare_atoms_to_merge (tmp_fat, to_merge, to_remove);
          if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
        }
        merge_all_atoms_to_mol (to_merge, to_merge -> multi - to_remove -> multi);
        tmp_fat = get_active_atom (to_merge -> id, to_merge -> atoms-2);
        g_free (tmp_fat -> next);
        tmp_fat -> next = NULL;
        to_merge -> atoms --;

        if (to_remove == tmp_field -> first_molecule)
        {
          tmp_field -> first_molecule = to_remove -> next;
          tmp_field -> first_molecule -> prev = NULL;
        }
        else
        {
          if (to_remove -> next != NULL)
          {
            tmp_fmol = to_remove -> prev;
            tmp_fmol -> next = to_remove -> next;
            to_remove -> next -> prev = tmp_fmol;
          }
          else
          {
            to_remove = to_remove -> prev;
            to_remove -> next = NULL;
          }
        }

        tmp_field -> molecules --;
        clean_up_molecules_info (TRUE);
      }
      break;
    default:
      field_unselect_all ();
      done = TRUE;
      break;
  }
  if (done) destroy_this_dialog (rmol);
}

/*!
  \fn G_MODULE_EXPORT void remove_molecule_from_field (GSimpleAction * action, GVariant * parameter, gpointer data)

  \brief remove molecule from force field - creating the dialog

  \param action the GAction sending the signal
  \param parameter GVariant parameter of the GAction, if any
  \param data the associated data pointer
*/
G_MODULE_EXPORT void remove_molecule_from_field (GSimpleAction * action, GVariant * parameter, gpointer data)
{
  to_remove = (field_molecule *) data;
  int i, j;
  field_object = 0;
  gchar * str = g_strdup_printf (_("Select the molecule to merge molecule \"%s\" with"), to_remove -> name);
  GtkWidget * rmol = dialogmodal (str, GTK_WINDOW(field_assistant));
  g_free (str);
  gtk_dialog_add_button (GTK_DIALOG(rmol), _("Apply"), GTK_RESPONSE_APPLY);

  GtkWidget * remove_tree =  NULL;
  GtkTreeIter iter;

  gchar * mol_title[4] = {i18n("Id."), i18n("Name"), i18n("Multiplicity"), i18n("Viz.3D & Merge with")};
  gchar * ctype[4]={"text", "text", "text", "active"};
  GType col_type[4] = {G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN};

  active_col = 3;
  a_mol = 0;
  new_mol = allocint (1);
  GtkTreeStore * remove_model = gtk_tree_store_newv (4, col_type);
  remove_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(remove_model));
  for (i=0; i<4; i++)
  {
    if (i < 3)
    {
      remove_renderer[i] = gtk_cell_renderer_text_new ();
    }
    else
    {
      remove_renderer[i] = gtk_cell_renderer_toggle_new ();
      gtk_cell_renderer_toggle_set_radio (GTK_CELL_RENDERER_TOGGLE(remove_renderer[i]), TRUE);
      g_signal_connect (G_OBJECT(remove_renderer[i]), "toggled", G_CALLBACK(select_mol), & remove_model);
    }
    remove_col[i] = gtk_tree_view_column_new_with_attributes (_(mol_title[i]), remove_renderer[i], ctype[i], i, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(remove_tree), remove_col[i]);
    if (i < 3)
    {
      gtk_tree_view_column_set_cell_data_func (remove_col[i], remove_renderer[i], molecule_set_color, NULL, NULL);
    }
    else
    {
      gtk_tree_view_column_set_cell_data_func (remove_col[i], remove_renderer[i], set_sensitive_mol, NULL, NULL);
    }
  }
  // fill model
  tmp_fmol = tmp_field -> first_molecule;
  j = 0;
  for (i=0; i<tmp_field -> molecules; i++)
  {
    if (tmp_fmol != to_remove && tmp_fmol -> mol -> id == to_remove -> mol -> id)
    {
      j ++;
      gtk_tree_store_append (remove_model, & iter, NULL);
      gtk_tree_store_set (remove_model, & iter, 0, i+1,
                                                1, tmp_fmol -> name,
                                                2, tmp_fmol -> multi,
                                                3, 0, -1);
    }
    if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
  }
  num_field_objects = tmp_field -> molecules;
  g_object_unref (remove_model);
  gtk_tree_view_expand_all (GTK_TREE_VIEW(remove_tree));

  i = ((j+1)*40 < 500) ? (j+1)*40 : 500;
  GtkWidget * scrollsets = create_scroll (dialog_get_content_area (rmol), 375, i, GTK_SHADOW_ETCHED_IN);
  add_container_child (CONTAINER_SCR, scrollsets, remove_tree);
  remove_label = gtk_label_new (remove_text(-1, -1, to_remove -> name));
  gtk_label_set_use_markup (GTK_LABEL(remove_label), TRUE);
  add_box_child_start (GTK_ORIENTATION_VERTICAL, dialog_get_content_area (rmol), remove_label, FALSE, FALSE, 0);
  run_this_gtk_dialog (rmol, G_CALLBACK(run_remove_molecule_from_field), NULL);
}
