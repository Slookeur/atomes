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
* @file cp2k_files.c
* @short Functions to handle CP2K basis sets and pseudopotentials
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'cp2k_files.c'
*
* Contains:
*

 - The functions to handle CP2K basis sets and pseudopotentials

*
* List of functions:

  int cp2k_is_basis_in_database (int sp);
  int cp2k_is_pseudo_in_database (int sp);
  int prepare_cp2k_basis_pseudo_list (int sp, int * sp_id, int n_basis_pseudo, char * bp_elem[n_basis_pseudo]);

  gchar * get_nth_elem (int sp, int id, int obj);
  gchar * get_nth_key (int sp, int id, int obj);

  G_MODULE_EXPORT void changed_basis_pseudo_box (GtkComboBox * box, gpointer data);

  GtkWidget * prepare_basis_combo (int sp, int obj);

*/

#include "global.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"
#include "calc.h"
#include "cp2k.h"

extern GtkWidget * cp2k_spec_combo;

/*!
  \fn gchar * get_nth_elem (int sp, int id, int obj)

  \brief retrieve basis or pseudopotential name string for element from global list

  \param sp the chemical species
  \param id the list element id of 'sp' to find
  \param obj 0 = basis sets, 1 = pseudopotentials
*/
gchar * get_nth_elem (int sp, int id, int obj)
{
  int i, j;
  j = -1;
  if (obj)
  {
    for (i=0; i<N_POTS; i++)
    {
      if (g_strcmp0 (pot_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", cp2k_pot[i]);
        }
      }
    }
  }
  else
  {
    for (i=0; i<N_MOLOPT; i++)
    {
      if (g_strcmp0 (molopt_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", cp2k_molopt[i]);
        }
      }
    }
    for (i=0; i<N_GTH; i++)
    {
      if (g_strcmp0 (gth_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", cp2k_gth[i]);
        }
      }
    }
    for (i=0; i<N_BASIS; i++)
    {
      if (g_strcmp0 (basis_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", cp2k_basis[i]);
        }
      }
    }
  }
  return NULL;
}

/*!
  \fn gchar * get_nth_key (int sp, int id, int obj)

  \brief retrieve basis or pseudopotential key string for element from global list

  \param sp the chemical species
  \param id the list element id of 'sp' to find
  \param obj 0 = basis sets, 1 = pseudopotentials
*/
gchar * get_nth_key (int sp, int id, int obj)
{
  int i, j;
  j = -1;
  if (obj)
  {
    for (i=0; i<N_POTS; i++)
    {
      if (g_strcmp0 (pot_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", pot_keys[i]);
        }
      }
    }
  }
  else
  {
    for (i=0; i<N_MOLOPT; i++)
    {
      if (g_strcmp0 (molopt_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", molopt_keys[i]);
        }
      }
    }
    for (i=0; i<N_GTH; i++)
    {
      if (g_strcmp0 (gth_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", gth_keys[i]);
        }
      }
    }
    for (i=0; i<N_BASIS; i++)
    {
      if (g_strcmp0 (basis_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
      {
        j++;
        if (j == id)
        {
          return g_strdup_printf ("%s", basis_keys[i]);
        }
      }
    }
  }
  return NULL;
}

/*!
  \fn int cp2k_is_basis_in_database (int sp)

  \brief does this chemical species have basis set(s) in store ?

  \param sp the chemical species
*/
int cp2k_is_basis_in_database (int sp)
{
  int i;
  for (i=0; i<N_MOLOPT; i++)
  {
    if (g_strcmp0 (molopt_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0) return 0;
  }
  for (i=0; i<N_GTH; i++)
  {
    if (g_strcmp0 (gth_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0) return 0;
  }
  for (i=0; i<N_BASIS; i++)
  {
    if (g_strcmp0 (basis_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0) return 0;
  }
  return -1;
}

/*!
  \fn int cp2k_is_pseudo_in_database (int sp)

  \brief does this chemical species have pseudopotential(s) in store ?

  \param sp the chemical species
*/
int cp2k_is_pseudo_in_database (int sp)
{
  int i;
  for (i=0; i<N_POTS; i++)
  {
    if (g_strcmp0 (pot_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0) return 0;
  }
  return -1;
}

/*!
  \fn int prepare_cp2k_basis_pseudo_list (int sp, int * sp_id, int n_basis_pseudo, char * bp_elem[n_basis_pseudo])

  \brief find matching chemical element in basis / pseudo database

  \param sp the chemical species
  \param sp_id the list of entries to save
  \param n_basis_pseudo the number of element(s) to check
  \param bp_elem the string list to check
*/
int prepare_cp2k_basis_pseudo_list (int sp, int * sp_id, int n_basis_pseudo, char * bp_elem[n_basis_pseudo])
{
  int i, j;
  j = 0;
  for (i=0; i<n_basis_pseudo; i++)
  {
    if (g_strcmp0 (bp_elem[i], exact_name(qm_proj -> chemistry -> label[sp])) == 0)
    {
      j ++;
      sp_id[j-1] = i;
    }
  }
  return j;
}

/*!
  \fn G_MODULE_EXPORT void changed_basis_pseudo_box (GtkComboBox * box, gpointer data)

  \brief change basis or pseudopotential

  \param box the GtkComboBox sending the signal
  \param data the associated data pointer
*/
G_MODULE_EXPORT void changed_basis_pseudo_box (GtkComboBox * box, gpointer data)
{
  int i, j, k;
  i = combo_get_active (cp2k_spec_combo);
  j = GPOINTER_TO_INT(data);
  k = combo_get_active ((GtkWidget *)box);
  if (k > -1)
  {
    tmp_cp2k -> spec_data[i][j] = k;
    if (tmp_cp2k -> spec_files[i][j] != NULL)
    {
      g_free (tmp_cp2k -> spec_files[i][j]);
      tmp_cp2k -> spec_files[i][j] = NULL;
    }
  }
}

/*!
  \fn GtkWidget * prepare_basis_combo (int sp, int obj)

  \brief prepare basis set / pseudopotential combo box

  \param sp the chemical species
  \param obj 0 = basis, 1 = pseudopotentials
*/
GtkWidget * prepare_basis_combo (int sp, int obj)
{
  int i, j;
  GtkWidget * widg = create_combo ();
  int * sp_id = g_malloc0 (100*sizeof*sp_id);
  if (obj)
  {
    i = prepare_cp2k_basis_pseudo_list (sp, sp_id, N_POTS, pot_elem);
    if (i > 0)
    {
      for (j=0; j<i; j++)
      {
        combo_text_append (widg, pot_keys[sp_id[j]]);
      }
    }
  }
  else
  {
    i = prepare_cp2k_basis_pseudo_list (sp, sp_id, N_MOLOPT, molopt_elem);
    if (i > 0)
    {
      for (j=0; j<i; j++)
      {
        combo_text_append (widg, molopt_keys[sp_id[j]]);
      }
    }
    i = prepare_cp2k_basis_pseudo_list (sp, sp_id, N_GTH, gth_elem);
    if (i > 0)
    {
      for (j=0; j<i; j++)
      {
        combo_text_append (widg, gth_keys[sp_id[j]]);
      }
    }
    i = prepare_cp2k_basis_pseudo_list (sp, sp_id, N_BASIS, basis_elem);
    if (i > 0)
    {
      for (j=0; j<i; j++)
      {
        combo_text_append (widg, basis_keys[sp_id[j]]);
      }
    }
  }
  if (tmp_cp2k -> spec_files[sp][obj] == NULL)
  {
    combo_set_active (widg, tmp_cp2k -> spec_data[sp][obj]);
  }
  else
  {
    widget_set_sensitive (widg, FALSE);
  }
  g_signal_connect (G_OBJECT (widg), "changed", G_CALLBACK(changed_basis_pseudo_box), GINT_TO_POINTER(obj));
  return widg;
}
