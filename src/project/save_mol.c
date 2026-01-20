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
* @file save_mol.c
* @short Functions to save molecules information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_mol.c'
*
* Contains:
*

 - The functions to save molecules information in the atomes project file format

*
* List of functions:

  int save_atom_m (FILE * fp, project * this_proj, int s, int a);
  int save_this_mol (FILE * fp, project * this_proj, molecule * tmp);
  int save_mol (FILE * fp, project * this_proj);

*/

#include "global.h"
#include "project.h"

/*molecule {
  int id;                                 // Molecule id number
  int md;                                 // MD step
  int multiplicity;                       // Multiplicity
  int * fragments;                        // Fragments list
  int natoms;                             // Number of atoms
  int nspec;                              // Number of chemical species
  int * species;                          // Number of atom by species
  atom ** atoms;                   // The list of all atoms
  int nbonds;                             // Number of chemical bonds
  int ** pbonds;                          // Number of chemical bonds by geometries
  int nangles;                            // Number of bond angles
  int *** pangles;                        // Number of bond angles by geometries
  int ** lgeo;                            // list of coordination spheres (by species)
  molecule * next;
  molecule * prev;
};*/

/*!
  \fn int save_atom_m (FILE * fp, project * this_proj, int s, int a)

  \brief save atom data to file

  \param fp the file pointer
  \param this_proj the target project
  \param s the MD step
  \param a the atom number
*/
int save_atom_m (FILE * fp, project * this_proj, int s, int a)
{
  if (fwrite (& this_proj -> atoms[s][a].coord[2], sizeof(int), 1, fp) != 1) return ERROR_RW;
  if (fwrite (& this_proj -> atoms[s][a].coord[3], sizeof(int), 1, fp) != 1) return ERROR_RW;
  return OK;
}

/*!
  \fn int save_this_mol (FILE * fp, project * this_proj, molecule * tmp)

  \brief save this molecule data to file

  \param fp the file pointer
  \param this_proj the target project
  \param tmp the molecule that contains the data
*/
int save_this_mol (FILE * fp, project * this_proj, molecule * tmp)
{
  if (fwrite (& tmp -> id, sizeof(int), 1, fp) != 1) return 0;
  if (fwrite (& tmp -> md, sizeof(int), 1, fp) != 1) return 0;
  if (fwrite (& tmp -> multiplicity, sizeof(int), 1, fp) != 1) return 0;
  if (fwrite (tmp -> fragments, sizeof(int), tmp -> multiplicity, fp) != tmp -> multiplicity) return 0;
  if (fwrite (& tmp -> natoms, sizeof(int), 1, fp) != 1) return 0;
  if (fwrite (& tmp -> nspec, sizeof(int), 1, fp) != 1) return 0;
  if (fwrite (tmp -> species, sizeof(int), this_proj -> nspec, fp) != this_proj -> nspec) return 0;
  return 1;
}

/*!
  \fn int save_mol (FILE * fp, project * this_proj)

  \brief save molecule information to file

  \param fp the file pointer
  \param this_proj the target project
*/
int save_mol (FILE * fp, project * this_proj)
{
  int i, j;
  i = 0;
  if (! this_proj -> modelfc)
  {
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_MOL;
    return OK;
  }
  i = 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_MOL;
  for (i=1; i<4; i++)
  {
    if (fwrite (& this_proj -> coord -> totcoord[i], sizeof(int), 1, fp) != 1) return ERROR_MOL;
  }
  if (fwrite (this_proj -> modelfc -> mol_by_step, sizeof(int), this_proj -> steps, fp) != this_proj -> steps) return ERROR_MOL;
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j<this_proj -> modelfc -> mol_by_step[i]; j++)
    {
      if (! save_this_mol (fp, this_proj, & this_proj -> modelfc -> mols[i][j])) return ERROR_MOL;
    }
  }
  for (i=0; i<this_proj -> steps; i++)
  {
    for (j=0; j< this_proj -> natomes; j++)
    {
      if (save_atom_m (fp, this_proj, i, j) != OK) return ERROR_MOL;
    }
  }
  return OK;
}
