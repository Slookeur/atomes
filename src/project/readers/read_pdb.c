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
* @file read_pdb.c
* @short Functions to read PDB files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_pdb.c'
*
* Contains:
*

 - The functions to read PDB files

*
* List of functions:

  int pdb_get_atoms_data (int linec);
  int open_pdb_file (int linec);

  double get_z_from_pdb_name (char * name);

*/

#include "global.h"
#include "glview.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "bind.h"
#include "readers.h"
#include <ctype.h>
#ifdef OPENMP
#  include <omp.h>
#endif

/*!
  \fn double get_z_from_pdb_name (char * name)

  \brief get Z from the PDB atom string

  \param name the string from the PDB file
*/
double get_z_from_pdb_name (char * name)
{
  if (strlen(name) == 2) name[1] = tolower ((unsigned char)name[1]);
  return get_z_from_periodic_table (name);
}

/*!
  \fn int pdb_get_atoms_data (int linec)

  \brief get the atomic data from the PDB file

  \param linec Total number of lines
*/
int pdb_get_atoms_data (int linec)
{
  typedef struct pdb_atom pdb_atom;
  struct pdb_atom
  {
    int id;
    int sp;
    double nz;
    double x, y, z;
    pdb_atom * prev;
    pdb_atom * next;
  };
#ifdef OPENMP
  int h, i, j, k, l;
  int res;
  int numth = omp_get_max_threads ();
  pdb_atom ** first_at = g_malloc0(numth*sizeof*first_at);
  pdb_atom * other_at = NULL;
  gchar * saved_line;
  gboolean add_spec;
  h = 0;
  res = 1;
  #pragma omp parallel for num_threads(numth) private(i,j,k,l,this_line,saved_line,this_word,other_at,add_spec) shared(h,this_reader,res,coord_line,first_at)
  for (i=0; i<linec; i++)
  {
    if (! res) goto ends;
    this_line = g_strdup_printf ("%s", coord_line[i]);
    this_word = strtok_r (this_line, " ", & saved_line);
    if (this_word)
    {
      if (g_strcmp0(this_word, "HETATM") == 0 || g_strcmp0(this_word, "ATOM") == 0)
      {
        h ++;
        j = omp_get_thread_num();
        if (! first_at[j])
        {
          first_at[j] = g_malloc0(sizeof*first_at[j]);
          other_at = g_malloc0(sizeof*other_at);
          other_at = first_at[j];
        }
        else
        {
          other_at -> next = g_malloc0(sizeof*other_at -> next);
          other_at -> prev = g_malloc0(sizeof*other_at -> prev);
          other_at -> next -> prev = other_at;
          other_at = other_at -> next;
        }
        for (k=0; k<13; k++)
        {
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            add_reader_info (g_strdup_printf ("Wrong file format - record <b>%d</b> on line <b>%d</b> is corrupted !", k+2, i+1), 0);
            res = 0;
            goto ends;
          }
          if (k == 8) other_at -> x = string_to_double ((gpointer)this_word);
          if (k == 9) other_at -> y = string_to_double ((gpointer)this_word);
          if (k == 10) other_at -> z = string_to_double ((gpointer)this_word);
          if (k == 12)
          {
            other_at -> nz = get_z_from_pdb_name (this_word);
            add_spec = TRUE;
            #pragma omp critical
            if (other_at -> nz)
            {
              if (this_reader -> z)
              {
                for (l=0; l<this_reader -> nspec; l++)
                {
                  if (active_chem -> chem_prop[CHEM_Z][l] == other_at -> nz)
                  {
                    other_at -> sp = l;
                    if (! i) this_reader -> nsps[l] ++;
                    add_spec = FALSE;
                    break;
                  }
                }
              }
              else
              {
                this_reader -> z = allocdouble (1);
                this_reader -> nsps = allocint (1);
              }
              if (add_spec)
              {
                if (this_reader -> nspec)
                {
                  this_reader -> z = g_realloc (this_reader -> z, (this_reader -> nspec+1)*sizeof*this_reader -> z);
                  this_reader -> nsps = g_realloc (this_reader -> nsps, (this_reader -> nspec+1)*sizeof*this_reader -> nsps);
                }
                other_at -> sp = this_reader -> nspec;
                this_reader -> nsps[this_reader -> nspec] ++;
                this_reader -> nspec ++;
              }
            }
          }
        }
      }
    }
    ends:;
  }
  if (! res)
  {
    g_free (first_at);
    g_free (other_at);
    return 0;
  }
  active_project -> natomes = h;
  active_project -> steps = 1;
  active_project -> nspec = this_reader -> nspec;
  if (active_project -> natomes) return 0;
  alloc_proj_data (active_project, 1);
  active_project_changed (activep);
  j = 0;
  for (i=0; i<numth; i++)
  {
    other_at = first_at[i];
    while (other_at)
    {
      active_project -> atoms[0][j].id = j;
      active_project -> atoms[0][j].sp = other_at -> sp;
      active_project -> atoms[0][j].x = other_at -> x;
      active_project -> atoms[0][j].y = other_at -> y;
      active_project -> atoms[0][j].z = other_at -> z;
      j ++;
      other_at = other_at -> next;
      g_free (other_at -> prev);
    }
    // Get back results
  }
#else

#endif
  return active_project -> natomes;
}

/*!
  \fn int open_pdb_file (int linec)

  \brief open PDB file

  \param linec Number of lines in the file
*/
int open_pdb_file (int linec)
{
  if (! pdb_get_atoms_data (linec)) return 2;
  return 0;
}
