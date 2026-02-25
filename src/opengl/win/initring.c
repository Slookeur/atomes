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
* @file initring.c
* @short Functions to retrieve rings data from Fortran90
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'initring.c'
*
* Contains:
*

 - The functions to retrieve rings data from Fortran90

*
* List of functions:

  void send_rings_opengl_ (int * id, int * st, int * ta, int * ri, int nring[* ta+1]);
  void send_atom_rings_id_opengl_ (int * st, int * at, int * id, int * ta, int * num, int ring[* num]);
  void allocate_all_rings_ (int * id, int * st, int * ta, int * nring);

*/

#include "global.h"
#include "glwindow.h"

/*!
  \fn void send_rings_opengl_ (int * id, int * st, int * ta, int * ri, int nring[*ta+1])

  \brief get single ring data for the glwin from Fortran90

  \param id type of ring
  \param st the MD step
  \param ta size of the ring
  \param ri ring size id
  \param nring ring's atom list
*/
void send_rings_opengl_ (int * id, int * st, int * ta, int * ri, int nring[* ta+1])
{
  int i;
  active_glwin -> show_rpoly[* id][* st][* ta][* ri] = FALSE;
  for (i=0; i< * ta+1; i++)
  {
    active_glwin -> all_rings[* id][* st][* ta][* ri][i] = nring[i] - 1;
  }
}

/*!
  \fn void send_atom_rings_id_opengl_ (int * st, int * at, int * id, int * ta, int * num, int ring[*num])

  \brief get rings data for an atom from Fortran90

  \param st the MD step
  \param at atom id
  \param id type of ring
  \param ta size of the ring
  \param num number of ring(s) of that size
  \param ring the list(s) of atoms of the(these) ring(s)
*/
void send_atom_rings_id_opengl_ (int * st, int * at, int * id, int * ta, int * num, int ring[* num])
{
  if (ring != NULL)
  {
    int i;
    active_project -> atoms[* st][* at].rings[* id][* ta] = allocint (* num + 1);
    active_project -> atoms[* st][* at].rings[* id][* ta][0] = * num;
    for (i=0; i < * num; i++)
    {
      active_project -> atoms[* st][* at].rings[* id][* ta][i+1] = ring[i] - 1;
    }
  }
}

/*!
  \fn void allocate_all_rings_ (int * id, int * st, int * ta, int * nring)

  \brief allocate data to store ring statistics results for the glwin

  \param id the type of ring
  \param st the MD step
  \param ta the size of the ring
  \param nring the number of ring(s) of that size
*/
void allocate_all_rings_ (int * id, int * st, int * ta, int * nring)
{
  active_glwin -> all_rings[* id][* st][* ta - 1] = allocdint (* nring, * ta);
  active_glwin -> show_rpoly[* id][* st][* ta - 1] = allocint (* nring);
  active_glwin -> num_rings[* id][* st][* ta - 1] = * nring;
}
