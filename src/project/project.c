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
* @file project.c
* @short Project management functions
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'project.c'
*
* Contains:
*

 - Project management functions

*
* List of functions:

  void save_pos_ (int * nat, int lot[* nat], int * num, double xpos[* num], double ypos[* num], double zpos[* num]);
  void send_steps_ (int * steps);

  project * get_project_by_id (int p);

*/

#include "global.h"
#include "glview.h"
#include "callbacks.h"
#include "interface.h"
#include "bind.h"

workspace workzone;
project * active_project = NULL;
chemical_data * active_chem = NULL;
coord_info * active_coord = NULL;
cell_info * active_cell = NULL;
box_info * active_box = NULL;
image * active_image = NULL;
glwin * active_glwin = NULL;
project * opengl_project = NULL;

/*!
  \fn void save_pos_ (int * nat, int lot[*nat], int * num, double xpos[*num], double ypos[*num], double zpos[*num])

  \brief retrieve atomic coordinates from Fortran90

  \param nat Number of atoms
  \param lot List of chemical species by atoms
  \param num Number of coordinates (NA x NS)
  \param xpos x coordinates
  \param ypos y coordinates
  \param zpos z coordinates
*/
void save_pos_ (int * nat, int lot[* nat], int * num, double xpos[* num], double ypos[* num], double zpos[* num])
{
  int i, j, k;

  k = 0;

  for ( i=0 ; i < active_project -> steps ; i++ )
  {
    for (j=0; j < active_project -> natomes; j++)
    {
      active_project -> atoms[i][j].x = xpos[k];
      active_project -> atoms[i][j].y = ypos[k];
      active_project -> atoms[i][j].z = zpos[k];
      active_project -> atoms[i][j].sp = lot[j]-1;
      active_project -> atoms[i][j].id = j;
      active_project -> atoms[i][j].show[0] = TRUE;
      active_project -> atoms[i][j].show[1] = TRUE;
      active_project -> atoms[i][j].label[0] = FALSE;
      active_project -> atoms[i][j].label[1] = FALSE;
      if (active_glwin == NULL)
      {
        active_project -> atoms[i][j].pick[0] = FALSE;
      }
      else if (! active_image -> selected[0] -> selected)
      {
        active_project -> atoms[i][j].pick[0] = FALSE;
      }
      active_project -> atoms[i][j].cloned = FALSE;
      k++;
    }
  }
}

/*!
  \fn void send_steps_ (int * steps)

  \brief retrieve the number of MD steps from Fortran90

  \param steps the number of MD steps
*/
void send_steps_ (int * steps)
{
  active_project -> steps = * steps;
}

/*!
  \fn project * get_project_by_id (int p)

  \brief get project pointer using id number

  \param p the id number
*/
project * get_project_by_id (int p)
{
  project * tmp = workzone.first;
  int i;
  for (i=0; i<nprojects; i++)
  {
    if (tmp -> id == p) return tmp;
    if (tmp -> next != NULL) tmp = tmp -> next;
  }
  return NULL;
}
