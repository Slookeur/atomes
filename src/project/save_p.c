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
* @file save_p.c
* @short Functions to start saving an atomes project file
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_p.c'
*
* Contains:
*

 - The functions to start saving an atomes project file

*
* List of functions:

  int save_this_string (FILE * fp, gchar * string);
  int save_project (FILE * fp, project * this_proj, int npi);

*/

#include "global.h"
#include "project.h"

/*!
  \fn int save_this_string (FILE * fp, gchar * string)

  \brief save string to file

  \param fp the file pointer
  \param string the string to save
*/
int save_this_string (FILE * fp, gchar * string)
{
  int i;
  if (string)
  {
    i = strlen (string);
    if (i > 0)
    {
      i ++;
      if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
      if (fwrite (string, sizeof(char), i, fp) != i) return ERROR_RW;
    }
    else
    {
      i = 0;
      if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
    }
  }
  else
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_RW;
  }
  return OK;
}

/*!
  \fn int save_project (FILE * fp, project * this_proj, int npi)

  \brief save project to file

  \param fp the file pointer
  \param this_proj the target project
  \param npi the total number of projects in the workspace
*/
int save_project (FILE * fp, project * this_proj, int npi)
{
  int i, j, k;
  gchar * ver;

  // First 2 lines for compatibility issues
  i = 2;
  j = 8;
  ver = g_strdup_printf ("%%\n%% project file v-%1d.%1d\n%%\n", i, j);
  if (save_this_string (fp, ver) != OK)
  {
    g_free (ver);
    return ERROR_PROJECT;
  }
  g_free (ver);
  if (save_this_string (fp, this_proj -> name) != OK) return ERROR_PROJECT;
  if (fwrite (& this_proj -> tfile, sizeof(this_proj -> tfile), 1, fp) != 1) return ERROR_PROJECT;
  if (this_proj -> tfile > -1)
  {
    if (save_this_string (fp, this_proj -> coordfile) != OK) return ERROR_PROJECT;
  }
  if (this_proj -> bondfile != NULL)
  {
    if (save_this_string (fp, this_proj -> bondfile) != OK) return ERROR_PROJECT;
  }
  else
  {
    i = -1;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  }
  if (fwrite (this_proj -> runok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (this_proj -> initok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (this_proj -> visok, sizeof(gboolean), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (& this_proj -> nspec, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> natomes, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> steps, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> cell.pbc, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> cell.frac, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> cell.ltype, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  i = (this_proj -> cell.npt) ? this_proj -> steps : 1;
  if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  for (j=0; j<i; j++)
  {
    for (k=0; k<3; k++)
    {
      if (fwrite (this_proj -> cell.box[j].vect[k], sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
    }
    if (fwrite (this_proj -> cell.box[j].param[0], sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
    if (fwrite (this_proj -> cell.box[j].param[1], sizeof(double), 3, fp) != 3) return ERROR_PROJECT;
  }
  if (fwrite (& this_proj -> cell.crystal, sizeof(gboolean), 1, fp) != 1) return ERROR_PROJECT;
  if (this_proj -> cell.sp_group)
  {
    if (fwrite (& this_proj -> cell.sp_group -> id, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
    if (save_this_string (fp, this_proj -> cell.sp_group -> bravais) != OK) return ERROR_PROJECT;
    if (save_this_string (fp, this_proj -> cell.sp_group -> hms) != OK) return ERROR_PROJECT;
    if (save_this_string (fp, this_proj -> cell.sp_group -> setting) != OK) return ERROR_PROJECT;
  }
  else
  {
    i = 0;
    if (fwrite (& i, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  }

  if (fwrite (& this_proj -> run, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (& this_proj -> initgl, sizeof(gboolean), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (this_proj -> modelgl -> pixels, sizeof(int), 2, fp) != 2) return ERROR_PROJECT;
  if (fwrite (this_proj -> num_delta, sizeof(int), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (this_proj -> delta, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (this_proj -> rsearch, sizeof(int), 2, fp) != 2) return ERROR_PROJECT;
  for (i=0; i<5; i++)
  {
    if (fwrite (this_proj -> rsparam[i], sizeof(int), 6, fp) != 6) return ERROR_PROJECT;
    if (fwrite (this_proj -> rsdata[i], sizeof(double), 5, fp) != 5) return ERROR_PROJECT;
  }
  if (fwrite (& this_proj -> csearch, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (fwrite (this_proj -> csparam, sizeof(int), 7, fp) != 7) return ERROR_PROJECT;
  if (fwrite (this_proj -> csdata, sizeof(double), 2, fp) != 2) return ERROR_PROJECT;
  if (fwrite (this_proj -> min, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (this_proj -> max, sizeof(double), NGRAPHS, fp) != NGRAPHS) return ERROR_PROJECT;
  if (fwrite (& this_proj -> tunit, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
  if (this_proj -> natomes != 0 && this_proj -> nspec != 0)
  {
    for (i=0; i<this_proj -> nspec; i++)
    {
      if (save_this_string (fp, this_proj -> chemistry -> label[i]) != OK) return ERROR_PROJECT;
      if (save_this_string (fp, this_proj -> chemistry -> element[i]) != OK) return ERROR_PROJECT;
    }
    if (fwrite (this_proj -> chemistry -> nsps, sizeof(int), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_PROJECT;
    if (fwrite (this_proj -> chemistry -> formula, sizeof(int), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_PROJECT;
    for (i=0; i<CHEM_PARAMS; i++)
    {
      if (fwrite (this_proj -> chemistry -> chem_prop[i], sizeof(double), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_PROJECT;
    }
    if (fwrite (& this_proj -> chemistry -> grtotcutoff, sizeof(double), 1, fp) != 1) return ERROR_PROJECT;
    for (i=0; i<this_proj -> nspec; i++)
    {
      if (fwrite (this_proj -> chemistry -> cutoffs[i], sizeof(double), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_PROJECT;
    }
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        if (save_atom_a (fp, this_proj, i, j) != OK) return ERROR_ATOM_A;
      }
    }
    if (this_proj -> run)
    {
      k = 0;
      for (i=0; i<NGRAPHS; i++)
      {
        for (j=0; j<this_proj -> numc[i]; j++)
        {
          if (this_proj -> curves[i][j] -> ndata != 0)
          {
            k ++;
          }
        }
      }
      if (fwrite (& k, sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
      if (k)
      {
        if (fwrite (& this_proj -> numc[SP], sizeof(int), 1, fp) != 1) return ERROR_PROJECT;
        if (this_proj -> numc[SP])
        {
          for (i=0; i<this_proj -> numc[SP]; i++)
          {
            if (save_this_string (fp, this_proj -> curves[SP][i] -> name) != OK) return ERROR_PROJECT;
          }
        }
        for (i=0; i< NGRAPHS; i++)
        {
          for (j=0; j<this_proj -> numc[i]; j++)
          {
            if (this_proj -> curves[i][j] -> ndata != 0)
            {
              if (save_project_curve (fp, npi, this_proj, i, j) != OK) return ERROR_CURVE;
            }
          }
        }
      }
      if (this_proj -> initgl)
      {
        if (fwrite (& this_proj -> modelgl -> bonding, sizeof(gboolean), 1, fp) != 1) return ERROR_COORD;
        if (fwrite (this_proj -> modelgl -> adv_bonding, sizeof(gboolean), 2, fp) != 2) return ERROR_COORD;
        if (fwrite (this_proj -> coord -> totcoord, sizeof(int), 10, fp) != 10) return ERROR_COORD;
        // Save molecule
        if ((this_proj -> natomes > ATOM_LIMIT || this_proj -> steps > STEP_LIMIT) && this_proj -> modelgl -> adv_bonding[1])
        {
          if (save_mol (fp, this_proj) != OK) return ERROR_MOL;
        }
        // saving bonding info
        if (save_bonding (fp, this_proj) != OK) return ERROR_COORD;
        // saving glwin info
        i = save_opengl_image (fp, this_proj, this_proj -> modelgl -> anim -> last -> img, this_proj -> nspec);
        if (i != OK) return i;

        i = save_dlp_field_data (fp, this_proj);
        if (i != OK) return i;
        i = save_lmp_field_data (fp, this_proj);
        if (i != OK) return i;

        for (i=0; i<2; i++)
        {
          j = save_cpmd_data (fp, i, this_proj);
          if (j != OK) return j;
        }
        for (i=0; i<2; i++)
        {
          j = save_cp2k_data (fp, i, this_proj);
          if (j != OK) return j;
        }
      }
    }
  }
  else
  {
    // error
    return ERROR_NO_WAY;
  }
#ifdef DEBUG
//  debugioproj (this_proj, "WRITE");
#endif
  return OK;
}
