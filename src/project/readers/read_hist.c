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
* @file read_hist.c
* @short Functions to read DL-POLY history file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_hist.c'
*
* Contains:
*

 - The functions to read DL-POLY history file

*
* List of functions:

  int hist_get_data (int linec);
  int hist_get_content ();
  int open_hist_file (int linec);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "project.h"
#include <ctype.h>
#include "cbuild_edit.h"
#include "readers.h"
#ifdef OPENMP
#  include <omp.h>
#endif

extern void check_for_species (double v, int ato);

/*!
  \fn int hist_get_data (int linec)

  \brief read data from the DL-POLY history file

  \param linec Total number of lines
*/
int hist_get_data (int linec)
{
  int i;
  if (linec < 7) return 0;
#ifdef OPENMP
  this_line = g_strdup_printf ("%s", coord_line[1]);
#else
  line_node * tmp_line;
  tail = head;
  tmp_line = tail;
  tail = tail -> next;
  g_free (tmp_line);
  this_line = g_strdup_printf ("%s", tail -> line);
#endif // OPENMP
  this_word = strtok (this_line, " ");
  this_reader -> traj = (int)string_to_double ((gpointer)this_word);
  this_word = strtok (NULL, " ");
  if (! this_word)
  {
    add_reader_info ("Wrong file format - record <b>ii</b> line is corrupted !", 0);
    return 0;
  }
  this_reader -> lattice.pbc = (int)string_to_double ((gpointer)this_word);
  this_word = strtok (NULL, " ");
  if (! this_word)
  {
    add_reader_info ("Wrong file format - record <b>ii</b> line is corrupted !", 0);
    return 0;
  }
  this_reader -> natomes = (int)string_to_double ((gpointer)this_word);
  i = linec - 2;
  if (i%(4 + (2+this_reader -> traj)*this_reader -> natomes)) return 0;
  this_reader -> steps = i/(4 + (2+this_reader -> traj)*this_reader -> natomes);
  return 1;
}

/*!
  \fn int hist_get_content ()

  \brief read the content of the DL-POLY history file
*/
int hist_get_content ()
{
  gchar * lil[3] = {"ii", "iii", "iv"};
  int i, j, k, l;
  double v;
  this_reader -> nspec = 0;
  active_project -> steps = this_reader -> steps;
  active_project -> natomes = this_reader -> natomes;
  allocatoms (active_project);
  this_reader -> z = allocdouble (1);
  this_reader -> nsps = allocint (1);
  this_reader -> lattice.box = g_malloc0(this_reader -> steps*sizeof*this_reader -> lattice.box);
  int res = 1;
#ifdef OPENMP
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
    for (i=0; i<this_reader -> steps; i++)
    {
      k = 3 + i*(this_reader -> natomes*(2+this_reader -> traj) + 4);
      for (j=0; j<3; j++)
      {
        this_line = g_strdup_printf ("%s", coord_line[k+j]);
        this_word = strtok_r (this_line, " ", & saved_line);
        for (l=0; l<3; l++)
        {
          if (! this_word)
          {
            format_error (i+1, -1, lil[l], k+j);
            return 0;
          }
          this_reader -> lattice.box[i].vect[j][l] = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
        }
      }
      k += 3;
      #pragma omp parallel for num_threads(numth) private(j,l,v,this_line,saved_line,this_word) shared(i,k,lil,coord_line,this_reader,active_project,res)
      for (j=0; j<this_reader -> natomes; j++)
      {
        if (res == 2) goto enda;
        this_line = g_strdup_printf ("%s", coord_line[k+j*(2+this_reader -> traj)]);
        this_word = strtok_r (this_line, " ", & saved_line);
        for (l=0; l<3; l++)
        {
          if (! this_word)
          {
            format_error (i+1, j+1, lil[l], k+j*(2+this_reader -> traj));
            res = 0;
            goto enda;
          }
          if (l < 2) this_word = strtok_r (NULL, " ", & saved_line);
        }
        v = string_to_double ((gpointer)this_word);
        if (v > 0.0)
        {
          if (! i)
          {
            #pragma omp critical
            check_for_species (v, j);
          }
          g_free (this_line);
          this_line = g_strdup_printf ("%s", coord_line[k+j*(2+this_reader -> traj)+1]);
          this_word = strtok_r (this_line, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[0], k+j*(2+this_reader -> traj));
            res = 0;
            goto enda;
          }
          active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[1], k+j*(2+this_reader -> traj));
            res = 0;
            goto enda;
          }
          active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[2], k+j*(2+this_reader -> traj));
            res = 0;
            goto enda;
          }
          active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
        }
        else
        {
          format_error (i+1, j+1, lil[2], k+j*(2+this_reader -> traj));
          res = 0;
          goto enda;
        }
        g_free (this_line);
        enda:;
      }
      if (res == 0) break;
    }
  }
  else
  {
    // OpenMP on MD steps
    #pragma omp parallel for num_threads(numth) private(i,j,k,l,v,this_line,saved_line,this_word) shared(lil,coord_line,this_reader,active_project,res)
    for (i=0; i<this_reader -> steps; i++)
    {
      if (! res) goto ends;
      k = 3 + i*(this_reader -> natomes*(2+this_reader -> traj)+4);
      for (j=0; j<3; j++)
      {
        this_line = g_strdup_printf ("%s", coord_line[k+j]);
        this_word = strtok_r (this_line, " ", & saved_line);
        for (l=0; l<3; l++)
        {
          if (! this_word)
          {
            format_error (i+1, -1, lil[l], k+j);
            res = 0;
            goto ends;
          }
          this_reader -> lattice.box[i].vect[j][l] = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
        }
      }
      k += 3;
      for (j=0; j<this_reader -> natomes; j++)
      {
        this_line = g_strdup_printf ("%s", coord_line[k+j*(2+this_reader -> traj)]);
        this_word = strtok_r (this_line, " ", & saved_line);
        for (l=0; l<3; l++)
        {
          if (! this_word)
          {
            format_error (i+1, j+1, lil[l], k+j*(2+this_reader -> traj));
            res = 0;
            goto ends;
          }
          if (l < 2) this_word = strtok_r (NULL, " ", & saved_line);
        }
        v = string_to_double ((gpointer)this_word);
        if (v > 0.0)
        {
          if (! i) check_for_species (v, j);
          g_free (this_line);
          this_line = g_strdup_printf ("%s", coord_line[k+j*(2+this_reader -> traj)+1]);
          this_word = strtok_r (this_line, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[0], k+j*(2+this_reader -> traj)+1);
            res = 0;
            goto ends;
          }
          active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[1], k+j*(2+this_reader -> traj)+1);
            res = 0;
            goto ends;
          }
          active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
          this_word = strtok_r (NULL, " ", & saved_line);
          if (! this_word)
          {
            format_error (i+1, j+1, lil[2], k+j*(2+this_reader -> traj)+1);
            res = 0;
            goto ends;
          }
          active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
        }
        else
        {
          format_error (i+1, j+1, lil[2], k+j*(2+this_reader -> traj));
          res = 0;
          goto ends;
        }
        g_free (this_line);
      }
      ends:;
    }
  }
#else
  line_node * tmp_line;
  tmp_line = tail;
  tail = tail -> next;
  g_free (tmp_line);
  for (i=0; i<this_reader -> steps; i++)
  {
    tmp_line = tail;
    tail = tail -> next;
    g_free (tmp_line);
    k = 3 + i*(this_reader -> natomes*(2+this_reader -> traj) + 4);
    for (j=0; j<3; j++)
    {
      this_line = g_strdup_printf ("%s", tail -> line);
      this_word = strtok (this_line, " ");
      for (l=0; l<3; l++)
      {
        if (! this_word)
        {
          format_error (i+1, -1, lil[l], k+j);
          return 0;
        }
        this_reader -> lattice.box[i].vect[j][l] = string_to_double ((gpointer)this_word);
        this_word = strtok (NULL, " ");
      }
      tmp_line = tail;
      tail = tail -> next;
      g_free (tmp_line);
    }
    k += 3;
    for (j=0; j<this_reader -> natomes; j++)
    {
      this_line = g_strdup_printf ("%s", tail -> line);
      this_word = strtok (this_line, " ");
      for (l=0; l<3; l++)
      {
        if (! this_word)
        {
          format_error (i+1, j+1, lil[l], k+2*j);
          return 0;
        }
        if (l < 2) this_word = strtok (NULL, " ");
      }
      v = string_to_double ((gpointer)this_word);
      if (v > 0.0)
      {
        if (! i) check_for_species (v, j);
        tmp_line = tail;
        tail = tail -> next;
        g_free (tmp_line);
        this_line = g_strdup_printf ("%s", tail -> line);
        this_word = strtok (this_line, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lil[0], k+2*j+1);
          return 0;
        }
        active_project -> atoms[i][j].x = string_to_double ((gpointer)this_word);
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lil[1], k+2*j+1);
          return 0;
        }
        active_project -> atoms[i][j].y = string_to_double ((gpointer)this_word);
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          format_error (i+1, j+1, lil[2], k+2*j+1);
          return 0;
        }
        active_project -> atoms[i][j].z = string_to_double ((gpointer)this_word);
      }
      else
      {
        format_error (i+1, j+1, lil[l], k+2*j+1);
        return 0;
      }
      tmp_line = tail;
      tail = tail -> next;
      g_free (tmp_line);
    }
  }
#endif
  if (! res) return res;
  gboolean add_spec;
  for (i=0; i<this_reader -> nspec; i++)
  {
    add_spec = FALSE;
    for (j=1; j<120; j++)
    {
      if (fabs(periodic_table_info[j].M - this_reader -> z[i]) < 0.1)
      {
        this_reader -> z[i] = (float)j;
        add_spec = TRUE;
        break;
      }
    }
    if (! add_spec)
    {
      gchar * str = g_strdup_printf ("Cannot find species with a mass of %f !", this_reader -> z[i]);
      add_reader_info (str, 0);
      g_free (str);
      return 0;
    }
  }
  for (i=1; i<active_project -> steps; i++)
  {
    for (j=0; j<active_project -> natomes; j++)
    {
      active_project -> atoms[i][j].sp = active_project -> atoms[0][j].sp;
    }
  }
  this_reader -> lattice.npt = FALSE;
  for (i=1; i<this_reader -> steps; i++)
  {
    for (j=0; j<3; j++)
    {
      for (k=0; k<3; k++)
      {
        if (this_reader -> lattice.box[i].vect[j][k] !=  this_reader -> lattice.box[0].vect[j][k])
        {
          this_reader -> lattice.npt = TRUE;
          break;
        }
      }
      if (this_reader -> lattice.npt) break;
    }
    if (this_reader -> lattice.npt) break;
  }
  return 1;
}

/*!
  \fn int open_hist_file (int linec)

  \brief open DL-POLY history file

  \param linec Number of lines in the file
*/
int open_hist_file (int linec)
{
  int res;
  int i, j, k, l;
  if (hist_get_data(linec))
  {
    reader_info ("hist", "Number of atoms", this_reader -> natomes);
    reader_info ("hist", "Number of steps", this_reader -> steps);
    if (hist_get_content())
    {
      active_cell -> ltype = 2;
      active_cell -> pbc = this_reader -> lattice.pbc;
      active_cell -> npt = this_reader -> lattice.npt;
      i = (active_cell -> npt) ? this_reader -> steps : 1;
      if (active_cell -> npt)
      {
        g_free (active_cell -> box);
        active_cell -> box = g_malloc0(i*sizeof*active_cell -> box);
        active_box = & active_cell -> box[0];
      }
      for (j=0; j<i; j++)
      {
        for (k=0; k<3; k++)
        {
          for (l=0; l<3; l++)
          {
            active_cell -> box[j].vect[k][l] = this_reader -> lattice.box[j].vect[k][l];
          }
        }
      }
      active_cell -> has_a_box = TRUE;
      active_cell -> crystal = FALSE;
      res = 0;
    }
    else
    {
      // Content error
      res = 2;
    }
  }
  else
  {
    // Data error
    res = 2;
  }
  return res;
}
