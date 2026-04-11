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
* @file read_mol.c
* @short Functions to read molecule(s) data in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_mol.c'
*
* Contains:
*

 - The functions to read molecule(s) data in the atomes project file format

*
* List of functions:

  int read_atom_m (FILE * fp, int s, int a);
  int read_this_mol (FILE * fp, molecule * tmp);
  int read_mol (FILE * fp);

*/

#include "global.h"
#include "project.h"
#include "initcoord.h"
#include "submenus.h"

extern void duplicate_molecule (molecule * new_mol, molecule * old_mol);

/*!
  \fn int read_atom_m (FILE * fp, int s, int a)

  \brief read atom fragment and molecule data

  \param fp the file pointer
  \param s the MD step
  \param a the atom number
*/
int read_atom_m (FILE * fp, int s, int a)
{
  if (fread (& active_project -> atoms[s][a].coord[2], sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fread (& active_project -> atoms[s][a].coord[3], sizeof(int), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int read_this_mol (FILE * fp, molecule * tmp)

  \brief read molecule data

  \param fp the file pointer
  \param tmp the molecule to store the data
*/
int read_this_mol (FILE * fp, molecule * tmp)
{
  if (fread (& tmp -> id, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (fread (& tmp -> md, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (fread (& tmp -> multiplicity, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (! tmp -> multiplicity) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  tmp -> fragments = allocint (tmp -> multiplicity);
  if (fread (tmp -> fragments, sizeof(int), tmp -> multiplicity, fp) != tmp -> multiplicity) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (fread (& tmp -> natoms, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (fread (& tmp -> nspec, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  tmp -> species = allocint (active_project -> nspec);
  if (fread (tmp -> species, sizeof(int), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  return OK;
}

/*!
  \fn int read_mol (FILE * fp)

  \brief read molecule(s) information from file

  \param fp the file pointer
*/
int read_mol (FILE * fp)
{
  int i, j;
  if (fread (& i, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
  if (! i) return OK;
  active_project -> modelfc = g_malloc0(sizeof*active_project -> modelfc);
  for (i=1; i<4; i++)
  {
    if (fread (& j, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
    if (i == 1 && j != active_coord -> totcoord[1]) return signal_error (__FILE__, __func__, __LINE__, ERROR_MOL);
    if (i > 1) active_coord -> totcoord[i] = j;
  }
  active_project -> modelfc -> mol_by_step = allocint (active_project -> steps);
  if (fread (active_project -> modelfc -> mol_by_step, sizeof(int), active_project -> steps, fp) != active_project -> steps) return ERROR_MOL;
  active_project -> modelfc -> mols = g_malloc0(active_project -> steps*sizeof*active_project -> modelfc -> mols);
  for (i=0; i<active_project -> steps; i++)
  {
    active_project -> modelfc -> mols[i] = g_malloc0(active_project -> modelfc -> mol_by_step[i]*sizeof*active_project -> modelfc -> mols[i]);
  }

  molecule * tmp = g_malloc0(sizeof*tmp);
  for (i=0; i<active_project -> steps; i++)
  {
    for (j=0; j<active_project -> modelfc -> mol_by_step[i]; j++)
    {
      if (read_this_mol(fp, & active_project -> modelfc -> mols[i][j]) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_MOL;
      }
    }
  }
  for (i=0; i<active_project -> steps; i++)
  {
    for (j=0; j< active_project -> natomes; j++)
    {
      if (read_atom_m (fp, i, j) != OK)
      {
        update_error_trace (__FILE__, __func__, __LINE__-2);
        return ERROR_MOL;
      }
    }
  }

#ifdef GTK3
  // Now recreate menus
  for (i=2; i<4; i++)
  {
    init_opengl_coords (i, active_coord -> totcoord[i], 1);
    init_menu_fragmol_ (& i);
    active_glwin -> adv_bonding[i-2] = TRUE;
    for (j=2; j<4; j++)
    {
      detach_frag_mol_menu (active_glwin, i, j);
    }

    active_glwin -> ogl_coord[i+1] = destroy_this_widget (active_glwin -> ogl_coord[i+1]);
    if (i == 2) active_glwin -> ogl_coord[3] = menu_item_new_with_submenu ("Fragment(s)", active_project -> coord -> totcoord[2], add_menu_coord (active_glwin, 0, 2));
    if (i == 3) active_glwin -> ogl_coord[4] = menu_item_new_with_submenu ("Molecule(s)", active_project -> coord -> totcoord[2], add_menu_coord (active_glwin, 0, 3));
    GtkWidget * cmenu = gtk_menu_item_get_submenu (GTK_MENU_ITEM (active_glwin -> ogl_coord[0]));
    gtk_menu_shell_insert (GTK_MENU_SHELL(cmenu), active_glwin -> ogl_coord[i+1], i+2);
  }
#endif
  return OK;
}
