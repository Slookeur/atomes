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
* @file read_bond.c
* @short Function to read bonding information in the atomes project file format
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'read_bond.c'
*
* Contains:
*

 - The function to read bonding information in the atomes project file format

*
* List of functions:

  int read_bonding (FILE * fp);

*/

#include "global.h"
#include "project.h"
#include "glview.h"

extern void new_coord_menus (project * this_proj, coord_info * coord, int new_spec, int nmols,
                             gboolean * showcoord[2], gboolean * showpoly[2], gboolean * showfrag,
                             gboolean update_it, gboolean update_mol);

/*!
  \fn int read_bonding (FILE * fp)

  \brief read bonding information from file

  \param fp the file pointer
*/
int read_bonding (FILE * fp)
{
  int i, j, k, l, m;
  distance clo;
  coord_info * coord = g_malloc0(sizeof*coord);
  coord -> species = active_project -> nspec;
  image * img = active_glwin -> anim -> last -> img;
  gboolean read_bond = FALSE;

  if (! active_glwin -> bonding || ! active_glwin -> adv_bonding[1] || active_project -> natomes > ATOM_LIMIT || active_project -> steps > reading_step_limit)
  {
    read_bond = TRUE;
  }
  if (read_bond)
  {
    active_glwin -> bonds = allocdint (active_project -> steps, 2);
    active_glwin -> bondid = g_malloc0(active_project -> steps*sizeof*active_glwin -> bondid);
    for (i=0; i<active_project -> steps; i++)
    {
      for (j=0; j<active_project -> natomes; j++)
      {
        if (fread (active_project -> atoms[i][j].coord, sizeof(int), 5, fp) != 5) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
        if (fread (& active_project -> atoms[i][j].numv, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
        if (active_project -> atoms[i][j].numv)
        {
          active_project -> atoms[i][j].vois = allocint (active_project -> atoms[i][j].numv);
          if (fread (active_project -> atoms[i][j].vois, sizeof(int), active_project -> atoms[i][j].numv, fp) != active_project -> atoms[i][j].numv) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
        }
      }
      if (fread (active_glwin -> bonds[i], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
      active_glwin -> bondid[i] = g_malloc0(2*sizeof*active_glwin -> bondid[i]);
      for (j=0; j<2; j++)
      {
        if (active_glwin -> bonds[i][j])
        {
          active_glwin -> allbonds[j] += active_glwin -> bonds[i][j];
          active_glwin -> bondid[i][j] = allocdint (active_glwin -> bonds[i][j], 2);
          if (j) active_glwin -> clones[i] = g_malloc0(active_glwin -> bonds[i][1]*sizeof*active_glwin -> clones[i]);
          for (k=0; k<active_glwin -> bonds[i][j]; k++)
          {
            if (fread (active_glwin -> bondid[i][j][k], sizeof(int), 2, fp) != 2) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
            if (j)
            {
              l = active_glwin -> bondid[i][j][k][0];
              m = active_glwin -> bondid[i][j][k][1];
              clo = distance_3d (active_cell, (active_cell -> npt) ? i : 0, & active_project -> atoms[i][l], & active_project -> atoms[i][m]);
              active_glwin -> clones[i][k].x = clo.x;
              active_glwin -> clones[i][k].y = clo.y;
              active_glwin -> clones[i][k].z = clo.z;
            }
          }
        }
      }
    }

    for (i=0; i<2; i++)
    {
      coord -> ntg[i] = allocint (coord -> species);
      if (fread (coord -> ntg[i], sizeof(int), coord -> species, fp) != coord -> species) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
      coord -> geolist[i] = g_malloc0(coord -> species*sizeof*coord -> geolist[i]);
      if (i == 1) coord -> partial_geo = g_malloc0(coord -> species*sizeof*coord -> partial_geo);
      for (j=0; j<coord -> species; j++)
      {
        coord -> geolist[i][j] = g_malloc0(coord -> ntg[i][j]*sizeof*coord -> geolist[i][j]);
        if (fread (coord -> geolist[i][j], sizeof(int), coord -> ntg[i][j], fp) != coord -> ntg[i][j]) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
        if (i == 1)
        {
          coord -> partial_geo[j] = g_malloc0(coord -> ntg[i][j]*sizeof*coord -> partial_geo[j]);
          for (k=0; k<coord -> ntg[i][j]; k++)
          {
            coord -> partial_geo[j][k] = g_malloc0(coord -> species*sizeof*coord -> partial_geo[j][k]);
            if (fread (coord -> partial_geo[j][k], sizeof(int), coord -> species, fp) != coord -> species) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
          }
        }
      }
    }
    coord -> cmax = 0;
    coord -> cmin = 20;
    for (i=0; i<coord -> species; i++)
    {
      for (j=0; j<coord -> ntg[1][i]; j++)
      {
        coord -> cmax = max(coord -> cmax, coord -> geolist[1][i][j]);
        coord -> cmin = min(coord -> cmin, coord -> geolist[1][i][j]);
      }
    }
  }

  for (i=0; i<10; i++)
  {
    if (fread (& j, sizeof(int), 1, fp) != 1) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
    if (i < 2 && active_glwin -> bonding && j != active_project -> coord -> totcoord[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
    if (i > 1 && i < 4 && active_glwin -> adv_bonding[i-2] && j != active_project -> coord -> totcoord[i]) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
    if (i < 2)
    {
      if (fread (img -> show_atom[i], sizeof(gboolean), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
      if (fread (img -> show_label[i], sizeof(gboolean), active_project -> nspec, fp) != active_project -> nspec) return signal_error (__FILE__, __func__, __LINE__, ERROR_COORD);
    }

    if (active_project -> coord -> totcoord[i])
    {
      if (! img -> show_poly[i] && (i < 2 || (i > 3 && i < 9))) img -> show_poly[i] = allocbool(active_project -> coord -> totcoord[i]);
      if (! img -> show_coord[i]) img -> show_coord[i] = allocbool(active_project -> coord -> totcoord[i]);
      if (fread (img -> show_coord[i], sizeof(gboolean), active_project -> coord -> totcoord[i], fp) != active_project -> coord -> totcoord[i]) return ERROR_RW;
      if (i < 2 || (i > 3 && i < 9))
      {
        if (fread (img -> show_poly[i], sizeof(gboolean), active_project -> coord -> totcoord[i], fp) != active_project -> coord -> totcoord[i]) return ERROR_RW;
      }
    }
  }
  if (! active_glwin -> bonding || ! active_glwin -> adv_bonding[1] || active_project -> natomes > ATOM_LIMIT || active_project -> steps > reading_step_limit)
  {
    gboolean * showfrag = duplicate_bool (active_project -> coord -> totcoord[2], img -> show_coord[2]);
    gboolean * showcoord[2];
    gboolean * showpoly[2];
    for (i=0; i<2; i++)
    {
      showcoord[i] = duplicate_bool (active_project -> coord -> totcoord[i], img -> show_coord[i]);
      showpoly[i] = duplicate_bool (active_project -> coord -> totcoord[i], img -> show_poly[i]);
    }
    for (i=0; i<10; i++) coord -> totcoord[i] = active_project -> coord -> totcoord[i];
    active_project -> coord -> cmax = coord -> cmax;
    active_project -> coord -> cmin = coord -> cmin;
    new_coord_menus (active_project, coord, active_project -> nspec, 0, showcoord, showpoly, showfrag, TRUE, TRUE);
    g_free (coord);
  }

  return OK;
}
