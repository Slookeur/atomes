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
* @file read_c3d.c
* @short Functions to read Chem3d atomic coordinates
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_c3d.c'
*
* Contains:
*

 - The functions to read Chem3d atomic coordinates

*
* List of functions:

  int c3d_get_atom_coordinates ();
  int open_c3d_file (int linec);

*/

#include "global.h"
#include "glview.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "bind.h"
#include "readers.h"
#ifdef OPENMP
#  include <omp.h>
#endif

extern void check_for_species (double v, int ato);

/*!
  \fn int c3d_get_atom_coordinates ()

  \brief get the atomic coordinates from the C3D file
*/
int c3d_get_atom_coordinates ()
{
  int i, j, k;
  double v;
  gchar * lia[5] = {"a", "b", "c", "d", "e"};
  this_reader -> nspec = 0;
  active_project -> steps = this_reader -> steps;
  active_project -> natomes = this_reader -> natomes;
  allocatoms (active_project);
  this_reader -> z = allocdouble (1);
  this_reader -> nsps = allocint (1);
#ifdef OPENMP
  int v_dummy;
  int res;
  int numth = omp_get_max_threads ();
  gboolean doatoms =  FALSE;
  gchar * saved_line;
  if (this_reader -> steps < numth)
  {
    if (numth >= 2*(this_reader -> steps-1))
    {
      doatoms = TRUE;
    }
    else
    {
      numth = this_reader -> steps;
    }
  }

  if (doatoms)
  {
    // OpenMP on atoms
    res = 0;
    for (i=0; i<this_reader -> steps; i++)
    {
      k = i*(this_reader -> natomes + 1) + 1;
      #pragma omp parallel for num_threads(numth) private(j,v,v_dummy,this_line,saved_line,this_word) shared(i,k,lia,coord_line,this_reader,active_project,res)
      for (j=0; j<this_reader -> natomes; j++)
      {
        if (res == 2) goto enda;
        this_line = g_strdup_printf ("%s", coord_line[k+j]);
        this_word = strtok_r (this_line, " ", & saved_line);
        if (! this_word)
        {
          format_error (i+1, j+1, lia[0], k+j);
          res = 2;
          goto enda;
        }
        v = get_z_from_periodic_table (this_word);
        v_dummy = 0;
        if (! v)
        {
          #pragma omp critical
          v_dummy = set_v_dummy (this_word);
        }
        if (v || v_dummy)
        {
          if (! i)
          {
            v = v + v_dummy * 0.1;
            #pragma omp critical
            check_for_species (v, j);
          }
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[1], k+j);
            res = 2;
            goto enda;
          }
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[2], k+j);
            res = 2;
            goto enda;
          }
          active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[3], k+j);
            res = 2;
            goto enda;
          }
          active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[4], k+j);
            res = 2;
            goto enda;
          }
          active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
        }
        else
        {
          format_error (i+1, j+1, lia[0], k+j);
          res = 2;
          goto enda;
        }
        g_free (this_line);
        enda:;
      }
      if (res == 2) break;
    }
  }
  else
  {
    res = 0;
    #pragma omp parallel for num_threads(numth) private(i,j,k,v,v_dummy,this_line,saved_line,this_word) shared(lia,coord_line,this_reader,active_project,res)
    for (i=0; i<this_reader -> steps; i++)
    {
      if (res == 2) goto ends;
      k = i*(this_reader -> natomes + 1) + 1;
      for (j=0; j<this_reader -> natomes; j++)
      {
        this_line = g_strdup_printf ("%s", coord_line[k+j]);
        this_word = strtok_r (this_line, " ", & saved_line);
        if (! this_word)
        {
          format_error (i+1, j+1, lia[0], k+j);
          res = 2;
          goto ends;
        }
        v = get_z_from_periodic_table (this_word);
        v_dummy = 0;
        if (! v)
        {
          #pragma omp critical
          v_dummy = set_v_dummy (this_word);
        }
        if (v || v_dummy)
        {
          if (! i)
          {
            v = v + v_dummy * 0.1;
            check_for_species (v, j);
          }
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[1], k+j);
            res = 2;
            goto ends;
          }
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[2], k+j);
            res = 2;
            goto ends;
          }
          active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[3], k+j);
            res = 2;
            goto ends;
          }
          active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lia[4], k+j);
            res = 2;
            goto ends;
          }
          active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
        }
        else
        {
          format_error (i+1, j+1, lia[0], k+j);
          res = 2;
          goto ends;
        }
        g_free (this_line);
      }
      ends:;
    }
  }
  g_free (coord_line);
  if (res == 2) return 2;
#else
  line_node * tmp_line;
  tail = head;
  k = 0;
  for (i=0; i<active_project -> steps; i++)
  {
    tmp_line = tail;
    tail = tail -> next;
    g_free (tmp_line);
    k ++;
    for (j=0; j<active_project -> natomes; j++)
    {
      this_line = g_strdup_printf ("%s", tail -> line);
      this_word = strtok (this_line, " ");
      if (! this_word)
      {
        format_error (i+1, j+1, lia[0], k+j);
        return 2;
      }
      v = get_z_from_periodic_table (this_word);
      if (v)
      {
        if (! i) check_for_species (v, j);
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lia[1], k+j);
          return 2;
        }
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lia[2], k+j);
          return 2;
        }
        active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lia[3], k+j);
          return 2;
        }
        active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lia[4], k+j);
          return 2;
        }
        active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
      }
      else
      {
        format_error (i+1, j+1, lia[0], k+j);
        return 2;
      }
      tmp_line = tail;
      tail = tail -> next;
      g_free (tmp_line);
      k ++;
    }
  }
#endif
  for (i=1; i<active_project -> steps; i++)
  {
    for (j=0; j<active_project -> natomes; j++)
    {
      active_project -> atoms[i][j].sp = active_project -> atoms[0][j].sp;
    }
  }
  return 0;
}

/*!
  \fn int open_c3d_file (int linec)

  \brief open C3D file

  \param linec Number of lines in the file
*/
int open_c3d_file (int linec)
{
  int res;
#ifdef OPENMP
  this_line = g_strdup_printf ("%s", coord_line[0]);
  this_word = strtok (this_line, " ");
  if (! this_word)
  {
    add_reader_info ("Wrong file format - cannot find the number of atoms !", 0);
    add_reader_info ("Wrong file format - first line is corrupted !", 0);
    res = 2;
    goto end;
  }
  this_reader -> natomes = (int)string_to_double ((gpointer)this_word);
  reader_info ("c3d", "Number of atoms", this_reader -> natomes);
  g_free (this_line);
  if (linec%(this_reader -> natomes + 1) != 0)
  {
    res = 2;
  }
  else
  {
    this_reader -> steps = linec / (this_reader -> natomes + 1);
    reader_info ("c3d", "Number of steps", this_reader -> steps);
    res = (this_reader -> steps > 1) ? 2 : c3d_get_atom_coordinates ();
  }
#else
  this_line = g_strdup_printf ("%s", head -> line);
  this_word = strtok (this_line, " ");
  if (! this_word)
  {
    add_reader_info ("Wrong file format - cannot find the number of atoms !", 0);
    add_reader_info ("Wrong file format - the first line is corrupted !", 0);
    res = 2;
    goto end;
  }
  this_reader -> natomes = (int)string_to_double ((gpointer)this_word);
  reader_info ("chem3d", "Number of atoms", this_reader -> natomes);
  g_free (this_line);
  if (linec%(this_reader -> natomes + 1) != 0)
  {
    res = 2;
  }
  else
  {
    this_reader -> steps = linec / (this_reader -> natomes + 1);
    reader_info ("chem3d", "Number of steps", this_reader -> steps);
    res = (this_reader -> steps > 1) ? 2 : c3d_get_atom_coordinates ();
  }
#endif
  end:
  return res;
}
