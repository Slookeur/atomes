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
* @file dlp_viz.c
* @short functions to handle the visualization events when creating a classical force field
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_viz.c'
*
* Contains:
*

 - The functions to handle the visualization events when creating a classical force field

*
* List of functions:

  int get_field_objects (int id, int jd);

  gboolean show_field_object (int id, int jd, int kd);

  void field_selection (int i, int viz, int lab, int aid);
  void viz_fragment (field_molecule * fmol, int id, int viz);
  void field_unselect_all ();
  void visualize_bonds (int viz, int aid,
                         field_atom* at,
                         field_atom* bt);
  void visualize_angles (int viz, int aid,
                         field_atom* at,
                         field_atom* bt,
                         field_atom* ct);
  void visualize_dihedrals (int viz, int did,
                            field_atom* at,
                            field_atom* bt,
                            field_atom* ct,
                            field_atom* dt);
  void visualize_imp_inv (int viz, int dih, int iid,
                          field_atom* at,
                          field_atom* bt,
                          field_atom* ct,
                          field_atom* dt);
  void visualize_body (int viz, int bd, field_nth_body * body);
  void select_object (int id, int jd, int kd);
  void visualize_single_struct (int id, int jd, int kd, int * ids);
  void visualize_object (int id, int jd, int kd);
  void check_to_visualize_properties_for_this_field_mol (int pid, int mol);
  void check_to_visualize_properties (int pid);
  void update_mol_tree (int a, int b);

  G_MODULE_EXPORT void on_toggle_visualize_or_select_object (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data);
  G_MODULE_EXPORT void visualize_or_select_all_elements (GtkTreeViewColumn * col, gpointer data);

*/

#include "dlp_field.h"
#include "project.h"
#include "glwindow.h"
#include "glview.h"

tint toviz;

extern int is_special[MAXDATA][11];
extern void update_selection_list (atom_selection * at_list, atom * at, gboolean add);
extern void init_default_shaders (glwin * view);

/*!
  \fn void field_selection (int i, int viz, int lab, int aid)

  \brief select / unselect atom

  \param i the atom id in the model
  \param viz visualization status (0= hide, 1 = show)
  \param lab the value to display for the label
  \param aid the value to use for the color
*/
void field_selection (int i, int viz, int lab, int aid)
{
  if (viz && tmp_proj -> atoms[0][i].pick[0] != viz)
  {
    tmp_view -> picked ++;
  }
  else if (! viz && tmp_proj -> atoms[0][i].pick[0] != viz)
  {
    tmp_view -> picked --;
  }
  tmp_proj -> atoms[0][i].pick[0] = viz;
  tmp_proj -> atoms[0][i].label[0] = lab;
  tmp_proj -> atoms[0][i].coord[4] = aid;
}

/*!
  \fn void viz_fragment (field_molecule * fmol, int id, int viz)

  \brief show / hide fragment

  \param fmol the target field molecule
  \param id the fragment id
  \param viz visualization status (0 = hide, 1 = show)
*/
void viz_fragment (field_molecule * fmol, int id, int viz)
{
  int i;
  for (i=0; i<tmp_proj -> natomes; i++)
  {
    if (tmp_proj -> atoms[0][i].coord[2] == fmol -> fragments[id]) field_selection (i, viz, viz, id);
  }
  init_default_shaders (tmp_view);
}

/*!
  \fn void field_unselect_all ()

  \brief unselect all atoms
*/
void field_unselect_all ()
{
  int i;
  for (i=0; i<tmp_proj -> natomes; i++) field_selection (i, FALSE, FALSE, 0);
  init_default_shaders (tmp_view);
}

extern gboolean in_bond (int at, int bd[2]);

/*!
  \fn void visualize_bonds (int viz, int aid, field_atom * at, field_atom * bt)

  \brief show / hide bond / bond restraint

  \param viz visualization status (0 = hide, 1 = show)
  \param aid the id of the structural element
  \param at 1st field atom
  \param bt 2nd field atom
*/
void visualize_bonds (int viz, int bid,
                      field_atom* at,
                      field_atom* bt)
{
  int i, j, k, l;
  for (i=0; i<at -> num; i++)
  {
    j = at -> list[i];
    for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].faid == bt -> id)
      {
        field_selection (j, viz, FALSE, bid);
        field_selection (l, viz, FALSE, bid);
      }
    }
  }
}

/*!
  \fn void visualize_angles (int viz, int aid,
                             field_atom* at,
                             field_atom* bt,
                             field_atom* ct)

  \brief show / hide angle / angle restraint

  \param viz visualization status (0 = hide, 1 = show)
  \param aid the id of the structural element
  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
*/
void visualize_angles (int viz, int aid,
                       field_atom* at,
                       field_atom* bt,
                       field_atom* ct)
{
  int i, j, k, l, m, n;
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
            field_selection (j, viz, FALSE, aid);
            field_selection (l, viz, FALSE, aid);
            field_selection (n, viz, FALSE, aid);
          }
        }
      }
    }
  }
}

/*!
  \fn void visualize_dihedrals (int viz, int did,
                                field_atom* at,
                                field_atom* bt,
                                field_atom* ct,
                                field_atom* dt)

  \brief show / hide dihedral / dihedral restraint

  \param viz visualization status (0 = hide, 1 = show)
  \param did the id of the structural element
  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
  \param dt 4th field atom
*/
void visualize_dihedrals (int viz, int did,
                          field_atom* at,
                          field_atom* bt,
                          field_atom* ct,
                          field_atom* dt)
{
  int i, j, k, l, m, n, o, p;
  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].faid == tmp_fbt -> id)
      {
        for (m=0; m<tmp_proj -> atoms[0][l].numv; m++)
        {
          n = tmp_proj -> atoms[0][l].vois[m];
          if (n != j && tmp_proj -> atoms[0][n].faid == tmp_fct -> id)
          {
            for (o=0; o<tmp_proj -> atoms[0][n].numv; o++)
            {
              p = tmp_proj -> atoms[0][n].vois[o];
              if (p != j && p != l && tmp_proj -> atoms[0][p].faid == tmp_fdt -> id)
              {
                field_selection (j, viz, FALSE, did);
                field_selection (l, viz, FALSE, did);
                field_selection (n, viz, FALSE, did);
                field_selection (p, viz, FALSE, did);
              }
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void visualize_imp_inv (int viz, int dih, int iid,
                              field_atom* at,
                              field_atom* bt,
                              field_atom* ct,
                              field_atom* dt)

  \brief show / hide improper or inversion

  \param viz visualization status (0 = hide, 1 = show)
  \param dih 6 = improper, 7 = inversion
  \param iid the id of the structural element
  \param at 1st field atom
  \param bt 2nd field atom
  \param ct 3rd field atom
  \param dt 4th field atom
*/
void visualize_imp_inv (int viz, int dih, int iid,
                        field_atom* at,
                        field_atom* bt,
                        field_atom* ct,
                        field_atom* dt)
{
  int i, j, k, l, m, n, o, p;
  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    if ((tmp_proj -> atoms[0][j].numv > 2 && dih == 6) || (tmp_proj -> atoms[0][j].numv == 3 && dih == 7))
    {
      for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
      {
        l = tmp_proj -> atoms[0][j].vois[k];
        if (tmp_proj -> atoms[0][l].faid == tmp_fbt -> id)
        {
          for (m=0; m<tmp_proj -> atoms[0][j].numv; m++)
          {
            if (m != k)
            {
              n = tmp_proj -> atoms[0][j].vois[m];
              if (tmp_proj -> atoms[0][n].faid == tmp_fct -> id)
              {
                for (o=0; o<tmp_proj -> atoms[0][j].numv; o++)
                {
                  if (o != k && o != m)
                  {
                    p = tmp_proj -> atoms[0][j].vois[o];
                    if (tmp_proj -> atoms[0][p].faid == tmp_fdt -> id)
                    {
                      field_selection (j, viz, FALSE, iid);
                      field_selection (l, viz, FALSE, iid);
                      field_selection (n, viz, FALSE, iid);
                      field_selection (p, viz, FALSE, iid);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void visualize_body (int viz, int bd, field_nth_body * body)

  \brief show / hide non bonded interaction

  \param viz visualization status (0 = hide, 1 = show)
  \param bd the type of non bonded interaction
  \param body the target non bonded interaction
*/
void visualize_body (int viz, int bd, field_nth_body * body)
{
  int h, i, j, k, l;
  l = body_at (body -> bd);
  for (h=0; h<l; h++)
  {
    for (i=0; i < body -> na[h]; i++)
    {
      tmp_fat = get_active_atom (body -> ma[h][i], body -> a[h][i]);
      for (j=0; j < tmp_fat -> num; j++)
      {
        k = tmp_fat -> list[j];
        field_selection (k, viz, viz, bd);
      }
    }
  }
}

/*!
  \fn int get_field_objects (int id, int jd)

  \brief get the number of this type of field object

  \param id the tyoe of field object
  \param jd the field molecule id
*/
int get_field_objects (int id, int jd)
{
  int kd;
  int i;
  switch (id)
  {
    case -1:
      kd = 0;
      for (i=0; i<tmp_field -> molecules; i++) kd = max(kd, get_active_field_molecule (i) -> mol -> natoms);
      break;
    case 0:
      kd = tmp_field -> molecules;
      break;
    case 1:
      kd = 0;
      for (i=0; i<tmp_field -> molecules; i++) kd = max(kd, get_active_field_molecule (i) -> atoms);
      break;
    case 2:
      kd = get_active_field_molecule (jd) -> shells;
      break;
    case 3:
      kd = get_active_field_molecule (jd) -> constraints;
      break;
    case 4:
      kd = get_active_field_molecule (jd) -> pmfs;
      break;
    case 5:
      kd = get_active_field_molecule (jd) -> rigids;
      break;
    case 6:
      kd = get_active_field_molecule (jd) -> tethered;
      break;
    case SEXTERN:
      kd = tmp_field -> extern_fields;
      break;
    default:
      if (id < MOLIMIT)
      {
        kd = get_active_field_molecule (jd) -> nstruct[id-7];
      }
      else
      {
        kd = tmp_field -> nbody[id - MOLIMIT];
      }
      break;
  }
  return kd;
}

/*!
  \fn void select_object (int id, int jd, int kd)

  \brief select structural element

  \param id the type of structural element
  \param jd the object id, if any
  \param kd the field molecule id
*/
void select_object (int id, int jd, int kd)
{
  int i;
  field_object = id;
  num_field_objects = get_field_objects (id, kd);
  switch (id)
  {
    case 2:
    // Select a core-shell unit in a molecule
      tmp_fshell = get_active_shell (kd, jd);
      if (tmp_fshell -> ia[0] < 0 || tmp_fshell -> ia[1] < 0) toviz.c = FALSE;
      tmp_fshell -> use = toviz.c;
      break;
    case 3:
    // Select a contraint bond in a molecule
      tmp_fcons = get_active_constraint (kd, jd);
      if (! tmp_fcons -> ia[0] || ! tmp_fcons -> ia[1]) toviz.c = FALSE;
      tmp_fcons -> use = toviz.c;
      break;
    case 4:
    // Select a PMF in a molecule
      tmp_fpmf = get_active_field_molecule (kd) -> first_pmf;
      for (i=0; i < get_active_field_molecule(kd) -> pmfs; i++)
      {
        tmp_fpmf -> use = FALSE;
        if (tmp_fpmf -> next != NULL) tmp_fpmf = tmp_fpmf -> next;
      }
      tmp_fpmf = get_active_pmf (kd, jd);
      if (tmp_fpmf -> num[0] > 0 && tmp_fpmf -> num[1] > 0) tmp_fpmf -> use = toviz.c;
      break;
    case SEXTERN:
      tmp_fext = tmp_field -> first_external;
      for (i=0; i < tmp_field -> extern_fields; i++)
      {
        tmp_fext -> use = FALSE;
        if (tmp_fext -> next != NULL) tmp_fext = tmp_fext -> next;
      }
      tmp_fext = get_active_external (jd);
      tmp_fext -> use = toviz.c;
      break;
    default:
      if (id < MOLIMIT)
      {
        get_active_struct (id-7, kd, jd) -> def -> use = toviz.c;
      }
      else
      {
        get_active_body (jd, id - MOLIMIT) -> use = toviz.c;
      }
      break;
  }
}

/*!
  \fn void visualize_single_struct (int id, int jd, int kd, int * ids)

  \brief visualize single structural element

  \param id the type of structural element
  \param jd the object id, if any
  \param kd the field molecule id
  \param ids the list of atom id in the fragment
*/
void visualize_single_struct (int id, int jd, int kd, int * ids)
{
  int i, j, k, l, m, n, o, p;
  int * idat;
  i = struct_id(id);
  idat = allocint (i);
  for (j=0; j<i; j++)
  {
    k = ids[j];
    idat[j] = get_active_atom (kd, tmp_fmol -> atoms_id[k][0].a) -> id;
  }
  for (j=0; j<i; j++)
  {
    k = ids[j];
    for (l=0; l< tmp_fmol -> multi; l++)
    {
      m = tmp_fmol -> atoms_id[k][l].b;
      n = get_active_atom (kd, tmp_fmol -> atoms_id[k][l].a) -> list[m];
      field_selection (n, 0, 0, k);
    }
  }
  g_free (idat);
  i = ids[0];
  j = ids[1];
  for (l=0; l< tmp_fmol -> multi; l++)
  {
    m = tmp_fmol -> atoms_id[i][l].b;
    n = get_active_atom (kd, tmp_fmol -> atoms_id[i][l].a) -> list[m];
    field_selection (n, toviz.c, 0, jd);
    o = tmp_fmol -> atoms_id[j][l].b;
    p = get_active_atom (kd, tmp_fmol -> atoms_id[j][l].a) -> list[o];
    field_selection (p, toviz.c,  0, jd);
  }
  if (id > 8 && id < 15)
  {
    k = ids[2];
    for (l=0; l< tmp_fmol -> multi; l++)
    {
      o = tmp_fmol -> atoms_id[k][l].b;
      p = get_active_atom (kd, tmp_fmol -> atoms_id[k][l].a) -> list[o];
      field_selection (p, toviz.c, 0, jd);
    }
    if (id > 10)
    {
      i = ids[3];
      for (l=0; l< tmp_fmol -> multi; l++)
      {
        o = tmp_fmol -> atoms_id[i][l].b;
        p = get_active_atom (kd, tmp_fmol -> atoms_id[i][l].a) -> list[o];
        field_selection (p, toviz.c, 0, jd);
      }
    }
  }
}

/*!
  \fn void visualize_object (int id, int jd, int kd)

  \brief visualize object and update OpenGL rendering

  \param id the type of object
  \param jd the molecule id, if any
  \param kd the object id, if any
*/
void visualize_object (int id, int jd, int kd)
{
  int l, m, n, o, p, q, r, s;
  field_atom* tshell;
  field_object = id;
  num_field_objects = get_field_objects (id, kd);
  switch (id)
  {
    case -1:
      // View molecule atoms id
      tmp_fmol = get_active_field_molecule (jd);
      tmp_fmol -> show_id = toviz.c;
      tmp_fat = tmp_fmol -> first_atom;
      for (m=0; m < tmp_fmol -> atoms; m++)
      {
        for (n=0; n < tmp_fat -> num; n++)
        {
          field_selection (tmp_fat -> list[n], toviz.c, toviz.c, tmp_fat -> list_id[n]);
        }
        if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
      }
      break;
    case 0:
      // View an entire molecule
      tmp_fmol = get_active_field_molecule (jd);
      tmp_fmol -> show = toviz.c;
      for (m=0; m<tmp_fmol -> multi; m++)
      {
        toviz.a = 2;
        toviz.b = tmp_fmol -> fragments[m];
        for (n=0; n<tmp_proj -> natomes; n++)
        {
          if (tmp_proj -> atoms[0][n].coord[2] == toviz.b) field_selection (n, toviz.c, toviz.c, jd);
        }
      }
      break;
    case 1:
      // View a type of atoms in a molecule
      tmp_fat = get_active_atom (kd, jd);
      tmp_fat -> show = toviz.c;
      for (m=0; m<tmp_fat -> num; m++)
      {
        n = tmp_fat -> list[m];
        field_selection (n, toviz.c, toviz.c, jd);
      }
      break;
    case 2:
      // View a type of Core-shell unit in a molecule
      tmp_fshell = get_active_shell (kd, jd);
      tmp_fshell -> show = toviz.c;
      for (m=0; m<2; m++)
      {
        if (tmp_fshell -> ia[m])
        {
          n = tmp_fshell -> ia[0]-1;
          tshell = get_active_atom (tmp_fmol -> id, tmp_fmol -> atoms_id[0][n].a);
          for (o=0; o<tmp_fmol -> multi; o++)
          {
            p = tmp_fmol -> atoms_id[n][o].b;
            field_selection (tshell -> list[p], toviz.c, toviz.c, jd);
          }
        }
      }
      break;
    case 3:
      // View a type of bond constraint in a molecule
      tmp_fcons = get_active_constraint (kd, jd);
      tmp_fcons -> show = toviz.c;
      for (l=0; l<2; l++)
      {
        if (tmp_fcons -> ia[l])
        {
          for (m=0; m<tmp_fmol -> multi; m++)
          {
            n = tmp_fmol -> atoms_id[tmp_fcons -> ia[l]-1][m].a;
            o = tmp_fmol -> atoms_id[tmp_fcons -> ia[l]-1][m].b;
            p = get_active_atom (tmp_fmol -> id, n) -> list[o];
            field_selection (p, toviz.c, toviz.c, jd);
          }
        }
      }
      break;
    case 4:
      tmp_fpmf = get_active_pmf (kd, jd);
      tmp_fpmf -> show = toviz.c;
      for (m=0; m<2; m++)
      {
        if (tmp_fpmf -> num[m] > 0)
        {
          for (n=0; n<tmp_fpmf -> num[m]; n++)
          {
            o = tmp_fpmf -> list[m][n];
            for (p=0; p<tmp_fmol -> multi;p++)
            {
              q = tmp_fmol -> atoms_id[o][p].a;
              r = tmp_fmol -> atoms_id[o][p].b;
              s = get_active_atom (tmp_fmol -> id, q) -> list[r];
              field_selection (s, toviz.c, toviz.c, jd);
            }
          }
        }
      }
      break;
    case 5:
      tmp_frig = get_active_rigid (kd, jd);
      tmp_frig -> show = toviz.c;
      if (tmp_frig -> num > 0)
      {
        for (n=0; n<tmp_frig -> num; n++)
        {
          o = tmp_frig -> list[n];
          for (p=0; p<tmp_fmol -> multi;p++)
          {
            q = tmp_fmol -> atoms_id[o][p].a;
            r = tmp_fmol -> atoms_id[o][p].b;
            s = get_active_atom (tmp_fmol -> id, q) -> list[r];
            field_selection (s, toviz.c, toviz.c, jd);
          }
        }
      }
      break;
    case 6:
      tmp_ftet = get_active_tethered (kd, jd);
      tmp_ftet -> show = toviz.c;
      if (tmp_ftet -> num)
      {
        o = tmp_ftet -> num-1;
        p = tmp_fmol -> atoms_id[o][0].a;
        for (q=0; q<tmp_fmol -> multi;q++)
        {
          r = tmp_fmol -> atoms_id[o][q].b;
          s = get_active_atom (tmp_fmol -> id, p) -> list[r];
          field_selection (s, toviz.c, toviz.c, jd);
        }
      }
      break;
    case SEXTERN:

      break;
    default:
      if (id < MOLIMIT)
      {
       // View a type of bond / angles / dihedral / improper / inversion in a molecule
        tmp_fstr = get_active_struct (id-7, kd, jd);
        tmp_fat = get_active_atom (kd, tmp_fstr -> aid[0]);
        tmp_fbt = get_active_atom (kd, tmp_fstr -> aid[1]);
        if (id > 8) tmp_fct = get_active_atom (kd, tmp_fstr -> aid[2]);
        if (id > 10) tmp_fdt = get_active_atom (kd, tmp_fstr -> aid[3]);
        tmp_fstr -> def -> show = toviz.c;
        if (id < 9)
        {
          visualize_bonds (tmp_fstr -> def -> show, jd, tmp_fat, tmp_fbt);
        }
        else if (id < 11)
        {
          visualize_angles (tmp_fstr -> def -> show, jd, tmp_fat, tmp_fbt, tmp_fct);
        }
        else if (id < 13)
        {
          visualize_dihedrals (tmp_fstr -> def -> show, jd, tmp_fat, tmp_fbt, tmp_fct, tmp_fdt);
        }
        else if (id < 15)
        {
          visualize_imp_inv (tmp_fstr -> def -> show, id-7, jd, tmp_fat, tmp_fbt, tmp_fct, tmp_fdt);
        }
      }
      else
      {
        // View a non-bonded interaction
        tmp_fbody = get_active_body (jd, id - MOLIMIT);
        tmp_fbody -> show = toviz.c;
        visualize_body (tmp_fbody -> show, jd, tmp_fbody);
      }
      break;
  }
}

/*!
  \fn void check_to_visualize_properties_for_this_field_mol (int pid, int mol)

  \brief check if rendering is required for object in molecule

  \param pid the type of field object
  \param mol the target field molecule
*/
void check_to_visualize_properties_for_this_field_mol (int pid, int mol)
{
  int h, i, j;
  field_prop * propv;
  h = toviz.c;
  toviz.c = 1;
  tmp_fmol = get_active_field_molecule (mol);
  i = get_field_objects (pid, mol);
  for (j=0; j<i; j++)
  {
    if (show_field_object (pid, j, mol)) visualize_object (pid, j, mol);
    if (pid > 6 && pid < MOLIMIT)
    {
      propv = get_active_struct (pid-7, mol, j) -> other;
      while (propv)
      {
        if (propv -> show) visualize_single_struct (pid, j, mol, propv -> aid);
        propv = propv -> next;
      }
    }
  }
  toviz.c = h;
}

/*!
  \fn void check_to_visualize_properties (int pid)

  \brief check if it is requried to update rendering

  \param pid the page number in the assistant
*/
void check_to_visualize_properties (int pid)
{
  int i, j;
  i = (pid < MOLIMIT) ? tmp_field -> molecules : 1;
  for (j=0; j<i; j++)
  {
    check_to_visualize_properties_for_this_field_mol (pid, j);
  }
  init_default_shaders (tmp_view);
}

/*!
  \fn void update_mol_tree (int a, int b)

  \brief update the field molecule tree model

  \param a 0 = show molecule id, 1 = show atom id in fragment
  \param b the target field molecule id
*/
void update_mol_tree (int a, int b)
{
  int i, j;
  j = toviz.c;
  for (i=0; i<tmp_field -> molecules; i++)
  {
    if (get_active_field_molecule (i) -> show_id && ! a)
    {
      toviz.c = 0;
      visualize_object (-1, i, 0);
      check_to_visualize_properties (-1);
      toviz.c = j;
    }
    else if (get_active_field_molecule (i) -> show && a)
    {
      toviz.c = 0;
      visualize_object (0, i, 0);
      check_to_visualize_properties (0);
      toviz.c = j;
    }
  }
  if (a)
  {
    get_active_field_molecule (b) -> show_id = j;
  }
  else
  {
    get_active_field_molecule (b) -> show = j;
  }
  gtk_tree_store_clear (field_model[0]);
  init_default_shaders (tmp_view);
  fill_field_model (field_model[0], 0, -1);
}

/*!
  \fn G_MODULE_EXPORT void on_toggle_visualize_or_select_object (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)

  \brief on visualize force field object toggle callback

  \param cell_renderer the GtkCellRendererToggle sending the signal
  \param string_path the path in the tree store
  \param data the associated data pointer
*/
G_MODULE_EXPORT void on_toggle_visualize_or_select_object (GtkCellRendererToggle * cell_renderer, gchar * string_path, gpointer data)
{
  int i, j, k, l, m, n;
  int * ids = NULL;
  tint * dat = (tint * )data;
  GtkTreeIter iter;
  gboolean forall;

  if (gtk_cell_renderer_toggle_get_active(cell_renderer))
  {
    toviz.c = 0;
  }
  else
  {
    toviz.c = 1;
  }

  i = dat -> b;
  GtkTreePath * path = gtk_tree_path_new_from_string (string_path);
  gtk_tree_model_get_iter (GTK_TREE_MODEL(field_model[i]), & iter, path);
  forall = TRUE;
  if (i > 6 && i < MOLIMIT)
  {
    gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), & iter, 0, & k, -1);
    if (! k)
    {
      gchar * str;
      forall = FALSE;
      j = combo_get_active (combo_mol[i-1]);
      tmp_fmol = get_active_field_molecule (j);
      l =  struct_id(i);
      ids = allocint (l);
      if (dat -> c == 0)
      {
        field_object = i;
        num_field_objects = get_field_objects (i, tmp_fmol -> id);
      }
      for (m=1; m<l+1; m++)
      {
        gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), & iter, m, & str, -1);
        n = (int) string_to_double ((gpointer)str) - 1;
        ids[m-1] = n;
      }
      gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), & iter, field_v[i]+1, & m, -1);
      tmp_fstr = get_active_struct (i-7, j, m);
      if (ids[0] < 0)
      {
        tmp_fprop = tmp_fstr -> def;
      }
      else
      {
        if (tmp_fstr -> other)
        {
          tmp_fprop = get_active_prop_using_atoms (tmp_fstr -> other, l, ids);
          if (! tmp_fprop)
          {
            tmp_fprop = tmp_fstr -> other;
            while (tmp_fprop -> next) tmp_fprop = tmp_fprop -> next;
            tmp_fprop -> next = duplicate_field_prop (tmp_fstr -> def, i-7);
            tmp_fprop = tmp_fprop -> next;
            for (n=0; n<l; n++) tmp_fprop -> aid[n] = ids[n];
          }
        }
        else
        {
          tmp_fstr -> other = duplicate_field_prop (tmp_fstr -> def, i-7);
          for (n=0; n<l; n++) tmp_fstr -> other -> aid[n] = ids[n];
          tmp_fprop = tmp_fstr -> other;
        }
      }
      if (dat -> c) tmp_fprop -> use = toviz.c;
      if (! dat -> c) tmp_fprop -> show = toviz.c;
      adjust_field_prop (i-7, l, NULL, ids, tmp_fprop -> key);
      j = m-1;
    }
    else
    {
      j = k-1;
    }
  }
  else
  {
    gtk_tree_model_get (GTK_TREE_MODEL(field_model[i]), & iter, 0, & k, -1);
    j = k-1;
  }

  if (forall)
  {
    k = -1;
    if (i > 0 && i < MOLIMIT)
    {
      k = combo_get_active (combo_mol[i-1]);
      tmp_fmol = get_active_field_molecule (k);
    }
    if (i == 0 && dat -> c == 1)
    {
      update_mol_tree (1, j);
      visualize_object (-1, j, k);
      check_to_visualize_properties (i);
    }
    else if (dat -> c == 0)
    {
      if (i == 0) update_mol_tree (0, j);
      visualize_object (i, j, k);
      check_to_visualize_properties (i);
    }
    else
    {
      select_object (i, j, k);
    }
  }
  else if (dat -> c == 0)
  {
    k = combo_get_active (combo_mol[i-1]);
    visualize_single_struct (i, j+1, k, ids);
    check_to_visualize_properties (i);
  }

  if (ids) g_free (ids);
  if (forall || i == 4 || i == SEXTERN)
  {
    gtk_tree_store_clear (field_model[i]);
    fill_field_model (field_model[i], i, k);
  }
  else if (! forall && i > 0 && i != 4 && i != SEXTERN)
  {
    for (l=0; l<field_v[i]; l++)
    {
      if (is_special[i][l] - 2 == dat -> c) break;
    }
    gtk_tree_store_set (field_model[i], & iter, l, toviz.c, -1);
  }
}

/*!
  \fn gboolean show_field_object (int id, int jd, int kd)

  \brief is the field object visible ?

  \param id the type of object
  \param jd the object id, if any
  \param kd the field molecule id, if any
*/
gboolean show_field_object (int id, int jd, int kd)
{
  gboolean show = FALSE;
  switch (id)
  {
    case -1:
      show = get_active_field_molecule (jd) -> show;
      break;
    case 0:
      show = get_active_field_molecule (jd) -> show;
      break;
    case 1:
      show = (jd < get_active_field_molecule (kd) -> atoms) ? get_active_atom (kd, jd) -> show : FALSE;
      break;
    case 2:
      show = get_active_shell (kd, jd) -> show;
      break;
    case 3:
      show = get_active_constraint (kd, jd) -> show;
      break;
    case 4:
      show = get_active_pmf (kd, jd) -> show;
      break;
    case 5:
      show = get_active_rigid (kd, jd) -> show;
      break;
    case 6:
      show = get_active_tethered (kd, jd) -> show;
      break;
    default:
      if (id < MOLIMIT)
      {
        show = get_active_struct (id-7, kd, jd) -> def -> show;
      }
      else
      {
        show = get_active_body (jd, id - MOLIMIT) -> show;
      }
      break;
  }
  return show;
}

/*!
  \fn G_MODULE_EXPORT void visualize_or_select_all_elements (GtkTreeViewColumn * col, gpointer data)

  \brief select all element(s) in the column for visualization

  \param col the target GtkTreeViewColumn
  \param data the associated data pointer
*/
G_MODULE_EXPORT void visualize_or_select_all_elements (GtkTreeViewColumn * col, gpointer data)
{
  int i, j, k, l;
  tint * dat = (tint *)data;
  i = dat -> b;
  j = -1;
  if (i > 0 && i < MOLIMIT) j = combo_get_active (combo_mol[i-1]);
  num_field_objects = get_field_objects (i, j);
  if (i < 7)
  {
    switch (i)
    {
      case 0:
        toviz.c = (dat -> c) ? get_active_field_molecule (0) -> show_id : get_active_field_molecule (0) -> show;
        break;
      case 1:
        toviz.c = get_active_field_molecule (j) -> first_atom -> show;
        break;
      case 2:
        toviz.c = (dat -> c) ? get_active_field_molecule (j) -> first_shell -> use : get_active_field_molecule (j) -> first_shell -> show;
        break;
      case 3:
        toviz.c = (dat -> c) ? get_active_field_molecule (j) -> first_constraint -> use : get_active_field_molecule (j) -> first_constraint -> show;
        break;
      case 4:
        toviz.c = (dat -> c) ? get_active_field_molecule (j) -> first_pmf -> use : get_active_field_molecule (j) -> first_pmf -> show;
        break;
      case 5:
        toviz.c = (dat -> c) ? get_active_field_molecule (j) -> first_rigid -> use : get_active_field_molecule (j) -> first_rigid -> show;
        break;
      case 6:
        toviz.c = (dat -> c) ? get_active_field_molecule (j) -> first_tethered-> use : get_active_field_molecule (j) -> first_tethered -> show;
        break;
    }
    toviz.c = ! toviz.c;
  }
  else if (i > 6 && i < MOLIMIT)
  {
    if (num_field_objects)
    {
      tmp_fmol = get_active_field_molecule (j);
      tmp_fstr = tmp_fmol -> first_struct[i-7];
      toviz.c = (dat -> c == 0) ? ! tmp_fstr -> def -> show : ! tmp_fstr -> def -> use;
      while (tmp_fstr)
      {
        if (dat -> c) tmp_fstr -> def -> use = toviz.c;
        if (! dat -> c) tmp_fstr -> def -> show = toviz.c;
        if (tmp_fstr -> other)
        {
          tmp_fprop = tmp_fstr -> other;
          while (tmp_fprop)
          {
            if (dat -> c) tmp_fprop -> use = toviz.c;
            if (! dat -> c) tmp_fprop -> show = toviz.c;
            tmp_fprop = tmp_fprop -> next;
          }
        }
        tmp_fstr = tmp_fstr -> next;
      }
    }
  }
  else
  {
    tmp_fbody = tmp_field -> first_body[i - MOLIMIT];
    toviz.c = (dat -> c == 0) ? ! tmp_fbody -> show : ! tmp_fbody -> use;
    while (tmp_fbody)
    {
      if (dat -> c) tmp_fbody -> use = toviz.c;
      if (! dat -> c) tmp_fbody -> show = toviz.c;
      tmp_fbody = tmp_fbody -> next;
    }
  }

  if (num_field_objects)
  {
    if (i == 0 && dat -> c == 1)
    {
      l = num_field_objects;
      num_field_objects = get_field_objects (-1, j);
      for (k=0; k<l; k++)
      {
        field_object = k;
        update_mol_tree (1, k);
        visualize_object (-1, k, -1);
      }
      check_to_visualize_properties (i);
    }
    else if (dat -> c == 0)
    {
      for (k=0; k<num_field_objects; k++)
      {
        field_object = k;
        if (i == 0) update_mol_tree (0, k);
        visualize_object (i, k, j);
      }
      check_to_visualize_properties (i);
    }
    else
    {
      for (k=0; k<num_field_objects; k++)
      {
        select_object (i, k, j);
      }
    }
    gtk_tree_store_clear (field_model[i]);
    fill_field_model (field_model[i], i, j);
  }
}
