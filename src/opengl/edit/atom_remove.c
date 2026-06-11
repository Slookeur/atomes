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
* @file atom_remove.c
* @short Functions to remove bond(s) from a project \n
         Function to prepare the passivation (removal followed by insertion)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'atom_remove.c'
*
* Contains:
*

 - The functions to remove bond(s) from a project
 - The function to prepare the passivation (removal followed by insertion)

*
* List of functions:

  int test_this_fragment (int natomes, int fcoord, int fid, atom * atom_list, int * old_id, gboolean remove);

  gboolean * remove_bonds_from_project (project * this_proj, atomic_object * this_object, int * old_id, atom * new_list, gboolean remove, gboolean passivate);

  void set_mol_data (int the_atom, int the_mol);
  void to_remove_this_list_of_objects (project * this_proj, atom_search * asearch);
  void to_passivate_using_the_objects (project * this_proj, atom_search * asearch);

*/

#include "atom_edit.h"

int tmbs;
int molcounter;
int atoms_in_frag;
int * tigl;
int * migl;
int * togl;
int * ats_in_frag;
int * ats_id;
int * contacts;
int ** neighbors;

extern atom_search * allocate_atom_search (int proj, int action, int searchid, int tsize);

/*!
  \fn void set_mol_data_list (int the_atom, atom ** mol_list, int the_mol)

  \brief set molecule id for atom

  \param the_atom the atom id
  \param mol_list the list of atom(s) to look up
  \param the_mol the molecule id
*/

void set_mol_data_list (int aid, atom ** mol_list, int the_mol)
{
  int ab, ac, ad;
  molcounter ++;
  int the_atom = tigl[aid];
  togl[the_atom] = the_mol;
  tmbs ++;

  if (molcounter == 10000) goto end;
  if (tmbs == atoms_in_frag) goto end;
  int mid = migl[aid];
  for (ab=0; ab < mol_list[mid] -> numv; ab++)
  {
    ac = mol_list[mid] -> vois[ab];
    ad = tigl[ac];
    if (! togl[ad] && molcounter < 100000) set_mol_data_list (ac, mol_list, the_mol);
    if (tmbs == atoms_in_frag) goto end;
    if (molcounter == 10000) goto end;
  }

  molcounter --;
  end:;
}

/*!
  \fn int test_this_fragment (int natomes, int fcoord, int fid, atom * atom_list, int * old_id, gboolean remove)

  \brief verify that all atom(s) in the fragment are connected somehow, otherwise create new fragment

  \param natomes number of atom(s)
  \param fcoord the number of fragment(s)
  \param fid the fragment id
  \param atom_list the atom(s) list
  \param old_id the atom(s) old id list
  \param remove remove (1) or motion (0) action
*/
int test_this_fragment (int natomes, int fcoord, int fid, atom ** atom_list, int * old_id, gboolean remove)
{
  int i, j, k, l, m, n;
  gboolean modif;
  tigl = allocint (natomes);
  migl = allocint (natomes);
  i = 0;
  for (j=0; j<natomes; j++)
  {
    modif = FALSE;
    if (! remove)
    {
      modif = TRUE;
    }
    else if (old_id)
    {
      if (atom_list[j])
      {
        if (old_id[atom_list[j] -> id] > 0) modif = TRUE;
      }
    }
    if (modif)
    {
      if (atom_list[j] -> coord[2] == fid)
      {
         tigl[atom_list[j] -> id] = i;
         migl[atom_list[j] -> id] = j;
         i ++;
      }
    }
  }

  if (i)
  {
    togl = allocint (i);
    atoms_in_frag = i;

    // Check each atoms (recursive testing)
    int totmol = 0;
    tmbs = molcounter = 0;
    start:;
    i = 0;
    for (j=0; j<natomes; j++)
    {
      if (atom_list[j])
      {
        if (atom_list[j] -> coord[2] == fid)
        {
          k = tigl[atom_list[j] -> id];
          if (! molcounter && ! togl[k])
          {
            totmol ++;
            set_mol_data_list (atom_list[j] -> id, atom_list, totmol);
            if (tmbs == atoms_in_frag) goto end;
            if (molcounter == 10000) goto start;
            molcounter = 0;
          }
          else if (molcounter > 0 && togl[k] == totmol)
          {
            for (l=0; l<atom_list[j] -> numv; l++)
            {
              m = atom_list[j] -> vois[l];
              n = tigl[m];
              if (! togl[n])
              {
                molcounter = 0;
                i = 1;
                set_mol_data_list (m, atom_list, totmol);
                if (tmbs == atoms_in_frag) goto end;
                if (molcounter == 10000) goto start;
                molcounter = 1;
              }
            }
          }
        }
      }
    }
    if (tmbs < atoms_in_frag)
    {
      molcounter = 0;
      goto start;
    }
    if (molcounter) goto start;
    end:;
    i = 0;
    for (j=0; j<natomes; j++)
    {
      if (atom_list[j])
      {
        k = atom_list[j] -> id;
        l = tigl[k];
        if (togl[l] > 1)
        {
          atom_list[j] -> coord[2] = fcoord + togl[l] - 2;
          // g_debug ("togl[l] = %d, j= %d, atom_list[j] -> coord[2]= %d", togl[l], j, atom_list[j] -> coord[2]);
        }
        i = max (i, togl[l]);
      }
    }
    g_free (togl);
  }
  g_free (tigl);
  g_free (migl);
  return  i-1;
}

/*!
  \fn gboolean * remove_bonds_from_project (project * this_proj, atomic_object * this_object, int * old_id, atom * new_list, gboolean remove, gboolean passivate)

  \brief remove bond(s) from project

  \param this_proj the target project
  \param this_object the target insert object, if any
  \param old_id the atom(s) id list
  \param new_list the new atom list
  \param remove remove (1) or motion (0) action
  \param passivate passivate (1) or not (0)
*/
gboolean * remove_bonds_from_project (project * this_proj, atomic_object * this_object, int * old_id, atom * new_list, gboolean remove, gboolean passivate)
{
  int h, i, j, k, l, m;
  int tmpbond[2];
  int ** tmpbondid[2];
  atom * tmp_list;
  gboolean * frag_to_test;
  gboolean * frag_to_remove;
  gboolean * show_frag = NULL;
  int * per_frag;
  int * in_frag;
  int * tmp_vois = NULL;
  int * id_mod = NULL;

  /* tmp_list = new_list;
  while (tmp_list)
  {
    i = tmp_list -> id;
    if ((remove && old_id[i] > 0) || ! remove)
    {
      g_debug ("Keeping atom: %d", i+1);
      g_debug ("        atom: %d has: %d neighbor(s):", i+1, tmp_list -> numv);
      for (j=0; j<tmp_list -> numv; j++)
      {
      g_debug ("           n: %d, atom= %d", j+1, tmp_list -> vois[j]+1);
      }
    }
    tmp_list = tmp_list -> next;
  }*/

  int tcf = (this_proj) ? this_proj -> coord -> totcoord[2] : this_object -> coord -> totcoord[2];
  if (this_proj)
  {
    if (tcf)
    {
      show_frag  = duplicate_bool (tcf, this_proj -> modelgl -> anim -> last -> img -> show_coord[2]);
    }
    else
    {
      show_frag = allocbool (1);
      show_frag[0] = TRUE;
      tcf = 1;
    }
  }

  int nat = (this_proj) ? this_proj -> natomes : this_object -> atoms;
  per_frag = allocint (tcf);
  in_frag = allocint (tcf);
  id_mod = allocint (nat);
  tmp_vois = allocint (20);
  h = -1;
  tmp_list = new_list;
  for (i=0; i<nat; i++)
  {
    j = (this_proj) ? this_proj -> atoms[0][i].coord[2] : tmp_list -> coord[2];
    per_frag[j] ++;
    if (old_id[i] < 0)
    {
      in_frag[j] ++;
      if (! remove && ! passivate)
      {
        h ++;
        id_mod[tmp_list -> id] = h;
      }
    }
    else
    {
      h ++;
      id_mod[tmp_list -> id] = h;
    }

    if (old_id[i] > 0 || ! remove)
    {
      if (tmp_list -> numv > 0)
      {
        k = 0;
        // Save the neighbors list for this atom
        for (l=0; l<tmp_list -> numv; l++)
        {
          m = tmp_list -> vois[l];
          if ((remove && old_id[m] > 0) || (! remove && old_id[i] > 0 && old_id[m] > 0) || (! remove && old_id[i] < 0 && old_id[m] < 0))
          {
            tmp_vois[k] = m;
            k ++;
          }
        }
        g_free (tmp_list -> vois);
        tmp_list -> vois = NULL;
        tmp_list -> numv = k;
        if (k)
        {
          // Correct the neighbor list for this atom
          tmp_list -> vois = allocint (k);
          for (l=0; l<k; l++) tmp_list -> vois[l] = tmp_vois[l];
        }
      }
    }
    if ( old_id[i] > 0 || ! remove || passivate)  tmp_list = tmp_list -> next;
  }
  g_free (tmp_vois);

  // Taking care of the isolated atom(s)
  tmp_list = new_list;
  while (tmp_list)
  {
    i =  tmp_list -> coord[2];
    if (! tmp_list -> numv && per_frag[i] > 1 && old_id[tmp_list -> id] > 0)
    {
      // If the atom has no neighbors, and if it is not the last one in the fragment
      show_frag = g_realloc (show_frag, (tcf+1)*sizeof*show_frag);
      per_frag = g_realloc (per_frag, (tcf+1)*sizeof*per_frag);
      in_frag = g_realloc (in_frag, (tcf+1)*sizeof*in_frag);
      show_frag[tcf] = show_frag[i];
      per_frag[tcf] = 1;
      per_frag[i] --;
      tmp_list -> coord[2] = tcf;
      tcf ++;
    }
    tmp_list = tmp_list -> next;
  }

  frag_to_test = allocbool (tcf);
  frag_to_remove = allocbool (tcf);
  for (i=0; i<tcf; i++)
  {
    if (in_frag[i] == per_frag[i])
    {
      if (remove)
      {
        frag_to_remove[i] = TRUE;
      }
      else
      {
        frag_to_test[i] = TRUE;
      }
    }
    else if (per_frag[i] > 1 && in_frag[i])
    {
      frag_to_test[i] = TRUE;
    }
    // g_debug ("i= %d, frag_to_test[%d]= %d, frag_to_remove[%d]= %d, in_frag[%d]= %d, per_frag[%d]= %d", i, i, frag_to_test[i], i, frag_to_remove[i], i, in_frag[i], i, per_frag[i]);
  }
  g_free (in_frag);
  g_free (per_frag);
  atom ** atom_list = g_malloc0(nat*sizeof*atom_list);
  tmp_list = new_list;
  while (tmp_list)
  {
    i = tmp_list -> id;
    if (old_id[i] > 0 || ! remove)
    {
      atom_list[i] = duplicate_atom (tmp_list);
    }
    tmp_list = tmp_list -> next;
  }
  if (remove && this_proj)
  {
    i = 0;
    // Removing fragment(s) if needed
    int * new_fid = allocint (tcf);
    for (j=0; j<tcf; j++) new_fid[j] = j;
    for (j=0; j<tcf-1; j++)
    {
      if (frag_to_remove[j])
      {
        for (k=j+1; k<tcf; k++) new_fid[k] --;
      }
    }
    for (j=0; j<tcf; j++)
    {
      if (frag_to_remove[j])
      {
        for (k=j; k<tcf-i-1; k++) show_frag[k] = show_frag[k+1];
        for (k=j; k<tcf-i-1; k++) frag_to_test[k] = frag_to_test[k+1];
        i ++;
      }
    }
    for (j=0; j<nat; j++)
    {
      if (atom_list[j]) atom_list[j] -> coord[2] = new_fid[atom_list[j] -> coord[2]];
    }
    g_free (new_fid);
    tcf -= i;
    if (i)
    {
      show_frag = g_realloc (show_frag, tcf*sizeof*show_frag);
    }
  }
  i = tcf;
  for (j=0; j<i; j++)
  {
    if (frag_to_test[j])
    {
      // g_debug ("testing frag[%d], remove= %d, tcf= %d, nat= %d, j= %d", j, remove, tcf, nat, j);
      k = test_this_fragment (nat, tcf, j, atom_list, old_id, remove);
      // g_debug ("After testing: k= %d", k);
      if (k > 0)
      {
        show_frag = g_realloc (show_frag, (tcf+k)*sizeof*show_frag);
        for (l=tcf; l<tcf+k; l++) show_frag[l] = show_frag[j];
        tcf += k;
      }
    }
  }
  g_free (frag_to_test);
  g_free (frag_to_remove);
  if (this_proj)
  {
    this_proj -> coord -> totcoord[2] = tcf;
  }
  else
  {
    this_object -> coord -> totcoord[2] = tcf;
  }
  tmp_list = new_list;
  while (tmp_list)
  {
    i = tmp_list -> id;
    if (old_id[i] > 0 || ! remove)
    {
      tmp_list -> coord[2] = atom_list[i] -> coord[2];
      g_free (atom_list[i]);
      // g_debug ("End:: id= %d, coord[2]= %d", tmp_list -> id, tmp_list -> coord[2]);
    }
    tmp_list = tmp_list -> next;
  }
  g_free (atom_list);

  if (this_proj)
  {
    for (i=0; i<2; i++)
    {
      tmpbondid[i] = NULL;
      tmpbond[i] = 0;
    }
    for (i=0; i<2; i++)
    {
      if (this_proj -> modelgl -> bonds[0][i])
      {
        tmpbondid[i] = allocdint (this_proj -> modelgl -> bonds[0][i], 2);
        j = 0;
        for (k=0; k<this_proj -> modelgl -> bonds[0][i]; k++)
        {
          l = this_proj -> modelgl -> bondid[0][i][k][0];
          m = this_proj -> modelgl -> bondid[0][i][k][1];
          if (old_id[l] > 0 && old_id[m] > 0)
          {
            tmpbondid[i][j][0] = l;
            tmpbondid[i][j][1] = m;
            j ++;
          }
          else if (! remove && (old_id[l] < 0 && old_id[m] < 0))
          {
            tmpbondid[i][j][0] = l;
            tmpbondid[i][j][1] = m;
            j ++;
          }
        }
        tmpbond[i] = j;
        // g_debug ("i= %d, tmpbond[%d]= %d", i, i, tmpbond[i]);
      }
    }
    distance clo;
    for (i=0; i<2; i++)
    {
      if (this_proj -> modelgl -> bonds[0][i])
      {
        g_free (this_proj -> modelgl -> bondid[0][i]);
        this_proj -> modelgl -> bondid[0][i] = NULL;
        if (i)
        {
          g_free (this_proj -> modelgl -> clones[0]);
          this_proj -> modelgl -> clones[0] = NULL;
        }
        if (tmpbond[i])
        {
          if (i) this_proj -> modelgl -> clones[0] = g_malloc0(tmpbond[i]*sizeof*this_proj -> modelgl -> clones[0]);
          this_proj -> modelgl -> bondid[0][i] = allocdint (tmpbond[i], 2);
          for (k=0; k<tmpbond[i]; k++)
          {
            this_proj -> modelgl -> bondid[0][i][k][0] = id_mod[tmpbondid[i][k][0]];
            this_proj -> modelgl -> bondid[0][i][k][1] = id_mod[tmpbondid[i][k][1]];
            //g_debug ("bc:: i= %d, j= %d, a= %d, b= %d", i, k, tmpbondid[i][k][0], tmpbondid[i][k][1]);
            if (i)
            {
              l = this_proj -> modelgl -> bondid[0][i][k][0];
              m = this_proj -> modelgl -> bondid[0][i][k][1];
              clo = distance_3d (& this_proj -> cell, 0, & this_proj -> atoms[0][l], & this_proj -> atoms[0][m]);
              this_proj -> modelgl -> clones[0][k].x = clo.x;
              this_proj -> modelgl -> clones[0][k].y = clo.y;
              this_proj -> modelgl -> clones[0][k].z = clo.z;
            }
          }
        }
        this_proj -> modelgl -> bonds[0][i] = tmpbond[i];
        this_proj -> modelgl -> allbonds[i] = tmpbond[i];
      }
      if (tmpbondid[i])
      {
        g_free (tmpbondid[i]);
        tmpbondid[i] = NULL;
      }
    }
  }

  if (this_proj)
  {
    tmp_list = new_list;
    while (tmp_list)
    {
      i = tmp_list -> id;
      if (old_id[i] > 0 || ! remove)
      {
        if (tmp_list -> numv > 0)
        {
          for (j=0; j<tmp_list -> numv; j++)
          {
            k = tmp_list -> vois[j];
            tmp_list -> vois[j] = id_mod[k];
          }
        }
      }
      tmp_list = tmp_list -> next;
    }
  }
  if (id_mod)
  {
    g_free (id_mod);
    id_mod = NULL;
  }
  /*tmp_list = new_list;
  while (tmp_list)
  {
    i = tmp_list -> id;
    if ((remove && old_id[i] > 0) || ! remove)
    {
      g_debug ("Correct atom: %d", i+1);
      g_debug ("        atom: %d has: %d neighbor(s):", i+1, tmp_list -> numv);
      for (j=0; j<tmp_list -> numv; j++)
      {
      g_debug ("           n: %d, atom= %d", j+1, tmp_list -> vois[j]+1);
      }
    }
    tmp_list = tmp_list -> next;
  }*/
  return show_frag;
}

/*!
  \fn void to_remove_this_list_of_objects (project * this_proj, atom_search * asearch)

  \brief prepaer to remove a list of object(s) from a project, one object after another.

  \param this_proj the target project
  \param asearch the target atom search
*/
void to_remove_this_list_of_objects (project * this_proj, atom_search * asearch)
{
  int i, j;
  atomic_object * tmp_object;
  atomic_object * tmp_replace;
  if (asearch -> action == REPLACE)
  {
    if (asearch -> pointer[0].c == 3)
    {
      tmp_replace = this_proj -> modelgl -> atom_win -> to_be_inserted[0];
    }
    else
    {
      tmp_replace = this_proj -> modelgl -> atom_win -> to_be_inserted[3];
    }
  }
  int filter = get_asearch_filter (asearch);
  int object = get_asearch_object (asearch);
  remove_search = allocate_atom_search (this_proj -> id, REMOVE, REMOVE, this_proj -> natomes);
  for (i=0; i<asearch -> todo_size; i++)
  {
    if (asearch -> todo[i])
    {
      tmp_object = NULL;
      if (filter > 0 && filter < 3 && ((object && ! asearch -> passivating) || (object == 2 && asearch -> passivating)))
      {
        tmp_object = create_object_from_atom_coordination (this_proj, filter-1, i, remove_search);
      }
      else if (filter > 2 && ((object && ! asearch -> passivating) || (object == 2 && asearch -> passivating)))
      {
        j = (asearch -> mode && filter == 4) ? filter - 1 : filter;
        tmp_object = create_object_from_frag_mol (this_proj, j-1, i, remove_search);
      }
      else
      {
        remove_search -> todo[i] = 1;
        if (asearch -> action == REPLACE)
        {
          tmp_object = g_malloc0(sizeof*tmp_object);
          tmp_object -> baryc = allocdouble (3);
          tmp_object -> baryc[0] = this_proj -> atoms[0][i].x;
          tmp_object -> baryc[1] = this_proj -> atoms[0][i].y;
          tmp_object -> baryc[2] = this_proj -> atoms[0][i].z;
        }
      }
      if (asearch -> action == REPLACE)
      {
        for (j=0; j<3; j++) tmp_replace -> baryc[j] += tmp_object -> baryc[j];
        tmp_replace = tmp_replace -> next;
      }
      if (tmp_object) g_free (tmp_object);
    }
  }

  i = 0;
  for (j=0; j<this_proj -> natomes; j++)
  {
    if (remove_search -> todo[j]) i++;
  }
  remove_search -> in_selection = i;
}

/*!
  \fn void to_passivate_using_the_objects (project * this_proj, atom_search * asearch)

  \brief prepare passivation (delete of an object, then insert of another one at the same location)

  \param this_proj the target project
  \param asearch the target atom search
*/
void to_passivate_using_the_objects (project * this_proj, atom_search * asearch)
{
  int i, j, k, l, m;
  atomic_object * object = NULL;
  atomic_object * tmp_oba, * tmp_obb;
  int filter = get_asearch_filter (asearch);
  int num_elem = asearch -> todo_size;
  int * passivate_todo = duplicate_int (num_elem, asearch -> todo);
  g_free (asearch -> todo);
  allocate_todo (asearch, this_proj -> natomes);
  if (this_proj -> modelgl -> atom_win -> to_be_inserted[3])
  {
    tmp_oba = this_proj -> modelgl -> atom_win -> to_be_inserted[3];
    object = duplicate_atomic_object (tmp_oba);
    tmp_obb = object;
    while (tmp_oba -> next)
    {
      tmp_oba = tmp_oba -> next;
      tmp_obb -> next = duplicate_atomic_object (tmp_oba);
      tmp_obb = tmp_obb -> next;
    }
    g_free (this_proj -> modelgl -> atom_win -> to_be_inserted[3]);
    this_proj -> modelgl -> atom_win -> to_be_inserted[3] = NULL;
  }
  gboolean doit;
  asearch -> in_selection = 0;
  tmp_oba = object;
  for (i=0; i<num_elem; i++)
  {
    if (passivate_todo[i])
    {
      for (j=0; j<this_proj -> natomes; j++)
      {
        doit = FALSE;
        if (this_proj -> atoms[0][j].pick[0])
        {
          k = this_proj -> atoms[0][j].sp;
          switch (filter)
          {
            case 0:
              if (k == i) doit = TRUE;
              break;
            case 1:
              if (this_proj -> atoms[0][j].numv == i) doit = TRUE;
              break;
            case 2:
              l = this_proj -> atoms[0][j].coord[1];
              for (m=0;m<k;m++) l += this_proj -> coord -> ntg[1][m];
              if (l == i) doit = TRUE;
              break;
            default:
              if (this_proj -> atoms[0][j].coord[filter-1] == i) doit = TRUE;
              break;
          }
          if (doit)
          {
            asearch -> todo[j] = 1;
            if (this_proj -> modelgl -> atom_win -> to_be_inserted[3] == NULL)
            {
              this_proj -> modelgl -> atom_win -> to_be_inserted[3] = duplicate_atomic_object (tmp_oba);
              tmp_obb = this_proj -> modelgl -> atom_win -> to_be_inserted[3];
            }
            else
            {
              tmp_obb -> next = duplicate_atomic_object (tmp_oba);
              tmp_obb -> next -> prev = tmp_obb;
              tmp_obb = tmp_obb -> next;
            }
            tmp_obb -> id = j;
            asearch -> in_selection ++;
          }
        }
      }
      if (tmp_oba -> next != NULL) tmp_oba = tmp_oba -> next;
    }
  }
  if (passivate_todo) g_free (passivate_todo);
  apply_action (this_proj, asearch);
}
