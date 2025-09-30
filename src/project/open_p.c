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
* @file open_p.c
* @short Functions to start the reading of an atomes project file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'open_p.c'
*
* Contains:
*

 - The functions to start the reading of an atomes project file

*
* List of functions:

  int open_project (FILE * fp, int npi);

  char * read_string (int i, FILE * fp);

  gchar * read_this_string (FILE * fp);

  void initcnames (int w, int s);
  void allocatoms (project * this_proj);
  void alloc_proj_data (project * this_proj, int cid);

  chemical_data * alloc_chem_data (int spec);

*/

#include "global.h"
#include "bind.h"
#include "interface.h"
#include "callbacks.h"
#include "project.h"
#include "curve.h"
#include "glview.h"
#include "preferences.h"

extern void alloc_curves (int c);
extern void init_box_calc ();
extern void set_color_map_sensitive (glwin * view);
extern void initgr (int r);
extern void initsq (int r);
extern void initbd ();
extern void initang ();
extern void initrng ();
extern void initchn ();
extern void initmsd ();
extern void initsh (int s);

gboolean old_la_bo_ax_gr;

/*!
  \fn char * read_string (int i, FILE * fp)

  \brief read a string from a file

  \param i the size of the string to read
  \param fp the file pointer
*/
char * read_string (int i, FILE * fp)
{
  char * tmp = NULL;
  tmp = g_malloc0 (i*sizeof*tmp);
  int j;
  for (j=0; j<i; j++)
  {
    tmp[j] = fgetc(fp);
  }
  //return fgets (tmp, i+1, fp);
  return tmp;
}

/*!
  \fn gchar * read_this_string (FILE * fp)

  \brief is there a string to read in this file ? yes do it

  \param fp the file pointer
*/
gchar * read_this_string (FILE * fp)
{
  int i;
  if (fread (& i, sizeof(int), 1, fp) != 1) return NULL;
  if (i > 0)
  {
    gchar * str = g_strdup_printf ("%s", read_string (i, fp));
    return str;
  }
  return NULL;
}


/*!
  \fn void initcnames (int w, int s)

  \brief initialize curve namees

  \param w calculation id
  \param s initialize spherical harmonics or not
*/
void initcnames (int w, int s)
{
  switch (w)
  {
    case GR:
      initgr (w);
      break;
    case SQ:
      initsq (w);
      break;
    case SK:
      initsq (w);
      break;
    case GK:
      initgr (w);
      break;
    case BD:
      initbd ();
      break;
    case AN:
      initang ();
      break;
    case RI:
      initrng ();
      break;
    case CH:
      initchn ();
      break;
    case SP:
      initsh (0);
      break;
    default:
      initmsd ();
      break;
  }
}

/*!
  \fn void allocatoms (project * this_proj)

  \brief allocate project data

  \param this_proj the target project
*/
void allocatoms (project * this_proj)
{
  int i, j;
  if (this_proj -> atoms != NULL)
  {
    g_free (this_proj -> atoms);
    this_proj -> atoms = NULL;
  }
  this_proj -> atoms = g_malloc0 (this_proj -> steps*sizeof*this_proj -> atoms);
  for (i=0; i < this_proj -> steps; i++)
  {
    this_proj -> atoms[i] = g_malloc0 (this_proj -> natomes*sizeof*this_proj -> atoms[i]);
    for (j=0; j<this_proj -> natomes; j++)
    {
      this_proj -> atoms[i][j].style = NONE;
    }
  }
}

/*!
  \fn chemical_data * alloc_chem_data (int spec)

  \brief allocate chemistry data

  \param spec the number of chemical species
*/
chemical_data * alloc_chem_data (int spec)
{
  chemical_data * chem = g_malloc0 (sizeof*chem);
  chem -> label = g_malloc0 (spec*sizeof*chem -> label);
  chem -> element = g_malloc0 (spec*sizeof*chem -> element);
  chem -> nsps = allocint (spec);
  chem -> formula = allocint (spec);
  chem -> grtotcutoff = default_totcut;
  chem -> cutoffs = allocddouble (spec, spec);
  chem -> chem_prop = allocddouble (CHEM_PARAMS, spec);
  return chem;
}

/*!
  \fn void alloc_proj_data (project * this_proj, int cid)

  \brief allocate data

  \param this_proj the target project
  \param cid Allocate chemistry data (1/0)
*/
void alloc_proj_data (project * this_proj, int cid)
{
  if (cid) this_proj -> chemistry = alloc_chem_data (this_proj -> nspec);
  allocatoms (this_proj);
}

/*!
  \fn int open_project (FILE * fp, int npi)

  \brief open atomes project file

  \param fp the file pointer
  \param npi the total number of projects in the workspace
*/
int open_project (FILE * fp, int npi)
{
  int i, j, k;
  gchar * ver;
  // First 2 lines for compatibility issues
  if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  ver = g_malloc0 (i*sizeof*ver);
  if (fread (ver, sizeof(char), i, fp) != i) return ERROR_PROJECT;

  gboolean labels_in_file = FALSE;
  gboolean correct_x = TRUE;
  old_la_bo_ax_gr = TRUE;
  // test on ver for version
  if (g_strcmp0(ver, "%\n% project file v-2.6\n%\n") == 0)
  {
    labels_in_file = TRUE;
  }
  else if (g_strcmp0(ver, "%\n% project file v-2.7\n%\n") == 0)
  {
    labels_in_file = TRUE;
    correct_x = FALSE;
  }
  else if (g_strcmp0(ver, "%\n% project file v-2.8\n%\n") == 0)
  {
    old_la_bo_ax_gr = FALSE;
    labels_in_file = TRUE;
    correct_x = FALSE;
  }

 #ifdef DEBUG
  g_debug ("%s", ver);
 #endif // DEBUG
  g_free (ver);

  // After that we read the data
  active_project -> name = read_this_string (fp);
  if (active_project -> name == NULL) return ERROR_PROJECT;
#ifdef DEBUG
  g_debug ("OPEN_PROJECT: Project name= %s",active_project -> name);
#endif
  if (fread (& active_project -> tfile, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (active_project -> tfile > -1)
  {
    active_project -> coordfile = read_this_string (fp);
    if (active_project -> coordfile == NULL) return ERROR_PROJECT;
  }
  if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (i > -1)
  {
    active_project -> bondfile = read_this_string (fp);
    if (active_project -> bondfile == NULL) return ERROR_PROJECT;
  }
  if (fread (active_project -> runok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (active_project -> initok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (active_project -> visok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (& active_project -> nspec, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_project -> natomes, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_project -> steps, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_cell -> pbc, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_cell -> frac, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_cell -> ltype,  sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& i,  sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (i > 1 && i != active_project -> steps) return ERROR_PROJECT;
  if (i > 1) active_cell -> npt = TRUE;
  active_cell -> box = g_malloc0 (i*sizeof*active_cell -> box);
  active_box = & active_cell -> box[0];
  for (j=0; j<i; j++)
  {
    for (k=0; k<3; k++)
    {
      if (fread (active_cell -> box[j].vect[k],  sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
    }
    if (fread (active_cell -> box[j].param[0],  sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
    if (fread (active_cell -> box[j].param[1],  sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
  }
  if (fread (& active_cell -> crystal, sizeof(gboolean), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (active_cell -> crystal && i)
  {
    active_cell -> sp_group = g_malloc0(sizeof*active_cell -> sp_group);
    active_cell -> sp_group -> id = i;
    active_cell -> sp_group -> bravais = read_this_string (fp);
    if (! active_cell -> sp_group -> bravais) return ERROR_PROJECT;
    active_cell -> sp_group -> hms = read_this_string (fp);
    if (! active_cell -> sp_group -> hms) return ERROR_PROJECT;
    active_cell -> sp_group -> setting = read_this_string (fp);
    if (! active_cell -> sp_group -> setting) return ERROR_PROJECT;
  }
  if (fread (& active_project -> run, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (& active_project -> initgl, sizeof(gboolean), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (active_project -> tmp_pixels, sizeof(int), 2, fp) != 2) return ERROR_PROJECT;
  if (fread (active_project -> num_delta, sizeof(int), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (active_project -> delta, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (active_project -> rsearch, sizeof(int), 2, fp) != 2) return ERROR_PROJECT;
  for (i=0; i<5; i++)
  {
    if (fread (active_project -> rsparam[i], sizeof(int), 6, fp) != 6) return ERROR_PROJECT;
    if (fread (active_project -> rsdata[i], sizeof(double), 5, fp) != 5) return ERROR_PROJECT;
  }
  if (fread (& active_project -> csearch, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fread (active_project -> csparam, sizeof(int), 7, fp) != 7) return ERROR_PROJECT;
  if (fread (active_project -> csdata, sizeof(double), 2, fp) != 2) return ERROR_PROJECT;
  if (fread (active_project -> min, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (active_project -> max, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fread (& active_project -> tunit, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (active_project -> natomes != 0 && active_project -> nspec != 0)
  {
    alloc_proj_data (active_project, 1);
    active_chem = active_project -> chemistry;
    if (labels_in_file)
    {
      for (i=0; i<active_project -> nspec; i++)
      {
        active_chem -> label[i] = read_this_string (fp);
        active_chem -> element[i] = read_this_string (fp);
      }
    }
    if (fread (active_chem -> nsps, sizeof(int), active_project -> nspec, fp) != active_project -> nspec) return ERROR_PROJECT;
    if (fread (active_chem -> formula, sizeof(int), active_project -> nspec, fp) != active_project -> nspec) return ERROR_PROJECT;
    j = 0;
    for (i=0; i<active_project -> nspec; i++) j+= active_chem -> nsps[i];
    if (j != active_project -> natomes) return ERROR_PROJECT;
    for (i=0; i<CHEM_PARAMS; i++)
    {
      if (fread (active_chem -> chem_prop[i], sizeof(double), active_project -> nspec, fp) != active_project -> nspec) return ERROR_PROJECT;
    }
    if (correct_x)
    {
      for (i=0; i<active_project -> nspec; i++)
      {
        active_chem -> chem_prop[CHEM_X][i] = active_chem -> chem_prop[CHEM_Z][i];
      }
    }
    if (fread (& active_chem -> grtotcutoff, sizeof(double), 1, fp) != 1) return ERROR_PROJECT;
    for ( i = 0 ; i < active_project -> nspec ; i ++ )
    {
      if (fread (active_chem -> cutoffs[i], sizeof(double), active_project -> nspec, fp) != active_project -> nspec) return ERROR_PROJECT;
    }
    for (i=0; i<active_project -> steps; i++)
    {
      for (j=0; j< active_project -> natomes; j++)
      {
        if (read_atom_a (fp, active_project, i, j) != OK) return ERROR_ATOM_A;
      }
    }
    init_box_calc ();
    if (active_project -> run)
    {
#ifdef DEBUG
      g_debug ("OPEN_PROJECT:: So far so good ... still");
      g_debug ("OPEN_PROJECT:: RUN PROJECT\n");
#endif
      i = alloc_data_ (& active_project -> natomes,
                       & active_project -> nspec,
                       & active_project -> steps);
      if (i == 1)
      {
        if (! labels_in_file)
        {
          j = 1;
          prep_spec_ (active_chem -> chem_prop[CHEM_Z], active_chem -> nsps, & j);
        }
        initcwidgets ();
        // Read curves
        for (i=0; i<NGRAPHS; i++) if (active_project -> initok[i]) initcnames (i, 0);
        if (fread (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
#ifdef DEBUG
        g_debug ("\n**********************************************\n curves to read= %d\n**********************************************\n", i);
#endif
        if (i != 0)
        {
          j = 0;
          if (fread (& j, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
          if (j)
          {
            active_project -> numc[SP] = j;
            active_project -> numwid += j;
            alloc_curves (SP);
            addcurwidgets (activep, SP, 0);
            active_project -> initok[SP] = TRUE;
            for (k=0; k<j; k++)
            {
              active_project -> curves[SP][k] -> name = read_this_string (fp);
              if (active_project -> curves[SP][k] -> name == NULL) return ERROR_PROJECT;
            }
          }
          for (j=0; j<i; j++)
          {
            if (read_project_curve (fp, npi, activep) != OK)
            {
              // error
              return ERROR_CURVE;
            }
          }
        }
        fill_tool_model();
      }
      else
      {
        return ERROR_PROJECT;
      }
    }
  }
  else
  {
    // error
    return ERROR_NO_WAY;
  }
/* #ifdef DEBUG
  debugioproj (active_project, "READ INIT");
#endif */
  if (update_project() == 1)
  {
    if (active_project -> initgl)
    {
      gboolean tmp_bonding;
      if (fread (& tmp_bonding, sizeof(gboolean), 1, fp) != 1) return ERROR_PROJECT;
      if (fread (tmp_adv_bonding, sizeof(gboolean), 2, fp) != 2) return ERROR_PROJECT;
      apply_project (TRUE);
      fill_tool_model ();
      int tmpcoord[10];
      if (fread (tmpcoord, sizeof(int), 10, fp) != 10) return ERROR_PROJECT;
      if (active_glwin -> bonding)
      {
        for (i=0; i<2; i++) if (tmpcoord[i] != active_project -> coord -> totcoord[i]) return ERROR_PROJECT;
        for (i=2; i<4; i++)
        {
          if (active_glwin -> adv_bonding[i-2])
          {
            if (tmpcoord[i] != active_project -> coord -> totcoord[i]) return ERROR_PROJECT;
          }
        }
      }
      for (i=0; i<10; i++) active_project -> coord -> totcoord[i] = tmpcoord[i];
      // Read molecule info
      if ((active_project -> natomes > ATOM_LIMIT || active_project -> steps > STEP_LIMIT) && tmp_adv_bonding[1])
      {
        if (read_mol (fp) != OK) return ERROR_MOL;
      }
      for (i=0; i<2; i++) active_glwin -> adv_bonding[i] = tmp_adv_bonding[i];
      active_glwin -> bonding = tmp_bonding;

      if (read_bonding (fp) != OK) return ERROR_COORD;
      i = read_opengl_image (fp, active_project, active_glwin -> anim -> last -> img, active_project -> nspec);
      if (i != OK) return i;

      i = read_dlp_field_data (fp, active_project);
      if (i != OK) return i;
      i = read_lmp_field_data (fp, active_project);
      if (i != OK) return i;

      for (i=0; i<2; i++)
      {
        j = read_cpmd_data (fp, i, active_project);
        if (j != OK) return j;
      }
      for (i=0; i<2; i++)
      {
        j = read_cp2k_data (fp, i, active_project);
        if (j != OK) return j;
      }
#ifdef GTK3
      // GTK3 Menu Action To Check
      set_color_map_sensitive (active_glwin);
#endif
      return OK;
    }
    return OK;
  }
  else
  {
    return ERROR_UPDATE;
  }
}
