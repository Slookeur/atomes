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
* @file initmol.c
* @short Functions to retrieve fragments data from Fortran90 \n
         Functions to retrieve molecules data from Fortran90 \n
         Functions to analyze molecule topology
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'initmol.c'
*
* Contains:
*

 - The functions to retrieve fragments data from Fortran90
 - The functions to retrieve molecules data from Fortran90
 - The functions to analyze molecule topology

*
* List of functions:

  int * merge_mol_data (int val_a, int val_b, int table_a[val_a], int table_b[val_b]);

  gboolean are_identical_molecules (search_molecule * mol_a, search_molecule * mol_b);

  void duplicate_molecule (molecule * new_mol, search_molecule * old_mol);
  void allocate_mol_for_step_ (int * sid, int * mol_in_step);
  void allocate_mol_data_ ();
  void send_mol_neighbors_ (int * stp, int * mol, int * aid, int * nvs, int neigh[* nvs]);
  void update_mol_details (search_molecule * mol, int sp, int cp);
  void send_mol_details_ (int * stp, int * mol, int * ats, int * sps, int spec_in_mol[* sps], int atom_in_mol[* ats]);
  void free_search_molecule_data (search_molecule * smol);
  void setup_molecules_ (int * stepid);
  void setup_menu_molecules_ ();
  void setup_fragments_ (int * sid, int coord[active_project -> natomes]);

  search_molecule * duplicate_search_molecule (search_molecule * old_mol);

*/

#include "global.h"
#include "interface.h"
#include "bind.h"
#include "color_box.h"
#include "glwindow.h"
#include "initcoord.h"

typedef struct search_molecule search_molecule;
struct search_molecule
{
  int id;                                 // Molecule id number
  int md;                                 // MD step
  int multiplicity;                       // Multiplicity
  int * fragments;                        // Fragments list
  int natoms;                             // Number of atoms
  int * atoms;                            // Temporary atom list
  int nspec;                              // Number of chemical species
  int * species;                          // Number of atom by species
  int nbonds;                             // Number of chemical bonds
  int ** pbonds;                          // Number of chemical bonds by geometries
  int nangles;                            // Number of bond angles
  int *** pangles;                        // Number of bond angles by geometries
  int ** lgeo;                            // list of coordination spheres (by species)
  search_molecule * next;
  search_molecule * prev;
};

int * pgeo;
search_molecule * tmp_search = NULL;
search_molecule ** in_calc_mol = NULL;
extern molecule * tmp_mol;

/*!
  \fn void duplicate_molecule (molecule * new_mol, search_molecule * old_mol)

  \brief create a copy of a molecule data structure

  \param new_mol the new molecule
  \param old_mol the molecule to copy
*/
void duplicate_molecule (molecule * new_mol, search_molecule * old_mol)
{
  new_mol -> id = old_mol -> id;
  new_mol -> md = old_mol -> md;
  new_mol -> multiplicity = old_mol -> multiplicity;
  new_mol -> fragments = duplicate_int (old_mol -> multiplicity, old_mol -> fragments);
  new_mol -> natoms = old_mol -> natoms;
  new_mol -> nspec = old_mol -> nspec;
  new_mol -> species = duplicate_int (active_project -> nspec, old_mol -> species);
}

/*!
  \fn search_molecule * duplicate_search_molecule (search_molecule * old_mol)

  \brief create a copy of a search molecule data structure

  \param old_mol the search molecule data structure to copy
*/
search_molecule * duplicate_search_molecule (search_molecule * old_mol)
{
  search_molecule * new_mol = g_malloc0(sizeof*new_mol);
  new_mol -> id = old_mol -> id;
  new_mol -> md = old_mol -> md;
  new_mol -> multiplicity = old_mol -> multiplicity;
  new_mol -> fragments = duplicate_int (old_mol -> multiplicity, old_mol -> fragments);
  new_mol -> natoms = old_mol -> natoms;
  new_mol -> nspec = old_mol -> nspec;
  new_mol -> species = duplicate_int (active_project -> nspec, old_mol -> species);
  int j, k;
  new_mol -> pbonds = g_malloc0(active_project -> coord -> totcoord[1]*sizeof*new_mol -> pbonds);
  new_mol -> pangles = g_malloc0(active_project -> coord -> totcoord[1]*sizeof*new_mol -> pangles);
  if (old_mol -> atoms)
  {
    new_mol -> atoms = duplicate_int (old_mol -> natoms*old_mol -> multiplicity, old_mol -> atoms);
  }
  new_mol -> nbonds = old_mol -> nbonds;
  if (new_mol -> nbonds)
  {
    for (j=0; j<active_project -> coord -> totcoord[1]; j++)
    {
      new_mol -> pbonds[j] = duplicate_int (active_project -> coord -> totcoord[1], old_mol -> pbonds[j]);
    }
    new_mol -> nangles = old_mol -> nangles;
    if (new_mol -> nangles)
    {
      for (j=0; j<active_project -> coord -> totcoord[1]; j++)
      {
        new_mol -> pangles[j] = g_malloc0(active_project -> coord -> totcoord[1]*sizeof*new_mol -> pangles[j]);
        for (k=0; k<active_project -> coord -> totcoord[1]; k++)
        {
          new_mol -> pangles[j][k] = duplicate_int (active_project -> coord -> totcoord[1], old_mol -> pangles[j][k]);
        }
      }
    }
  }
  new_mol -> lgeo = g_malloc0(active_project -> nspec*sizeof*new_mol -> lgeo);
  for (j=0; j<active_project -> nspec; j++)
  {
    new_mol -> lgeo[j] = duplicate_int (active_project -> coord -> totcoord[1], old_mol -> lgeo[j]);
  }
  return new_mol;
}

/*!
  \fn void allocate_mol_for_step_ (int * sid, int * mol_in_step)

  \brief allocate the data to store molecule information for a MD step

  \param sid the MD step
  \param mol_in_step the number of molecule(s) for this MD step
*/
void allocate_mol_for_step_ (int * sid, int * mol_in_step)
{
  in_calc_mol[* sid -1] = g_malloc0(* mol_in_step*sizeof*in_calc_mol[* sid -1]);
  active_project -> modelfc -> mol_by_step[* sid - 1] = * mol_in_step;
}

/*!
  \fn void allocate_mol_data_ ()

  \brief allocate data to store molecule information
*/
void allocate_mol_data_ ()
{
  int i;
  if (active_project -> modelfc != NULL)
  {
    if (active_project -> modelfc -> mols != NULL)
    {
      g_free (active_project -> modelfc -> mols);
      active_project -> modelfc -> mols = NULL;
    }
    active_project -> modelfc = NULL;
  }
  in_calc_mol = g_malloc0(active_project -> steps*sizeof*in_calc_mol);
  active_project -> modelfc = g_malloc0(sizeof*active_project -> modelfc);
  active_project -> modelfc -> mols = g_malloc0(active_project -> steps*sizeof*active_project -> modelfc -> mols);
  active_project -> modelfc -> mol_by_step = allocint (active_project -> steps);
  for (i=0; i<2; i++)
  {
    if (active_project -> force_field[i]) g_free (active_project -> force_field[i]);
    active_project -> force_field[i] = NULL;
  }
  pgeo = g_malloc0((active_project -> nspec+1)*sizeof*pgeo);
  for (i=1; i<active_project -> nspec+1; i++)
  {
    pgeo[i] = pgeo[i-1] + active_coord -> ntg[1][i-1];
  }
}

/*!
  \fn void send_mol_neighbors_ (int * stp, int * mol, int * aid, int * nvs, int neigh[*nvs])

  \brief update molecule typology information from Fortran90

  \param stp the MD step
  \param mol the molecule id
  \param aid the atom id
  \param nvs the number of neighbor(s) for this atom
  \param neigh the list of neighbor(s) for this atom
*/
void send_mol_neighbors_ (int * stp, int * mol, int * aid, int * nvs, int neigh[* nvs])
{
  int i, j, k, l, m, n, o, p, q, r, s, t, u;
  search_molecule * tmp_mol = & in_calc_mol[* stp - 1][* mol - 1];
  i = active_project -> atoms[* stp - 1][* aid - 1].sp;
  j = active_project -> atoms[* stp - 1][* aid - 1].coord[1];
  k = pgeo[i] + j;
  for (l=0; l < * nvs; l++)
  {
    m =  neigh[l]-1;
    n = active_project -> atoms[tmp_mol -> md][m].sp;
    o = active_project -> atoms[tmp_mol -> md][m].coord[1];
    p = pgeo[n] + o;
    tmp_mol -> pbonds[k][p] ++;
  }
  if (* nvs > 1)
  {
    for (l=0; l<*nvs-1; l++)
    {
      m =  neigh[l]-1;
      n = active_project -> atoms[tmp_mol -> md][m].sp;
      o = active_project -> atoms[tmp_mol -> md][m].coord[1];
      p = pgeo[n] + o;
      for (q= l+1; q<*nvs; q++)
      {
        r =  neigh[q]-1;
        s = active_project -> atoms[tmp_mol -> md][r].sp;
        t = active_project -> atoms[tmp_mol -> md][r].coord[1];
        u = pgeo[s] + t;
        tmp_mol -> pangles[p][k][u] ++;
        tmp_mol -> pangles[u][k][p] ++;
      }
    }
  }
}

/*!
  \fn void update_mol_details (search_molecule * mol, int sp, int cp)

  \brief update molecule information

  \param mol the molecule to update
  \param sp the chemical species
  \param cp the partial coordination id
*/
void update_mol_details (search_molecule * mol, int sp, int cp)
{
  int i, j, k, l, m;
  for (i=0; i<active_project -> nspec; i++)
  {
    j = active_coord -> partial_geo[sp][cp][i];
    mol -> nbonds += j;
    if (j)
    {
      for (k=0; k<active_project -> nspec; k++)
      {
        l = active_coord -> partial_geo[sp][cp][k];
        if (k != i || l > 1)
        {
          m = (j+l)*((j+l)-1)/2;
          mol -> nangles += m;
        }
      }
    }
  }
}

/*!
  \fn void send_mol_details_ (int * stp, int * mol, int * ats, int * sps, int spec_in_mol[*sps], int atom_in_mol[*ats])

  \brief overall molecule information from Fortran90

  \param stp the MD step
  \param mol the molecule id
  \param ats the number of atom(s) in the molecule
  \param sps the number of chemical species in the molecule
  \param spec_in_mol the number of atom(s) by chemical species in the molecule
  \param atom_in_mol the list of atom(s) in the molecule
*/
void send_mol_details_ (int * stp, int * mol, int * ats, int * sps, int spec_in_mol[* sps], int atom_in_mol[* ats])
{
  int i, j, k, l;
  search_molecule * tmp_mol = & in_calc_mol[* stp - 1][* mol - 1];
  tmp_mol -> id = * mol - 1;
  tmp_mol -> md = * stp - 1;
  tmp_mol -> multiplicity = 1;
  tmp_mol -> fragments = allocint (1);
  tmp_mol -> fragments[0] = * mol - 1;
  tmp_mol -> natoms = * ats;
  tmp_mol -> lgeo = allocdint (active_project -> nspec, active_coord -> totcoord[1]);
  tmp_mol -> pbonds = allocdint (active_coord -> totcoord[1], active_coord -> totcoord[1]);
  tmp_mol -> pangles = alloctint (active_coord -> totcoord[1], active_coord -> totcoord[1], active_coord -> totcoord[1]);
  tmp_mol -> atoms = duplicate_int (* ats, atom_in_mol);
  for (i=0; i< * ats; i++)
  {
    j = atom_in_mol[i]-1;
    k = active_project -> atoms[0][j].sp;
    l = active_project -> atoms[* stp - 1][j].coord[1];
    tmp_mol -> lgeo[k][pgeo[k]+l] ++;
    update_mol_details (tmp_mol, k, l);
  }
  j = 0;
  for (i=0; i<active_project -> nspec; i++)
  {
    if (spec_in_mol[i] > 0) j++;
  }
  tmp_mol -> nspec = j;
  tmp_mol -> species = duplicate_int (active_project -> nspec, spec_in_mol);
  tmp_mol -> nbonds /= 2;
  tmp_mol -> nangles /= 2;
}

/*!
  \fn gboolean are_identical_molecules (search_molecule * mol_a, search_molecule * mol_b)

  \brief test if 2 molecules are identicals

  \param mol_a the 1st molecule
  \param mol_b the 2nd molecule
*/
gboolean are_identical_molecules (search_molecule * mol_a, search_molecule * mol_b)
{
  int i, j, k;
/*#ifdef DEBUG
  g_debug ("AID:: mol_a -> id= %d, mol_b-> id= %d", mol_a -> id, mol_b -> id);
  g_debug ("AID:: mol_a -> natoms= %d, mol_b-> natoms= %d", mol_a -> natoms, mol_b -> natoms);
  g_debug ("AID:: mol_a -> nspec= %d, mol_b-> nspec= %d", mol_a -> nspec, mol_b -> nspec);
  g_debug ("AID:: mol_a -> nbonds= %d, mol_b-> nbonds= %d", mol_a -> nbonds, mol_b -> nbonds);
  g_debug ("AID:: mol_a -> nangles= %d, mol_b-> nangles= %d", mol_a -> nangles, mol_b -> nangles);
#endif*/
  if (mol_a -> md != mol_b -> md) return FALSE;
  if (mol_a -> natoms != mol_b -> natoms) return FALSE;
  if (mol_a -> nspec != mol_b -> nspec) return FALSE;
  for (i=0; i < active_project -> nspec; i++)
  {
    if (mol_a -> species[i] != mol_b -> species[i]) return FALSE;
  }

  if (mol_a -> nbonds != mol_b -> nbonds) return FALSE;
  if (mol_a -> nangles != mol_b -> nangles) return FALSE;

  for (i=0; i < active_project -> nspec; i++)
  {
    for (j=0; j< active_coord -> totcoord[1]; j++)
    {
      if (mol_a -> lgeo[i][j] != mol_b -> lgeo[i][j]) return FALSE;
    }
  }

  if (mol_a -> nbonds > 1)
  {
    for (i=0; i < active_coord -> totcoord[1]; i++)
    {
      for (j=0; j < active_coord -> totcoord[1]; j++)
      {
        if (mol_a -> pbonds[i][j] != mol_b -> pbonds[i][j]) return FALSE;
      }
    }
    if (mol_a -> nangles > 1)
    {
      for (i=0; i < active_coord -> totcoord[1]; i++)
      {
        for (j=0; j < active_coord -> totcoord[1]; j++)
        {
          for (k=0; k < active_coord -> totcoord[1]; k++)
          {
            if (mol_a -> pangles[i][j][k] != mol_b -> pangles[i][j][k]) return FALSE;
          }
        }
      }
    }
  }
  return TRUE;
}

/*!
  \fn int * merge_mol_data (int val_a, int val_b, int table_a[val_a], int table_b[val_b])

  \brief merge molecule a and molecule b data

  \param val_a multiplicity for molecule a
  \param val_b multiplicity for molecule b
  \param table_a the list of molecular fragment(s) for molecule a
  \param table_b the list of molecular fragment(s) for molecule b
*/
int * merge_mol_data (int val_a, int val_b, int table_a[val_a], int table_b[val_b])
{
  int * p_data;
  int i;
  p_data = allocint (val_a + val_b);
  for (i=0; i<val_a; i++)
  {
    p_data[i] = table_a[i];
  }
  for (i=0; i<val_b; i++)
  {
    p_data[val_a+i] = table_b[i];
  }
  return p_data;
}

/*!
  \fn void free_search_molecule_data (search_molecule * smol)

  \brief free search molecule data structure

  \param smol the search molecule data structure to free
*/
void free_search_molecule_data (search_molecule * smol)
{
  int i, j;
  if (smol -> atoms)
  {
    g_free (smol -> atoms);
    smol -> atoms = NULL;
  }
  if (smol -> pbonds)
  {
    for (i=0; i<active_coord -> totcoord[1]; i++)
    {
      if (smol -> pbonds[i])
      {
        g_free (smol -> pbonds[i]);
        smol -> pbonds[i] = NULL;
      }
    }
    g_free (smol -> pbonds);
    smol -> pbonds = NULL;
  }
  if (smol -> pangles)
  {
    for (i=0; i<active_coord -> totcoord[1]; i++)
    {
      if (smol -> pangles[i])
      {
        for (j=0; j<active_coord -> totcoord[1]; j++)
        {
          if (smol -> pangles[i][j])
          {
            g_free (smol -> pangles[i][j]);
            smol -> pangles[i][j] = NULL;
          }
        }
        g_free (smol -> pangles[i]);
        smol -> pangles[i] = NULL;
      }
    }
    g_free (smol -> pangles);
    smol -> pangles = NULL;
  }
  if (smol -> lgeo)
  {
    for (i=0; i<active_project -> nspec; i++)
    {
      if (smol -> lgeo[i])
      {
        g_free (smol -> lgeo[i]);
        smol -> lgeo[i] = NULL;
      }
    }
    g_free (smol -> lgeo);
    smol -> lgeo = NULL;
  }
  g_free (smol -> species);
  smol -> species = NULL;
  g_free (smol -> fragments);
  smol -> fragments = NULL;
}

/*!
  \fn void setup_molecules_ (int * stepid)

  \brief setup molecule data

  \param stepid the MD step
*/
void setup_molecules_ (int * stepid)
{
  int i, j, k, l, m, n;
  search_molecule * mtmp_at, * mtmp_bt;
  search_molecule * first_mol = NULL;
  search_molecule * tmp_mol;
  gboolean add_it;
  int * tmp_data;
  i = * stepid -1;
  j = 0;
  for (k=0; k<active_project -> modelfc -> mol_by_step[i]; k++)
  {
    mtmp_bt = & in_calc_mol[i][k];
    add_it = TRUE;
    mtmp_at = first_mol;
    while (mtmp_at)
    {
      if (are_identical_molecules (mtmp_at, mtmp_bt))
      {
        tmp_data = merge_mol_data (mtmp_at -> multiplicity, mtmp_bt -> multiplicity, mtmp_at -> fragments, mtmp_bt -> fragments);
        g_free (mtmp_at -> fragments);
        mtmp_at -> fragments = duplicate_int (mtmp_at -> multiplicity+mtmp_bt -> multiplicity, tmp_data);
        g_free (tmp_data);
        tmp_data = merge_mol_data (mtmp_at -> natoms*mtmp_at -> multiplicity, mtmp_bt -> natoms*mtmp_bt -> multiplicity, mtmp_at -> atoms, mtmp_bt -> atoms);
        g_free (mtmp_at -> atoms);
        mtmp_at -> atoms = duplicate_int (mtmp_at -> multiplicity+mtmp_bt -> multiplicity, tmp_data);
        g_free (tmp_data);
        mtmp_at -> multiplicity ++;
        add_it = FALSE;
        break;
      }
      mtmp_at = mtmp_at -> next;
    }
    if (add_it)
    {
      if (first_mol)
      {
        mtmp_at = first_mol;
        while (mtmp_at -> next) mtmp_at = mtmp_at -> next;
        mtmp_at -> next = duplicate_search_molecule (mtmp_bt);
        mtmp_at -> next -> prev = mtmp_at;
      }
      else
      {
        first_mol = duplicate_search_molecule (mtmp_bt);
      }
      j ++;
    }
    free_search_molecule_data (mtmp_bt);
  }
  g_free (in_calc_mol[i]);
  in_calc_mol[i] = NULL;
  active_project -> modelfc -> mol_by_step[i] = j;
  active_project -> modelfc -> mols[i] = g_malloc0(j*sizeof*active_project -> modelfc -> mols[i]);
  tmp_mol = first_mol;
  for (k=0; k<j; k++)
  {
    l = tmp_mol -> natoms*tmp_mol -> multiplicity;
    for (m=0; m<l; m++)
    {
      n = tmp_mol -> atoms[m] - 1;
      active_project -> atoms[i][n].coord[3] = k;
    }
    duplicate_molecule (& active_project -> modelfc -> mols[i][k], tmp_mol);
    active_project -> modelfc -> mols[i][k].id = k;
    if (k < j-1) tmp_mol = tmp_mol -> next;
  }
  tmp_mol = first_mol;
  while (tmp_mol)
  {
    free_search_molecule_data (tmp_mol);
    if (tmp_mol -> next)
    {
      tmp_mol = tmp_mol -> next;
      g_free (tmp_mol -> prev);
    }
    else
    {
      g_free (tmp_mol);
      tmp_mol = NULL;
    }
  }
}

/*!
  \fn void setup_menu_molecules_ ()

  \brief prepare the menu elements related to molecule(s)
*/
void setup_menu_molecules_ ()
{
  int i, j;
  g_free (in_calc_mol);
  in_calc_mol = NULL;
  g_free (pgeo);
  pgeo = NULL;
  i = 0;
  for (j=0; j<active_project -> steps; j++)
  {
    i = max (i, active_project -> modelfc -> mol_by_step[j]);
  }
  active_coord -> totcoord[3] = i;
  i = 3;
  init_menu_fragmol_ (& i);
}

/*!
  \fn void setup_fragments_ (int * sid, int coord[active_project->natomes])

  \brief atom(s) fragment(s) information from Fortran90

  \param sid the MD step
  \param coord the fragment(s) information
*/
void setup_fragments_ (int * sid, int coord[active_project -> natomes])
{
  int i;
  for (i=0; i < active_project -> natomes; i++)
  {
    active_project -> atoms[* sid-1][i].coord[2] = coord[i] - 1;
  }
}
