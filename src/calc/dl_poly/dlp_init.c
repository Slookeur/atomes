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
* @file dlp_init.c
* @short Functions to initialize the creation of a classical force field
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_init.c'
*
* Contains:
*

 - The functions to initialize the creation of a classical force field

*
* List of functions:

  int get_position_in_field_atom_from_model_id (int fat, int at);
  int get_field_atom_id_from_model_id (field_molecule * fmol, int at);
  int get_fragment_atom_id_from_model_id (field_molecule * fmol, int at);
  int get_atom_id_in_fragment_from_model_id (int frag, int at);
  int set_atom_id (field_atom* at, int c, int p, int id);
  int get_struct_id_from_atom_id (int ids, int * aid);
  int prepare_field_atom (int i, int j, int k, int l, int m);
  int test_for_bonds (field_atom* at, field_atom* bt);
  int prepare_field_struct (int ids, int sid, int yes_no_num, int * aid);
  int bonds_between_atoms (int n, field_atom* at, field_atom* bt, int a, int b);
  int test_for_angles (field_atom* at,
                       field_atom* bt,
                       field_atom* ct);
  int angles_from_bonds (int n,
                         field_atom* at,
                         field_atom* bt,
                         field_atom* ct);
  int test_for_dihedrals (field_atom* at,
                          field_atom* bt,
                          field_atom* ct,
                          field_atom* dt);
  int dihedrals_from_angles (int n,
                             field_atom* at,
                             field_atom* bt,
                             field_atom* ct,
                             field_atom* dt);
  int impropers_inversion (int n, int stru,
                           int at, int bt, int ct, int dt,
                           int a, int b, int c, int d);
  int coord_sphere_multiplicity (atom * at, int id, gboolean set_atom_id, gboolean in_field_atom);
  int find_neighbor_loop (int frag, int aid, int fai,
                          field_atom* fat,
                          field_neighbor * ngmb,
                          int sid, gboolean save_it);
  int setup_atomic_weight (int seq);
  int init_vdw (gboolean init);

  gboolean was_not_created_struct (int ids, int num, int * aid);
  gboolean in_bond (int at, int bd[2]);
  gboolean are_neighbors (field_neighbor * ngb, int at);
  gboolean are_in_bond (atom ato, int at);
  gboolean is_numbering_possible (int frag);
  gboolean is_this_numbering_possible_for_this_atom (int frag, field_neighbor * ngma, int atom);
  gboolean id_n_fold_atoms_in_fragment (int frag, int limit, int num_ngb, int search_type,
                                        int init, field_neighbor * ngma_init);
  gboolean id_atoms_in_fragment (int frag, int seed);

  gchar * set_field_atom_name (field_atom* ato, field_molecule * mol);

  void init_all_atoms (int i);
  void init_all_bonds ();
  void init_all_angles ();
  void init_all_dihedrals ();
  void init_all_impropers_inversions (int stru);
  void get_weight (int seq);
  void find_atom_id_in_field_molecule ();
  void clean_field_struct_list (field_struct * stru);
  void init_all_field_struct (gboolean init);
  void init_field_molecule (int i);
  void init_dlpoly_field (classical_field * new_field);
  void init_lammps_field (classical_field * new_field);

  field_atom* init_field_atom (int id, int type, int at, int nat, int coo, int * list);
  field_shell * init_field_shell (int id, int ia, int ib);
  field_constraint * init_field_constraint (int id, int ia, int ib);
  field_pmf * init_field_pmf (int id, int num[2], int * list[2], float * w[2]);
  field_rigid * init_field_rigid (int id, int num, int * list);
  field_tethered * init_field_tethered (int id, int num);
  field_prop * init_field_prop (int ti, int key, gboolean show, gboolean use);
  field_struct * init_field_struct (int st, int ai, int an, int * aid);
  field_nth_body * init_field_nth_body (int bi, int bd, int * na, int ** ma, int ** ba);
  field_external * init_field_external (int bi);
  field_neighbor * get_init_neighbor (int a);

  classical_field * create_force_field_data_structure (int ai);

*/

#include "dlp_field.h"
#include "interface.h"

extern void print_all_field_struct (field_molecule * mol, int str);

int * astr;
float val;
extern int * duplicate_int (int num, int * old_val);
extern float * duplicate_float (int num, float * old_val);
int id_atom;
int multi;
int a_multi;

/*!
  \fn int get_position_in_field_atom_from_model_id (int fat, int at)

  \brief retrieve atom position id in field atom id using atom id in model

  \param fat the target field atom
  \param at the target model atom id
*/
int get_position_in_field_atom_from_model_id (int fat, int at)
{
  int i;
  field_atom* tmp_ft = get_active_atom (tmp_fmol -> id, fat);
  for (i=0; i<tmp_ft -> num; i++)
  {
    if (tmp_ft -> list[i] == at) return i;
  }
  return -1;
}

/*!
  \fn int get_field_atom_id_from_model_id (field_molecule * fmol, int at)

  \brief retrieve field atom id using atom id in model

  \param fmol the target field molecule
  \param at the target model atom id
*/
int get_field_atom_id_from_model_id (field_molecule * fmol, int at)
{
  int i;
  field_atom* fat = fmol -> first_atom;
  while (fat)
  {
    for (i=0; i< fat -> num; i++)
    {
      if (at == fat -> list[i]) return fat -> id;
    }
    fat = fat -> next;
  }
  return -1;
}

/*!
  \fn int get_fragment_atom_id_from_model_id (field_molecule * fmol, int at)

  \brief retrieve fragment atom id using atom id in model

  \param fmol the target field molecule
  \param at the target model atom id
*/
int get_fragment_atom_id_from_model_id (field_molecule * fmol, int at)
{
  int i;
  field_atom* fat = fmol -> first_atom;
  while (fat)
  {
    for (i=0; i< fat -> num; i++)
    {
      if (at == fat -> list[i]) return fat -> list_id[i];
    }
    fat = fat -> next;
  }
  return -1;
}

/*!
  \fn int get_atom_id_in_fragment_from_model_id (int frag, int at)

  \brief retrieve field atom id in fragment from model id

  \param frag the fragment id
  \param at the atom id in the model
*/
int get_atom_id_in_fragment_from_model_id (int frag, int at)
{
  int i, j, k;
  field_atom* tmp_ft;
  for (i=0; i< tmp_fmol -> mol -> natoms; i++)
  {
    if (tmp_fmol -> atoms_id[i][frag].a > -1 &&  tmp_fmol -> atoms_id[i][frag].b > -1)
    {
      tmp_ft = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][frag].a);
      j = tmp_fmol -> atoms_id[i][frag].b;
      k = tmp_ft -> list[j];
      if (k == at) return i;
    }
  }
  return -1;
}

/*!
  \fn int set_atom_id (field_atom* at, int c, int p, int id)

  \brief set new fragment/molecule atom parameters

  \param at the target field atom
  \param c the fragment id
  \param p the list id in field atom
  \param id the field atom id
*/
int set_atom_id (field_atom* at, int c, int p, int id)
{
  if (c == tmp_fmol -> fragments[0])
  {
    id_atom ++;
    tmp_fmol -> atoms_id[id_atom][0].a = id;
    tmp_fmol -> atoms_id[id_atom][0].b = p;
    return id_atom;
  }
  else
  {
    return -1;
  }
}

/*!
  \fn gchar * set_field_atom_name (field_atom* ato, field_molecule * mol)

  \brief get name string for field atom

  \param ato the target field atom
  \param mol the target field molecule
*/
gchar * set_field_atom_name (field_atom* ato, field_molecule * mol)
{
  int i, j;
  gchar * str;
  field_atom* tmpatf;
  switch (ato -> type)
  {
    case SPEC_ONLY:
      str = exact_name(tmp_proj -> chemistry -> label[ato -> sp]);
      break;
    case OTHER:
      tmpatf = mol -> first_atom;
      j = 1;
      while (tmpatf)
      {
        if (tmpatf -> sp == ato -> sp) j ++;
        tmpatf = tmpatf -> next;
      }
      str = exact_name(g_strdup_printf ("%s-%d-%d", tmp_proj -> chemistry -> label[ato -> sp], j, mol -> id+1));
      break;
    default:
      i = ato -> list[0];
      j = tmp_proj -> atoms[0][i].coord[ato -> type-1];
      switch (ato -> type)
      {
        case TOTAL_COORD_AND_SPEC:
          str = g_strdup_printf ("%s-%d-%d",
                                 exact_name(tmp_proj -> chemistry -> label[ato -> sp]),
                                 tmp_coord -> geolist[0][ato -> sp][j],
                                 mol -> id+1);
          break;
        case PARTIAL_COORD_AND_SPEC:
          str = g_strdup_printf ("%s-%d-%d",
                                 exact_name(tmp_proj -> chemistry -> label[ato -> sp]),
                                 j+1,
                                 mol -> id+1);
          /*str = g_strdup_printf ("%s-%d",
                                 exact_name (env_name (tmp_proj, j, ato -> sp, ato -> type, NULL)),
                                 mol -> id+1);*/
          break;
      }
  }
  return str;
}

/*!
  \fn field_atom* init_field_atom (int id, int type, int at, int nat, int coo, int * list)

  \brief initialize a new type of field atom

  \param id the new field atom id
  \param type the new field atom coordination type
  \param at the new field atom species
  \param nat the number of atom(s) of for this field atom type
  \param coo the coordination id in coordination type, if any
  \param list the list of atoms that match this type of field atom
*/
field_atom* init_field_atom (int id, int type, int at, int nat, int coo, int * list)
{
  int h, i, j, k, l, m, n;
  field_atom* ato;
  ato = g_malloc0 (sizeof*ato);
  ato -> id = id;
  ato -> type = type;
  ato -> num = nat;
  ato -> list = allocint (ato -> num);
  ato -> list_id = allocint (ato -> num);
  ato -> frozen_id = allocbool (ato -> num);
  h = 0;
  switch (type)
  {
    case OTHER:
      for (i=0; i<tmp_fmol -> multi; i++)
      {
        for (j=0; j < nat/tmp_fmol -> multi; j++)
        {
          k = list[j];
          l = tmp_fmol -> atoms_id[k][i].a;
          m = tmp_fmol -> atoms_id[k][i].b;
          n = get_active_atom (tmp_fmol -> id, l) -> list[m];
          ato -> list[h] = n;
          ato -> list_id[h] = k;
          ato -> frozen_id[h] = FALSE;
          tmp_fmol -> atoms_id[k][i].a = ato -> id;
          tmp_fmol -> atoms_id[k][i].b = h;
          tmp_proj -> atoms[0][n].faid = ato -> id;
          h ++;
        }
      }
      break;
    default:
      for (i=0; i < tmp_fmol -> multi; i++)
      {
        j = tmp_fmol -> fragments[i];
        for (k=0; k < tmp_proj -> natomes; k++)
        {
          if (tmp_proj -> atoms[0][k].coord[2] == j && tmp_proj -> atoms[0][k].sp == at)
          {
            if (type == SPEC_ONLY)
            {
              ato -> list[h] = k;
              ato -> frozen_id[h] = FALSE;
              ato -> list_id[h] = set_atom_id (ato, tmp_proj -> atoms[0][k].coord[2], h, id);
              h ++;
            }
            else
            {
              if (tmp_proj -> atoms[0][k].coord[type-1] == coo)
              {
                ato -> list[h] = k;
                ato -> frozen_id[h] = FALSE;
                ato -> list_id[h] = set_atom_id (ato, tmp_proj -> atoms[0][k].coord[2], h, id);
                h ++;
              }
            }
          }
        }
      }
      break;
  }
#ifdef DEBUG
  if (h != ato -> num) g_debug ("Something is wrong in with this field atom: id= %d, sp= %d, co= %d", id, at, coo);
#endif
  ato -> sp = at;

  ato -> mass = 0.0;//get_force_field_atom_mass (at, 0);
  ato -> fid = (ato -> mass == 0.0) ? -1 : tmp_field -> type;
  ato -> afid = (ato -> mass == 0.0) ? -1 : 0;
  ato -> mass = (ato -> mass == 0.0) ? tmp_proj -> chemistry -> chem_prop[CHEM_M][at] : ato -> mass;
  ato -> name = g_strdup_printf ("%s", set_field_atom_name (ato, tmp_fmol));
  ato -> charge = 0.0;
  ato -> frozen = 0;
  ato -> show = FALSE;
  ato -> next = NULL;
  ato -> prev = NULL;
  return ato;
}

/*!
  \fn field_shell * init_field_shell (int id, int ia, int ib)

  \brief initialize field core shell interaction

  \param id the new shell id
  \param ia 1st atom id in field molecule
  \param ib 2nd atom id in field molecule
*/
field_shell * init_field_shell (int id, int ia, int ib)
{
  field_shell * shell;
  shell = g_malloc0 (sizeof*shell);
  shell -> id = id;
  shell -> ia[0] = ia;
  shell -> ia[1] = ib;
  shell -> k2 = 0.0;
  shell -> k4 = 0.0;
  shell -> show = FALSE;
  shell -> use = TRUE;
  shell -> next = NULL;
  shell -> prev = NULL;
  return shell;
}

/*!
  \fn field_constraint * init_field_constraint (int id, int ia, int ib)

  \brief initialize field bond constraint

  \param id the new field constraint id
  \param ia 1st atom id in field molecule
  \param ib 2nd atom id in field molecule
*/
field_constraint * init_field_constraint (int id, int ia, int ib)
{
  field_constraint * cons;
  cons = g_malloc0 (sizeof*cons);
  cons -> id = id;
  cons -> ia[0] = ia;
  cons -> ia[1] = ib;
  cons -> av = -1.0;
  cons -> length = 0.0;
  cons -> show = FALSE;
  cons -> use = TRUE;
  cons -> next = NULL;
  cons -> prev = NULL;
  return cons;
}

/*!
  \fn field_pmf * init_field_pmf (int id, int num[2], int * list[2], float * w[2])

  \brief initialize new field mean force potential

  \param id the id of the new field PMF
  \param num the numbers of atom(s)
  \param list the lists of atom(s)
  \param w the weight list
*/
field_pmf * init_field_pmf (int id, int num[2], int * list[2], float * w[2])
{
  field_pmf * pmf;
  pmf = g_malloc0 (sizeof*pmf);
  pmf -> id = id;
  pmf -> av = -1.0;
  pmf -> length = 0.0;
  int i;
  if (num != NULL)
  {
    for (i=0; i<2; i++)
    {
      pmf -> num[i] = num[i];
      pmf -> list[i] = duplicate_int (num[i], list[i]);
      pmf -> weight[i] = duplicate_float (num[i], w[i]);
    }
  }
  else
  {
    for (i=0; i<2; i++)
    {
      pmf -> num[i] = 0;
      pmf -> list[i] = NULL;
      pmf -> weight[i] = NULL;
    }
  }
  pmf -> show = FALSE;
  pmf -> use = TRUE;
  pmf -> next = NULL;
  pmf -> prev = NULL;
  return pmf;
}

/*!
  \fn field_rigid * init_field_rigid (int id, int num, int * list)

  \brief initialize new field rigid constraint

  \param id the id of the new field rigid
  \param num the number of atom(s) to fix
  \param list the list of atom(s) to fix
*/
field_rigid * init_field_rigid (int id, int num, int * list)
{
  field_rigid * rig;
  rig = g_malloc0 (sizeof*rig);
  rig -> id = id;
  rig -> num = num;
  rig -> list = NULL;
  if (list != NULL) rig -> list = duplicate_int (num, list);
  rig -> show = FALSE;
  rig -> use = TRUE;
  rig -> next = NULL;
  rig -> prev = NULL;
  return rig;
}

/*!
  \fn field_tethered * init_field_tethered (int id, int num)

  \brief initialize new field tethered potential

  \param id the new field tethered potential id
  \param num the atom id in the field molecule
*/
field_tethered * init_field_tethered (int id, int num)
{
  field_tethered * tet;
  tet = g_malloc0 (sizeof*tet);
  tet -> id = id;
  tet -> num = num;
  tet -> key = 0;
  tet -> val = allocfloat (fvalues[activef][0][0]);
  tet -> show = FALSE;
  tet -> use = TRUE;
  tet -> next = NULL;
  tet -> prev = NULL;
  return tet;
}

/*!
  \fn field_prop * init_field_prop (int ti, int key, gboolean show, gboolean use)

  \brief initialize new field molecule structural property

  \param ti the type of field structural element to initialize
  \param key the formalism to use
  \param show visualize in the 3D window (yes / no)
  \param use use to create the force field input file (yes / no)
*/
field_prop * init_field_prop (int ti, int key, gboolean show, gboolean use)
{
  field_prop * prop;
  prop = g_malloc0 (sizeof*prop);
  prop -> aid = allocint (struct_id(ti+7));
  prop -> key = key;
  prop -> pid = -1;
  prop -> fpid = -1;
  prop -> val = allocfloat (fvalues[activef][ti+1][key]);
  prop -> show = show;
  prop -> use = use;
  prop -> next = NULL;
  prop -> prev = NULL;
  return prop;
}

/*!
  \fn int get_struct_id_from_atom_id (int ids, int * aid)

  \brief retrieve field structural element id from a list of atom id

  \param ids the type of structural element
  \param aid the list of atom id
*/
int get_struct_id_from_atom_id (int ids, int * aid)
{
  int i, j;
  field_struct * stru = tmp_fmol -> first_struct[ids];
  j = struct_id (ids+7);
  int res = 0;
  while (stru)
  {
    res = stru -> id + 1;
    for (i=0; i<j; i++)
    {
      if (stru -> aid[i] != aid[i])
      {
        res = 0;
        break;
      }
    }
    if (! res)
    {
      res = -(stru -> id + 1);
      for (i=0; i<j; i++)
      {
        if (stru -> aid[i] != aid[j-1-i])
        {
          res = 0;
          break;
        }
      }
    }
    if (res != 0) break;
    stru = stru -> next;
  }
  return res;
}

/*!
  \fn gboolean was_not_created_struct (int ids, int num, int * aid)

  \brief was this structural element already created ?

  \param ids the type of structural element
  \param num the number of atoms
  \param aid the list of atom id
*/
gboolean was_not_created_struct (int ids, int num, int * aid)
{
  if (num == 0) return TRUE;
  int j, k;
  k = struct_id(ids+7);
  tmp_fstr = tmp_fmol -> first_struct[ids];
  gboolean res;
  while (tmp_fstr)
  {
    res = FALSE;
    if (ids < 6)
    {
      for (j=0; j<k; j++)
      {
        if (tmp_fstr -> aid[j] != aid[j])
        {
         res = TRUE;
         break;
        }
      }
      if (res)
      {
        res = FALSE;
        for (j=0; j<k; j++)
        {
          if (tmp_fstr -> aid[k-1-j] != aid[j])
          {
            res = TRUE;
            break;
          }
        }
      }
    }
    else if (ids == 6)
    {
      for (j=0; j<k; j++)
      {
        if (tmp_fstr -> aid[j] != aid[j])
        {
         res = TRUE;
         break;
        }
      }
      if (res)
      {
        if (tmp_fstr -> aid[0] != aid[0])
        {
          res = TRUE;
        }
        else if (tmp_fstr -> aid[3] != aid[3])
        {
          res = TRUE;
        }
        else if (tmp_fstr -> aid[1] == aid[2] && tmp_fstr -> aid[2] == aid[1])
        {
          res = FALSE;
        }
      }
    }
    else
    {
      if (tmp_fstr -> aid[0] != aid[0])
      {
        res = TRUE;
      }
      else
      {
        if (tmp_fstr -> aid[1] != aid[1])
        {
          res = TRUE;
        }
        else if (tmp_fstr -> aid[2] == aid[2] && tmp_fstr -> aid[3] == aid[3])
        {
          res = FALSE;
        }
        else if (tmp_fstr -> aid[2] == aid[3] && tmp_fstr -> aid[3] == aid[2])
        {
          res = FALSE;
        }
        else
        {
          res = TRUE;
        }
      }
    }
    if (! res) return FALSE;
    tmp_fstr = tmp_fstr -> next;
  }
  return TRUE;
}

/*!
  \fn field_struct * init_field_struct (int st, int ai, int an, int * aid)

  \brief initialize field molecule new structural element

  \param st the type of structural element (0 = bond, 1 = angle, 2 = dihedral)
  \param ai the id of the new structural property
  \param an the number of atoms
  \param aid the list of field atoms
*/
field_struct * init_field_struct (int st, int ai, int an, int * aid)
{
  field_struct * str;
  str = g_malloc0 (sizeof*str);
  str -> st = st;
  str -> id = ai;
  if (st == 6 || st == 7)
  {
    str -> num = 0;
    str -> av = 0.0;
  }
  else
  {
    str -> num = an;
    str -> av = val;
  }
  str -> aid = NULL;
  if (aid != NULL) str -> aid = duplicate_int (struct_id(st+7), aid);
  str -> def = init_field_prop (st, 0, FALSE, TRUE);
  str -> other = NULL;
  str -> next = NULL;
  str -> prev = NULL;
  return str;
}

/*!
  \fn field_nth_body * init_field_nth_body (int bi, int bd, int * na, int ** ma, int ** ba)

  \brief initialize new field non bonded interaction

  \param bi the new non bonded interaction id
  \param bd the type of non bonded interaction
  \param na the list of field atom(s), if any
  \param ma the list of field molecule(s), if any
  \param ba the list of atom id in field molecule, if any
*/
field_nth_body * init_field_nth_body (int bi, int bd, int * na, int ** ma, int ** ba)
{
  int i, j;
  field_nth_body * nthbd;
  nthbd = g_malloc0 (sizeof*nthbd);
  nthbd -> id = bi;
  nthbd -> bd = bd;
  if (! bd)
  {
    nthbd -> fpid = allocint(2);
    nthbd -> fpid[0] = nthbd -> fpid[1] = -1;
  }
  j = body_at (bd);
  // Id in Vdw list
  if (na == NULL)
  {
    nthbd -> na = allocint (j);
    for (i=0; i<j; i++) nthbd -> na[i] = -1;
  }
  else
  {
    nthbd -> na = duplicate_int (j, na);
  }
  // Molecule id
  nthbd -> ma = NULL;
  nthbd -> ma = g_malloc0 (j*sizeof*nthbd -> ma);
  // Field atom id in molecule
  nthbd -> a = NULL;
  nthbd -> a = g_malloc0 (j*sizeof*nthbd -> a);
  for (i=0; i < j; i++)
  {
    if (ma != NULL) nthbd -> ma[i] = duplicate_int (na[i], ma[i]);
    if (ba != NULL) nthbd -> a[i] = duplicate_int (na[i], ba[i]);
  }
  nthbd -> key = 0;
  nthbd -> val = allocfloat (fvalues[activef][9+bd][nthbd -> key]);
  nthbd -> show = FALSE;
  nthbd -> use = TRUE;
  nthbd -> next = NULL;
  nthbd -> prev = NULL;
  return nthbd;
}

/*!
  \fn field_external * init_field_external (int bi)

  \brief initialize new field external potential

  \param bi the id of the new field external potential
*/
field_external * init_field_external (int bi)
{
  field_external * nfext;
  nfext = g_malloc0 (sizeof*nfext);
  nfext -> id = bi;
  nfext -> key = -1;
  nfext -> val = NULL;
  nfext -> use = FALSE;
  nfext -> next = NULL;
  nfext -> prev = NULL;
  return nfext;
}

/*!
  \fn int prepare_field_atom (int i, int j, int k, int l, int m)

  \brief initialiaze new type of field atom

  \param i the new field atom id
  \param j the new field atom coordination type
  \param k the new field atom species
  \param l the number of atom(s) of for this field atom type
  \param m the coordination id in coordination type, if any
*/
int prepare_field_atom (int i, int j, int k, int l, int m)
{
  if (tmp_fmol -> first_atom == NULL)
  {
    tmp_fmol -> first_atom = init_field_atom (i, j, k, l, m, NULL);
    tmp_fat = tmp_fmol -> first_atom;
  }
  else
  {
    tmp_fat -> next = init_field_atom (i, j, k, l, m, NULL);
    tmp_fat -> next -> prev = g_malloc0 (sizeof*tmp_fat -> next -> prev);
    tmp_fat -> next -> prev = tmp_fat;
    tmp_fat = tmp_fat -> next;
  }
  return 1;
}

/*!
  \fn void init_all_atoms (int i)

  \brief initialize all field atom(s)

  \param i the molecule id in the model, if any
*/
void init_all_atoms (int i)
{
  int j, k, l, m, n;

  tmp_fmol -> first_atom = NULL;
  k = 0;
  switch (tmp_field -> atom_init)
  {
    case SPEC_ONLY:
      for (j=0; j<tmp_proj -> nspec; j++)
      {
        if (tmp_mol -> species[j] > 0)
        {
          k += prepare_field_atom (k, tmp_field -> atom_init, j, tmp_mol -> species[j]*tmp_fmol -> multi, 0);
        }
      }
      break;
    case OTHER:

      break;
    default:
      for (j=0; j<tmp_proj -> nspec; j++)
      {
        if (tmp_mol -> species[j] > 0)
        {
          for (l=0; l < tmp_coord -> totcoord[tmp_field -> atom_init-1]; l++)
          {
            n = 0;
            for (m=0; m < tmp_proj -> natomes; m++)
            {
              if (tmp_proj -> atoms[0][m].sp == j
                  && tmp_proj -> atoms[0][m].coord[3] == i
                  && tmp_proj -> atoms[0][m].coord[tmp_field -> atom_init -1] == l) n ++;
            }
            if (n > 0)
            {
              k += prepare_field_atom (k, tmp_field -> atom_init, j, n, l);
            }
          }
        }
      }
      break;
  }
  tmp_fmol -> atoms = k;
}

/*!
  \fn gboolean in_bond (int at, int bd[2])

  \brief is atom at in bond bd

  \param at the target atom id
  \param bd the bond atoms id
*/
gboolean in_bond (int at, int bd[2])
{
  if (at == bd[0] || at == bd[1])
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*!
  \fn gboolean are_neighbors (field_neighbor * ngb, int at)

  \brief test if 2 atoms are neighbors

  \param ngb the neighbor data structure to test
  \param at the target atom id in the model
*/
gboolean are_neighbors (field_neighbor * ngb, int at)
{
  int i;
  for (i=0; i<ngb -> num; i++)
  {
    if (ngb -> vois[i] == at) return TRUE;
  }
  return FALSE;
}

/*!
  \fn gboolean are_in_bond (atom ato, int at)

  \brief is at in ato neighbors ?

  \param ato the target atom
  \param at the target atom id
*/
gboolean are_in_bond (atom ato, int at)
{
  int i;
  for (i=0; i<ato.numv; i++)
  {
    if (ato.vois[i] == at) return TRUE;
  }
  return FALSE;
}

/*!
  \fn int test_for_bonds (field_atom* at, field_atom* bt)

  \brief search for bond(s) between 2 field atoms

  \param at 1st field atom
  \param bt 2nd field atom
*/
int test_for_bonds (field_atom* at, field_atom* bt)
{
  int i, j, k, l, m;
  m = 0;
  val = 0.0;
  for (i=0; i < at -> num; i++)
  {
    j = at -> list[i];
    for (k= 0; k < tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].faid == bt -> id)
      {
        m ++;
        val += distance_3d (& tmp_proj -> cell, 0, & tmp_proj -> atoms[0][j], & tmp_proj -> atoms[0][l]).length;
      }
    }
  }
  if (m > 0) val /= m;
  if (at -> id == bt -> id) m /= 2;
  return m / tmp_fmol -> multi;
}

/*!
  \fn int prepare_field_struct (int ids, int sid, int yes_no_num, int * aid)

  \brief prepare the creation of a field structural element

  \param ids the type of structural element (0 to 7)
  \param sid the id of the new structural element
  \param yes_no_num is there data to save (1 or > 1) or not (0) ?
  \param aid the list of field atoms
*/
int prepare_field_struct (int ids, int sid, int yes_no_num, int * aid)
{
  if (yes_no_num > 0)
  {
    if (tmp_fmol -> first_struct[ids] == NULL)
    {
      tmp_fmol -> first_struct[ids] = init_field_struct (ids, sid, yes_no_num, aid);
      tmp_fstr = tmp_fmol -> first_struct[ids];
      return 1;
    }
    else if (was_not_created_struct (ids, sid, aid))
    {
      tmp_fstr = tmp_fmol -> first_struct[ids];
      while (tmp_fstr -> next) tmp_fstr = tmp_fstr -> next;
      tmp_fstr -> next = init_field_struct (ids, sid, yes_no_num, aid);
      tmp_fstr -> next -> prev = tmp_fstr;
      tmp_fstr = tmp_fstr -> next;
      return 1;
    }
  }
  return 0;
}

/*!
  \fn int bonds_between_atoms (int n, field_atom* at, field_atom* bt, int a, int b)

  \brief search for bond(s) between two field atoms

  \param n the number of different bond types already found
  \param at 1st field atom
  \param bt 2nd field atom
  \param a 1st atom chemical species
  \param b 2nd atom chemical species
*/
int bonds_between_atoms (int n, field_atom* at, field_atom* bt, int a, int b)
{
  if ((at -> sp == a && bt -> sp == b) || (at -> sp == b && bt -> sp == a))
  {
    astr[0] = at -> id;
    astr[1] = bt -> id;
    n += prepare_field_struct (0, n, test_for_bonds (at, bt), astr);
  }
  return n;
}

/*!
  \fn void init_all_bonds ()

  \brief find, and initialize all bond(s)
*/
void init_all_bonds ()
{
  int j, k, l;
  tmp_fmol -> first_struct[0] = NULL;
  k = 0;
  for (j=0; j< tmp_proj -> nspec; j++)
  {
    tmp_fat = tmp_fmol -> first_atom;
    while (tmp_fat)
    {
      tmp_fbt = tmp_fat;
      while (tmp_fbt)
      {
        k = bonds_between_atoms (k, tmp_fat, tmp_fbt, j, j);
        tmp_fbt = tmp_fbt -> next;
      }
      tmp_fat = tmp_fat -> next;
    }
  }
  for (j=0; j<tmp_proj -> nspec-1; j++)
  {
    for (l=j+1; l<tmp_proj -> nspec; l++)
    {
      tmp_fat = tmp_fmol -> first_atom;
      while (tmp_fat)
      {
        tmp_fbt = tmp_fat -> next;
        while (tmp_fbt)
        {
          k = bonds_between_atoms (k, tmp_fat, tmp_fbt, j, l);
          tmp_fbt = tmp_fbt -> next;
        }
        tmp_fat = tmp_fat -> next;
      }
    }
  }
  tmp_fmol -> nstruct[0] = k;
}

/*!
  \fn int test_for_angles (field_atom * at, field_atom * bt, field_atom * ct)

  \brief search for angle(s) between these field atoms

  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
*/
int test_for_angles (field_atom* at,
                     field_atom* bt,
                     field_atom* ct)
{
  int i, j, k, l, m, n, o;
  val = 0.0;
  o = 0;
  for (i=0; i<at -> num; i++)
  {
    j = at -> list[i];
    for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].numv >= 2 && tmp_proj -> atoms[0][l].faid == bt -> id)
      {
        for (m=0; m<tmp_proj -> atoms[0][l].numv; m++)
        {
          n = tmp_proj -> atoms[0][l].vois[m];
          if (n != j && tmp_proj -> atoms[0][n].faid == ct -> id)
          {
            o ++;
            val += angle_3d (& tmp_proj -> cell, 0,
                             & tmp_proj -> atoms[0][j],
                             & tmp_proj -> atoms[0][l],
                             & tmp_proj -> atoms[0][n]).angle;
          }
        }
      }
    }
  }
  if (o > 0) val /= o;
  if (at -> id == ct -> id) o /= 2;
  return o / tmp_fmol -> multi;
}

/*!
  \fn int angles_from_bonds (int n, field_atom * at, field_atom * bt, field_atom * ct)

  \brief find, and initialize, angles using bonds

  \param n the number of different angle types already found
  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
*/
int angles_from_bonds (int n, field_atom* at,
                              field_atom* bt,
                              field_atom* ct)
{
  astr[0] = at -> id;
  astr[1] = bt -> id;
  astr[2] = ct -> id;
  n += prepare_field_struct (2, n, test_for_angles (at, bt, ct), astr);
  return n;
}

/*!
  \fn void init_all_angles ()

  \brief find, and intialiaze, all angle(s) using bonds
*/
void init_all_angles ()
{
  int m, p;

  tmp_fmol -> first_struct[2] = NULL;
  p = 0;
  field_struct * tmp_fst = tmp_fmol -> first_struct[0];
  for (m=0; m < tmp_fmol -> nstruct[0]; m++)
  {
    tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fst -> aid[0]);
    tmp_fbt = get_active_atom (tmp_fmol -> id, tmp_fst -> aid[1]);
    tmp_fct = tmp_fmol -> first_atom;
    while (tmp_fct)
    {
      p = angles_from_bonds (p, tmp_fat, tmp_fbt, tmp_fct);
      p = angles_from_bonds (p, tmp_fbt, tmp_fat, tmp_fct);
      tmp_fct = tmp_fct -> next;
    }
    if (tmp_fst -> next != NULL) tmp_fst = tmp_fst -> next;
  }
  tmp_fmol -> nstruct[2] = p;
}

/*!
  \fn int test_for_dihedrals (field_atom * at, field_atom * bt, field_atom * ct, field_atom * dt)

  \brief search for dihedral(s) between these field atoms

  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
  \param dt 4th field atom
*/
int test_for_dihedrals (field_atom* at,
                        field_atom* bt,
                        field_atom* ct,
                        field_atom* dt)
{
  int i, j, k, l, m, n, o, p, q;
  q = 0;
  val = 0.0;
  for (i=0; i<at -> num; i++)
  {
    j = at -> list[i];
    for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].faid == bt -> id)
      {
        for (m=0; m<tmp_proj -> atoms[0][l].numv; m++)
        {
          n = tmp_proj -> atoms[0][l].vois[m];
          if (n != j && tmp_proj -> atoms[0][n].faid == ct -> id)
          {
            for (o=0; o<tmp_proj -> atoms[0][n].numv; o++)
            {
              p = tmp_proj -> atoms[0][n].vois[o];
              if (p != j && p != l && tmp_proj -> atoms[0][p].faid == dt -> id)
              {
                q ++;
                val += dihedral_3d (& tmp_proj -> cell, 0,
                                    & tmp_proj -> atoms[0][j],
                                    & tmp_proj -> atoms[0][l],
                                    & tmp_proj -> atoms[0][n],
                                    & tmp_proj -> atoms[0][p]).angle;
              }
            }
          }
        }
      }
    }
  }
  if (q > 0) val /= q;
  if (at -> id == dt -> id && bt -> id == ct -> id) q /= 2;
  return q / tmp_fmol -> multi;
}

/*!
  \fn int dihedrals_from_angles (int n, field_atom * at, field_atom * bt, field_atom * ct, field_atom * dt)

  \brief find, and initialize, dihedrals using angles

  \param n the number of different dihedral types already found
  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
  \param dt 4th field atom
*/
int dihedrals_from_angles (int n,
                           field_atom* at,
                           field_atom* bt,
                           field_atom* ct,
                           field_atom* dt)
{
  astr[0] = at -> id;
  astr[1] = bt -> id;
  astr[2] = ct -> id;
  astr[3] = dt -> id;
  n += prepare_field_struct (4, n, test_for_dihedrals (at, bt, ct, dt), astr);
  return n;
}

/*!
  \fn void init_all_dihedrals ()

  \brief find, and intialiaze, all dihedral(s) using bonds
*/
void init_all_dihedrals ()
{
  int n, p;

  tmp_fmol -> first_struct[4] = NULL;
  p = 0;
  field_struct * tmp_fst = tmp_fmol -> first_struct[2];
  for (n=0; n< tmp_fmol -> nstruct[2]; n++)
  {
    tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fst -> aid[0]);
    tmp_fbt = get_active_atom (tmp_fmol -> id, tmp_fst -> aid[1]);
    tmp_fct = get_active_atom (tmp_fmol -> id, tmp_fst -> aid[2]);
    tmp_fdt = tmp_fmol -> first_atom;
    while (tmp_fdt)
    {
      p = dihedrals_from_angles (p, tmp_fat, tmp_fbt, tmp_fct, tmp_fdt);
      p = dihedrals_from_angles (p, tmp_fct, tmp_fbt, tmp_fat, tmp_fdt);
      tmp_fdt = tmp_fdt -> next;
    }
    if (tmp_fst -> next != NULL) tmp_fst = tmp_fst -> next;
  }
  tmp_fmol -> nstruct[4] = p;
}

/*!
  \fn int impropers_inversion (int n, int stru,
                               int at, int bt, int ct, int dt,
                               int a, int b, int c, int d)

  \brief prepare new improper / inversion

  \param n the actual number of structural element
  \param stru the type of structural element, 6 = impropers, 7 = inversions
  \param at 1st field atom id
  \param bt 2nd field atom id
  \param ct 3rd field atom id
  \param dt 4th field atom id
  \param a 1st atom id in molecule/fragment
  \param b 2nd atom id in molecule/fragment
  \param c 3rd atom id in molecule/fragment
  \param d 4th atom id in molecule/fragment
*/
int impropers_inversion (int n, int stru,
                         int at, int bt, int ct, int dt,
                         int a, int b, int c, int d)
{
  astr[0] = at;
  astr[1] = bt;
  astr[2] = ct;
  astr[3] = dt;
  double v;
  //g_debug ("Improp:: at= %d, bt= %d, ct= %d, dt= %d", at, bt, ct, dt);
  //g_debug ("Improp:: a= %d, b= %d, c= %d, d= %d", a, b, c, d);
  n += prepare_field_struct (stru, n, 1, astr);
  //g_debug ("tmp_fstr -> id= %d", tmp_fstr -> id);
  if (stru == 6)
  {
    v = dihedral_3d (& tmp_proj -> cell, 0,
                     & tmp_proj -> atoms[0][b],
                     & tmp_proj -> atoms[0][c],
                     & tmp_proj -> atoms[0][a],
                     & tmp_proj -> atoms[0][d]).angle;
  }
  else
  {
    v = inversion_3d (& tmp_proj -> cell, 0,
                      & tmp_proj -> atoms[0][a],
                      & tmp_proj -> atoms[0][b],
                      & tmp_proj -> atoms[0][c],
                      & tmp_proj -> atoms[0][d]).angle;
  }
  tmp_fstr -> av += v;
  tmp_fstr -> num ++;
  //g_debug ("tmp_fstr -> id = %d, tmp_fstr -> num = %d, v= %f, av= %f\n", tmp_fstr -> id, tmp_fstr -> num, v, tmp_fstr -> av);
  return n;
}

/*!
  \fn void init_all_impropers_inversions (int stru)

  \brief initialize all impropers and inversions

  \param stru 6 = improper(s), 7 = inversion(s)
*/
void init_all_impropers_inversions (int stru)
{
  int i, j, k, l, m, n, p;
  int * atid, * matid;
  atid = allocint (tmp_proj -> coord -> cmax+1);
  matid = allocint (tmp_proj -> coord -> cmax+1);
  tmp_fmol -> first_struct[stru] = NULL;
  p = 0;
  for (i=0; i<tmp_fmol -> mol -> natoms; i++)
  {
    matid[0] = tmp_fmol -> atoms_id[i][0].a;
    j = tmp_fmol -> atoms_id[i][0].b;
    k = atid[0] = get_active_atom (tmp_fmol -> id, matid[0]) -> list[j];
    if ((tmp_proj -> atoms[0][k].numv > 2 && stru == 6) || (tmp_proj -> atoms[0][k].numv == 3 && stru == 7))
    {
      for (m=0; m<tmp_proj -> atoms[0][k].numv; m++)
      {
        atid[m+1] = tmp_proj -> atoms[0][k].vois[m];
        l = tmp_proj -> atoms[0][k].vois[m];
        matid[m+1] = tmp_proj -> atoms[0][l].faid;
      }
      if (stru == 6)
      {
        for (l=0; l<tmp_proj -> atoms[0][k].numv-2; l++)
        {
          for (m=l+1; m<tmp_proj -> atoms[0][k].numv-1; m++)
          {
            for (n=m+1; n<tmp_proj -> atoms[0][k].numv; n++)
            {
              p = impropers_inversion (p, stru,
                                       matid[0], matid[l+1], matid[m+1], matid[n+1],
                                       atid[0], atid[l+1], atid[m+1], atid[n+1]);
              p = impropers_inversion (p, stru,
                                       matid[0], matid[l+1], matid[n+1], matid[m+1],
                                       atid[0], atid[l+1], atid[n+1], atid[m+1]);
              p = impropers_inversion (p, stru,
                                       matid[0], matid[m+1], matid[n+1], matid[l+1],
                                       atid[0], atid[m+1], atid[n+1], atid[l+1]);
            }
          }
        }
      }
      else
      {
        p = impropers_inversion (p, stru,
                                 matid[0], matid[1], matid[2], matid[3],
                                 atid[0], atid[1], atid[2], atid[3]);
        p = impropers_inversion (p, stru,
                                 matid[0], matid[2], matid[1], matid[3],
                                 atid[0], atid[2], atid[1], atid[3]);
        p = impropers_inversion (p, stru,
                                 matid[0], matid[3], matid[1], matid[2],
                                 atid[0], atid[3], atid[1], atid[2]);
      }
    }
  }
  tmp_fmol -> nstruct[stru] = p;
  tmp_fstr = tmp_fmol -> first_struct[stru];
  for (i=0; i<tmp_fmol -> nstruct[stru]; i++)
  {
    tmp_fstr -> def -> use = FALSE;
    tmp_fstr -> av /= tmp_fstr -> num;
    if (tmp_fstr -> next != NULL) tmp_fstr = tmp_fstr -> next;
  }
  g_free (atid);
  g_free (matid);
}

/*!
  \fn int coord_sphere_multiplicity (atom * at, int id, gboolean set_atom_id)

  \brief get coord sphere multiplicity in the model

  \param at the target atom in the model
  \param id the atom id in the fragment
  \param set_atom_id adjust field atom parameters (yes / no)
*/
int coord_sphere_multiplicity (atom * at, int id, gboolean set_atom_id)
{
  int j, k, l, m;
  gboolean not_alone;
  multi = 0;
  a_multi = 0;
  tmp_fct = tmp_fmol -> first_atom;
  while (tmp_fct)
  {
    if (tmp_fct -> sp == at -> sp)
    {
      for (j=0; j<tmp_fct -> num; j++)
      {
        k = tmp_fct -> list[j];
        not_alone = TRUE;
        if (k != at -> id)
        {
          for (l=0; l<2; l++)
          {
            if (tmp_proj -> atoms[0][k].coord[l] != at -> coord[l])
            {
              not_alone = FALSE;
              break;
            }
          }
          if (not_alone)
          {
            multi ++;
            if (tmp_fct -> id == tmp_fat -> id)  a_multi ++;
            if (set_atom_id)
            {
              l = tmp_proj -> atoms[0][k].coord[2];
              for (m=1; m<tmp_fmol -> multi; m++)
              {
                if (l == tmp_fmol -> fragments[m])
                {
                  tmp_fmol -> atoms_id[id][m].a = tmp_fct -> id;
                  tmp_fmol -> atoms_id[id][m].b = j;
                  tmp_fct -> list_id[j] = id;
                }
              }
            }
          }
        }
      }
    }
    tmp_fct = tmp_fct -> next;
  }
  return multi;
}

int assigned;
int * i_weight; // For the molecule, indice = atom id for the molecule
int * n_weight; // For the analyzed multi, indice = atom id for the project
field_neighbor * init_ngb = NULL;

/*!
  \fn field_neighbor * get_init_neighbor (int a)

  \brief retrieve neighbor data structure for atom a

  \param a the atom id in the fragment
*/
field_neighbor * get_init_neighbor (int a)
{
  field_neighbor * ngb = NULL;
  ngb = init_ngb;
  while (ngb)
  {
    if (ngb -> id == a) break;
    ngb = ngb -> next;
  }
  return ngb;
}

/*!
  \fn gboolean is_numbering_possible (int frag)

  \brief compare if the atom numbering of two fragment(s) are identical, and it should

  \param frag the fragment id
*/
gboolean is_numbering_possible (int frag)
{
  int i, j, k, l, m, n, o , p;
  field_neighbor * ngma, * ngmb;
  field_atom* fat, * fbt;

  for (i=0; i<tmp_fmol -> mol -> natoms-1; i++)
  {
    if (tmp_fmol -> atoms_id[i][frag].a > -1 &&  tmp_fmol -> atoms_id[i][frag].b > -1)
    {
      ngma = get_init_neighbor (i);
      j = tmp_fmol -> atoms_id[i][frag].a;
      k = tmp_fmol -> atoms_id[i][frag].b;
      fat = get_active_atom(tmp_fmol -> id, j);
      l = fat -> list[k];
      for (m=i+1; m<tmp_fmol -> mol -> natoms; m++)
      {
        if (tmp_fmol -> atoms_id[m][frag].a > -1 &&  tmp_fmol -> atoms_id[m][frag].b > -1)
        {
          ngmb = get_init_neighbor (m);
          n = tmp_fmol -> atoms_id[m][frag].a;
          o = tmp_fmol -> atoms_id[m][frag].b;
          fbt = get_active_atom(tmp_fmol -> id, n);
          p = fbt -> list[o];
          if (are_in_bond (tmp_proj -> atoms[0][l], p) != are_neighbors (ngma, m))
          {
            return FALSE;
          }
          if (are_in_bond (tmp_proj -> atoms[0][p], l) != are_neighbors (ngmb, i))
          {
            return FALSE;
          }
        }
      }
    }
  }
  return TRUE;
}

/*!
  \fn gboolean is_this_numbering_possible_for_this_atom (int frag, field_neighbor * ngma, int atom)

  \brief check if the id in fragment for this atom is possible or not

  \param frag the fragment id
  \param ngma the neigbbor data structure for this fragment
  \param atom the atom id in the model
*/
gboolean is_this_numbering_possible_for_this_atom (int frag, field_neighbor * ngma, int atom)
{
  int m, n, o, p;
  field_atom* fbt;

  for (m=0; m<tmp_fmol -> mol -> natoms; m++)
  {
    if (m != ngma -> id)
    {
      if (tmp_fmol -> atoms_id[m][frag].a > -1 &&  tmp_fmol -> atoms_id[m][frag].b > -1)
      {
        n = tmp_fmol -> atoms_id[m][frag].a;
        o = tmp_fmol -> atoms_id[m][frag].b;
        fbt = get_active_atom(tmp_fmol -> id, n);
        p = fbt -> list[o];
        if (are_in_bond (tmp_proj -> atoms[0][atom], p) != are_neighbors (ngma, m))
        {
          return FALSE;
        }
      }
    }
  }
  return TRUE;
}

/*!
  \fn int find_neighbor_loop (int frag, int aid, int fai,
                              field_atom* fat,
                              field_neighbor * ngmb,
                              int sid, gboolean save_it)

  \brief find / set up field atom data for a fragment

  \param frag the fragment id
  \param aid the atom id in the model
  \param fai the atom id in the fragment
  \param fat the field atom
  \param ngmb the neighbor(s) list for the fragment
  \param sid iter to save
  \param save_it save field atom data (yes / no)
*/
int find_neighbor_loop (int frag, int aid, int fai,
                        field_atom* fat,
                        field_neighbor * ngmb,
                        int sid, gboolean save_it)
{
  int i, j, k;
  int iter = 0;

  i = fat -> num / tmp_fmol -> multi;
  for (j=frag*i; j < (frag+1)*i; j++)
  {
    if (fat -> list_id[j] < 0)
    {
      k = fat -> list[j];
      if (are_in_bond (tmp_proj -> atoms[0][k], aid))
      {
        //g_debug ("Are linked, n_weight[%d]= %d, i_weight[%d]= %d", k+1, n_weight[k], j+1, i_weight[j]);
        if (n_weight[k] == i_weight[ngmb -> id])
        {
          tmp_fmol -> atoms_id[fai][frag].a = fat -> id;
          tmp_fmol -> atoms_id[fai][frag].b = j;
          fat -> list_id[j] = fai;
          if (save_it && iter == sid) return k;
          if (is_this_numbering_possible_for_this_atom (frag, ngmb, k)) iter ++;
          tmp_fmol -> atoms_id[fai][frag].a = -1;
          tmp_fmol -> atoms_id[fai][frag].b = -1;
          fat -> list_id[j] = -1;
        }
      }
    }
  }
  return iter;
}

/*!
  \fn gboolean id_n_fold_atoms_in_fragment (int frag, int limit, int num_ngb, int search_type,
                                            int init, field_neighbor * ngma_init)

  \brief find atom id in fragment using the neighbor list

  \param frag the fragment id
  \param limit the total number of atom(s) in the fragment
  \param num_ngb the neighbor(s) list for the fragment
  \param search_type search type (0 = browse field molecule atom list, 1 = browse field atom list)
  \param init the atom id in the model
  \param ngma_init the target
*/
gboolean id_n_fold_atoms_in_fragment (int frag, int limit, int num_ngb, int search_type,
                                      int init, field_neighbor * ngma_init)
{
  int i, j, k, l, m, n, o, p, q, r, s, t;
  field_neighbor * ngma, * ngmb, * ngmc;
  field_atom* fat, * fbt;
  if (assigned < limit)
  {
    if (search_type)
    {
      i = ngma_init -> id;
      j = tmp_fmol -> atoms_id[i][frag].a;
      for (k=0; k<ngma_init -> num; k++)
      {
        l = ngma_init -> vois[k];
        ngma = get_init_neighbor (l);
        if (tmp_fmol -> atoms_id[l][frag].a < 0 && tmp_fmol -> atoms_id[l][frag].b < 0)
        {
          if (ngma -> num > num_ngb)
          {
            m = tmp_fmol -> atoms_id[l][0].a;
            fat = get_active_atom (tmp_fmol -> id, m);

            n = find_neighbor_loop (frag, init, l, fat, ngma, 0, FALSE);
            for (o=0; o<n; o++)
            {
              p = find_neighbor_loop (frag, init, l, fat, ngma, o, TRUE);
              assigned ++;
              if (assigned == limit)
              {
                return TRUE;
              }
              else if (id_n_fold_atoms_in_fragment(frag, limit, num_ngb, 1, p, ngma))
              {
                return TRUE;
              }
              else
              {
                for (q=0; q<tmp_fmol -> mol -> natoms; q++)
                {
                  if (q != l && tmp_fmol -> atoms_id[q][frag].a > -1 && tmp_fmol -> atoms_id[q][frag].b > -1)
                  {
                    ngmb = get_init_neighbor(q);
                    t = 0;
                    for (r=0; r<ngmb -> num; r++)
                    {
                      s = ngmb -> vois[r];
                      ngmc = get_init_neighbor(s);
                      if (tmp_fmol -> atoms_id[s][frag].a < 0 && tmp_fmol -> atoms_id[s][frag].b < 0 && ngmc -> num > num_ngb) t ++;
                    }
                    if (t)
                    {
                      r = tmp_fmol -> atoms_id[q][frag].a;
                      fbt = get_active_atom (tmp_fmol -> id, r);
                      s = tmp_fmol -> atoms_id[q][frag].b;
                      t = fbt -> list[s];
                      if (id_n_fold_atoms_in_fragment(frag, limit, num_ngb, 1, t, ngmb))
                      {
                        return TRUE;
                      }
                    }
                  }
                }
                if (id_n_fold_atoms_in_fragment(frag, limit, num_ngb, 0, -1, NULL))
                {
                  return TRUE;
                }
                else
                {
                  assigned --;
                  tmp_fmol -> atoms_id[l][frag].a = -1;
                  p = tmp_fmol -> atoms_id[l][frag].b;
                  fat -> list_id[p] = -1;
                  tmp_fmol -> atoms_id[l][frag].b = -1;
                }
              }
            }
          }
        }
      }
      return FALSE;
    }
    else
    {
      for (i=0; i<tmp_fmol -> mol -> natoms; i++)
      {
        if (tmp_fmol -> atoms_id[i][frag].a < 0 && tmp_fmol -> atoms_id[i][frag].b <0)
        {
          j = 0;
          ngma = get_init_neighbor (i);
          if (ngma -> num > num_ngb)
          {
            for (k=0; k<tmp_proj -> natomes; k++)
            {
              if (tmp_proj -> atoms[0][k].coord[2] == tmp_fmol -> fragments[frag])
              {
                j ++;
                if ((n_weight[k] == i_weight[i]) && (ngma -> num == tmp_proj -> atoms[0][k].numv))
                {
                  if (get_atom_id_in_fragment_from_model_id (frag, k) < 0)
                  {
                    assigned ++;
                    l = get_field_atom_id_from_model_id (tmp_fmol, k);
                    tmp_fmol -> atoms_id[i][frag].a = l;
                    m = get_position_in_field_atom_from_model_id (l, k);
                    tmp_fmol -> atoms_id[i][frag].b = m;
                    get_active_atom(tmp_fmol -> id, l) -> list_id[m] = i;
                    if (is_this_numbering_possible_for_this_atom (frag, ngma, k))
                    {
                      if (id_n_fold_atoms_in_fragment(frag, limit, num_ngb, 1, k, ngma))
                      {
                        return TRUE;
                      }
                      else if (id_n_fold_atoms_in_fragment(frag, limit, num_ngb, 0, -1, NULL))
                      {
                        return TRUE;
                      }
                    }
                    tmp_fmol -> atoms_id[i][frag].a = -1;
                    tmp_fmol -> atoms_id[i][frag].b = -1;
                    get_active_atom(tmp_fmol -> id, l) -> list_id[m] = -1;
                    assigned --;
                  }
                }
                if (j == tmp_fmol -> mol -> natoms)
                {
                  break;
                }
              }
            }
            return FALSE;
          }
        }
      }
      return FALSE;
    }
  }
  else
  {
    return TRUE;
  }
}

/*!
  \fn gboolean id_atoms_in_fragment (int frag, int seed)

  \brief find the atom field id in the fragment

  \param frag the fragment id
  \param seed seed
*/
gboolean id_atoms_in_fragment (int frag, int seed)
{
  int i, j;
  i = 0;
  for (j=0; j<tmp_proj -> natomes; j++)
  {
    if (tmp_proj -> atoms[0][j].coord[2] == tmp_fmol -> fragments[frag])
    {
      if (tmp_proj -> atoms[0][j].numv > 1) i++;
    }
  }
  if (seed > -1)
  {
    field_neighbor * ngb = get_init_neighbor (seed);
    if (id_n_fold_atoms_in_fragment (frag, i, 1, 0, seed, ngb))
    {
      return id_n_fold_atoms_in_fragment (frag, tmp_fmol -> mol -> natoms, 0, 0, seed, ngb);
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    if (id_n_fold_atoms_in_fragment (frag, i, 1, 0, -1, NULL))
    {
      return id_n_fold_atoms_in_fragment (frag, tmp_fmol -> mol -> natoms, 0, 0, -1, NULL);
    }
    else
    {
      return FALSE;
    }
  }
}

/*!
  \fn void get_weight (int seq)

  \brief compute field weigth for the atom(s) in the model

  \param seq weighting sequence
*/
void get_weight (int seq)
{
  int h, i, j, k;
  for (i=0; i<tmp_proj -> natomes; i++)
  {
    for (j=0; j<tmp_proj -> atoms[0][i].numv; j++)
    {
      k = tmp_proj -> atoms[0][i].vois[j];
      n_weight[i] += tmp_proj -> atoms[0][k].numv;
    }
  }
  int * weight = duplicate_int (tmp_proj -> natomes, n_weight);
  for (h=1; h<seq; h++)
  {
    for (i=0; i<tmp_proj -> natomes; i++)
    {
      for (j=0; j<tmp_proj -> atoms[0][i].numv; j++)
      {
        k = tmp_proj -> atoms[0][i].vois[j];
        weight[i] += n_weight[k];
      }
    }
    for (i=0; i<tmp_proj -> natomes; i++)
    {
      n_weight[i] = weight[i];
    }
  }
  g_free (weight);

#ifdef DEBUG
  /*for (i=0; i<tmp_proj -> natomes; i++)
  {
    j = tmp_proj -> atoms[0][i].sp;
    g_debug ("N_WEIGHT:: %s i= %d, n_weight[i] = %d", tmp_proj -> chemistry -> label[j], i+1, n_weight[i]);
  }*/
#endif
}

/*!
  \fn int setup_atomic_weight (int seq)

  \brief setup atom(s) field weight

  \param seq weighting sequencce
*/
int setup_atomic_weight (int seq)
{
  int h, i, j, k, l;
  field_neighbor * ngm;
  i_weight = allocint (tmp_fmol -> mol -> natoms);
  init_ngb = NULL;
  for (i=0; i<tmp_proj -> natomes; i++)
  {
    if (tmp_proj -> atoms[0][i].coord[2] == tmp_fmol -> fragments[0])
    {
      if (init_ngb)
      {
        ngm -> next = g_malloc0(sizeof*ngm);
        ngm -> next -> prev = ngm;
        ngm = ngm -> next;
      }
      else
      {
        init_ngb = g_malloc0(sizeof*init_ngb);
        ngm = init_ngb;
      }
      ngm -> num = tmp_proj -> atoms[0][i].numv;
      ngm -> vois = duplicate_int (ngm -> num, tmp_proj -> atoms[0][i].vois);
      j = get_atom_id_in_fragment_from_model_id (0, i);
      ngm -> id = j;
      for (k=0; k<ngm -> num; k++)
      {
        l = ngm -> vois[k];
        ngm -> vois[k] = get_atom_id_in_fragment_from_model_id (0, l);
        i_weight[j] += tmp_proj -> atoms[0][l].numv;
      }
    }
  }

  int * weight = duplicate_int (tmp_fmol -> mol -> natoms, i_weight);
  for (h=1; h<seq; h++)
  {
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      ngm = get_init_neighbor (i);
      if (ngm)
      {
        for (j=0; j<ngm -> num; j++)
        {
          k = ngm -> vois[j];
          weight[i] += i_weight[k];
        }
      }
    }
    for (i=0; i<tmp_fmol -> mol -> natoms; i++)
    {
      i_weight[i] = weight[i];
    }
  }

#ifdef DEBUG
  /*field_atom* fat;
  for (i=0; i<tmp_fmol -> mol -> natoms; i++)
  {
    fat = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][0].a);
    j = tmp_fmol -> atoms_id[i][0].b;
    k = fat -> list[j];
    l = tmp_proj -> atoms[0][k].sp;
    g_debug ("I_WEIGHT:: %s i= %d, i_weight[i] = %d", tmp_proj -> chemistry -> label[l], i, i_weight[i]);
  }*/
#endif

  g_free (weight);

  int seed = -1;
  for (i=0; i<tmp_fmol -> mol -> natoms; i++)
  {
    seed = i;
    for (j=0; j<tmp_fmol -> mol -> natoms; j++)
    {
      if (j!=i)
      {
        if (i_weight[i] == i_weight[j])
        {
          seed = -1;
          break;
        }
      }
    }
    if (seed > -1) break;
  }
  get_weight (seq);
  return seed;
}

/*!
  \fn void find_atom_id_in_field_molecule ()

  \brief find field atom id in field molecule (fragment multiplicity > 1)
*/
void find_atom_id_in_field_molecule ()
{
  int i, j, k, l;
  gboolean sym = FALSE;
  gboolean done;
  int ** coordnum;
  int start = 1;
  int seed = -1;
  assigned = 0;

  coordnum = allocdint(tmp_proj -> nspec, tmp_coord -> totcoord[1]);
  tmp_fat = tmp_fmol -> first_atom;
  while (tmp_fat)
  {
    if (tmp_fat -> num == tmp_fmol -> multi)
    {
      i = tmp_fat -> list_id[0];
      // Multiplicity = num of atoms ...
      // Unic atom, ie. as much as identical molecular fragments
      for (j=1; j<tmp_fmol -> multi; j++)
      {
        for (k=1; k<tmp_fat -> num; k++)
        {
          l = tmp_fat -> list[k];
          if (tmp_proj -> atoms[0][l].coord[2] == tmp_fmol -> fragments[j])
          {
            tmp_fmol -> atoms_id[i][j].a = tmp_fmol -> atoms_id[i][0].a;
            tmp_fmol -> atoms_id[i][j].b = k;
            tmp_fat -> list_id[j] = i;
            break;
          }
        }
      }
      assigned ++;
    }
    tmp_fat = tmp_fat -> next;
  }

  for (i=0; i< tmp_fmol -> mol -> natoms; i++)
  {
    tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[i][0].a);
    done = TRUE;
    for (j=0; j<tmp_fat -> num; j++)
    {
      if (tmp_fat -> list_id[j] < 0)
      {
        done = FALSE;
        break;
      }
    }
    if (! done)
    {
      j = tmp_fmol -> atoms_id[i][0].b;
      k = tmp_fat -> list[j];
      l = coord_sphere_multiplicity (& tmp_proj -> atoms[0][k], i, FALSE);
      coordnum[tmp_proj -> atoms[0][k].sp][tmp_proj -> atoms[0][k].coord[1]] = l+1;
      if (l == tmp_fmol -> multi - 1 || a_multi == tmp_fmol -> multi - 1)
      {
        coord_sphere_multiplicity (& tmp_proj -> atoms[0][k], i, TRUE);
        assigned ++;
      }
    }
  }
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    for (j=0; j<tmp_coord -> totcoord[1]; j++)
    {
      if (coordnum[i][j] > 1 && coordnum[i][j] % tmp_fmol -> multi != 0) g_debug ("This should not happen");
      coordnum[i][j] /= tmp_fmol -> multi;
    }
  }
  for (k=6; k>0; k--)
  {
    sym = TRUE;
    for (i=0; i<tmp_proj -> nspec; i++)
    {
      for (j=0; j<tmp_coord -> totcoord[1]; j++)
      {
        if (coordnum[i][j] > 1 && coordnum[i][j] >= k && coordnum[i][j] % k != 0) sym = FALSE;
      }
    }
    if (sym) break;
  }
  g_free (coordnum);

  if (assigned < tmp_fmol -> mol -> natoms)
  {
#ifdef DEBUG
    if ((sym || assigned == 0))
    {
      g_debug ("Molecule seems to be symmetric, k= %d", k);
      g_debug ("Mol -> id:          %d", tmp_fmol -> id);
      g_debug ("Mol -> multi=       %d", tmp_fmol -> multi);
      g_debug ("Mol -> field atoms= %d", tmp_fmol -> atoms);
      g_debug ("Mol -> atomes=      %d", tmp_fmol -> mol -> natoms);
      g_debug ("Assigned so far=    %d", assigned);
    }
#endif

    // k is a weight factor
    // The values of the total coord of each atom's neighbors
    // is sum k times over, this is a safety for systems with
    // a peculiar symetry, otherwise it could be very very long
    k = 3;
    j = assigned;
    gboolean done = FALSE;
    if (n_weight) g_free (n_weight);
    n_weight = NULL;
    n_weight = allocint (tmp_proj -> natomes);
    while (! done && (seed < 0 || (tmp_fmol -> mol -> natoms && k < tmp_fmol -> mol -> natoms)))
    {
      seed = setup_atomic_weight (k);
      if (seed < 0) k ++;
      done = TRUE;
    }
#ifdef DEBUG
    g_debug ("Assigning using: seed= %d, w_factor= %d", seed, k);
#endif

    for (i=start; i<tmp_fmol -> multi; i++)
    {
#ifdef DEBUG
      g_debug ("   -> fragment id: %d", i+1);
#endif
      assigned = j;
      while (! id_atoms_in_fragment (i, seed) && k < tmp_fmol -> mol -> natoms)
      {
#ifdef DEBUG
        g_debug ("**********************************");
        g_debug ("*** NOT ABLE TO ASSIGN NUMBERS ***");
        g_debug ("***   Mol -> Id=    %3d        ***", tmp_fmol -> id);
        g_debug ("***   Mol -> Multi= %3d        ***", tmp_fmol -> multi);
        g_debug ("***   Fragment=     %3d        ***", i+1);
        g_debug ("***   k=            %3d        ***", k);
        g_debug ("***   assigned=     %10d ***", assigned);
        g_debug ("**********************************");
#endif
        k ++;
        if (n_weight) g_free (n_weight);
        n_weight = NULL;
        n_weight = allocint (tmp_proj -> natomes);
        seed = setup_atomic_weight (k);
      }
      for (l=0; l<tmp_proj -> natomes; l++)
      {
        if (tmp_proj -> atoms[0][l].coord[2] == tmp_fmol -> fragments[i])
        {
          tmp_proj -> atoms[0][l].faid = get_field_atom_id_from_model_id (tmp_fmol, l);
          tmp_proj -> atoms[0][l].fid = get_atom_id_in_fragment_from_model_id (i, l);
        }
      }
#ifdef DEBUG
      g_debug ("   -> done !");
#endif
    }
  }
  if (i_weight) g_free (i_weight);
  i_weight = NULL;
  if (n_weight)  g_free (n_weight);
  n_weight = NULL;
  if (init_ngb) g_free (init_ngb);
  init_ngb = NULL;
#ifdef DEBUG
  g_debug ("Assigning completed !");
#endif
}

/*!
  \fn void clean_field_struct_list (field_struct * stru)

  \brief clean force field structural property(ies)

  \param stru
*/
void clean_field_struct_list (field_struct * stru)
{
  while (stru)
  {
    if (stru -> next)
    {
      stru = stru -> next;
      g_free (stru -> prev);
    }
    else
    {
      g_free (stru);
      stru = NULL;
    }
  }
}

/*!
  \fn void init_all_field_struct (gboolean init)

  \brief initialize the force field structural properties for a field molecule

  \param init search for the atom(s) force field id number(s)
*/
void init_all_field_struct (gboolean init)
{
  int i;
  for (i=0; i<tmp_proj -> natomes; i++)
  {
    if (tmp_proj -> atoms[0][i].coord[2] == tmp_fmol -> fragments[0])
    {
      tmp_proj -> atoms[0][i].faid = get_field_atom_id_from_model_id (tmp_fmol, i);
      tmp_proj -> atoms[0][i].fid = get_atom_id_in_fragment_from_model_id (0, i);
    }
  }
  if (tmp_fmol -> multi > 1 && init) find_atom_id_in_field_molecule ();

  for (i=0; i<8; i++)
  {
    tmp_fmol -> nstruct[i] = 0;
    if (tmp_fmol -> first_struct[i] && tmp_fmol -> nstruct[i])
    {
      clean_field_struct_list (tmp_fmol -> first_struct[i]);
    }
    tmp_fmol -> first_struct[i] = NULL;
  }

  // Bonds start
  astr = g_malloc0 (2*sizeof*astr);
  init_all_bonds ();
  g_free (astr);

  if (tmp_fmol -> nstruct[0] > 0)
  {
    tmp_fmol -> first_struct[1] = duplicate_field_struct_list (tmp_fmol -> first_struct[0], TRUE);
    tmp_fmol -> nstruct[1] = tmp_fmol -> nstruct[0];
  }
  // Bonds end

  // Angle start
  astr = g_malloc0 (3*sizeof*astr);
  init_all_angles ();
  g_free (astr);
  if (tmp_fmol -> nstruct[2] > 0)
  {
    tmp_fmol -> nstruct[3] = tmp_fmol -> nstruct[2];
    tmp_fmol -> first_struct[3] = duplicate_field_struct_list (tmp_fmol -> first_struct[2], TRUE);
  }
  // Angle end

  // Dihedral start
  astr = g_malloc0 (4*sizeof*astr);
  init_all_dihedrals ();

  // Torsional restraints
  if (tmp_fmol -> nstruct[4] > 0)
  {
    tmp_fmol -> nstruct[5] = tmp_fmol -> nstruct[4];
    tmp_fmol -> first_struct[5] = duplicate_field_struct_list (tmp_fmol -> first_struct[4], TRUE);
  }

// Improper and inversion angles
  for (i=6; i<8; i++) init_all_impropers_inversions (i);
  g_free (astr);
}

/*!
  \fn void init_field_molecule (int i)

  \brief initialiaze of field molecule data structurre

  \param i the id of the field molecule to initialize
*/
void init_field_molecule (int i)
{
  int j, k;
  tmp_fmol -> id = i;
  tmp_fmol -> multi = tmp_mol -> multiplicity;
  tmp_fmol -> name = g_strdup_printf("MOL-%d", i+1);
  tmp_fmol -> fragments = NULL;
  tmp_fmol -> fragments = allocint (tmp_fmol -> multi);
  for (j=0; j<tmp_fmol -> multi; j++)
  {
    tmp_fmol -> fragments[j] = tmp_mol -> fragments[j];
  }
  tmp_fmol -> mol = tmp_mol;

  // Atoms start
  id_atom = -1;
  tmp_fmol -> atoms_id = g_malloc (tmp_mol -> natoms*sizeof*tmp_fmol -> atoms_id);
  for (j=0; j<tmp_mol -> natoms; j++)
  {
    tmp_fmol -> atoms_id[j] = g_malloc0 (tmp_fmol -> multi*sizeof*tmp_fmol -> atoms_id[j]);
    for (k=0; k<tmp_fmol -> multi; k++)
    {
      tmp_fmol -> atoms_id[j][k].a = -1;
      tmp_fmol -> atoms_id[j][k].b = -1;
    }
  }

  init_all_atoms (i);
  // Atoms end

  // setup_field_molecule_neighbors (i, tmp_proj);

  tmp_fmol -> shells = 0;
  // Field struct
  init_all_field_struct (TRUE);

  tmp_fmol -> shells = 0;
  tmp_fmol -> first_shell = NULL;
  tmp_fmol -> constraints = 0;
  tmp_fmol -> first_constraint = NULL;
  tmp_fmol -> pmfs = 0;
  tmp_fmol -> first_pmf = NULL;
  tmp_fmol -> rigids = 0;
  tmp_fmol -> first_rigid = NULL;
  tmp_fmol -> tethered = 0;
  tmp_fmol -> first_tethered = NULL;

  tmp_fmol -> show = FALSE;
  tmp_fmol -> show_id = FALSE;
  tmp_fmol -> next = NULL;
  tmp_fmol -> prev = NULL;
}

/*!
  \fn int init_vdw (gboolean init)

  \brief initialize the VdW section of a classical force field

  \param init
*/
int init_vdw (gboolean init)
{
  int nvdw;
  int i, j, k, l;
  tmp_fmol = tmp_field -> first_molecule;
  i = 0;
  while (tmp_fmol)
  {
    i += tmp_fmol -> atoms;
    tmp_fmol = tmp_fmol -> next;
  }
  nvdw = i;
  gchar ** to_be_vdw = g_malloc (nvdw*sizeof*to_be_vdw);
  int * vdw_mlist = allocint (nvdw);
  int ** vdw_aids = allocdint (nvdw,nvdw);
  int ** vdw_mids = allocdint (nvdw,nvdw);
  int vdw_na[2];
  int * vdw_a[2], * vdw_ma[2];
  gboolean add_vdw;
  i = 0;
  tmp_fmol = tmp_field -> first_molecule;
  while (tmp_fmol)
  {
    tmp_fat = tmp_fmol -> first_atom;
    while (tmp_fat)
    {
      add_vdw = TRUE;
      for (j=0; j<i; j++)
      {
        if (g_strcmp0 (to_be_vdw[j], tmp_fat -> name) == 0)
        {
          add_vdw = FALSE;
          vdw_mlist[j] ++;
          vdw_mids[j][vdw_mlist[j]] = tmp_fmol -> id;
          vdw_aids[j][vdw_mlist[j]] = tmp_fat -> id;
          break;
        }
      }
      if (add_vdw)
      {
        to_be_vdw[j] = g_strdup_printf ("%s", tmp_fat -> name);
        vdw_mids[j][0] = tmp_fmol -> id;
        vdw_aids[j][0] = tmp_fat -> id;
        i ++;
      }
      tmp_fat = tmp_fat -> next;
    }
    tmp_fmol = tmp_fmol -> next;
  }

  j = 0 ;
  tmp_fbody = NULL;
  for (k=0; k<i; k++)
  {
    vdw_na[0] = vdw_na[1] = vdw_mlist[k]+1;
    vdw_a[0] = vdw_a[1] = duplicate_int (vdw_na[0], vdw_aids[k]);
    vdw_ma[0] = vdw_ma[1] = duplicate_int (vdw_na[0], vdw_mids[k]);
    if (tmp_fbody == NULL)
    {
      if (init)
      {
        tmp_field -> first_body[0] = init_field_nth_body (j, 0, vdw_na, vdw_ma, vdw_a);
        tmp_fbody = tmp_field -> first_body[0];
      }
      else
      {
        comp_fbody = init_field_nth_body (j, 0, vdw_na, vdw_ma, vdw_a);
        tmp_fbody = comp_fbody;
      }
    }
    else
    {
      tmp_fbody -> next = init_field_nth_body (j, 0, vdw_na, vdw_ma, vdw_a);
      tmp_fbody -> next -> prev = tmp_fbody;
      tmp_fbody = tmp_fbody -> next;
    }
    j ++;
  }
  for (k=0; k<i-1; k++)
  {
    for (l=k+1; l<i; l++)
    {
      vdw_na[0] = vdw_mlist[k]+1;
      vdw_na[1] = vdw_mlist[l]+1;
      vdw_a[0] = duplicate_int (vdw_na[0], vdw_aids[k]);
      vdw_a[1] = duplicate_int (vdw_na[1], vdw_aids[l]);
      vdw_ma[0] = duplicate_int (vdw_na[0], vdw_mids[k]);
      vdw_ma[1] = duplicate_int (vdw_na[1], vdw_mids[l]);
      tmp_fbody -> next = init_field_nth_body (j, 0, vdw_na, vdw_ma, vdw_a);
      tmp_fbody -> next -> prev = tmp_fbody;
      tmp_fbody = tmp_fbody -> next;
      j ++;
    }
  }
  g_free (to_be_vdw);
  g_free (vdw_aids);
  g_free (vdw_mids);
  g_free (vdw_mlist);
  return j;
}

/*!
  \fn void init_dlpoly_field (classical_field * new_field)

  \brief initialize DL-POLY classical force field

  \param new_field the force field to initialize
*/
void init_dlpoly_field (classical_field * new_field)
{
  // Preparing DLPOLY Control file
  new_field -> sys_opts = allocdouble (17);
  new_field -> io_opts = allocdouble (23);
  new_field -> ana_opts = allocdouble (17);
  new_field -> elec_opts = allocdouble (11);
  new_field -> vdw_opts = allocdouble (6);
  new_field -> met_opts = allocdouble (2);
  new_field -> equi_opts = allocdouble (17);
  new_field -> thermo_opts= allocdouble (10);
  new_field -> md_opts = allocdouble (20);
  new_field -> out_opts = allocdouble (31);
/*
    new_field -> io_opts[]
  */
  // Set job time control (yes/no)
  new_field -> io_opts[0] = 1.0;
  // Job time control (s):
  new_field -> io_opts[1] = 10000.0;
  // Set job closure time (yes/no)
  new_field -> io_opts[2] = 1.0;
  // Job closure time (s):
  new_field -> io_opts[3] = 100.0;
  // 4 (1/0) General I/O read interace
  // 5 General I/O method (0= MPI, 1= FORTRAN, 2= Traditional, 3= netCDF)
  // Reader count:
  new_field -> io_opts[6] = 12;
  // Batch size:
  new_field -> io_opts[7] = 2000000;
  // Buffer size:
  new_field -> io_opts[8] = 20000;
  // 9 (1/0) Error check on general read I/O
  // 10 (1/0) General I/O write interace
  // 11 General I/O method (0= MPI, 1= FORTRAN, 2= Traditional, 3= netCDF)
  // Real precision (0 = 32 bits, 1= 64 bits)
   new_field -> io_opts[12] = 1;
  // 13 Type (0= Sorted, 1= Unsorted)
  // Reader count:
  new_field -> io_opts[14] = 12;
  // Batch size:
  new_field -> io_opts[15] = 2000000;
  // Buffer size:
  new_field -> io_opts[16] = 20000;
  // 17 (1/0) Error check on general write I/O
  // 18 (1/0) Define random number generator seeds
  // n1 seed:
  new_field -> io_opts[19] = 1;
  // n2 seed:
  new_field -> io_opts[20] = 20;
  // n3 seed:
  new_field -> io_opts[21] = 300;
  // 22 (1/0) limit the number of processors in the z direction

  /*
    new_field -> sys_opts[]
  */
  // Relative dieletric constant:
  new_field -> sys_opts[0] = 1.0;
  // 1 = allowed local variation of the system density (%)
  // 2 (1/0) Ingore the particle indices in CONFIG file
  // 3 (1/0) Ignore strict checks
  // 4 (1/0) Skip detailed topology reporting
  // 5 (1/0) Ignore center of mass removal
  // 6 (1/0) Set tolerance of the relaxed shell model
  // Tolerance for the relaxed shell model:
  new_field -> sys_opts[7] = 1.0;
  // 8 (1/0) Set the subscelling threshold density of particles per link cell
  // Subscelling threshold density of particles per link cell:
  new_field -> sys_opts[9] = 50.0;
  // 10 (1/0) Create an expanded version of the current model ...
  // ... on x:
  new_field -> sys_opts[11] = 1.0;
  // ... on y:
  new_field -> sys_opts[12] = 1.0;
  // .. on z:
  new_field -> sys_opts[13] = 1.0;
  // 14 (1/0) Restart a calculation
  // 15 (0= Continue current, 1= Start new without T reset, 2= Start new with T reset)

  /*
    new_field -> equi_opts[]
  */
  new_field -> equi_opts[0] = 1.0;
  // Equilibrate for the first N steps, N=
  new_field -> equi_opts[1] = 20.0;
  // Scale temperature every N steps, N=
  new_field -> equi_opts[3] = 1.0;
  // Cap forces fmax=
  new_field -> equi_opts[5] = 1000.0;
  // Resample the momenta distribution during N steps N=
  new_field -> equi_opts[7] = 1.0;
  // 8 (1/0) = Minimize system configuration
  // 9 Minimize target (0=Force, 1=Energy, 2= Distance)
  // Minimization threshold:
  new_field -> equi_opts[10] = 50.0;
  // Minimization to be done every N steps, N=
  new_field -> equi_opts[11] = 1.0;
  // 12 (1/0) = Optimize system configuration from start
  // 13 Minimize target (0=Force, 1=Energy, 2= Distance)
  //  Minimization threshold:
  new_field -> equi_opts[14] = 50.0;
  // 15 (1/0) = Perform zero temperature (10K) optimization
  // 16 (1/0) = Include equilibration data in statistics

  /*
    new_field -> vdw_opts[]
  */
  // Vdw cutoff
  // 0 (1/0) Use VdW interactions
  new_field -> vdw_opts[0] = 1.0;
  // Vdw Cutoff radius
  new_field -> vdw_opts[1] = 12.0;
  // 2 (1/0) Enforce direct VdW calculation
  // 3 (1/0) Apply force shifting
  // 4 (1/0) Apply mixing rule
  // 5 Mixing rule (0= LB, 1= FH, 2= H, 3= HHG, 4= TT, 5= F)

  /*
    new_field -> elec_opts[]
  */
  // 0 (1/0) Evaluate electrostatics
  new_field -> elec_opts[0] = 1.0;
  // Long range cutoff:
  new_field -> elec_opts[1] = 12.0;
  // 2 (1/0) Add padding to rcut
  // 3 Padding in angstrom
  // 4 (1/0) Use extended coulombic exclusion
  // 5 Electrostatic calculation method (0= Direct, ...)
  new_field -> elec_opts[5] = 2.0;
  // Ewald precision:
  new_field -> elec_opts[6] = 1e-20;
  // 7 to 9 method options
  // 10 Evaluation every N steps, N=
  tmp_field -> elec_opts[10] = 1.0;

  /*
    new_field -> met_opts[]
  */
  // 0 (1/0) Enable direct calculation of metal interactions
  // 1 (1/0) Switch the default embeding fuctions

  // Ensemble (0= NVE, 1= NVT, 2= NPT, 4= NST)
  // new_field -> ensemble
  // Thermostat = f(ensemble):
  // new_field -> thermostat
  /*
    new_field -> thermo_opts[]
  */
  // 0-5 ensemble + thermostat related options
  // 6 (1/0) Use pseudo thermal bath
  // 7 Thermostat type (0= L+D, 1= L, 2= G, 3= D)
  // Thickness:
  new_field -> thermo_opts[8] = 2.0;
  // 9 = Target temperature

  /*
    new_field -> md_opts[]
  */
  // MD target temperature (K)
  new_field -> md_opts[0] = 300.0;
  // 1 Verlet integrator (0 = velocity, 1 = leapfrog)
  // Number of MD steps:
  new_field -> md_opts[2] = 100.0;
  // 3 Time step (0 = fixed, 1 = variable)
  // Time step delta(t)= (ps)
  new_field -> md_opts[4] = 0.001;
  // Target pressure (katms):
  new_field -> md_opts[5] = 0.001;
  // Variable time step, maximum time step:
  new_field -> md_opts[6] = 0.001;
  // Variable time step, maximum distance allowed:
  new_field -> md_opts[7] = 0.5;
  // Variable time step, minimun distance allowed:
  new_field -> md_opts[8] = 0.01;
  // Shake iterations limit, in cycles:
  new_field -> md_opts[9] = 250.0;
  // Shake tolerance:
  new_field -> md_opts[10] = 1e-6;
  // FIQA iterations limit, in cycles:
  new_field -> md_opts[11] = 100;
  // FIQA quaternions tolerance:
  new_field -> md_opts[12] = 1e-8;
  // 13 (1/0) Initiate impact on particles
  // Index if the particle to impact:
  new_field -> md_opts[14] = 1.0;
  // Time step of the impact:
  new_field -> md_opts[15] = 1.0;
  // Energy of the imapct (keV):
  new_field -> md_opts[16] = 1.0;
  // Direction of the impact (from center of mass) ...
  // ... on x:
  new_field -> md_opts[17] = 1.0;
  // ... on y:
  new_field -> md_opts[18] = 1.0;
  // ... on z:
  new_field -> md_opts[19] = 1.0;

  /*
    new_field -> out_opts[]
  */
  // 0 (1/0) Print Defect file ...
  // 1 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[2] = 1.0;
  // ... with a site inteerstitial cutoff:
  new_field -> out_opts[3] = 0.75;
  // 4 (1/0) Print Displacement file ..
  // 5 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[6] = 1.0;
  // ... with a qualifying cutoff:
  new_field -> out_opts[7] = 0.15;
  // 8 (1/0) Print HISTORY file ...
  // 9 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[10] = 1.0;
  // 11 Data level (0, 1, 2)
  // 12 (1/0) Print Atom's MSD ...
  // 13 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[14] = 1.0;
  // 15 (1/0) Print system data ...
  // ... every N steps, N=
  new_field -> out_opts[16] = 1.0;
  // 17 (1/0) Accumulate statistics ...
  // ... every N steps, N=
  new_field -> out_opts[18] = 1.0;
  // 19 (1/0) Rolling average stack ...
  // ...  to N steps, N=
  new_field -> out_opts[20] = 1.0;
  // 21 (1/0) RDF ...
  // ... every N steps, N=
  new_field -> out_opts[22] = 1.0;
  // ... with a bin size=
  new_field -> out_opts[23] = 0.05;
  // 24 (1/0) Overall VAF analysis ...
  // ... every N steps, N=
  new_field -> out_opts[25] = 1.0;
  // ... with a bin size=
  new_field -> out_opts[26] = 50.0;
  // 27 (1/0) Z density profile ...
  // ... every N steps, N=
  new_field -> out_opts[28] = 1.0;
  // 29 (1/0) ignoring time-av. VAFs
  // Dump restart information every N steps, N=
  new_field -> out_opts[30] = 1000.0;

  /*
    new_field -> ana_opts[]
  */
  // 0 (1/0) Activate all intra-molecular PDF analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[1] = 1.0;
  // ... number of delta r in:
  new_field -> ana_opts[2] = 100.0;
  // ... Cutoff:
  new_field -> ana_opts[3] = 2.0;
  // 4 (1/0) Activate bonds PDF analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[5] = 1.0;
  // ... number of delta r in:
  new_field -> ana_opts[6] = 100.0;
  // ... Cutoff:
  new_field -> ana_opts[7] = 2.0;
  // 8 (1/0) Activate angles analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[9] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[10] = 90.0;
  // 11 (1/0) Activate dihedrals analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[12] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[13] = 90.0;
  // 14 (1/0) Activate inversions analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[15] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[16] = 90.0;

}

/*!
  \fn void init_lammps_field (classical_field * new_field)

  \brief initialize a LAMMPS classical force field

  \param new_field the force field to initialize
*/
void init_lammps_field (classical_field * new_field)
{
  // Preparing LAMMPS Control file
  new_field -> sys_opts = allocdouble (17);
  new_field -> io_opts = allocdouble (23);
  new_field -> ana_opts = allocdouble (17);
  new_field -> elec_opts = allocdouble (11);
  new_field -> vdw_opts = allocdouble (6);
  new_field -> met_opts = allocdouble (2);
  new_field -> equi_opts = allocdouble (17);
  new_field -> thermo_opts= allocdouble (10);
  new_field -> md_opts = allocdouble (20);
  new_field -> out_opts = allocdouble (31);
/*
    new_field -> io_opts[]
  */
  // Set job time control (yes/no)
  new_field -> io_opts[0] = 1.0;
  // Job time control (s):
  new_field -> io_opts[1] = 10000.0;
  // Set job closure time (yes/no)
  new_field -> io_opts[2] = 1.0;
  // Job closure time (s):
  new_field -> io_opts[3] = 100.0;
  // 4 (1/0) General I/O read interace
  // 5 General I/O method (0= MPI, 1= FORTRAN, 2= Traditional, 3= netCDF)
  // Reader count:
  new_field -> io_opts[6] = 12;
  // Batch size:
  new_field -> io_opts[7] = 2000000;
  // Buffer size:
  new_field -> io_opts[8] = 20000;
  // 9 (1/0) Error check on general read I/O
  // 10 (1/0) General I/O write interace
  // 11 General I/O method (0= MPI, 1= FORTRAN, 2= Traditional, 3= netCDF)
  // Real precision (0 = 32 bits, 1= 64 bits)
   new_field -> io_opts[12] = 1;
  // 13 Type (0= Sorted, 1= Unsorted)
  // Reader count:
  new_field -> io_opts[14] = 12;
  // Batch size:
  new_field -> io_opts[15] = 2000000;
  // Buffer size:
  new_field -> io_opts[16] = 20000;
  // 17 (1/0) Error check on general write I/O
  // 18 (1/0) Define random number generator seeds
  // n1 seed:
  new_field -> io_opts[19] = 1;
  // n2 seed:
  new_field -> io_opts[20] = 20;
  // n3 seed:
  new_field -> io_opts[21] = 300;
  // 22 (1/0) limit the number of processors in the z direction

  /*
    new_field -> sys_opts[]
  */
  // Relative dieletric constant:
  new_field -> sys_opts[0] = 1.0;
  // 1 = allowed local variation of the system density (%)
  // 2 (1/0) Ingore the particle indices in CONFIG file
  // 3 (1/0) Ignore strict checks
  // 4 (1/0) Skip detailed topology reporting
  // 5 (1/0) Ignore center of mass removal
  // 6 (1/0) Set tolerance of the relaxed shell model
  // Tolerance for the relaxed shell model:
  new_field -> sys_opts[7] = 1.0;
  // 8 (1/0) Set the subscelling threshold density of particles per link cell
  // Subscelling threshold density of particles per link cell:
  new_field -> sys_opts[9] = 50.0;
  // 10 (1/0) Create an expanded version of the current model ...
  // ... on x:
  new_field -> sys_opts[11] = 1.0;
  // ... on y:
  new_field -> sys_opts[12] = 1.0;
  // .. on z:
  new_field -> sys_opts[13] = 1.0;
  // 14 (1/0) Restart a calculation
  // 15 (0= Continue current, 1= Start new without T reset, 2= Start new with T reset)

  /*
    new_field -> equi_opts[]
  */
  new_field -> equi_opts[0] = 1.0;
  // Equilibrate for the first N steps, N=
  new_field -> equi_opts[1] = 20.0;
  // Scale temperature every N steps, N=
  new_field -> equi_opts[3] = 1.0;
  // Cap forces fmax=
  new_field -> equi_opts[5] = 1000.0;
  // Resample the momenta distribution during N steps N=
  new_field -> equi_opts[7] = 1.0;
  // 8 (1/0) = Minimize system configuration
  // 9 Minimize target (0=Force, 1=Energy, 2= Distance)
  // Minimization threshold:
  new_field -> equi_opts[10] = 50.0;
  // Minimization to be done every N steps, N=
  new_field -> equi_opts[11] = 1.0;
  // 12 (1/0) = Optimize system configuration from start
  // 13 Minimize target (0=Force, 1=Energy, 2= Distance)
  //  Minimization threshold:
  new_field -> equi_opts[14] = 50.0;
  // 15 (1/0) = Perform zero temperature (10K) optimization
  // 16 (1/0) = Include equilibration data in statistics

  /*
    new_field -> vdw_opts[]
  */
  // Vdw cutoff
  // 0 (1/0) Use VdW interactions
  new_field -> vdw_opts[0] = 1.0;
  // Vdw Cutoff radius
  new_field -> vdw_opts[1] = 12.0;
  // 2 (1/0) Enforce direct VdW calculation
  // 3 (1/0) Apply force shifting
  // 4 (1/0) Apply mixing rule
  // 5 Mixing rule (0= LB, 1= FH, 2= H, 3= HHG, 4= TT, 5= F)

  /*
    new_field -> elec_opts[]
  */
  // 0 (1/0) Evaluate electrostatics
  new_field -> elec_opts[0] = 1.0;
  // Long range cutoff:
  new_field -> elec_opts[1] = 12.0;
  // 2 (1/0) Add padding to rcut
  // 3 Padding in angstrom
  // 4 (1/0) Use extended coulombic exclusion
  // 5 Electrostatic calculation method (0= Direct, ...)
  new_field -> elec_opts[5] = 2.0;
  // Ewald precision:
  new_field -> elec_opts[6] = 1e-20;
  // 7 to 9 method options
  // 10 Evaluation every N steps, N=
  tmp_field -> elec_opts[10] = 1.0;

  /*
    new_field -> met_opts[]
  */
  // 0 (1/0) Enable direct calculation of metal interactions
  // 1 (1/0) Switch the default embeding fuctions

  // Ensemble (0= NVE, 1= NVT, 2= NPT, 4= NST)
  // new_field -> ensemble
  // Thermostat = f(ensemble):
  // new_field -> thermostat
  /*
    new_field -> thermo_opts[]
  */
  // 0-5 ensemble + thermostat related options
  // 6 (1/0) Use pseudo thermal bath
  // 7 Thermostat type (0= L+D, 1= L, 2= G, 3= D)
  // Thickness:
  new_field -> thermo_opts[8] = 2.0;
  // 9 = Target temperature

  /*
    new_field -> md_opts[]
  */
  // MD target temperature (K)
  new_field -> md_opts[0] = 300.0;
  // 1 Verlet integrator (0 = velocity, 1 = leapfrog)
  // Number of MD steps:
  new_field -> md_opts[2] = 100.0;
  // 3 Time step (0 = fixed, 1 = variable)
  // Time step delta(t)= (ps)
  new_field -> md_opts[4] = 0.001;
  // Target pressure (katms):
  new_field -> md_opts[5] = 0.001;
  // Variable time step, maximum time step:
  new_field -> md_opts[6] = 0.001;
  // Variable time step, maximum distance allowed:
  new_field -> md_opts[7] = 0.5;
  // Variable time step, minimun distance allowed:
  new_field -> md_opts[8] = 0.01;
  // Shake iterations limit, in cycles:
  new_field -> md_opts[9] = 250.0;
  // Shake tolerance:
  new_field -> md_opts[10] = 1e-6;
  // FIQA iterations limit, in cycles:
  new_field -> md_opts[11] = 100;
  // FIQA quaternions tolerance:
  new_field -> md_opts[12] = 1e-8;
  // 13 (1/0) Initiate impact on particles
  // Index if the particle to impact:
  new_field -> md_opts[14] = 1.0;
  // Time step of the impact:
  new_field -> md_opts[15] = 1.0;
  // Energy of the imapct (keV):
  new_field -> md_opts[16] = 1.0;
  // Direction of the impact (from center of mass) ...
  // ... on x:
  new_field -> md_opts[17] = 1.0;
  // ... on y:
  new_field -> md_opts[18] = 1.0;
  // ... on z:
  new_field -> md_opts[19] = 1.0;

  /*
    new_field -> out_opts[]
  */
  // 0 (1/0) Print Defect file ...
  // 1 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[2] = 1.0;
  // ... with a site inteerstitial cutoff:
  new_field -> out_opts[3] = 0.75;
  // 4 (1/0) Print Displacement file ..
  // 5 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[6] = 1.0;
  // ... with a qualifying cutoff:
  new_field -> out_opts[7] = 0.15;
  // 8 (1/0) Print HISTORY file ...
  // 9 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[10] = 1.0;
  // 11 Data level (0, 1, 2)
  // 12 (1/0) Print Atom's MSD ...
  // 13 ... starting from step
  // ... every N steps, N=
  new_field -> out_opts[14] = 1.0;
  // 15 (1/0) Print system data ...
  // ... every N steps, N=
  new_field -> out_opts[16] = 1.0;
  // 17 (1/0) Accumulate statistics ...
  // ... every N steps, N=
  new_field -> out_opts[18] = 1.0;
  // 19 (1/0) Rolling average stack ...
  // ...  to N steps, N=
  new_field -> out_opts[20] = 1.0;
  // 21 (1/0) RDF ...
  // ... every N steps, N=
  new_field -> out_opts[22] = 1.0;
  // ... with a bin size=
  new_field -> out_opts[23] = 0.05;
  // 24 (1/0) Overall VAF analysis ...
  // ... every N steps, N=
  new_field -> out_opts[25] = 1.0;
  // ... with a bin size=
  new_field -> out_opts[26] = 50.0;
  // 27 (1/0) Z density profile ...
  // ... every N steps, N=
  new_field -> out_opts[28] = 1.0;
  // 29 (1/0) ignoring time-av. VAFs
  // Dump restart information every N steps, N=
  new_field -> out_opts[30] = 1000.0;

  /*
    new_field -> ana_opts[]
  */
  // 0 (1/0) Activate all intra-molecular PDF analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[1] = 1.0;
  // ... number of delta r in:
  new_field -> ana_opts[2] = 100.0;
  // ... Cutoff:
  new_field -> ana_opts[3] = 2.0;
  // 4 (1/0) Activate bonds PDF analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[5] = 1.0;
  // ... number of delta r in:
  new_field -> ana_opts[6] = 100.0;
  // ... Cutoff:
  new_field -> ana_opts[7] = 2.0;
  // 8 (1/0) Activate angles analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[9] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[10] = 90.0;
  // 11 (1/0) Activate dihedrals analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[12] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[13] = 90.0;
  // 14 (1/0) Activate inversions analysis ...
  // ... every N steps, N=
  new_field -> ana_opts[15] = 1.0;
  // ... number of delta angle in:
  new_field -> ana_opts[16] = 90.0;
}

/*!
  \fn classical_field * create_force_field_data_structure (int ai)

  \brief initialize classical force field

  \param ai how to initialize the force field (0 = atomic species, 1 = total coordination, 2 = partial coordination)
*/
classical_field * create_force_field_data_structure (int ai)
{
  int i;
  classical_field * new_field;
  new_field = g_malloc0 (sizeof*new_field);
  tmp_field = new_field;
  new_field -> energy_unit = 1;
  new_field -> atom_init = ai;
  switch (activef)
  {
    case 0:
      init_dlpoly_field (new_field);
      break;
    case 1:
      init_lammps_field (new_field);
      break;
  }
  new_field -> molecules = tmp_proj -> modelfc -> mol_by_step[0];
  new_field -> first_molecule = g_malloc0 (sizeof*new_field -> first_molecule);
  tmp_fmol = new_field -> first_molecule;
#ifdef DEBUG
  g_debug ("Total num of molecules: %d", tmp_proj -> modelfc -> mol_by_step[0]);
#endif // DEBUG
  for (i=0; i < tmp_proj -> modelfc -> mol_by_step[0]; i++)
  {
    tmp_mol = & tmp_proj -> modelfc -> mols[0][i];
#ifdef DEBUG
    g_debug ("Init FORCE FIELD for: ");
    g_debug (" tmp_mol -> id=     %d", tmp_mol -> id);
    g_debug (" tmp_mol -> multi=  %d", tmp_mol -> multiplicity);
    g_debug (" tmp_mol -> natoms= %d", tmp_mol -> natoms);
#endif // DEBUG
    init_field_molecule (i);
    if (i <  tmp_proj -> modelfc -> mol_by_step[0]-1)
    {
      tmp_fmol -> next = g_malloc0 (sizeof*tmp_fmol -> next);
      tmp_fmol = tmp_fmol -> next;
    }
  }

  for (i=0; i<5; i++)
  {
    tmp_field -> nbody[i] = 0;
    tmp_field ->  first_body[i] = NULL;
  }
  // Tersoff cross terms
  tmp_field -> cross = NULL;


  // vdW start
  tmp_field -> nbody[0] = init_vdw (TRUE);
  // vdW end

  // External field(s) start
  new_field -> extern_fields = 0;
  new_field -> first_external = NULL;
  // External field(s) end

  // Non-bonded end
  return new_field;
}
