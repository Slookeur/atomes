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
* @file dlp_comp.c
* @short Functions to compare and adjust field body parameters
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_comp.c'
*
* Contains:
*

 - The functions to compare and adjust field body parameters

*
* List of functions:

  void compare_body (gchar * fatom, field_nth_body * new_body, int n_body, field_nth_body * old_body, int o_body);
  void compare_non_bonded (gchar * fatom);

*/

#include "global.h"
#include "interface.h"
#include "glwindow.h"
#include "glview.h"
#include "dlp_field.h"

field_nth_body * comp_fbody;

extern void duplicate_nbody_params (field_nth_body * new_fbody, field_nth_body * old_fbody);

/*!
  \fn void compare_body (gchar * fatom, field_nth_body * new_body, int n_body, field_nth_body * old_body, int o_body)

  \brief compare, and if require ajdust, two lists of field body properties

  \param fatom the name of the field atom to search for
  \param new_body 1st list of field body property(ies)
  \param n_body the number of field body in this 1st list
  \param old_body 2nd list of field body property(ies)
  \param o_body the number of field body in this 2nd list
*/
void compare_body (gchar * fatom, field_nth_body * new_body, int n_body, field_nth_body * old_body, int o_body)
{
  int i, j, k, l, m, n, o, p;
  field_nth_body * tmp_new = new_body;
  field_nth_body * tmp_old;
  field_nth_body * new_guy;
  field_nth_body * old_one;
  field_nth_body * new_one;
  gboolean doit;

  for (i=0; i<n_body; i++)
  {
    tmp_old = old_body;
    for (j=0; j<o_body; j++)
    {
      doit = FALSE;
      if (g_strcmp0 (fatom, get_active_atom (tmp_new -> ma[0][0], tmp_new -> a[0][0]) -> name) == 0 &&
          g_strcmp0 (fatom, get_active_atom (tmp_new -> ma[1][0], tmp_new -> a[1][0]) -> name) == 0) new_guy = tmp_new;
      for (k=0; k<2; k++)
      {
        for (l=0; l<2; l++)
        {
          if (g_strcmp0 (get_active_atom (tmp_new -> ma[k][0], tmp_new -> a[k][0]) -> name, get_active_atom (tmp_old -> ma[l][0], tmp_old -> a[l][0]) -> name) == 0) doit = TRUE;
          if (doit) break;
        }
        if (g_strcmp0 (get_active_atom (tmp_new -> ma[k][0], tmp_new -> a[k][0]) -> name, get_active_atom (tmp_old -> ma[k][0], tmp_old -> a[k][0]) -> name) == 0) doit = TRUE;
        if (doit) break;
      }
      // Same atoms copy the body params
      if (doit) duplicate_nbody_params (tmp_new, tmp_old);
      if (tmp_old -> next != NULL) tmp_old = tmp_old -> next;
    }
    if (tmp_new -> next != NULL) tmp_new = tmp_new -> next;
  }

  for (i=1; i<5; i++)
  {
    tmp_fbody = tmp_field -> first_body[i];
    k = body_at (i);
    new_one = get_active_body (tmp_field -> nbody[i], i);
    m = 0;
    for (j=0; j<tmp_field -> nbody[i]; j++)
    {
      for (l=0; l<k; l++)
      {
        if (tmp_fbody -> na[l] > 0)
        {
          doit = FALSE;
          for (n=0; n<tmp_fbody -> na[l]; n++)
          {
            if (g_strcmp0 (get_active_atom (tmp_fbody -> ma[l][n], tmp_fbody -> a[l][n]) -> name, fatom) == 0)
            {
              doit = TRUE;
              o = n;
              break;
            }
          }
          if (doit)
          {
            if (tmp_fbody -> na[l] > 1)
            {
              // Add new nth_body
              new_one -> next = duplicate_field_nth_body (tmp_fbody);
              old_one = duplicate_field_nth_body (tmp_fbody);
              new_one -> next -> prev = new_one;
              new_one = new_one -> next;
              new_one -> id ++;
              new_one -> na[l] = new_guy -> na[0];
              new_one -> a[l] = duplicate_int (new_guy -> na[0], new_guy -> a[0]);
              new_one -> ma[l] = duplicate_int (new_guy -> na[0], new_guy -> ma[0]);
              p = 0;
              old_one -> na[l] --;
              old_one -> a[l] = NULL;
              old_one -> a[l] = g_malloc0(old_one -> na[l]*sizeof*old_one -> a[l]);
              old_one -> ma[l] = NULL;
              old_one -> ma[l] = g_malloc0(old_one -> na[l]*sizeof*old_one -> ma[l]);
              for (n=0; n<tmp_fbody -> na[l]; n++)
              {
                if (n != o)
                {
                  old_one -> a[l][p] = tmp_fbody -> a[l][n];
                  old_one -> ma[l][p] = tmp_fbody -> ma[l][n];
                 }
              }
              tmp_fbody -> a[l] = NULL;
              tmp_fbody -> ma[l] = NULL;
              tmp_fbody -> na[l] --;
              tmp_fbody -> a[l] = duplicate_int (tmp_fbody -> na[l], old_one -> a[l]);
              tmp_fbody -> ma[l] = duplicate_int (tmp_fbody -> na[l], old_one -> ma[l]);
              m ++;
            }
            else if (tmp_fbody -> na[l] == 1)
            {
              tmp_fbody -> a[l][0] = new_guy -> a[0][0];
              tmp_fbody -> ma[l][0] = new_guy -> ma[0][0];
            }
          }
        }
      }
      if (tmp_fbody -> next != NULL) tmp_fbody = tmp_fbody -> next;
    }
    tmp_field -> nbody[i] += m;
  }

  tmp_field -> first_body[0] = NULL;
  tmp_field -> first_body[0] = duplicate_field_nth_body (new_body);
  tmp_new = tmp_field -> first_body[0];
  tmp_old = new_body;
  for (i=1; i<n_body; i++)
  {
    tmp_new -> next = duplicate_field_nth_body (tmp_old -> next);
    tmp_new -> next -> prev = tmp_new;
    tmp_new = tmp_new -> next;
    tmp_old = tmp_old -> next;
  }
  tmp_field -> nbody[0] = n_body;
}

/*!
  \fn void compare_non_bonded (gchar * fatom)

  \brief compare non bond interaction parameters

  \param fatom the name of the target field atom
*/
void compare_non_bonded (gchar * fatom)
{
  int nbody = init_vdw (FALSE);
  // comp_fbody is prepared in init_vdw
  compare_body (fatom, comp_fbody, nbody, tmp_field -> first_body[0], tmp_field -> nbody[0]);
}
