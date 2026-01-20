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
* @file save_bond.c
* @short Functions to save bonding information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'save_bond.c'
*
* Contains:
*

 - The functions to save bonding information in the atomes project file format

*
* List of functions:

  int save_bonding (FILE * fp, project * this_proj);

*/

#include "global.h"
#include "project.h"
#include "glview.h"
#include "initcoord.h"

/*!
  \fn int save_bonding (FILE * fp, project * this_proj)

  \brief save bonding information to file

  \param fp the file pointer
  \param this_proj the target project
*/
int save_bonding (FILE * fp, project * this_proj)
{
  int i, j, k;
  image * img = this_proj -> modelgl -> anim -> last -> img;
  if (! this_proj -> modelgl -> bonding || ! this_proj -> modelgl -> adv_bonding[1] || this_proj -> natomes > ATOM_LIMIT || this_proj -> steps > STEP_LIMIT)
  {
    for (i=0; i<this_proj -> steps; i++)
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        if (fwrite (this_proj -> atoms[i][j].coord, sizeof(int), 5, fp) != 5) return ERROR_COORD;
        if (fwrite (& this_proj -> atoms[i][j].numv, sizeof(int), 1, fp) != 1) return ERROR_COORD;
        if (this_proj -> atoms[i][j].numv)
        {
          if (fwrite (this_proj -> atoms[i][j].vois, sizeof(int), this_proj -> atoms[i][j].numv, fp) != this_proj -> atoms[i][j].numv) return ERROR_COORD;
        }
      }
      if (fwrite (this_proj -> modelgl -> bonds[i], sizeof(int), 2, fp) != 2) return ERROR_COORD;
      for (j=0; j<2; j++)
      {
        for (k=0; k<this_proj -> modelgl -> bonds[i][j]; k++)
        {
          if (fwrite (this_proj -> modelgl -> bondid[i][j][k], sizeof(int), 2, fp) != 2) return ERROR_COORD;
        }
      }
    }

    coord_info * coord = this_proj -> coord;
    for (i=0; i<2; i++)
    {
      if (fwrite (coord -> ntg[i], sizeof(int), coord -> species, fp) != coord -> species) return ERROR_COORD;
      for (j=0; j<coord -> species; j++)
      {
        if (fwrite (coord -> geolist[i][j], sizeof(int), coord -> ntg[i][j], fp) != coord -> ntg[i][j]) return ERROR_COORD;
        if (i == 1)
        {
          for (k=0; k<coord -> ntg[i][j]; k++)
          {
            if (fwrite (coord -> partial_geo[j][k], sizeof(int), coord -> species, fp) != coord -> species) return ERROR_COORD;
          }
        }
      }
    }
  }

  for (i=0; i<10; i++)
  {
    if (fwrite (& this_proj -> coord -> totcoord[i], sizeof(int), 1, fp) != 1) return ERROR_COORD;
    if (i < 2)
    {
      if (fwrite (img -> show_atom[i], sizeof(gboolean), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_COORD;
      if (fwrite (img -> show_label[i], sizeof(gboolean), this_proj -> nspec, fp) != this_proj -> nspec) return ERROR_COORD;
    }
    if (this_proj -> coord -> totcoord[i])
    {
      if (fwrite (img -> show_coord[i], sizeof(gboolean), this_proj -> coord -> totcoord[i], fp) != this_proj -> coord -> totcoord[i]) return ERROR_COORD;
      if (i < 2 || (i > 3 && i < 9))
      {
        if (fwrite (img -> show_poly[i], sizeof(gboolean), this_proj -> coord -> totcoord[i], fp) != this_proj -> coord -> totcoord[i]) return ERROR_COORD;
      }
    }
  }

  return OK;
}
