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
* @file dlp_active.c
* @short Functions to retrieve data in the force field structure
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_active.c'
*
* Contains:
*

 - The functions to retrieve data in the force field structure

*
* List of functions:

  field_molecule * get_active_field_molecule_from_model_id (project * this_proj, int aid);
  field_molecule * get_active_field_molecule (int a);
  field_nth_body * get_active_body (int a, int b);
  field_external * get_active_external (int a);
  field_atom* get_active_atom (int a, int b);
  field_shell * get_active_shell (int a, int b);
  field_constraint * get_active_constraint (int a, int b);
  field_pmf * get_active_pmf (int a, int b);
  field_rigid * get_active_rigid (int a, int b);
  field_tethered * get_active_tethered (int a, int b);
  field_prop * get_active_prop (struct  field_prop * pr, int a);
  field_prop * get_active_prop_using_atoms (struct  field_prop * pr, int ti, int * ids);
  field_struct * get_active_struct (int s, int a, int b);

*/

#include "dlp_field.h"

/*!
  \fn field_molecule * get_active_field_molecule_from_model_id (project * this_proj, int aid)

  \brief retrieve field molecule from overall atom id in the model

  \param this_proj the target project
  \param aid the target atom id
*/
field_molecule * get_active_field_molecule_from_model_id (project * this_proj, int aid)
{
  int i;
  field_molecule * fmol = this_proj -> force_field[activef] -> first_molecule;
  field_atom* fat;
  while (fmol)
  {
    fat = fmol -> first_atom;
    while (fat)
    {
      for (i=0; i<fat -> num; i++)
      {
        if (fat -> list[i] == aid) return fmol;
      }
      fat = fat -> next;
    }
    fmol = fmol -> next;
  }
  return NULL;
}

/*!
  \fn field_molecule * get_active_field_molecule (int a)

  \brief retrieve field molecule

  \param a the id of the field molecule to retrieve
*/
field_molecule * get_active_field_molecule (int a)
{
  int i;
  field_molecule * tfmol = tmp_field -> first_molecule;
  for (i=0; i<a; i++)
  {
    if (tfmol -> next != NULL) tfmol = tfmol -> next;
  }
  return tfmol;
}

/*!
  \fn field_nth_body * get_active_body (int a, int b)

  \brief retrieve field nth body interaction

  \param a the id of the body interaction to retrieve
  \param b the type of body interaction
*/
field_nth_body * get_active_body (int a, int b)
{
  int i;
  field_nth_body * body;
  body = tmp_field -> first_body[b];
  for (i=0; i<a; i++)
  {
    if (body -> next != NULL) body = body -> next;
  }
  return body;
}

/*!
  \fn field_external * get_active_external (int a)

  \brief retrieve external field property

  \param a the id of the external field property to retrieve
*/
field_external * get_active_external (int a)
{
  int i;
  field_external * external;
  external = tmp_field -> first_external;
  for (i=0; i<a; i++)
  {
    if (external -> next != NULL) external = external -> next;
  }
  return external;
}

/*!
  \fn field_atom* get_active_atom (int a, int b)

  \brief retrieve field atom

  \param a the id of the field molecule
  \param b the id of the field atom to retrieve
*/
field_atom* get_active_atom (int a, int b)
{
  int i;
  field_atom* ato;
  ato = get_active_field_molecule (a) -> first_atom;
  for (i=0; i<b; i++)
  {
    if (ato -> next != NULL) ato = ato -> next;
  }
  return ato;
}

/*!
  \fn field_shell * get_active_shell (int a, int b)

  \brief retrieve shell property

  \param a the id of the field molecule
  \param b the id of the shell property to retrieve
*/
field_shell * get_active_shell (int a, int b)
{
  int i;
  field_shell * shl;
  shl = get_active_field_molecule (a) -> first_shell;
  for (i=0; i<b; i++)
  {
    if (shl -> next != NULL) shl = shl -> next;
  }
  return shl;
}

/*!
  \fn field_constraint * get_active_constraint (int a, int b)

  \brief retrieve constraint property

  \param a the id of the field molecule
  \param b the id of the constraint to retrieve
*/
field_constraint * get_active_constraint (int a, int b)
{
  int i;
  field_constraint * cons;
  cons = get_active_field_molecule (a) -> first_constraint;
  for (i=0; i<b; i++)
  {
    if (cons -> next != NULL) cons = cons -> next;
  }
  return cons;
}

/*!
  \fn field_pmf * get_active_pmf (int a, int b)

  \brief retrieve PMF property

  \param a the id of the field molecule
  \param b the id of the PMF property to retrieve
*/
field_pmf * get_active_pmf (int a, int b)
{
  int i;
  field_pmf * pmf;
  pmf = get_active_field_molecule (a) -> first_pmf;
  for (i=0; i<b; i++)
  {
    if (pmf -> next != NULL) pmf = pmf -> next;
  }
  return pmf;
}

/*!
  \fn field_rigid * get_active_rigid (int a, int b)

  \brief retrieve rigid property

  \param a the id of the field molecule
  \param b the id of the rigid property to retrieve
*/
field_rigid * get_active_rigid (int a, int b)
{
  int i;
  field_rigid * rig;
  rig = get_active_field_molecule (a) -> first_rigid;
  for (i=0; i<b; i++)
  {
    if (rig -> next != NULL) rig = rig -> next;
  }
  return rig;
}

/*!
  \fn field_tethered * get_active_tethered (int a, int b)

  \brief retrieve tethered property

  \param a the id of the field molecule
  \param b the id of the tethered to retrieve
*/
field_tethered * get_active_tethered (int a, int b)
{
  int i;
  field_tethered * tet;
  tet = get_active_field_molecule (a) -> first_tethered;
  for (i=0; i<b; i++)
  {
    if (tet -> next != NULL) tet = tet -> next;
  }
  return tet;
}

/*!
  \fn field_prop * get_active_prop (struct  field_prop * pr, int a)

  \brief the field molecule structural property id to retrieve

  \param pr the pointer on the field molecule properties to browse
  \param a the id of the field molecule property to retrieve
*/
field_prop * get_active_prop (struct  field_prop * pr, int a)
{
  field_prop * prop;
  prop = pr;
  int i;
  for (i=0; i<a; i++)
  {
    if (prop -> next != NULL) prop = prop -> next;
  }
  return prop;
}

/*!
  \fn field_prop * get_active_prop_using_atoms (struct  field_prop * pr, int ti, int * ids)

  \brief retrieve field molecule structural property using atoms

  \param pr the pointer on the field molecule properties to browse
  \param ti the number of atoms for this property
  \param ids the atoms to search for
*/
field_prop * get_active_prop_using_atoms (struct  field_prop * pr, int ti, int * ids)
{
  field_prop * prop = NULL;
  prop = pr;
  gboolean done;
  int i;
  while (prop != NULL)
  {
    done = TRUE;
    for (i=0; i<ti; i++)
    {
      if (prop -> aid[i] != ids[i])
      {
        done = FALSE;
        break;
      }
    }
    if (done) break;
    prop = prop -> next;
  }
  return prop;
}

/*!
  \fn field_struct * get_active_struct (int s, int a, int b)

  \brief retrieve field structural property

  \param s the type of structural property
  \param a the field molecule id
  \param b the structural property id to retrieve
*/
field_struct * get_active_struct (int s, int a, int b)
{
  int i;
  field_struct * str;
  str = get_active_field_molecule (a) -> first_struct[s];
  for (i=0; i<b; i++)
  {
    if (str -> next != NULL) str = str -> next;
  }
  return str;
}
