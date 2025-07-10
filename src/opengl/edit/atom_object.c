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
* @file atom_object.c
* @short Functions to create atomic objects
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_object.c'
*
* Contains:
*

 - The functions to create atomic objects

*
* List of functions:

  int in_object_bond_list (atomic_object * object, int aid, int bid);
  int create_object_from_open_project (project * this_proj, int p);

  int * duplicate_z (int species, double * old_z);

  double get_object_dim (atomic_object * object);

  gboolean rebuild_atom_neighbors (project * this_proj, int step, atomic_object * object, int target, int aid, atom * at, gboolean * checked_at);

  void correct_pos_and_get_dim (atomic_object * object, gboolean adjust);
  void reconstruct_bonds (project * this_proj, int ifcl, int * bcid);
  void reconstruct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord);
  void correct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord);
  void create_object_from_library (int p);
  void clean_object_vois (project * this_proj, atomic_object * object, int * new_id, gboolean movtion);
  void clean_object_bonds (project * proj, int o_step, atomic_object * object, int * new_id, gboolean movtion);
  void add_object_atoms (atomic_object * this_object, project * this_proj,
                         int o_step, int numa, int * old_id, gboolean alloc_new_id, atom_search * remove);
  void adjust_object_frag_coord (atomic_object * object);
  void clean_this_object (int orig, int act, project * this_proj, atom_search * asearch);
  void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible);

  atomic_object * duplicate_atomic_object (atomic_object * old_obj);
  atomic_object * create_object_from_species (project * this_proj, int sid, atom_search * remove);
  atomic_object * create_object_from_selection (project * this_proj);
  atomic_object * create_object_from_atom_coordination (project * this_proj, int coord, int aid, atom_search * remove);
  atomic_object * create_object_from_overall_coordination (project * this_proj, int coord, int aid, atom_search * remove);
  atomic_object * create_object_from_frag_mol (project * this_proj, int coord, int geo, atom_search * remove);

  tint ulam_coord (glwin * view);

*/

#include "atom_edit.h"

extern atomic_object * cif_object;

/*!
  \fn double get_object_dim (atomic_object * object)

  \brief get estimate of an object dimension

  \param object
*/
double get_object_dim (atomic_object * object)
{
  double dmax = 0.0;
  vec3_t at, bt;
  vec3_t dist;
  int i, j;
  for (i=0; i<object -> atoms-1; i++)
  {
    at = vec3 (object -> at_list[i].x, object -> at_list[i].y, object -> at_list[i].z);
    for (j=i+1; j<object -> atoms; j++)
    {
      bt = vec3 (object -> at_list[j].x, object -> at_list[j].y, object -> at_list[j].z);
      dist = v3_sub(at, bt);
      dmax = max (dmax, v3_length(dist));
    }
  }
  return dmax+1.0;
}

/*!
  \fn void correct_pos_and_get_dim (atomic_object * object, gboolean adjust)

  \brief get the barycenter of the atomic coordinates of an object

  \param object the target insert object
  \param adjust center object coordinates or not
*/
void correct_pos_and_get_dim (atomic_object * object, gboolean adjust)
{
  int i;
  if (object -> baryc) g_free(object -> baryc);
  object -> baryc = allocdouble(3);
  for (i=0; i<object -> atoms; i++)
  {
    object -> baryc[0] += object -> at_list[i].x;
    object -> baryc[1] += object -> at_list[i].y;
    object -> baryc[2] += object -> at_list[i].z;
  }
  for (i=0; i<3; i++) object -> baryc[i] /= object -> atoms;
  if (adjust)
  {
    for (i=0; i<object -> atoms; i++)
    {
      object -> at_list[i].x -= object -> baryc[0];
      object -> at_list[i].y -= object -> baryc[1];
      object -> at_list[i].z -= object -> baryc[2];
    }
  }
  object -> dim = get_object_dim (object);
}

/*!
  \fn gboolean rebuild_atom_neighbors (project * this_proj, int step, atomic_object * object, int target, int aid, atom * at, gboolean * checked_at)

  \brief rebuild target atom id coordinates using PBC

  \param this_proj the target project
  \param step the MD step
  \param object the target insert object
  \param target the target atom id to correct
  \param aid the atom id
  \param at the target atom
  \param checked_at the list of already checked/corrected atom coordinates id
*/
gboolean rebuild_atom_neighbors (project * this_proj, int step, atomic_object * object, int target, int aid, atom * at, gboolean * checked_at)
{
  int i, j;
  distance dist;
  for (i=0; i<at -> numv; i++)
  {
    j = at -> vois[i];
    dist = distance_3d (& this_proj -> cell, step, at, & object -> at_list[j]);
    if (dist.pbc && ! checked_at[j])
    {
      object -> at_list[j].x = at -> x - dist.x;
      object -> at_list[j].y = at -> y - dist.y;
      object -> at_list[j].z = at -> z - dist.z;
    }
  }
  checked_at[aid] = TRUE;
  i = 0;
  for (j=0; j<object -> atoms; j++) if (checked_at[j]) i ++;
  if (i == target) return TRUE;
  for (i=0; i<at -> numv; i++)
  {
    j = at -> vois[i];
    if (! checked_at[j])
    {
      if (rebuild_atom_neighbors (this_proj, step, object, target, j, & object -> at_list[j], checked_at)) return TRUE;
    }
  }
  return FALSE;
}

/*!
  \fn void reconstruct_bonds (project * this_proj, int ifcl, int * bcid)

  \brief reconstruct the project bond(s)/clone(s) lists after reconstruction using PBC

  \param this_proj the target project
  \param ifcl number of clone bond(s) removed
  \param bcid the removed clone bond(s) atoms id
*/
void reconstruct_bonds (project * this_proj, int ifcl, int * bcid)
{
  int i, j, k, l;
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  int ** old_bid = NULL;
  old_bid = allocdint (this_proj -> modelgl -> bonds[o_step][0], 2);
  for (i=0; i<this_proj -> modelgl -> bonds[o_step][0]; i++)
  {
     for (j=0; j<2; j++) old_bid[i][j] = this_proj -> modelgl -> bondid[o_step][0][i][j];
  }
  g_free (this_proj -> modelgl -> bondid[o_step][0]);
  this_proj -> modelgl -> bondid[o_step][0] = allocdint (this_proj -> modelgl -> bonds[o_step][0]+ ifcl, 2);
  for (i=0; i<this_proj -> modelgl -> bonds[o_step][0]; i++)
  {
    for (j=0; j<2; j++) this_proj -> modelgl -> bondid[o_step][0][i][j] = old_bid[i][j];
  }
  if (old_bid) g_free (old_bid);
  old_bid = NULL;

  for (i=0; i<ifcl; i++)
  {
    j = bcid[i];
    k = this_proj -> modelgl -> bonds[o_step][0];
    for (l=0; l<2; l++)
    {
      this_proj -> modelgl -> bondid[o_step][0][k][l] = this_proj -> modelgl -> bondid[o_step][1][j][l];
    }
    this_proj -> modelgl -> bonds[o_step][0] ++;
    this_proj -> modelgl -> allbonds[0] ++;
    this_proj -> modelgl -> bondid[o_step][1][j][0] = -1;
  }

  vec3_t * old_clo = NULL;
  i = this_proj -> modelgl -> bonds[o_step][1] - ifcl;
  l = 0;
  if (i)
  {
    old_clo = g_malloc0 (i*sizeof*old_clo);
    old_bid = allocdint (i, 2);
    for (i=0; i<this_proj -> modelgl -> bonds[o_step][1]; i++)
    {
      if (this_proj -> modelgl -> bondid[o_step][1][i][0] > -1)
      {
        for (k=0; k<2; k++) old_bid[l][k] = this_proj -> modelgl -> bondid[o_step][1][i][k];
        old_clo[l].x = this_proj -> modelgl -> clones[o_step][i].x;
        old_clo[l].y = this_proj -> modelgl -> clones[o_step][i].y;
        old_clo[l].z = this_proj -> modelgl -> clones[o_step][i].z;
        l ++;
      }
    }
  }
  g_free (this_proj -> modelgl -> bondid[o_step][1]);
  this_proj -> modelgl -> bondid[o_step][1] = NULL;
  g_free (this_proj -> modelgl -> clones[o_step]);
  this_proj -> modelgl -> clones[o_step] = NULL;
  this_proj -> modelgl -> bonds[o_step][1] = this_proj -> modelgl -> allbonds[1] = l;
  if (l)
  {
    this_proj -> modelgl -> bondid[o_step][1] = allocdint (l, 2);
    this_proj -> modelgl -> clones[o_step] = g_malloc0 (l*sizeof*this_proj -> modelgl -> clones[o_step]);
    for (i=0; i<l; i++)
    {
      for (j=0; j<2; j++) this_proj -> modelgl -> bondid[o_step][1][i][j] = old_bid[i][j];
      this_proj -> modelgl -> clones[o_step][i].x = old_clo[i].x;
      this_proj -> modelgl -> clones[o_step][i].y = old_clo[i].y;
      this_proj -> modelgl -> clones[o_step][i].z = old_clo[i].z;
    }
    if (old_clo) g_free (old_clo);
    old_clo = NULL;
    if (old_bid) g_free (old_bid);
    old_bid = NULL;
  }
}

/*!
  \fn void reconstruct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord)

  \brief reconstruct object atomic coordinates using PBC

  \param this_object the target project
  \param this_proj the target insert object
  \param upcoord reconstruction of atomic coordinates using PBC ?
*/
void reconstruct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord)
{
  int h, i, j, k;
  if (this_proj -> coord -> totcoord[2])
  {
    int * tmp_multi = allocint (this_proj -> coord -> totcoord[2]);
    for (i=0; i<this_object -> atoms; i++)
    {
      j = this_object -> at_list[i].coord[2];
      tmp_multi[j] = 1;
    }
    gboolean * checked_at = allocbool (this_object -> atoms);
    h = (this_proj -> cell.npt) ? this_proj -> modelgl -> anim -> last -> img -> step : 0;
    for (i=0; i<this_proj -> coord -> totcoord[2]; i++)
    {
      if (tmp_multi[i])
      {
        k = 0;
        for (j=0; j<this_object -> atoms; j++)
        {
          checked_at[j] = FALSE;
          if (this_object -> at_list[j].coord[2] == i)
          {
            k ++;
          }
        }
        for (j=0; j<this_object -> atoms; j++)
        {
          if (this_object -> at_list[j].coord[2] == i)
          {
            rebuild_atom_neighbors (this_proj, h, this_object, k, j, & this_object -> at_list[j], checked_at);
          }
        }
      }
    }
    g_free (checked_at);
  }
  correct_pos_and_get_dim (this_object, TRUE);
  if (upcoord)
  {
    reconstruct_bonds (this_proj, this_object -> ifcl, this_object -> bcid);
    for (i=0; i<this_object -> atoms; i++)
    {
      j = this_object -> at_list[i].id;
      this_proj -> atoms[0][j].x = this_object -> at_list[i].x + this_object -> baryc[0];
      this_proj -> atoms[0][j].y = this_object -> at_list[i].y + this_object -> baryc[1];
      this_proj -> atoms[0][j].z = this_object -> at_list[i].z + this_object -> baryc[2];
      this_proj -> atoms[0][j].cloned = FALSE;
    }
    for (i=0; i<3; i++) this_proj -> modelgl -> saved_coord[i] = save_coordinates (this_proj, i);
  }
}

/*!
  \fn void correct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord)

  \brief correct the atomic coordinates for 'this_object'

  \param this_proj the target project
  \param this_object the target insert object
  \param upcoord reconstruction of atomic coordinates using PBC ?
*/
void correct_coordinates_for_object (project * this_proj, atomic_object * this_object, gboolean upcoord)
{
  if (this_object -> ifcl)
  {
    if (upcoord)
    {
      reconstruct_coordinates_for_object (this_proj, this_object, upcoord);
      this_object -> ifcl = 0;
      g_free (this_object -> bcid);
      this_object -> bcid = NULL;
    }
    else
    {
      correct_pos_and_get_dim (this_object, FALSE);
    }
  }
  else
  {
    correct_pos_and_get_dim (this_object, TRUE);
  }
}

/*!
  \fn tint ulam_coord (glwin * view)

  \brief shift insertion position for object not to have overlapping objects for multiple/repeated insertions

  \param view the target glwin
*/
tint ulam_coord (glwin * view)
{
  tint pos;
  int m;
  pos.a = pos.b = pos.c = 0;
  if (view -> builder_win) return pos;
  int p = view -> nth_copy;
  if (p > 0)
  {
    pos.c = p/9;
    m = p - pos.c*9;
    pos.b = m/3;
    pos.a = p - pos.c*9 - pos.b*3;
  }
  return pos;
}

int being_copied;
atomic_object * lib_object;

/*!
  \fn atomic_object * duplicate_atomic_object (atomic_object * old_obj)

  \brief duplicate an insert object

  \param old_obj the insert object to duplicate
*/
atomic_object * duplicate_atomic_object (atomic_object * old_obj)
{
  atomic_object * new_obj = g_malloc0 (sizeof*new_obj);
  new_obj -> origin = old_obj -> origin;
  new_obj -> type = old_obj -> type;
  new_obj -> dim = old_obj -> dim;
  new_obj -> name = g_strdup_printf ("%s", old_obj -> name);
  new_obj -> atoms = old_obj -> atoms;
  new_obj -> at_list = g_malloc0 (new_obj -> atoms*sizeof*new_obj -> at_list);
  int i;
  for (i=0; i<new_obj -> atoms; i++)
  {
    new_obj -> at_list[i] = * duplicate_atom (& old_obj -> at_list[i]);
  }
  new_obj -> old_z = duplicate_int (old_obj -> species, old_obj -> old_z);
  new_obj -> coord = duplicate_coord_info (old_obj -> coord);
  new_obj -> baryc = duplicate_double (3, old_obj -> baryc);

  new_obj -> occ = old_obj -> occ;
  new_obj -> species = old_obj -> species;
  if (old_obj -> bonds)
  {
    new_obj -> ibonds = g_malloc0 (old_obj -> bonds*sizeof*new_obj -> ibonds);
    int i;
    for (i=0; i<old_obj -> bonds; i++)
    {
      new_obj -> ibonds[i] = duplicate_int (2, old_obj -> ibonds[i]);
    }
  }
  new_obj -> bonds = old_obj -> bonds;
  return new_obj;
}

/*!
  \fn void create_object_from_library (int p)

  \brief create object using the molecular library

  \param p the project id of the library molecule in the workspace
*/
void create_object_from_library (int p)
{
  int i, j;
  lib_object = g_malloc0 (sizeof*lib_object);
  project * other_proj = get_project_by_id (p);
  i = other_proj -> natomes;
  lib_object -> type = FROM_LIBRARY;
  lib_object -> origin = p;
  lib_object -> name = g_strdup_printf ("%s", other_proj -> name);
  lib_object -> coord = duplicate_coord_info (other_proj -> coord);
  lib_object -> atoms = i;
  lib_object -> at_list = g_malloc0 (lib_object -> atoms*sizeof*lib_object -> at_list);
  lib_object -> occ = 1.0;
  lib_object -> species = other_proj -> nspec;
  lib_object -> old_z = allocint (other_proj -> nspec);
  for (j=0; j<other_proj -> nspec; j++) lib_object -> old_z[j] = (int) other_proj -> chemistry -> chem_prop[CHEM_Z][j];
  for (j=0; j<i; j++)
  {
    lib_object -> at_list[j] = * duplicate_atom (& other_proj -> atoms[0][j]);
  }
  correct_pos_and_get_dim (lib_object, TRUE);
  if (other_proj -> modelgl -> bonds[0][0])
  {
    i = other_proj -> modelgl -> bonds[0][0];
    lib_object -> ibonds = allocdint (i, 2);
    for (j=0; j<i; j++)
    {
      lib_object -> ibonds[j][0] = other_proj -> modelgl -> bondid[0][0][j][0];
      lib_object -> ibonds[j][1] = other_proj -> modelgl -> bondid[0][0][j][1];
    }
    lib_object -> bonds = i;
  }
  // Always one frag from library
  lib_object -> coord -> totcoord[2] = 1;
}

/*!
  \fn int in_object_bond_list (atomic_object * object, int aid, int bid)

  \brief is there a bond between atom aid and atom bid ?

  \param object the target insert object
  \param aid 1st atom id
  \param bid 2nd atom id
*/
int in_object_bond_list (atomic_object * object, int aid, int bid)
{
  int i;
  for (i=0; i<object -> bonds; i++)
  {
    if (object -> ibonds[i][0] == aid && object -> ibonds[i][1] == bid) return 1;
    if (object -> ibonds[i][1] == aid && object -> ibonds[i][0] == bid) return 1;
  }
  return 0;
}

/*!
  \fn void clean_object_vois (project * this_proj, atomic_object * object, int * new_id, gboolean movtion)

  \brief clean the object neigbours list

  \param this_proj the target project
  \param object the target insert object
  \param new_id list of atom's old id in the project 'this_proj'
  \param movtion reconstruction of atomic coordinates using PBC ?
*/
void clean_object_vois (project * this_proj, atomic_object * object, int * new_id, gboolean movtion)
{
  int i, j, k, l;
  int * tmpv;

  for (i=0; i<object -> atoms; i++)
  {
    if (object -> at_list[i].numv)
    {
      tmpv = duplicate_int (object -> at_list[i].numv, object -> at_list[i].vois);
      g_free (object -> at_list[i].vois);
      object -> at_list[i].vois = NULL;
      j = 0;
      for (k=0; k<object -> at_list[i].numv; k++)
      {
        l = tmpv[k];
        if (new_id[l])
        {
          j += (! movtion) ? in_object_bond_list(object, i, new_id[l]-1) : 1;
        }
      }
      if (j)
      {
        object -> at_list[i].vois = allocint (j);
        j = 0;
        for (k=0; k<object -> at_list[i].numv; k++)
        {
          l = tmpv[k];
          if (new_id[l])
          {
            if (! movtion || in_object_bond_list(object, i, new_id[l]-1))
            {
              object -> at_list[i].vois[j] = new_id[l]-1;
              j ++;
            }
          }
        }
      }
      object -> at_list[i].numv = j;
      g_free (tmpv);
    }
  }
}

/*!
  \fn void clean_object_bonds (project * proj, int o_step, atomic_object * object, int * new_id, gboolean movtion)

  \brief create the object bond list, and adjust the bond's atom id

  \param proj the target project
  \param o_step the MD step
  \param object the target insert object
  \param new_id list of atom's old id in the project 'this_proj'
  \param movtion reconstruction of atomic coordinates using PBC ?
*/
void clean_object_bonds (project * proj, int o_step, atomic_object * object, int * new_id, gboolean movtion)
{
  int h, i, j, k, l, m;
  gboolean doit;
  int ** tmpibonds = allocdint (proj -> modelgl -> bonds[o_step][0]+proj -> modelgl -> bonds[o_step][1], 2);
  if (proj -> modelgl -> bonds[o_step][1])
  {
    h = (movtion) ? 2 : 1;
  }
  else
  {
    h = 1;
  }
  i = 0;
  if (new_id) object -> bcid = allocint (proj -> modelgl -> bonds[o_step][1]);
  for (j=0; j<h; j++)
  {
    for (k=0; k<proj -> modelgl -> bonds[o_step][j]; k++)
    {
      l = proj -> modelgl -> bondid[o_step][j][k][0];
      m = proj -> modelgl -> bondid[o_step][j][k][1];
      doit = FALSE;
      if (new_id)
      {
        if (new_id[l] && new_id[m]) doit = TRUE;
      }
      else
      {
        doit = TRUE;
      }
      if (doit)
      {
        if (new_id)
        {
          tmpibonds[i][0] = new_id[l] - 1;
          tmpibonds[i][1] = new_id[m] - 1;
        }
        else
        {
          tmpibonds[i][0] = l;
          tmpibonds[i][1] = m;
        }
        i ++;
        if (j && new_id)
        {
          object -> bcid[object -> ifcl] = k;
          object -> ifcl ++;
        }
      }
    }
  }
  if (i)
  {
    object -> ibonds = allocdint (i, 2);
    for (j=0; j<i; j++)
    {
      object -> ibonds[j][0] = tmpibonds[j][0];
      object -> ibonds[j][1] = tmpibonds[j][1];
    }
  }
  g_free (tmpibonds);
  object -> bonds = i;
  if (new_id) clean_object_vois (proj, object, new_id, movtion);
}

/*!
  \fn void add_object_atoms (atomic_object * this_object, project * this_proj,
                             int o_step, int numa, int * old_id, gboolean alloc_new_id, atom_search * remove)

  \brief add atom list to insert object

  \param this_object the target insert object
  \param this_proj the target project
  \param o_step the MD step
  \param numa number of atom(s)
  \param old_id list of atom's old id in the project 'this_proj'
  \param alloc_new_id check_bonding check bonding ? (partial copy or not)
  \param remove remove search, if any
*/
void add_object_atoms (atomic_object * this_object, project * this_proj,
                       int o_step, int numa, int * old_id, gboolean check_bonding, atom_search * remove)
{
  int i, j;
  this_object -> atoms = numa;
  this_object -> at_list = g_malloc0 (this_object -> atoms*sizeof*this_object -> at_list);
  int * new_id = allocint (this_proj -> natomes);
  for (i=0; i<this_object -> atoms; i++)
  {
    j = old_id[i]-1;
    new_id[j] = i+1;
    if (remove) remove -> todo[j] = 1;
    this_object -> at_list[i] = * duplicate_atom (& this_proj -> atoms[o_step][j]);
    if (i)
    {
      this_object -> at_list[i].prev = & this_object -> at_list[i-1];
      this_object -> at_list[i-1].next = & this_object -> at_list[i];
    }
  }
  gboolean movtion = ((object_motion && this_proj -> modelgl -> rebuild[0][0]) || (! object_motion && this_proj -> modelgl -> rebuild[1][0]));
  if (check_bonding)
  {
    clean_object_bonds (this_proj, o_step, this_object, new_id, movtion);
  }
  correct_coordinates_for_object (this_proj, this_object, movtion);
  if (check_bonding)
  {
    check_coord_modification (this_proj, NULL, & this_object -> at_list[0], this_object, FALSE, FALSE);
  }
  g_free (old_id);
  g_free (new_id);
}

/*!
  \fn int * duplicate_z (int species, double * old_z)

  \brief duplicate z table when creating an object, integer is preferred to avoid comparison errors during action

  \param species the number of chemical species
  \param old_z the old z table to duplicate
*
*/
int * duplicate_z (int species, double * old_z)
{
  int i;
  int * new_z = allocint (species);
  for (i=0; i<species; i++)
  {
    new_z[i] = (int)old_z[i];
  }
  return new_z;
}

/*!
  \fn atomic_object * create_object_from_species (project * this_proj, int sid, atom_search * remove)

  \brief create object from all atom(s) of the same chemical species

  \param this_proj the target project
  \param sid the species id
  \param remove remove search, if any
*/
atomic_object * create_object_from_species (project * this_proj, int sid, atom_search * remove)
{
  int i, j;
  atomic_object * this_object =  g_malloc0 (sizeof*this_object);
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  this_object -> name = g_strdup_printf ("%s", this_proj -> chemistry -> label[sid]);
  i = this_proj -> chemistry -> nsps[sid];
  this_object -> type = FROM_SPEC;
  this_object -> origin = this_proj -> id;
  this_object -> atoms = i;
  this_object -> at_list = g_malloc0 (this_object -> atoms*sizeof*this_object -> at_list);
  this_object -> occ = 1.0;
  this_object -> coord = duplicate_coord_info (this_proj -> coord);
  this_object -> species = this_proj -> nspec;
  this_object -> old_z = duplicate_z (this_proj -> nspec, this_proj -> chemistry -> chem_prop[CHEM_Z]);
  int * new_id = NULL;
  new_id = allocint (this_proj -> natomes);
  gboolean check_bonding = FALSE;
  gboolean bonding = FALSE;
  i = 0;
  for (j=0; j<this_proj -> natomes; j++)
  {
    if (this_proj -> atoms[o_step][j].sp == sid)
    {
      if (this_proj -> atoms[o_step][j].numv) bonding = TRUE;
      new_id[i] = j+1;
      i ++;
    }
  }
  check_bonding = (i != this_proj -> natomes && bonding) ? TRUE : FALSE;
  add_object_atoms (this_object, this_proj, o_step, i, new_id, check_bonding, remove);
  return this_object;
}

/*!
  \fn atomic_object * create_object_from_selection (project * this_proj)

  \brief create object from atom selection

  \param this_proj the target project
*/
atomic_object * create_object_from_selection (project * this_proj)
{
  int i, j;
  atomic_object * this_object =  g_malloc0 (sizeof*this_object);
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  this_object -> name = g_strdup_printf ("From selection");
  i = 0;
  for (j=0; j<this_proj->natomes; j++)
  {
    if (pasted_todo[j]) i++;
  }
  this_object -> type = FROM_DATA;
  this_object -> origin = this_proj -> id;
  this_object -> atoms = i;
  this_object -> at_list = g_malloc0 (this_object -> atoms*sizeof*this_object -> at_list);
  this_object -> occ = 1.0;
  this_object -> coord = duplicate_coord_info (this_proj -> coord);
  this_object -> species = this_proj -> nspec;
  this_object -> old_z = duplicate_z (this_proj -> nspec, this_proj -> chemistry -> chem_prop[CHEM_Z]);
  i = 0;
  int * new_id = NULL;
  new_id = allocint(this_proj -> natomes);
  gboolean check_bonding = FALSE;
  gboolean bonding = FALSE;
  for (j=0; j<this_proj -> natomes; j++)
  {
    if (pasted_todo[j])
    {
      new_id[i] = j+1;
      if (this_proj -> atoms[o_step][j].numv) bonding = TRUE;
      i ++;
    }
  }
  check_bonding = (i != this_proj -> natomes && bonding) ? TRUE : FALSE;
  add_object_atoms (this_object, this_proj, o_step, i, new_id, check_bonding, NULL);
  if (pasted_todo)
  {
    g_free (pasted_todo);
    pasted_todo = NULL;
  }
  return this_object;
}

/*!
  \fn atomic_object * create_object_from_atom_coordination (project * this_proj, int coord, int aid, atom_search * remove)

  \brief create object from an atom and its nearest neighbors

  \param this_proj the target project
  \param coord 0 = total coordination, 1 = partial coordination
  \param aid the atom id
  \param remove remove search, if any
*/
atomic_object * create_object_from_atom_coordination (project * this_proj, int coord, int aid, atom_search * remove)
{
  int i, j, k;
  atomic_object * this_object =  g_malloc0 (sizeof*this_object);
  gchar * str;
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  i = this_proj -> atoms[o_step][aid].numv;
  k = this_proj -> atoms[o_step][aid].coord[coord];
  j = this_proj -> atoms[o_step][aid].sp;
  switch (coord)
  {
    case 0:
      if (i > 0)
      {
        str = g_strdup_printf ("%d-fold", i);
      }
      else
      {
        str = g_strdup_printf ("Isolated");
      }
      break;
    case 1:
      str = g_strdup_printf ("%s", env_name(this_proj, k, j, 1, NULL));
      break;
  }
  this_object -> name = g_strdup_printf ("%s - %s<sub>%d</sub>", str, this_proj -> chemistry -> label[j], aid+1);
  g_free (str);
  this_object -> type = - (coord + 3);
  this_object -> origin = this_proj -> id;
  this_object -> atoms = i+1;
  this_object -> at_list = g_malloc0 (this_object -> atoms*sizeof*this_object -> at_list);
  this_object -> occ = 1.0;
  this_object -> coord = duplicate_coord_info (this_proj -> coord);
  this_object -> species = this_proj -> nspec;
  this_object -> old_z = duplicate_int (this_proj -> nspec, (int *)this_proj -> chemistry -> chem_prop[CHEM_Z]);
  this_object -> at_list[0] = * duplicate_atom (& this_proj -> atoms[o_step][aid]);
  if (remove) remove_search -> todo[aid] = 1;
  gboolean movtion = (object_motion && this_proj -> modelgl -> rebuild[0][0]) || (! object_motion && this_proj -> modelgl -> rebuild[1][0]);
  if (this_proj -> atoms[o_step][aid].numv)
  {
    int * new_id = NULL;
    new_id = allocint (this_proj -> natomes);
    new_id[aid] = 1;
    for (i=0; i<this_proj -> atoms[o_step][aid].numv; i++)
    {
      j = this_proj -> atoms[o_step][aid].vois[i];
      if (remove) remove_search -> todo[j] = 1;
      new_id[j] = i+2;
      this_object -> at_list[i+1] = * duplicate_atom (& this_proj -> atoms[o_step][j]);
    }
    clean_object_bonds (this_proj, o_step, this_object, new_id, movtion);
    if (new_id)
    {
      g_free (new_id);
      new_id = NULL;
    }
    correct_coordinates_for_object (this_proj, this_object, movtion);
  }
  return this_object;
}

/*!
  \fn atomic_object * create_object_from_overall_coordination (project * this_proj, int coord, int aid, atom_search * remove)

  \brief create object from all the atom(s) that have the same exact coordination than the target atom

  \param this_proj the target project
  \param coord 0 = total coordination, 1 = partial coordination
  \param aid target atom id
  \param remove remove search, if any
*/
atomic_object * create_object_from_overall_coordination (project * this_proj, int coord, int aid, atom_search * remove)
{
  int i, j, k, l, m, n;
  atomic_object * this_object =  g_malloc0 (sizeof*this_object);
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  this_object -> coord = duplicate_coord_info (this_proj -> coord);
  this_object -> species = this_proj -> nspec;
  this_object -> old_z = duplicate_z (this_proj -> nspec, this_proj -> chemistry -> chem_prop[CHEM_Z]);
  this_object -> type = - (coord + 3);
  this_object -> origin = this_proj -> id;
  this_object -> occ = 1.0;
  int * new_id = NULL;
  new_id = allocint(this_proj -> natomes);
  gchar * str;
  gboolean check_bonding = FALSE;
  gboolean bonding = FALSE;
  switch (coord)
  {
    case 0:
      for (k=0; k<this_proj -> natomes; k++)
      {
        if (this_proj -> atoms[o_step][k].coord[0] == aid)
        {
          i = this_proj -> atoms[o_step][k].numv;
          break;
        }
      }
      if (i)
      {
        bonding = TRUE;
        str = g_strdup_printf ("%d-fold", i);
      }
      else
      {
        str = g_strdup_printf ("Isolated");
      }
      j = 0;
      for (k=0; k<this_proj -> natomes; k++)
      {
        if (this_proj -> atoms[o_step][k].numv == aid)
        {
          new_id[j] = k+1;
          j ++;
        }
      }
      break;
    case 1:
      j = 0;
      for (k=0; k<this_proj -> natomes; k++)
      {
        l = this_proj -> atoms[o_step][k].sp;
        m = this_proj -> atoms[o_step][k].coord[coord-1];
        for (n=0; n<l; n++) m += this_proj -> coord -> ntg[1][n];
        if (m == aid)
        {
          new_id[j] = k+1;
          if (this_proj -> atoms[o_step][k].numv) bonding = TRUE;
          j ++;
        }
      }
      str = g_strdup_printf ("%s", env_name(this_proj, l, aid, 1, NULL));
      break;
  }
  this_object -> name = g_strdup_printf ("All %s - atom(s)", str);
  g_free (str);
  check_bonding = (j != this_proj -> natomes && bonding) ? TRUE : FALSE;
  add_object_atoms (this_object, this_proj, o_step, j, new_id, check_bonding, remove);
  return this_object;
}

/*!
  \fn atomic_object * create_object_from_frag_mol (project * this_proj, int coord, int geo, atom_search * remove)

  \brief create object from a fragment or a molecule

  \param this_proj the target project
  \param coord 2 = fragment, 3 = molecule
  \param geo fragment or molecule id
  \param remove remove search, if any
*/
atomic_object * create_object_from_frag_mol (project * this_proj, int coord, int geo, atom_search * remove)
{
  int i, j;
  atomic_object * this_object =  g_malloc0 (sizeof*this_object);
  if (coord == 2)
  {
    this_object -> name = g_strdup_printf ("Fragment N°%d", geo+1);
  }
  else
  {
    this_object -> name = g_strdup_printf ("Molecule N°%d", geo+1);
  }
  int o_step = this_proj -> modelgl -> anim -> last -> img -> step;
  this_object -> type = - (coord + 3);
  this_object -> origin = this_proj -> id;
  this_object -> occ = 1.0;
  this_object -> coord = duplicate_coord_info (this_proj -> coord);
  this_object -> species = this_proj -> nspec;
  this_object -> old_z = duplicate_z (this_proj -> nspec, this_proj -> chemistry -> chem_prop[CHEM_Z]);
  int * new_id = NULL;
  new_id = allocint(this_proj -> natomes);
  gboolean check_bonding = FALSE;
  gboolean bonding = FALSE;
  i = 0;
  for (j=0; j<this_proj->natomes; j++)
  {
    if (this_proj -> atoms[o_step][j].coord[coord] == geo)
    {
      new_id[i] = j+1;
      if (this_proj -> atoms[o_step][j].numv) bonding = TRUE;
      i++;
    }
  }
  check_bonding = (i != this_proj -> natomes && bonding) ? TRUE : FALSE;
  add_object_atoms (this_object, this_proj, o_step, i, new_id, check_bonding, remove);
  return this_object;
}

/*!
  \fn void adjust_object_frag_coord (atomic_object * object)

  \brief adjust object number of fragment(s)

  \param object the target insert object
*/
void adjust_object_frag_coord (atomic_object * object)
{
  int i, j, k;
  int * corf = allocint (object -> coord -> totcoord[2]);
  int * rorf = allocint (object -> coord -> totcoord[2]);
  for (i=0; i<object -> atoms; i++)
  {
    j = object -> at_list[i].coord[2];
    rorf[j] ++;
  }
  j = k = 0;
  for (i=0; i<object -> coord -> totcoord[2]; i++)
  {
    corf[i] = k;
    if (rorf[i])
    {
      j++;
    }
    else
    {
      k++;
    }
  }
  object -> coord -> totcoord[2] = j;

  for (i=0; i<object -> atoms; i++)
  {
    j = corf[object -> at_list[i].coord[2]];
    object -> at_list[i].coord[2] -= j;
    object -> at_list[i].id = i;
  }
  remove_bonds_from_project (NULL, object, NULL, & object -> at_list[0], FALSE, FALSE);
}

/*!
  \fn int create_object_from_open_project (project * this_proj, int p)

  \brief create object from atom(s) of a project opened in the workspace

  \param this_proj the target project
  \param p the project id of the project that contains the atom(s) to copy
*/
int create_object_from_open_project (project * this_proj, int p)
{
  int i, j;
  lib_object =  g_malloc0 (sizeof*lib_object);
  project * other_proj;
  other_proj = get_project_by_id (p);
  int o_step = other_proj -> modelgl -> anim -> last -> img -> step;
  switch (this_proj -> modelgl -> other_status)
  {
    case 0:
      i = other_proj -> natomes - other_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected;
      lib_object -> name = g_strdup_printf ("All non-selected atom(s) from: %s", get_project_by_id(p) -> name);
      break;
    case 1:
      i = other_proj -> modelgl -> anim -> last -> img -> selected[0] -> selected;
      lib_object -> name = g_strdup_printf ("All selected atom(s) from: %s", get_project_by_id(p) -> name);
      break;
    case 2:
      i = other_proj -> natomes;
      lib_object -> name = g_strdup_printf ("All atom(s) from: %s", get_project_by_id(p) -> name);
      break;
  }
  lib_object -> type = FROM_PROJECT;
  lib_object -> origin = p;
  lib_object -> atoms = i;
  lib_object -> at_list = g_malloc0 (lib_object -> atoms*sizeof*lib_object -> at_list);
  lib_object -> occ = 1.0;
  lib_object -> coord = duplicate_coord_info (other_proj -> coord);
  lib_object -> species = other_proj -> nspec;
  lib_object -> old_z = duplicate_z (other_proj -> nspec, other_proj -> chemistry -> chem_prop[CHEM_Z]);
  int * new_id = NULL;
  gboolean check_bonding = FALSE;
  new_id = allocint (other_proj -> natomes);
  if (this_proj -> modelgl -> other_status == 2)
  {
    for (j=0; j<i; j++)
    {
      lib_object -> at_list[j] = * duplicate_atom (& other_proj -> atoms[o_step][j]);
      new_id[j] = j+1;
      if (j)
      {
        lib_object -> at_list[j].prev = & lib_object -> at_list[j-1];
        lib_object -> at_list[j-1].next = & lib_object -> at_list[j];
      }
    }
  }
  else
  {
    i = 0;
    for (j=0; j<other_proj -> natomes; j++)
    {
      if (other_proj -> atoms[o_step][j].pick[0] == this_proj -> modelgl -> other_status)
      {
        lib_object -> at_list[i] = * duplicate_atom (& other_proj -> atoms[o_step][j]);
        new_id[j] = i+1;
        if (i)
        {
          lib_object -> at_list[i].prev = & lib_object -> at_list[i-1];
          lib_object -> at_list[i-1].next = & lib_object -> at_list[i];
        }
        i ++;
      }
    }
  }
  check_bonding = (i != other_proj -> natomes && (other_proj -> modelgl -> bonds[o_step][0] || other_proj -> modelgl -> bonds[o_step][1])) ? TRUE : FALSE;
  clean_object_bonds (other_proj, o_step, lib_object, new_id, check_bonding);
  if (i < other_proj -> natomes)
  {
    for (j=0; j<lib_object -> atoms; j++) lib_object -> at_list[j].id = new_id[lib_object -> at_list[j].id]-1;
    if (check_bonding) check_coord_modification (other_proj, NULL, & lib_object -> at_list[0], lib_object, FALSE, FALSE);
  }
  correct_coordinates_for_object (other_proj, lib_object, TRUE);
  if (i < other_proj -> natomes && check_bonding) adjust_object_frag_coord (lib_object);

  return FROM_PROJECT;
}

/*!
  \fn void clean_this_object (int orig, int act, project * this_proj, atom_search * asearch)

  \brief clean object data

  \param orig - (fragmol id/species id +1), -1, or, orgin atom id
  \param act action in enum 'actions'
  \param this_proj the target project
  \param asearch the target atom search
*/
void clean_this_object (int orig, int act, project * this_proj, atom_search * asearch)
{
  atomic_object * tmp_object = NULL;
  atomic_object * object;
  switch (asearch -> action)
  {
    case REPLACE:
      object = (this_proj -> modelgl) ? this_proj -> modelgl -> atom_win -> to_be_inserted[act] : cif_object;
      break;
    default:
      object = this_proj -> modelgl -> atom_win -> to_be_moved[act];
      break;
  }
  while (object)
  {
    if ((object -> origin > -1 || object -> origin < -2) && object -> origin == orig)
    {
      if (object -> ibonds) g_free (object -> ibonds);
      if (object -> baryc) g_free (object -> baryc);
      if (object -> at_list) g_free (object -> at_list);
      if (object -> coord) g_free (object -> coord);
      object -> atoms = object -> bonds = 0;
      asearch -> in_selection --;
      if (object -> prev != NULL)
      {
        if (object -> next != NULL)
        {
          object -> next -> prev = object -> prev;
          object -> prev -> next = object -> next;
          tmp_object = object -> next;
          g_free (object);
          object = tmp_object;
        }
        else
        {
          object = object -> prev;
          g_free (object -> next);
          object -> next = NULL;
          break;
        }
      }
      else
      {
        if (object -> next != NULL)
        {
          object = object -> next;
          if (this_proj -> modelgl)
          {
            g_free (this_proj -> modelgl -> atom_win -> to_be_inserted[act]);
          }
          else
          {
            g_free (cif_object);
          }
          object -> prev = NULL;
          switch (asearch -> action)
          {
            case REPLACE:
              if (this_proj -> modelgl)
              {
                this_proj -> modelgl -> atom_win -> to_be_inserted[act] = object;
              }
              else
              {
                cif_object = object;
              }
              break;
            default:
              this_proj -> modelgl -> atom_win -> to_be_moved[act] = object;
              break;
          }
        }
        else
        {
          g_free (object);
          switch (asearch -> action)
          {
            case REPLACE:
              if (this_proj -> modelgl)
              {
                object = this_proj -> modelgl -> atom_win -> to_be_inserted[act] = NULL;
              }
              else
              {
                object = cif_object = NULL;
              }
              break;
            default:
              object = this_proj -> modelgl -> atom_win -> to_be_moved[act] = NULL;
              break;
          }
        }
      }
    }
    else
    {
      object = object -> next;
    }
  }
}

/*!
  \fn void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible)

  \brief to insert object in project

  \param stat in enum object_types
  \param orig - (fragmol id/species id +1), -1, or, orgin atom id
  \param this_proj the target project
  \param asearch the target atom search
  \param visible is the model edition window visible ?
*/
void to_insert_in_project (int stat, int orig, project * this_proj, atom_search * asearch, gboolean visible)
{
  int i, j;
  atomic_object * tmp_object = NULL;
  atomic_object * object;
  int act;
  if (asearch -> pointer[0].c == 3)
  {
    act = 0;
  }
  else if (asearch -> pointer[0].c == 5)
  {
    act = 1;
  }
  else
  {
    act = asearch -> pointer[0].c - 5;
  }
  if (! this_proj -> modelgl || ((! act || act == 3) && this_proj -> modelgl -> atom_win -> to_be_inserted[act]))
  {
    clean_this_object (orig, act, this_proj, asearch);
  }

  if (stat == FROM_DATA) lib_object = duplicate_atomic_object (copied_object);

  tint ulam;
  tmp_object = NULL;
  vec3_t coor_ins = vec3 (0.0,0.0,0.0);
  if (! visible)
  {
    if (act > 0)
    {
      if (this_proj -> modelgl)
      {
        if (this_proj -> modelgl -> anim && ! this_proj -> modelgl -> builder_win)
        {
          this_proj -> modelgl -> insert_coords = get_insertion_coordinates (this_proj -> modelgl);
          coor_ins = this_proj -> modelgl -> insert_coords;
        }
      }
    }
    else if (! act)
    {
      if (orig < -2 && asearch -> object && ! asearch -> mode)
      {
        int filter = get_asearch_filter (asearch);
        if (! filter)
        {
          tmp_object = create_object_from_species (this_proj, -orig-3, NULL);
        }
        else if (filter < 3)
        {
          tmp_object = create_object_from_atom_coordination (this_proj, filter-1, -orig-3, NULL);
        }
        else
        {
          tmp_object = create_object_from_frag_mol (this_proj, filter-1, -orig-3, NULL);
        }
        coor_ins = vec3 (tmp_object -> baryc[0], tmp_object -> baryc[1], tmp_object -> baryc[2]);
        if (tmp_object) g_free (tmp_object);
      }
      else if (orig > -1)
      {
        coor_ins = vec3 (this_proj -> atoms[0][orig].x, this_proj -> atoms[0][orig].y, this_proj -> atoms[0][orig].z);
      }
    }
  }
  if (stat > 0)
  {
    lib_object = g_malloc0 (sizeof*lib_object);
    lib_object -> type = stat;
    lib_object -> old_z = allocint (1);
    lib_object -> old_z[0] = (stat < 119) ? stat : 0.0;
    lib_object -> at_list = g_malloc0(sizeof*lib_object -> at_list);
    lib_object -> coord = g_malloc0 (sizeof*lib_object -> coord);
    lib_object -> coord -> species = 1;
    for (j=0; j<2; j++)
    {
      lib_object -> coord -> totcoord[j] = 1;
      lib_object -> coord -> ntg[j] = allocint (1);
      lib_object -> coord -> ntg[j][0] = 1;
      lib_object -> coord -> geolist[j] = allocdint (1, 1);
      if (j) lib_object -> coord -> partial_geo = alloctint (1, 1, 1);
    }
    lib_object -> coord -> totcoord[j] = 1;
    lib_object -> baryc = allocdouble (3);
    lib_object -> atoms = 1;
    lib_object -> occ = 1.0;
    lib_object -> species = 1;
    lib_object -> dim = get_object_dim (lib_object);
    if (stat > 119)
    {
      lib_object -> name = g_strdup_printf ("Empty position");
    }
    else
    {
      lib_object -> name = g_strdup_printf ("%s atom", periodic_table_info[stat].lab);
    }
  }

  if (this_proj -> modelgl)
  {
    ulam = ulam_coord (this_proj -> modelgl);
    if (this_proj -> modelgl -> atom_win -> to_be_inserted[act] == NULL)
    {
      this_proj -> modelgl -> atom_win -> to_be_inserted[act] = duplicate_atomic_object (lib_object);
      object = this_proj -> modelgl -> atom_win -> to_be_inserted[act];
    }
    else
    {
      object = this_proj -> modelgl -> atom_win -> to_be_inserted[act];
      while (object -> next) object = object -> next;
      object -> next = duplicate_atomic_object (lib_object);
      object -> next -> prev = object;
      object = object -> next;
    }
  }
  else
  {
    ulam.a = ulam.b = ulam.c = 0.0;
    if (cif_object == NULL)
    {
      cif_object = duplicate_atomic_object (lib_object);
      object = cif_object;
    }
    else
    {
      object = cif_object;
      while (object -> next) object = object -> next;
      object -> next = duplicate_atomic_object (lib_object);
      object -> next -> prev = object;
      object = object -> next;
    }
  }
  if (act)
  {
    object -> id = (object -> prev) ? object -> prev -> id + 1 : 0;
  }
  else
  {
    object -> id = (orig > -1) ? orig : (orig < -2) ? abs(orig) - 3 : abs(orig) - 1;
  }
  object -> origin = orig;

  for (i=0; i<object -> atoms; i++)
  {
    object -> at_list[i].x += coor_ins.x + object -> dim*ulam.a;
    object -> at_list[i].y += coor_ins.y + object -> dim*ulam.b;
    object -> at_list[i].z += coor_ins.z + object -> dim*ulam.c;
  }
  asearch -> in_selection ++;

  if (visible)
  {
    int * atid;
    if (act == 1 || act == 2)
    {
      if (object -> id > 0)
      {
        atid = duplicate_int (object -> id, asearch -> todo);
        g_free (asearch -> todo);
      }
      allocate_todo (asearch, object -> id+1);
      for (i=0; i<object -> id; i++)
      {
        asearch -> todo[i] = atid[i];
      }
      asearch -> todo[i] = 0;
      update_search_tree (asearch);
    }
  }
}
