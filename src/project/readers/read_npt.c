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
* @file read_npt.c
* @short Functions to read NPT data
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_npt.c'
*
* Contains:
*

 - The functions to read NPT data

*
* List of functions:

  int cell_get_lattice (int format);
  int open_cell_file (int format, gchar * filename);

  void add_cell_info (gchar * info);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "project.h"
#include <ctype.h>
#include "cbuild_edit.h"
#include "readers.h"

#ifdef G_OS_WIN32
  typedef intptr_t ssize_t;
  extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);
#endif

FILE * cellp;

typedef struct cell_file cell_file;
struct cell_file
{
  cell_info lattice;
  gchar * info;
};

cell_file * this_cell;

/*!
  \fn void add_cell_info (gchar * info)

  \brief append information message to the cell information

  \param info the cell information message
*/
void add_cell_info (gchar * info)
{
  this_cell -> info = (this_cell -> info) ? g_strdup_printf ("%s\n%s", this_cell -> info, info) : g_strdup_printf ("%s", info);
}

/*!
  \fn int cell_get_lattice (int format)

  \brief get the lattice parameters format from the file

  \param format File format
*/
int cell_get_lattice (int format)
{
  size_t length = 0;
  gchar * line = NULL;
  int i, j, k;
  char c;
  i = 0;
  for (c = getc(cellp); c != EOF; c = getc(cellp))  if (c == '\n') i ++;
  j = (format == 0 || format == 2) ? 1 : (format == 1) ? 2 : 3;
  k = i / j;
  i  = i - k * j;
  if (i) return 0;
  if (k != active_project -> steps)
  {
    add_cell_info (g_strdup_printf ("Wrong file format - %d steps in coordinates file", active_project -> steps));
    add_cell_info (g_strdup_printf ("Wrong file format - %d steps found in cell file", k));
    return 0;
  }
  rewind (cellp);
  this_cell -> lattice.box = g_malloc0(k*sizeof*this_cell -> lattice.box);
  for (i=0; i<active_project -> steps; i++)
  {
    if (format < 2)
    {
      for (j=0; j<2; j++)
      {
        if (! j || format == 1)
        {
          getline (& line, & length, cellp);
          this_line = g_strdup_printf ("%s", line);
          this_word = strtok (this_line, " ");
        }
        else
        {
          this_word = strtok (NULL, " ");
        }
        k = 0;
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].param[j][k] = string_to_double ((gpointer)this_word);
        k ++;
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].param[j][k] = string_to_double ((gpointer)this_word);
        k ++;
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].param[j][k] = string_to_double ((gpointer)this_word);
      }
    }
    else
    {
      for (j=0; j<3; j++)
      {
        if (! j || format == 3)
        {
          getline (& line, & length, cellp);
          this_line = g_strdup_printf ("%s", line);
          this_word = strtok (this_line, " ");
        }
        else
        {
          this_word = strtok (NULL, " ");
        }
        k = 0;
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].vect[j][k] = string_to_double ((gpointer)this_word);
        k ++;
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].vect[j][k] = string_to_double ((gpointer)this_word);
        k ++;
        this_word = strtok (NULL, " ");
        if (! this_word)
        {
          add_cell_info (g_strdup_printf ("Wrong file format - error at step %d !", i+1));
          return 0;
        }
        this_cell -> lattice.box[i].vect[j][k] = string_to_double ((gpointer)this_word);
      }
    }
  }
  this_cell -> lattice.npt = FALSE;
  for (i=1; i<active_project -> steps; i++)
  {
    if (format < 2)
    {
      for (j=0; j<2; j++)
      {
        for (k=0; k<3; k++)
        {
          if (this_cell -> lattice.box[i].param[j][k] !=  this_cell -> lattice.box[0].param[j][k])
          {
            this_cell -> lattice.npt = TRUE;
            break;
          }
        }
        if (this_cell -> lattice.npt) break;
      }
      if (this_cell -> lattice.npt) break;
    }
    else
    {
      for (j=0; j<3; j++)
      {
        for (k=0; k<3; k++)
        {
          if (this_cell -> lattice.box[i].vect[j][k] !=  this_cell -> lattice.box[0].vect[j][k])
          {
            this_cell -> lattice.npt = TRUE;
            break;
          }
        }
        if (this_cell -> lattice.npt) break;
      }
      if (this_cell -> lattice.npt) break;
    }
  }
  return 1;
}

/*!
  \fn int open_cell_file (int format, gchar * filename)

  \brief open the file that contains the cell parameters

  \param format File format
  \param filename File name
*/
int open_cell_file (int format, gchar * filename)
{
  int res;
  int i, j, k, l;
  cellp = fopen (filename, "r");
  if (cellp)
  {
    this_cell = g_malloc0(sizeof*this_cell);
    if (cell_get_lattice(format))
    {
      active_cell -> ltype = format/2 + 1;
      active_cell -> pbc = TRUE;
      active_cell -> npt = this_cell -> lattice.npt;
      i = (active_cell -> npt) ? active_project -> steps : 1;
      if (active_cell -> npt)
      {
        g_free (active_cell -> box);
        active_cell -> box = g_malloc0(i*sizeof*active_cell -> box);
        active_box = & active_cell -> box[0];
      }
      for (j=0; j<i; j++)
      {
        if (format < 2)
        {
          for (k=0; k<2; k++)
          {
            for (l=0; l<3; l++)
            {
              active_cell -> box[j].param[k][l] = this_cell -> lattice.box[j].param[k][l];
            }
          }
        }
        else
        {
          for (k=0; k<3; k++)
          {
            for (l=0; l<3; l++)
            {
              active_cell -> box[j].vect[k][l] = this_cell -> lattice.box[j].vect[k][l];
            }
          }
        }
      }
      active_cell -> has_a_box = TRUE;
      active_cell -> crystal = FALSE;
      res = 0;
   }
   else
   {
     // No or wrong cell data found
#ifdef DEBUG
     g_debug ("CELL-NPT:: Impossible to retrieve cell parameters !");
#endif
     res = 2;
    }
  }
  else
  {
    // Cannot open file
    res = 1;
  }
  if (this_cell)
  {
    if (this_cell -> info)
    {
      show_error (this_cell -> info, 0, MainWindow);
    }
    if (this_cell)
    {
      if (this_cell -> info) g_free (this_cell -> info);
      if (this_cell -> lattice.box) g_free (this_cell -> lattice.box);
      g_free (this_cell);
      this_cell = NULL;
    }
  }
  return res;
}
