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
* @file initchain.c
* @short Functions collecting chain(s) data from Fortran90 \n
         Functions allocating chain(s) data from Fortran90
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'initchain.c'
*
* Contains:
*

 - The functions collecting chain(s) data from Fortran90
 - The functions allocating chain(s) data from Fortran90

*
* List of functions:

  void send_chains_opengl_ (int * st, int * ta, int * ri, int nchain[* ta]);
  void send_atom_chains_id_opengl_ (int * st, int * at, int * ta, int * num, int nchain[* num]);
  void allocate_all_chains_ (int * st, int * ta, int * nring);

*/

#include "global.h"
#include "interface.h"
#include "bind.h"
#include "color_box.h"
#include "glwindow.h"

/*!
  \fn void send_chains_opengl_ (int * st, int * ta, int * ri, int nchain[* ta])

  \brief getting the chain data elemnts from Fortran90

  \param st the MD step
  \param ta the chain size
  \param ri the chain id
  \param nchain the chain element(s)
*/
void send_chains_opengl_ (int * st, int * ta, int * ri, int nchain[* ta])
{
  int i;
  for (i=0; i< * ta; i++)
  {
    active_glwin -> all_chains[* st][* ta - 1][* ri][i] = nchain[i] - 1;
  }
}

/*!
  \fn void send_atom_chains_id_opengl_ (int * st, int * at, int * ta, int * num, int nchain[*num])

  \brief allocate atom chains data from Fortran90 information

  \param st the MD step
  \param at the atom id
  \param ta the chain size
  \param num the number of chain(s)
  \param nchain the chain id(s)
*/
void send_atom_chains_id_opengl_ (int * st, int * at, int * ta, int * num, int nchain[* num])
{
  if (nchain != NULL)
  {
    int i;
    active_project -> atoms[* st][* at].chain[* ta - 1] = allocint (* num + 1);
    active_project -> atoms[* st][* at].chain[* ta - 1][0] = * num;
    for (i=0; i < * num; i++)
    {
      active_project -> atoms[* st][* at].chain[* ta - 1][i+1] = nchain[i] - 1;
    }
  }
}

/*!
  \fn void allocate_all_chains_ (int * st, int * ta, int * nring)

  \brief allocate chains data from Fortran90 information

  \param st the MD step
  \param ta the number of chain size(s)
  \param nring the number of chain(s) of that size
*/
void allocate_all_chains_ (int * st, int * ta, int * nring)
{
  active_glwin -> all_chains[* st][* ta - 1] = allocdint (* nring, * ta);
  active_glwin -> num_chains[* st][* ta - 1] = * nring;
}
