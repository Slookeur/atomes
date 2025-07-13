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
* @file atom_insert.c
* @short Functions to insert bond(s) in a project
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_insert.c'
*
* Contains:
*

 - The functions to insert bond(s) in a project

*
* List of functions:

  void add_bonds_to_project (project * this_proj, int removed, int nbd, int ** new_bond_list);
  void add_bonds_to_list (int ** new_bond_list, int nat, int nbd, atomic_object * object);
  void prepare_to_instert (gchar * key, project * this_proj, atom_search * asearch, gboolean visible);

  G_MODULE_EXPORT void set_atoms_to_insert (GtkComboBox * box, gpointer data);

*/

#include "atom_edit.h"

/*!
  \fn void add_bonds_to_project (project * this_proj, int removed, int nbd, int ** new_bond_list)

  \brief add bond list to project bond list

  \param this_proj the target project
  \param removed the number of atom(s) removed
  \param nbd the number of bond(s) to add
  \param new_bond_list the bond list to add
*/
void add_bonds_to_project (project * this_proj, int removed, int nbd, int ** new_bond_list)
{
  int i, j;
  if (nbd)
  {
    i = this_proj -> modelgl -> bonds[0][0];
    this_proj -> modelgl -> bondid[0][0] = g_realloc (this_proj -> modelgl -> bondid[0][0], (i+nbd)*sizeof*this_proj -> modelgl -> bondid[0][0]);
    for (j=0; j<nbd; j++)
    {
      this_proj -> modelgl -> bondid[0][0][j+i] = allocint (2);
      this_proj -> modelgl -> bondid[0][0][j+i][0] = new_bond_list[j][0] + this_proj -> natomes - removed;
      this_proj -> modelgl -> bondid[0][0][j+i][1] = new_bond_list[j][1] + this_proj -> natomes - removed;
    }
    this_proj -> modelgl -> bonds[0][0] += nbd;
    this_proj -> modelgl -> allbonds[0] += nbd;
  }
}

/*!
  \fn void add_bonds_to_list (int ** new_bond_list, int nat, int nbd, atomic_object * object)

  \brief add object bond(s) list to overall bond(s) list

  \param new_bond_list the bond list in the model
  \param nat the number of atom(s) in the model
  \param nbd the number of bond(s) in the model
  \param object the target insert object
*/
void add_bonds_to_list (int ** new_bond_list, int nat, int nbd, atomic_object * object)
{
  int i;
  if (object -> bonds)
  {
    for (i=0; i<object -> bonds; i++)
    {
      new_bond_list[i+nbd][0] = object -> ibonds[i][0] + nat;
      new_bond_list[i+nbd][1] = object -> ibonds[i][1] + nat;
    }
  }
}

/*!
  \fn void prepare_to_instert (gchar * key, project * this_proj, atom_search * asearch, gboolean visible)

  \brief prepare to insert something

  \param key the string describing the object to insert
  \param this_proj the target project
  \param asearch the target atom search
  \param visible is the model edition window visible
*/
void prepare_to_instert (gchar * key, project * this_proj, atom_search * asearch, gboolean visible)
{
  int i = get_selected_object_id (visible, this_proj -> id, key, asearch);
  if (i == FROM_PROJECT || i == FROM_DATA || i > 0) to_insert_in_project (i, -1, this_proj, asearch, visible);
}

/*!
  \fn G_MODULE_EXPORT void set_atoms_to_insert (GtkComboBox * box, gpointer data)

  \brief change the object to insert

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void set_atoms_to_insert (GtkComboBox * box, gpointer data)
{
  GValue val = {0, };
  GtkTreeModel * cmodel = gtk_combo_box_get_model (box);
  GtkTreeIter iter;
  if (gtk_combo_box_get_active_iter (box, & iter))
  {
    gtk_tree_model_get_value (cmodel, & iter, 0, & val);
    tint * dat = (tint *)data;
    gchar * str = g_strdup_printf ("%s", (char *)g_value_get_string (& val));
    project * this_proj = get_project_by_id (dat -> a);
    prepare_to_instert (str, this_proj, this_proj -> modelgl -> search_widg[dat -> c], TRUE);
    g_free (str);
  }
  combo_set_active ((GtkWidget *)box, 0);
}
