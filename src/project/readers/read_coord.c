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
* @file read_coord.c
* @short General functions to import atomic coordinates
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_coord.c'
*
* Contains:
*

 - The general functions to import atomic coordinates

*
* List of functions:

  gboolean set_dummy_in_use (gchar * this_word);

  int open_coord_file (gchar * filename, int fti);

  void add_reader_info (gchar * info, int mid);
  void reader_info (gchar * type, gchar * sinf, int val);
  void format_error (int stp, int ato, gchar * mot, int line);
  void check_for_species (double v, int ato);

*/

#include "global.h"
#include "glview.h"
#include "callbacks.h"
#include "interface.h"
#include "project.h"
#include "bind.h"
#include "cbuild_edit.h"
#ifdef OPENMP
#  include <omp.h>
#endif

extern int open_xyz_file (int linec);
extern int open_c3d_file (int linec);
extern int open_pdb_file (int linec);
extern int open_trj_file (int linec);
extern int open_vas_file (int linec);
extern int open_cif_configuration (int linec, int conf);
extern int open_cif_file (int linec);
extern int open_hist_file (int linec);
extern void allocatoms (project * this_proj);
extern chemical_data * alloc_chem_data (int spec);
extern int build_crystal (gboolean visible, project * this_proj, int c_step, gboolean to_wrap, gboolean show_clones, cell_info * cell, GtkWidget * widg);
extern const gchar * dfi[2];

extern atom_search * cif_search;
extern atomic_object * cif_object;
extern gboolean cif_multiple;

FILE * coordf;
coord_file * this_reader;
gchar ** coord_line = NULL;
gchar * this_line = NULL;
char * this_word;
line_node * head = NULL;
line_node * tail = NULL;

/*!
  \fn void add_reader_info (gchar * info, int mid)

  \brief append information message to the reader information

  \param info the reader information message
  \param mid message type (0 = error, 1 = warning)
*/
void add_reader_info (gchar * info, int mid)
{
  int i;
  gboolean append = TRUE;
  for (i=0; i<this_reader -> msg; i++)
  {
    if (g_strcmp0(this_reader -> info[i], info) == 0)
    {
      append = FALSE;
      break;
    }
  }
  if (append)
  {
    if (! this_reader -> msg)
    {
      this_reader -> info = g_malloc0 (sizeof*this_reader -> info);
    }
    else
    {
      this_reader -> info = g_realloc (this_reader -> info, (this_reader -> msg+1)*sizeof*this_reader -> info);
    }
    this_reader -> info[this_reader -> msg] = g_strdup_printf ("%s", info);
    this_reader -> msg ++;
  }
  if (! mid) this_reader -> mid = 0;
}

/*!
  \fn void reader_info (gchar * type, gchar * sinf, int val)

  \brief display reader information

  \param type File type
  \param sinf Information message
  \param val Value to present
*/
void reader_info (gchar * type, gchar * sinf, int val)
{
  g_print ("Reading coordinates [%s]: %s = %d\n", type, sinf, val);
}

/*!
  \fn void format_error (int stp, int ato, gchar * mot, int line)

  \brief Message to display an error message

  \param stp the MD step id
  \param ato Atom id
  \param mot Message
  \param line Line with the error
*/
void format_error (int stp, int ato, gchar * mot, int line)
{
  gchar * str;
  if (ato < 0)
  {
    str = g_strdup_printf ("Wrong file format: error at step %d !\n"
                           "Wrong file format: record <b>%s</b> on line <b>%d</b> is corrupted !",
                           stp, mot, line);
  }
  else
  {
    str = g_strdup_printf ("Wrong file format: error at step %d, atom %d !\n"
                           "Wrong file format: record <b>%s</b> on line <b>%d</b> is corrupted !",
                           stp, ato, mot, line+1);
  }
  add_reader_info (str, 0);
  g_free (str);
}

/*!
  \fn int set_v_dummy (gchar * this_word)

  \brief check if dummy is used for unknown species, if not then ask what to do

  \param this_word the chemical species label
*/
int set_v_dummy (gchar * this_word)
{
  int i;
  for (i=0; i<this_reader -> ndummy; i++)
  {
    if (g_strcmp0(this_reader -> dummy[i], this_word) == 0)
    {
      return i+1;
    }
  }
  gchar ** dummy = NULL;
  if (this_reader -> ndummy)
  {
    dummy = duplicate_strings (this_reader -> ndummy, this_reader -> dummy);
    g_free (this_reader -> dummy);
  }
  this_reader -> ndummy ++;
  this_reader -> dummy = g_malloc0(this_reader -> ndummy*sizeof*this_reader -> dummy);
  if (dummy)
  {
    for (i=0; i<this_reader -> ndummy-1; i++)
    {
      this_reader -> dummy[i] = g_strdup_printf ("%s", dummy[i]);
    }
  }
  this_reader -> dummy[this_reader -> ndummy-1] = g_strdup_printf ("%s", this_word);
  // Dummy added, then do we use this dummy ?
  gchar * str = g_strdup_printf ("Use dummy atom(s) for unknown %s species ?", this_word);
  gboolean use_dummy = ask_yes_no ("Use dummy atom(s) ?", str, GTK_MESSAGE_QUESTION, MainWindow);
  g_free (str);
  if (use_dummy)
  {
    str = g_strdup_printf ("Using dummy atom(s) for unknown %s species", this_word);
    add_reader_info (str, 1);
    g_free (str);
    return this_reader -> ndummy;
  }
  else
  {
    str = g_strdup_printf ("No dummy atom(s) for unknown %s species", this_word);
    add_reader_info (str, 1);
    g_free (str);
    return 0;
  }
}

/*!
  \fn void check_for_species (double v, int ato)

  \brief Fill the species for each atom and the associated data

  \param v Z
  \param ato the atom id
*/
void check_for_species (double v, int ato)
{
  int i;
  gboolean add_spec;
  add_spec = TRUE;
  if (this_reader -> nspec)
  {
    for (i=0; i<this_reader -> nspec; i++)
    {
      if (this_reader -> z[i] == v)
      {
        if (this_reader -> cartesian)
        {
          active_project -> atoms[0][ato].sp = i;
        }
        else
        {
          this_reader -> lot[ato] = i;
        }
        this_reader -> nsps[i] ++;
        add_spec = FALSE;
        break;
      }
    }
  }
  if (add_spec)
  {
    if (this_reader -> nspec)
    {
      this_reader -> z = g_realloc (this_reader -> z, (this_reader -> nspec+1)*sizeof*this_reader -> z);
      this_reader -> nsps = g_realloc (this_reader -> nsps, (this_reader -> nspec+1)*sizeof*this_reader -> nsps);
    }
    if (this_reader -> cartesian)
    {
      active_project -> atoms[0][ato].sp = this_reader -> nspec;
    }
    else
    {
      this_reader -> lot[ato] = this_reader -> nspec;
    }
    this_reader -> nsps[this_reader -> nspec] = 1;
    this_reader -> z[this_reader -> nspec] = v;
    this_reader -> nspec ++;
  }
}

/*!
  \fn int open_coord_file (gchar * filename, int fti)

  \brief open atomic coordinates file

  \param filename the file name
  \param fti the type of coordinates
*/
int open_coord_file (gchar * filename, int fti)
{
  int res = 0;
#ifdef OPENMP
  struct stat status;
  res = stat (filename, & status);
  if (res == -1)
  {
    add_reader_info ("Error - cannot get file statistics !\n", 0);
    return 1;
  }
  int fsize = status.st_size;
#endif
  coordf = fopen (filename, dfi[0]);
  if (! coordf)
  {
    add_reader_info ("Error - cannot open coordinates file !\n", 0);
    return 1;
  }
  int i, j, k, l;
#ifdef OPENMP
  gchar * coord_content = g_malloc0(fsize*sizeof*coord_content);
  fread (coord_content, fsize, 1, coordf);
  fclose (coordf);
  int linecount = 0;
  for (j=0; j<fsize; j++) if (coord_content[j] == '\n') linecount ++;
  coord_line = g_malloc0 (linecount*sizeof*coord_line);
  coord_line[0] = & coord_content[0];
  i = 1;
  for (j=0; j<fsize; j++)
  {
    if (coord_content[j] == '\n')
    {
      coord_content[j] = '\0';
      if (i < linecount)
      {
        coord_line[i] = & coord_content[j+1];
        i ++;
      }
    }
  }
#else
  gchar * buf = g_malloc0(LINE_SIZE*sizeof*buf);
  head = NULL;
  tail = NULL;
  i = 0;
  while (fgets(buf, LINE_SIZE, coordf))
  {
    if (head == NULL)
    {
      head = g_malloc0 (sizeof*head);
      tail = g_malloc0 (sizeof*tail);
      tail = head;
    }
    else
    {
      tail -> next = g_malloc0 (sizeof*tail -> next);
      if (fti == 9 || fti == 10)
      {
        tail -> next -> prev = g_malloc0 (sizeof*tail -> next -> prev);
        tail -> next -> prev = tail;
      }
      tail = tail -> next;
    }
    tail -> line = g_strdup_printf ("%s", buf);
    tail -> line = substitute_string (tail -> line, "\n", "\0");
    i ++;
  }
  g_free (buf);
  fclose (coordf);
#endif
  if (i)
  {
    this_reader -> cartesian = TRUE;
    if (fti < 2)
    {
      res = open_xyz_file (i);
    }
    else if (fti == 2)
    {
      res = open_c3d_file (i);
    }
    else if (fti < 5)
    {
      res = open_trj_file (i);
    }
    else if (fti < 7)
    {
      res = open_vas_file (i);
    }
    else if (fti > 6 && fti < 9)
    {
      res = open_pdb_file (i);
    }
    else if (fti > 8 && fti < 12)
    {
      if (fti == 11) cif_use_symmetry_positions = TRUE;
      this_reader -> cartesian = FALSE;
      if (fti == 10)
      {
        res = open_cif_file (i);
      }
      else
      {
        active_project -> steps = this_reader -> steps = 1;
        this_reader -> rounding = -1;
        cif_multiple = FALSE;
        res = open_cif_configuration (i, 0);
      }
    }
    else if (fti == 12)
    {
      res = open_hist_file (i);
    }
  }
  else
  {
    res = 1;
  }
#ifndef OPENMP
  if (tail) g_free (tail);
#endif
  if (! res)
  {
    if (fti == 9 && ! this_reader -> cartesian)
    {
      // this_reader -> lattice.sp_group -> sid = 2;
      // get_origin (this_reader -> lattice.sp_group);
      if (! cif_use_symmetry_positions)
      {
        // Test for all configurations, do build each:
        //  - a single trajectory ?
        //  - each in a single project ?
        i = 1;
        crystal_dist_chk = TRUE;
        crystal_crowded = FALSE;
        crystal_low_warning = TRUE;
        for (j=0; j<active_project -> steps; j++)
        {
          k = build_crystal (FALSE, active_project, j, TRUE, FALSE, & this_reader -> lattice, MainWindow);
          if (! k)
          {
            add_reader_info ("Error(s) trying to build crystal using the CIF file parameters !\n"
                             "This usually comes from: \n"
                             "\t - incorrect space group description\n"
                             "\t - incomplete space group description\n"
                             "\t - missing space group setting\n"
                             "\t - incorrect space group setting\n", 0);
            res = 3;
            goto end;
          }
          else if (k < 0)
          {
            add_reader_info ("Error(s) trying to build crystal using the CIF file parameters !\n"
                             "Information lead to change(s) between each configuration\n", 0);
            res = 3;
            goto end;
          }
          else if (k > 1 && i)
          {
            add_reader_info ("Potential issue(s) when building crystal !\n"
                             "This usually comes from: \n"
                             "\t - incorrect space group description\n"
                             "\t - incomplete space group description\n"
                             "\t - missing space group setting\n"
                             "\t - incorrect space group setting\n", 1);
            if (this_reader -> num_sym_pos && active_project -> steps == 1)
            {
               add_reader_info ("\nAnother model will be built using included symmetry positions\n", 1);
              cif_use_symmetry_positions = TRUE;
            }
            i = 0;
          }
        }
      }
    }
    for (i=0; i<active_project -> steps; i++)
    {
      for (j=0; j<active_project -> natomes; j++)
      {
        active_project -> atoms[i][j].id = j;
        active_project -> atoms[i][j].show[0] = TRUE;
        active_project -> atoms[i][j].show[1] = TRUE;
        active_project -> atoms[i][j].label[0] = FALSE;
        active_project -> atoms[i][j].label[1] = FALSE;
        active_project -> atoms[i][j].pick[0] = FALSE;
        active_project -> atoms[i][j].cloned = FALSE;
      }
    }
    if (fti != 9 || this_reader -> cartesian)
    {
      active_project -> nspec = this_reader -> nspec;
      active_project -> chemistry = alloc_chem_data (active_project -> nspec);
      active_project_changed (activep);
      k = l = 0;
      reader_info (coord_files_ext[fti], "Number of species", active_project -> nspec);
      for (i=0; i<active_project -> nspec; i++)
      {
        active_chem -> chem_prop[CHEM_Z][i] = this_reader -> z[i];
        j = (int)this_reader -> z[i];
        if (this_reader -> z[i] < 1.0)
        {
          active_chem -> label[i] = g_strdup_printf ("%s", this_reader -> dummy[l]);
          active_chem -> element[i] = g_strdup_printf ("Dummy %s", this_reader -> dummy[l]);
          active_chem -> chem_prop[CHEM_M][i] = 1.0;
          active_chem -> chem_prop[CHEM_R][i] = 0.5;
          l ++;
        }
        else
        {
          active_chem -> label[i] = g_strdup_printf ("%s", periodic_table_info[j].lab);
          active_chem -> element[i] = g_strdup_printf ("%s", periodic_table_info[j].name);
          active_chem -> chem_prop[CHEM_M][i] = set_mass_ (& j);
          active_chem -> chem_prop[CHEM_R][i] = set_radius_ (& j, & k);
          if (! active_chem -> chem_prop[CHEM_R][i])
          {
            gchar * str = g_strdup_printf ("For species %s, radius is equal to 0.0 !\n", active_chem -> label[i]);
            add_reader_info (str, 1);
            g_free (str);
          }
          active_chem -> chem_prop[CHEM_N][i] = set_neutron_ (& j);
          active_chem -> chem_prop[CHEM_X][i] = active_chem -> chem_prop[CHEM_Z][i];
        }
        active_chem -> nsps[i] = this_reader -> nsps[i];
        g_print ("Reading coordinates [%s]:\t %s, nsps[%d]= %d\n", coord_files_ext[fti], active_chem -> label[i], i+1, active_chem -> nsps[i]);
      }
    }
    else
    {
      reader_info (coord_files_ext[fti], "Number of species", active_project -> nspec);
      for (i=0; i<active_project -> nspec; i++)
      {
        g_print ("Reading coordinates [%s]:\t %s, nsps[%d]= %d\n", coord_files_ext[fti], active_chem -> label[i], i+1, active_chem -> nsps[i]);
      }
    }
  }
  end:;
  if (! (fti == 9 && cif_use_symmetry_positions) || res)
  {
    if (cif_search)
    {
      g_free (cif_search);
      cif_search = NULL;
    }
    if (cif_object)
    {
      g_free (cif_object);
      cif_object = NULL;
    }
    cif_use_symmetry_positions = FALSE;
  }
  return res;
}
