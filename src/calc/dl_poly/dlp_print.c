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
* @file dlp_print.c
* @short Functions to handle the output of the DL-POLY FIELD file \n
         Functions to handle the output of the DL-POLY CONTROL file \n
         Functions to handle the output of the DL-POLY CONFIG file \n
         Functions to fill the structural element(s) tree models in the assistant
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'dlp_print.c'
*
* Contains:
*

 - The functions to handle the output of the DL-POLY FIELD file
 - The functions to handle the output of the DL-POLY CONTROL file
 - The functions to handle the output of the DL-POLY CONFIG file
 - The functions to fill the structural element(s) tree models in the assistant

*
* List of functions:

  int get_num_struct_to_print (field_molecule * fmol, int sid);
  int get_pbc ();

  gboolean print_this_imp_inv (imp_inv * inv, int di, int a, int b, int c, int d);
  gboolean member_of_atom (field_atom* fat, int id);
  gboolean print_ana ();

  void print_field_prop (field_prop * pro, int st, field_molecule * mol);
  void print_field_struct (field_struct * stru, field_molecule * mol);
  void print_all_field_struct (field_molecule * mol, int str);
  void print_dlp_improper_inversion (int di, GtkTextBuffer * buf,  field_struct * dhii, int fi, GtkTreeStore * store, GtkTreeIter * iter);
  void print_dlp_dihedral (int dih, GtkTextBuffer * buf,  field_struct * dh, int fi, GtkTreeStore * store, GtkTreeIter * iter);
  void print_dlp_angle (int ai, GtkTextBuffer * buf,  field_struct * an, int fi, GtkTreeStore * store, GtkTreeIter * iter);
  void print_dlp_bond (int bi, GtkTextBuffer * buf,  field_struct * bd, int fi, GtkTreeStore * store, GtkTreeIter * iter);
  void print_dlp_rigid (GtkTextBuffer * buf, field_rigid * rig);
  void print_dlp_tet (GtkTextBuffer * buf, field_tethered * tet);
  void print_dlp_pmf (GtkTextBuffer * buf, field_pmf * pmf);
  void print_dlp_cons (GtkTextBuffer * buf, field_constraint * cons);
  void print_dlp_shell (GtkTextBuffer * buf, field_molecule * fmol, field_shell * shell);
  void print_dlp_atom (GtkTextBuffer * buf, int at, int numat);
  void print_dlp_molecule (GtkTextBuffer * buf, field_molecule * fmol);
  void print_dlp_body (GtkTextBuffer * buf, field_nth_body * body);
  void print_dlp_tersoff_cross (GtkTextBuffer * buf, field_nth_body * body_a, field_nth_body * body_b);
  void print_dlp_tersoff (GtkTextBuffer * buf, field_nth_body * body);
  void print_dlp_field (GtkTextBuffer * buf);
  void print_dlp_config (GtkTextBuffer * buf);
  void print_int (GtkTextBuffer * buf, int data);
  void print_control_int (GtkTextBuffer * buf, int data, gchar * info_a, gchar * info_b, gchar * key);
  void print_float (GtkTextBuffer * buf, double data);
  void print_control_float (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key);
  void print_sci (GtkTextBuffer * buf, double data);
  void print_control_sci (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key);
  void print_string (GtkTextBuffer * buf, gchar * string);
  void print_control_string (GtkTextBuffer * buf, gchar * string, gchar * info_a, gchar * info_b, gchar * key);
  void print_control_key (GtkTextBuffer * buf, gchar * info, gchar * key);
  void print_dlp_control (GtkTextBuffer * buf);

*/

#include "dlp_field.h"
#include "interface.h"

extern gboolean in_bond (int at, int bd[2]);
extern int get_num_vdw_max ();
extern gchar * get_body_element_name (field_nth_body * body, int aid, int nbd);

/*!
  \fn void print_field_prop (field_prop * pro, int st, field_molecule * mol)

  \brief print force field property

  \param pro the field property to print
  \param st the type of structural element
  \param mol the target field molecule
*/
void print_field_prop (field_prop * pro, int st, field_molecule * mol)
{
  int i, j, k, u, v;
  field_atom* fat;
  j = struct_id(st+7);
#ifdef DEBUG
  int w;
  g_debug ("Prop - natomes= %d", j);
#endif
  for (i=0; i<j; i++)
  {
#ifdef DEBUG
    g_debug ("   at[%d]= %d", i, pro -> aid[i]);
#endif
    if (mol != NULL)
    {
      for (k=0; k< mol -> multi; k++)
      {
        u = mol -> atoms_id[pro -> aid[i]][k].a;
        v = mol -> atoms_id[pro -> aid[i]][k].b;
        fat = get_active_atom (mol -> id, u);
        if (v > fat -> num)
        {
#ifdef DEBUG
          g_debug ("********************** BIG BUG BIG BUG BIG BUG **********************");
          g_debug ("     TO CHEK::   multi= %d::   at.a= %d, at.b= %d", k, u, v);
          g_debug ("********************** BIG BUG BIG BUG BIG BUG **********************");
#endif
        }
        else
        {
#ifdef DEBUG
          w = fat -> list[v];
          g_debug ("        multi= %d::   at.a= %d, at.b= %d, real_id= %d", k, u, v, w);
#endif
        }
      }
    }
  }

#ifdef DEBUG
  g_debug ("Prop - key= %d", pro -> key);
  j = fvalues[activef][st][pro -> key];
  for (i=0; i<j; i++)
  {
    g_debug ("   val[%d]= %f", i, pro -> val[i]);
  }
  g_debug ("Prop - show= %d", pro -> show);
  g_debug ("Prop - use= %d", pro -> use);
#endif
}

/*!
  \fn void print_field_struct (field_struct * stru, field_molecule * mol)

  \brief print force field structural element

  \param stru the target field structural element
  \param mol the target field molecule
*/
void print_field_struct (field_struct * stru, field_molecule * mol)
{
#ifdef DEBUG
  int i;
  g_debug (" ");
  g_debug ("Struct - st= %d", stru -> st);
  g_debug ("Struct - id= %d", stru -> id);
  g_debug ("Struct - num= %d", stru -> num);
  g_debug ("Struct - av= %f", stru -> av);
  g_debug ("Struct - natomes= %d", struct_id(stru -> st+7));
  for (i=0; i<struct_id(stru -> st+7); i++)
  {
    g_debug ("   at[%d]= %d", i, stru -> aid[i]);
  }
  g_debug ("Default property:: ");
#endif
  print_field_prop (stru -> def, stru -> st, NULL);
  if (stru -> other != NULL)
  {
#ifdef DEBUG
    g_debug ("Other property(ies):: ");
#endif
    field_prop * tmp_pr;
    tmp_pr = stru -> other;
    print_field_prop (tmp_pr, stru -> st, mol);
    while (tmp_pr -> next != NULL)
    {
      print_field_prop (tmp_pr -> next, stru -> st, mol);
      tmp_pr = tmp_pr -> next;
    }
  }
}

/*!
  \fn void print_all_field_struct (field_molecule * mol, int str)

  \brief print all field structural element(s)

  \param mol the target field molecule
  \param str the type of structural element
*/
void print_all_field_struct (field_molecule * mol, int str)
{
  int i;
  field_struct * tmp_s;
#ifdef DEBUG
  g_debug (" ");
  g_debug ("IN MOL:: %d", mol -> id);
  g_debug ("PRINTING STRUCT:: %d", str);
  g_debug ("Total Num of Struct %d:: NUM= %d", str, mol -> nstruct[str]);
#endif
  tmp_s = mol -> first_struct[str];
  for (i=0; i<mol -> nstruct[str]; i++)
  {
    print_field_struct (tmp_s, mol);
    if (tmp_s -> next != NULL) tmp_s = tmp_s -> next;
  }
#ifdef DEBUG
  g_debug ("END STRUCT :: %d", str);
#endif
}

typedef struct imp_inv imp_inv;
struct imp_inv
{
  int a;
  int b;
  int c;
  int d;
  imp_inv * next;
  imp_inv * prev;
};

/*!
  \fn gboolean print_this_imp_inv (imp_inv * inv, int di, int a, int b, int c, int d)

  \brief print this improper / inversion structure or not (already printed) ?

  \param inv the improper / inversion structure to check
  \param di 6 = improper, 7 = inversion
  \param a 1st atom id
  \param b 2nd atom id
  \param c 3rd atom id
  \param d 4th atom id
*/
gboolean print_this_imp_inv (imp_inv * inv, int di, int a, int b, int c, int d)
{
  if (! inv) return TRUE;
  while (inv)
  {
    if (di == 6)
    {
      if (inv -> a == a && inv -> d == d)
      {
        if (inv -> b == b && inv -> c == c) return FALSE;
        if (inv -> b == c && inv -> c == b) return FALSE;
      }
    }
    else
    {
      if (inv -> a == a && inv -> b == b) return FALSE;
      /*
      {
        if (inv -> b == b && inv -> c == d && inv -> d == c) return FALSE;
        if (inv -> b == c && inv -> c == b && inv -> d == d) return FALSE;
        if (inv -> b == c && inv -> c == d && inv -> d == b) return FALSE;
        if (inv -> b == d && inv -> c == b && inv -> d == c) return FALSE;
        if (inv -> b == d && inv -> c == c && inv -> d == b) return FALSE;
      }*/
    }
    inv = inv -> next;
  }
  return TRUE;
}

/*!
  \fn gboolean member_of_atom (field_atom* fat, int id)

  \brief is the id atom from the model in target field atom

  \param fat the target field atom
  \param id the atom id in the model
*/
gboolean member_of_atom (field_atom* fat, int id)
{
  int i;
  for (i=0; i<tmp_fat -> num; i++)
  {
    if (tmp_fat -> list[i] == id) return TRUE;
  }
  return FALSE;
}

/*!
  \fn void print_dlp_improper_inversion (int di, GtkTextBuffer * buf,  field_struct * dhii, int fi, GtkTreeStore * store, GtkTreeIter * iter)

  \brief print / fill tree store with force field improper(s)/inversion(s) information

  \param di 6 = improper(s), 7 = inversion(s)
  \param buf the GtkTextBuffer to print into, if input print
  \param dhii the field improper / inversion structural element(s) to print
  \param fi the target fragment id
  \param store the target GtkTreeStore to store, if assistant tab creation / refresh
  \param iter the target tree iter to store the data, if assistant tab creation / refresh
*/
void print_dlp_improper_inversion (int di, GtkTextBuffer * buf, field_struct * dhii, int fi, GtkTreeStore * store, GtkTreeIter * iter)
{
  int a, b, c, d, e, i, j, k, l, m, n, o, p, q, r, s, t, u, v;
  gboolean show;
  gchar * stra, * strb, * strc, * strd, * stre, * strf, * strg;
  float w;
  GtkTreeIter di_level;
  int * ids = allocint (4);
  imp_inv * first_imp_inv = NULL;
  imp_inv * this_ii = NULL;

  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    if (tmp_proj -> atoms[0][j].coord[2] == fi)
    {
      if ((tmp_proj -> atoms[0][j].numv > 2 && di == 6) || (tmp_proj -> atoms[0][j].numv == 3 && di == 7))
      {
        a = ids[0] = tmp_fat -> list_id[i];
        for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
        {
          l = tmp_proj -> atoms[0][j].vois[k];
          if (tmp_proj -> atoms[0][l].faid == tmp_fbt -> id)
          {
            b = ids[1] = tmp_proj -> atoms[0][l].fid;
            for (m=0; m<tmp_proj -> atoms[0][j].numv; m++)
            {
              if (m != k)
              {
                n = tmp_proj -> atoms[0][j].vois[m];
                if (tmp_proj -> atoms[0][n].faid == tmp_fct -> id)
                {
                  c = ids[2] = tmp_proj -> atoms[0][n].fid;
                  for (o=0; o<tmp_proj -> atoms[0][j].numv; o++)
                  {
                    if (o != k && o != m)
                    {
                      p = tmp_proj -> atoms[0][j].vois[o];
                      if (tmp_proj -> atoms[0][p].faid == tmp_fdt -> id)
                      {
                        d = ids[3] = tmp_proj -> atoms[0][p].fid;
                        if (print_this_imp_inv(first_imp_inv, di, a, b, c, d))
                        {
                          if (! first_imp_inv)
                          {
                            first_imp_inv = g_malloc0(sizeof*first_imp_inv);
                            this_ii = first_imp_inv;
                          }
                          else
                          {
                            this_ii -> next = g_malloc0(sizeof*this_ii);
                            this_ii -> next -> prev = this_ii;
                            this_ii = this_ii -> next;
                          }
                          this_ii -> a = a;
                          this_ii -> b = b;
                          this_ii -> c = c;
                          this_ii -> d = d;
                          tmp_fprop = get_active_prop_using_atoms (dhii -> other, 4, ids);
                          if (tmp_fprop == NULL)
                          {
                            tmp_fprop = dhii -> def;
                            if (buf == NULL) show = FALSE;
                          }
                          else if (buf == NULL)
                          {
                            show = tmp_fprop -> show;
                          }
                          if (buf != NULL && tmp_fprop -> use)
                          {
                            if (di == 6)
                            {
                              stra = g_strdup_printf ("%4s\t%d\t%d\t%d\t%d",fkeysw[activef][di+2][tmp_fprop -> key], b+1, a+1, c+1, d+1);
                            }
                            else
                            {
                              stra = g_strdup_printf ("%4s\t%d\t%d\t%d\t%d",fkeysw[activef][di+2][tmp_fprop -> key], a+1, b+1, c+1, d+1);
                            }
                            print_info (stra, NULL, buf);
                            g_free (stra);
                            for (e=0; e<fvalues[activef][di+1][tmp_fprop -> key]; e++)
                            {
                              stra = g_strdup_printf ("\t%15.10f", tmp_fprop -> val[e]);
                              print_info (stra, NULL, buf);
                              g_free (stra);
                              if (e == 2)
                              {
                                // Print 1-4 electrostatic interaction scale factor
                                stra = g_strdup_printf ("\t%15.10f", 0.0);
                                print_info (stra, NULL, buf);
                                g_free (stra);
                                // Print 1-4 van der Waals interaction scale factor
                                stra = g_strdup_printf ("\t%15.10f", 0.0);
                                print_info (stra, NULL, buf);
                                g_free (stra);
                              }
                            }
                            print_info ("\n", NULL, buf);
                          }
                          else if (buf == NULL)
                          {
                            stra = g_strdup_printf ("%d", a+1);
                            strb = g_strdup_printf ("%d", b+1);
                            strc = g_strdup_printf ("%d", c+1);
                            strd = g_strdup_printf ("%d", d+1);
                            w = 0.0;
                            for (e=0; e<tmp_fmol -> multi; e++)
                            {
                              q = tmp_fmol -> atoms_id[a][e].a;
                              r = tmp_fmol -> atoms_id[a][e].b;
                              s = get_active_atom (tmp_fmol -> id, q) -> list[r];
                              q = tmp_fmol -> atoms_id[b][e].a;
                              r = tmp_fmol -> atoms_id[b][e].b;
                              t = get_active_atom (tmp_fmol -> id, q) -> list[r];
                              q = tmp_fmol -> atoms_id[c][e].a;
                              r = tmp_fmol -> atoms_id[c][e].b;
                              u = get_active_atom (tmp_fmol -> id, q) -> list[r];
                              q = tmp_fmol -> atoms_id[d][e].a;
                              r = tmp_fmol -> atoms_id[d][e].b;
                              v = get_active_atom (tmp_fmol -> id, q) -> list[r];
                              if (di == 6)
                              {
                                w += dihedral_3d (& tmp_proj -> cell, 0,
                                                  & tmp_proj -> atoms[0][t],
                                                  & tmp_proj -> atoms[0][u],
                                                  & tmp_proj -> atoms[0][s],
                                                  & tmp_proj -> atoms[0][v]).angle;
                              }
                              else
                              {
                                w += inversion_3d (& tmp_proj -> cell, 0,
                                                   & tmp_proj -> atoms[0][s],
                                                   & tmp_proj -> atoms[0][t],
                                                   & tmp_proj -> atoms[0][u],
                                                   & tmp_proj -> atoms[0][v]).angle;
                              }
                            }

                            w /= tmp_fmol -> multi;
                            stre = g_strdup_printf ("%.3f", w);
                            strf = g_strdup_printf ("%s (%s)", fnames[activef][di+2][tmp_fprop -> key], fkeysw[activef][di+2][tmp_fprop -> key]);
                            strg = parameters_info (di+1, tmp_fprop -> key, fvars_dihedral[activef][tmp_fprop -> key], tmp_fprop -> val);
                            gtk_tree_store_append (store, & di_level, iter);
                            gtk_tree_store_set (store, & di_level, 0, 0,
                                                                   1, stra,
                                                                   2, strb,
                                                                   3, strc,
                                                                   4, strd,
                                                                   5, 0,
                                                                   6, stre,
                                                                   7, show,
                                                                   8, tmp_fprop -> use,
                                                                   9, strf,
                                                                  10, strg,
                                                                  11, dhii -> id, -1);
                            g_free (stra);
                            g_free (strb);
                            g_free (strc);
                            g_free (strd);
                            g_free (stre);
                            g_free (strf);
                            g_free (strg);
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
    }
  }
  if (first_imp_inv)
  {
    this_ii = first_imp_inv;
    while (this_ii -> next)
    {
      this_ii = this_ii -> next;
      g_free (this_ii -> prev);
    }
    g_free (this_ii);
  }
  g_free (ids);
}

/*!
  \fn void print_dlp_dihedral (int dih, GtkTextBuffer * buf,  field_struct * dh, int fi, GtkTreeStore * store, GtkTreeIter * iter)

  \brief print / fill tree store with force field dihedral(s) information

  \param dih 4 = dihderale(s), 5 = dihedral restraint(s)
  \param buf the GtkTextBuffer to print into, if input print
  \param dh the field dihedral / dihedral restraint structural element(s) to print
  \param fi the target fragment id
  \param store the target GtkTreeStore to store, if assistant tab creation / refresh
  \param iter the target tree iter to store the data, if assistant tab creation / refresh
*/
void print_dlp_dihedral (int dih, GtkTextBuffer * buf, field_struct * dh, int fi, GtkTreeStore * store, GtkTreeIter * iter)
{
  int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s;
  gboolean show;
  gchar * stra, * strb, * strc, * strd, * stre, * strf, * strg;
  float v;
  GtkTreeIter di_level;
  gboolean same_atom = FALSE;
  gboolean * already_done;
  if (tmp_fat -> id == tmp_fdt -> id && tmp_fbt -> id && tmp_fct -> id)
  {
    same_atom = TRUE;
    already_done = allocbool (tmp_fmol -> mol -> natoms);
  }
  int * ids = allocint (4);
  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    if (tmp_proj -> atoms[0][j].coord[2] == fi)
    {
      a = ids[0] = tmp_fat -> list_id[i];
      if (same_atom) already_done[a] = TRUE;
      for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
      {
        l = tmp_proj -> atoms[0][j].vois[k];
        if (tmp_proj -> atoms[0][l].faid == tmp_fbt -> id)
        {
          b = ids[1] = tmp_proj -> atoms[0][l].fid;
          for (m=0; m<tmp_proj -> atoms[0][l].numv; m++)
          {
            n = tmp_proj -> atoms[0][l].vois[m];
            if (n != j && tmp_proj -> atoms[0][n].faid == tmp_fct -> id)
            {
              c = ids[2] = tmp_proj -> atoms[0][n].fid;
              for (o=0; o<tmp_proj -> atoms[0][n].numv; o++)
              {
                p = tmp_proj -> atoms[0][n].vois[o];
                d = ids[3] = tmp_proj -> atoms[0][p].fid;
                if (p != j && p != l && tmp_proj -> atoms[0][p].faid == tmp_fdt -> id && (! same_atom || (same_atom && ! already_done[d])))
                {
                  tmp_fprop = get_active_prop_using_atoms (dh -> other, 4, ids);
                  if (tmp_fprop == NULL)
                  {
                    tmp_fprop = dh -> def;
                    if (buf == NULL) show = FALSE;
                  }
                  else if (buf == NULL)
                  {
                    show = tmp_fprop -> show;
                  }
                  if (buf != NULL && tmp_fprop -> use)
                  {
                    stra = g_strdup_printf ("%4s\t%d\t%d\t%d\t%d",fkeysw[activef][dih+2][tmp_fprop -> key], a+1, b+1, c+1, d+1);
                    print_info (stra, NULL, buf);
                    g_free (stra);
                    for (q=0; q<fvalues[activef][dih+1][tmp_fprop -> key]; q++)
                    {
                      stra = g_strdup_printf ("\t%15.10f", tmp_fprop -> val[q]);
                      print_info (stra, NULL, buf);
                      g_free (stra);
                      if (q == 2)
                      {
                        // Print 1-4 electrostatic interaction scale factor
                        stra = g_strdup_printf ("\t%15.10f", 0.0);
                        print_info (stra, NULL, buf);
                        g_free (stra);
                        // Print 1-4 van der Waals interaction scale factor
                        stra = g_strdup_printf ("\t%15.10f", 0.0);
                        print_info (stra, NULL, buf);
                        g_free (stra);
                      }
                    }
                    print_info ("\n", NULL, buf);
                  }
                  else if (buf == NULL)
                  {
                    stra = g_strdup_printf ("%d", a+1);
                    strb = g_strdup_printf ("%d", b+1);
                    strc = g_strdup_printf ("%d", c+1);
                    strd = g_strdup_printf ("%d", d+1);
                    v = 0.0;
                    for (q=0; q<tmp_fmol -> multi; q++)
                    {
                      r = tmp_fmol -> atoms_id[a][q].a;
                      s = tmp_fmol -> atoms_id[a][q].b;
                      e = get_active_atom (tmp_fmol -> id, r) -> list[s];
                      r = tmp_fmol -> atoms_id[b][q].a;
                      s = tmp_fmol -> atoms_id[b][q].b;
                      f = get_active_atom (tmp_fmol -> id, r) -> list[s];
                      r = tmp_fmol -> atoms_id[c][q].a;
                      s = tmp_fmol -> atoms_id[c][q].b;
                      g = get_active_atom (tmp_fmol -> id, r) -> list[s];
                      r = tmp_fmol -> atoms_id[d][q].a;
                      s = tmp_fmol -> atoms_id[d][q].b;
                      h = get_active_atom (tmp_fmol -> id, r) -> list[s];
                      v += dihedral_3d (& tmp_proj -> cell, 0,
                                        & tmp_proj -> atoms[0][e],
                                        & tmp_proj -> atoms[0][f],
                                        & tmp_proj -> atoms[0][g],
                                        & tmp_proj -> atoms[0][h]).angle;
                    }
                    v /= tmp_fmol -> multi;
                    stre = g_strdup_printf ("%.3f", v);
                    strf = g_strdup_printf ("%s (%s)", fnames[activef][dih+2][tmp_fprop -> key], fkeysw[activef][dih+2][tmp_fprop -> key]);
                    strg = parameters_info (dih+1, tmp_fprop -> key, fvars_dihedral[activef][tmp_fprop -> key], tmp_fprop -> val);
                    gtk_tree_store_append (store, & di_level, iter);
                    gtk_tree_store_set (store, & di_level, 0, 0,
                                                           1, stra,
                                                           2, strb,
                                                           3, strc,
                                                           4, strd,
                                                           5, 0,
                                                           6, stre,
                                                           7, show,
                                                           8, tmp_fprop -> use,
                                                           9, strf,
                                                          10, strg,
                                                          11, dh -> id, -1);
                    g_free (stra);
                    g_free (strb);
                    g_free (strc);
                    g_free (strd);
                    g_free (stre);
                    g_free (strf);
                    g_free (strg);
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  g_free (ids);
  if (same_atom) g_free (already_done);
}

/*!
  \fn void print_dlp_angle (int ai, GtkTextBuffer * buf,  field_struct * an, int fi, GtkTreeStore * store, GtkTreeIter * iter)

  \brief print / fill tree store with force field angle(s) information

  \param ai 2 = angle(s), 3 = angular restraint(s)
  \param buf the GtkTextBuffer to print into, if input print
  \param an the field angle / angle restraint structural element(s) to print
  \param fi the target fragment id
  \param store the target GtkTreeStore to store, if assistant tab creation / refresh
  \param iter the target tree iter to store the data, if assistant tab creation / refresh
*/
void print_dlp_angle (int ai, GtkTextBuffer * buf, field_struct * an, int fi, GtkTreeStore * store, GtkTreeIter * iter)
{
  int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, u;
  gboolean show;
  gchar * stra, * strb, * strc, * strd, * stre, * strf;
  float v;
  GtkTreeIter an_level;
  int * ids = allocint (3);
  gboolean same_atom = FALSE;
  gboolean * already_done;
  if (tmp_fat -> id == tmp_fct -> id)
  {
    same_atom = TRUE;
    already_done = allocbool (tmp_fmol -> mol -> natoms);
  }

  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    if (tmp_proj -> atoms[0][j].coord[2] == fi)
    {
      k = ids[0] = tmp_proj -> atoms[0][j].fid;
      if (same_atom) already_done[k] = TRUE;
      for (l=0; l<tmp_proj -> atoms[0][j].numv; l++)
      {
        m = tmp_proj -> atoms[0][j].vois[l];
        if (tmp_proj -> atoms[0][m].faid == tmp_fbt -> id)
        {
          n = ids[1] = tmp_proj -> atoms[0][m].fid;
          for (o=0; o<tmp_proj -> atoms[0][m].numv; o++)
          {
            p = tmp_proj -> atoms[0][m].vois[o];
            q = ids[2] = tmp_proj -> atoms[0][p].fid;
            if (p != j && tmp_proj -> atoms[0][p].faid == tmp_fct -> id && (! same_atom || (same_atom && ! already_done[q])))
            {
              tmp_fprop = get_active_prop_using_atoms (an -> other, 3, ids);
              if (tmp_fprop == NULL)
              {
                tmp_fprop = an -> def;
                if (buf == NULL) show = FALSE;
              }
              else if (buf == NULL)
              {
                show = tmp_fprop -> show;
              }
              if (buf != NULL && tmp_fprop -> use)
              {
                stra = g_strdup_printf ("%4s\t%d\t%d\t%d",fkeysw[activef][ai+2][tmp_fprop -> key], k+1, n+1, q+1);
                print_info (stra, NULL, buf);
                g_free (stra);
                for (u=0; u<fvalues[activef][ai+1][tmp_fprop -> key]; u++)
                {
                  stra = g_strdup_printf ("\t%15.10f", tmp_fprop -> val[u]);
                  print_info (stra, NULL, buf);
                  g_free (stra);
                }
                print_info ("\n", NULL, buf);
              }
              else if (buf == NULL)
              {
                stra = g_strdup_printf ("%d", k+1);
                strb = g_strdup_printf ("%d", n+1);
                strc = g_strdup_printf ("%d", q+1);
                v = 0.0;
                for (u=0; u<tmp_fmol -> multi; u++)
                {
                  a = tmp_fmol -> atoms_id[k][u].a;
                  b = tmp_fmol -> atoms_id[k][u].b;
                  c = get_active_atom (tmp_fmol -> id, a) -> list[b];
                  d = tmp_fmol -> atoms_id[n][u].a;
                  e = tmp_fmol -> atoms_id[n][u].b;
                  f = get_active_atom (tmp_fmol -> id, d) -> list[e];
                  e = tmp_fmol -> atoms_id[q][u].a;
                  g = tmp_fmol -> atoms_id[q][u].b;
                  h = get_active_atom (tmp_fmol -> id, e) -> list[g];
                  v += angle_3d (& tmp_proj -> cell, 0,
                                 & tmp_proj -> atoms[0][c],
                                 & tmp_proj -> atoms[0][f],
                                 & tmp_proj -> atoms[0][h]).angle;
                }
                v /= tmp_fmol -> multi;
                strd = g_strdup_printf ("%.3f", v);
                stre = g_strdup_printf ("%s (%s)", fnames[activef][ai+2][tmp_fprop -> key], fkeysw[activef][ai+2][tmp_fprop -> key]);
                strf = parameters_info (ai+1, tmp_fprop -> key, fvars_angle[activef][tmp_fprop -> key], tmp_fprop -> val);
                gtk_tree_store_append (store, & an_level, iter);
                gtk_tree_store_set (store, & an_level, 0, 0,
                                                       1, stra,
                                                       2, strb,
                                                       3, strc,
                                                       4, 0,
                                                       5, strd,
                                                       6, show,
                                                       7, tmp_fprop -> use,
                                                       8, stre,
                                                       9, strf,
                                                      10, an -> id, -1);
                g_free (stra);
                g_free (strb);
                g_free (strc);
                g_free (strd);
                g_free (stre);
                g_free (strf);
              }
            }
          }
        }
      }
    }
  }
  g_free (ids);
  if (same_atom) g_free (already_done);
}

/*!
  \fn void print_dlp_bond (int bi, GtkTextBuffer * buf,  field_struct * bd, int fi, GtkTreeStore * store, GtkTreeIter * iter)

  \brief print / fill tree store with force field bond(s) information

  \param bi 0 = bond(s), 1 = bond restraint(s)
  \param buf the GtkTextBuffer to print into, if input print
  \param bd the field bond / bond restraint structural element(s) to print
  \param fi the target fragment id
  \param store the target GtkTreeStore to store, if assistant tab creation / refresh
  \param iter the target tree iter to store the data, if assistant tab creation / refresh
*/
void print_dlp_bond (int bi, GtkTextBuffer * buf,  field_struct * bd, int fi, GtkTreeStore * store, GtkTreeIter * iter)
{
  int i, j, k, l, m, n, o, p, q, r, s, t, u;
  gboolean show;
  gchar * stra, * strb, * strc, * strd, * stre;
  float v;
  int * ids = allocint (2);
  GtkTreeIter bd_level;
  gboolean same_atom = FALSE;
  gboolean * already_done;
  if (tmp_fat -> id == tmp_fbt -> id)
  {
    same_atom = TRUE;
    already_done = allocbool (tmp_fmol -> mol -> natoms);
  }
  for (i=0; i<tmp_fat -> num; i++)
  {
    j = tmp_fat -> list[i];
    if (tmp_proj -> atoms[0][j].coord[2] == fi)
    {
      k = ids[0] = tmp_fat -> list_id[i];
      if (same_atom) already_done[k] = TRUE;
      for (l=0; l<tmp_proj -> atoms[0][j].numv; l++)
      {
        m = tmp_proj -> atoms[0][j].vois[l];
        n = ids[1] = tmp_proj -> atoms[0][m].fid;
        if (tmp_proj -> atoms[0][m].faid == tmp_fbt -> id && (! same_atom || (same_atom && ! already_done[n])))
        {
          tmp_fprop = get_active_prop_using_atoms (bd -> other, 2, ids);
          if (tmp_fprop == NULL)
          {
            tmp_fprop = bd -> def;
            if (buf == NULL) show = FALSE;
          }
          else if (buf == NULL)
          {
            show = tmp_fprop -> show;
          }
          if (buf != NULL && tmp_fprop -> use)
          {
            stra = g_strdup_printf ("%4s\t%d\t%d",fkeysw[activef][bi+2][tmp_fprop -> key], k+1, n+1);
            print_info (stra, NULL, buf);
            g_free (stra);
            for (o=0; o<fvalues[activef][bi+1][tmp_fprop -> key]; o++)
            {
              stra = g_strdup_printf ("\t%15.10f", tmp_fprop -> val[o]);
              print_info (stra, NULL, buf);
              g_free (stra);
            }
            print_info ("\n", NULL, buf);
          }
          else if (buf == NULL)
          {
            stra = g_strdup_printf ("%d", k+1);
            strb = g_strdup_printf ("%d", n+1);
            v = 0.0;
            for (o=0; o<tmp_fmol -> multi; o++)
            {
              p = tmp_fmol -> atoms_id[k][o].a;
              q = tmp_fmol -> atoms_id[k][o].b;
              r = get_active_atom (tmp_fmol -> id, p) -> list[q];
              s = tmp_fmol -> atoms_id[n][o].a;
              t = tmp_fmol -> atoms_id[n][o].b;
              u = get_active_atom (tmp_fmol -> id, s) -> list[t];
              v += distance_3d (& tmp_proj -> cell, 0, & tmp_proj -> atoms[0][r], & tmp_proj -> atoms[0][u]).length;
            }
            v /= tmp_fmol -> multi;
            strc = g_strdup_printf ("%.3f", v);
            strd = g_strdup_printf ("%s (%s)", fnames[activef][bi+2][tmp_fprop -> key], fkeysw[activef][bi+2][tmp_fprop -> key]);
            stre = parameters_info (bi+1, tmp_fprop -> key, fvars_bond[activef][tmp_fprop -> key], tmp_fprop -> val);
            gtk_tree_store_append (store, & bd_level, iter);
            gtk_tree_store_set (store, & bd_level, 0, 0,
                                                   1, stra,
                                                   2, strb,
                                                   3, 0,
                                                   4, strc,
                                                   5, show,
                                                   6, tmp_fprop -> use,
                                                   7, strd,
                                                   8, stre,
                                                   9, bd -> id, -1);
            g_free (stra);
            g_free (strb);
            g_free (strc);
            g_free (strd);
            g_free (stre);
          }
        }
      }
    }
  }
  g_free (ids);
  if (same_atom) g_free (already_done);
}

/*!
  \fn void print_dlp_rigid (GtkTextBuffer * buf, field_rigid * rig)

  \brief print force field rigid

  \param buf the GtkTextBuffer to print into
  \param rig the field rigid to print
*/
void print_dlp_rigid (GtkTextBuffer * buf, field_rigid * rig)
{
  gchar * str;
  int h, i, j, k, l, m, n;
  str = g_strdup_printf ("%d\t", rig -> num);
  j = 1;
  n = rig -> num;
  if (rig -> num > 15)
  {
    k = rig -> num - 15;
    l = k / 16;
    j += l + 1;
    n = k - l*16;
  }
  h = 0;
  for (i=0; i<j; i++)
  {
    k = (i) ? 1 : 0;
    l = (j == 1 || (j > 1 && i == j-1)) ? n : 15+k;
    for (m=0; m<l; m++)
    {
      str = g_strdup_printf ("%s\t%d", str, rig -> list[h]+1);
      h ++;
    }
    str = g_strdup_printf ("%s\n", str);
  }
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_dlp_tet (GtkTextBuffer * buf, field_tethered * tet)

  \brief print force field tethered potential

  \param buf the GtkTextBuffer to print into
  \param tet the field tethered potential to print
*/
void print_dlp_tet (GtkTextBuffer * buf, field_tethered * tet)
{
  gchar * str;
  str = g_strdup_printf ("%4s\t\%d", fkeysw[activef][1][tet -> key], tet -> num);
  print_info (str, NULL, buf);
  g_free (str);
  int i;
  for (i=0; i<fvalues[activef][0][tmp_ftet -> key]; i++)
  {
    str = g_strdup_printf ("\t%15.10f", tmp_ftet -> val[i]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  print_info ("\n", NULL, buf);
}

/*!
  \fn void print_dlp_pmf (GtkTextBuffer * buf, field_pmf * pmf)

  \brief print force field mean force potential

  \param buf the GtkTextBuffer to print into
  \param pmf the field PMF to print
*/
void print_dlp_pmf (GtkTextBuffer * buf, field_pmf * pmf)
{
  gchar * str;
  int i, j;
  print_info ("PMF", "bold", buf);
  str = g_strdup_printf ("\t%f\n", pmf -> length);
  print_info (str, NULL, buf);
  g_free (str);
  for (i=0; i<2; i++)
  {
    str = g_strdup_printf ("PMF UNIT %d\n", pmf -> num[i]);
    print_info (str, NULL, buf);
    g_free (str);
    for (j=0; j < pmf -> num[i]; j++)
    {
      str = g_strdup_printf ("%d\t%f\n", pmf -> list[i][j]+1, pmf -> weight[i][j]);
      print_info (str, NULL, buf);
      g_free (str);
    }
  }
}

/*!
  \fn void print_dlp_cons (GtkTextBuffer * buf, field_constraint * cons)

  \brief print force field constraint

  \param buf the GtkTextBuffer to print into
  \param cons the field constraint to print
*/
void print_dlp_cons (GtkTextBuffer * buf, field_constraint * cons)
{
  gchar * str;
  str = g_strdup_printf ("%d\t\%d\t%f\n", cons -> ia[0], cons -> ia[1], cons -> length);
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_dlp_shell (GtkTextBuffer * buf, field_molecule * fmol, field_shell * shell)

  \brief print force field core shell

  \param buf the GtkTextBuffer to print into
  \param fmol the target field molecule
  \param shell the field shell to print
*/
void print_dlp_shell (GtkTextBuffer * buf, field_molecule * fmol, field_shell * shell)
{
  gchar * str;
  str = g_strdup_printf ("%d\t\%d\t%f\t%f\n", shell -> ia[0], shell -> ia[1], shell -> k2, shell -> k4);
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn void print_dlp_atom (GtkTextBuffer * buf, int at, int numat)

  \brief print force field atom

  \param buf the GtkTextBuffer to print into
  \param at the list id in the target field atom
  \param numat the atom id in the fragment / molecule
*/
void print_dlp_atom (GtkTextBuffer * buf, int at, int numat)
{
  gchar * str;
  if (tmp_fat -> frozen_id[at])
  {
    str = g_strdup_printf ("%8s %15.10f %15.10f %d %d\n", tmp_fat -> name, tmp_fat -> mass, tmp_fat -> charge, numat, 1);
  }
  else
  {
    str = g_strdup_printf ("%8s %15.10f %15.10f %d\n", tmp_fat -> name, tmp_fat -> mass, tmp_fat -> charge, numat);
  }
  print_info (str, NULL, buf);
  g_free (str);
}

/*!
  \fn int get_num_struct_to_print (field_molecule * fmol, int sid)

  \brief find the number of structural element(s) to print

  \param fmol the target field molecule
  \param sid the type of structural element
*/
int get_num_struct_to_print (field_molecule * fmol, int sid)
{
  int i = 0;
  tmp_fstr = fmol -> first_struct[sid];
  while (tmp_fstr)
  {
    if (tmp_fstr -> def -> use)
    {
      i += tmp_fstr -> num;
    }
    else if (tmp_fstr -> other)
    {
      tmp_fprop = tmp_fstr -> other;
      while (tmp_fprop)
      {
        if (tmp_fprop -> use) i ++;
        tmp_fprop = tmp_fprop -> next;
      }
    }
    tmp_fstr = tmp_fstr -> next;
  }
  return i;
}

/*!
  \fn void print_dlp_molecule (GtkTextBuffer * buf, field_molecule * fmol)

  \brief print force field molecule

  \param buf the GtkTextBuffer to print into
  \param fmol the field molecule to print
*/
void print_dlp_molecule (GtkTextBuffer * buf, field_molecule * fmol)
{
  gchar * str;
  str = g_strdup_printf ("%s", fmol -> name);
  print_info (str, "bold_orange", buf);
  g_free (str);
  print_info ("\nNUMMOLS\t", "bold", buf);
  str = g_strdup_printf ("%d", fmol -> multi);
  print_info (str, "bold_green", buf);
  g_free (str);
  int i, j, k, l, m, n, o, p;

  j = 0;
  tmp_fat = fmol -> first_atom;
  for (i=0; i<fmol -> atoms; i++)
  {
    j += tmp_fat -> num;
    if (tmp_fat -> next != NULL) tmp_fat = tmp_fat -> next;
  }
  j /= fmol -> multi;
  if (j != fmol -> mol -> natoms) g_debug ("PRINT:: Error the number of atom(s) is wrong ?!");
  print_info ("\nATOMS\t", "bold", buf);
  str = g_strdup_printf ("%d\n", fmol -> mol -> natoms);
  print_info (str, "bold_blue", buf);
  g_free (str);
  for (i=0; i < fmol -> mol -> natoms ; i+=(m-i))
  {
    j = fmol -> atoms_id[i][0].a;
    tmp_fat = get_active_atom (fmol -> id, j);
    k = fmol -> atoms_id[i][0].b;
    l = tmp_fat -> frozen_id[k];
    for (m=i+1; m<fmol -> mol -> natoms; m++)
    {
      n = fmol -> atoms_id[m][0].a;
      o = fmol -> atoms_id[m][0].b;
      tmp_fbt = get_active_atom (fmol -> id, n);
      p = tmp_fbt -> frozen_id[o];
      if (j != n || l != p) break;
    }
    print_dlp_atom (buf, k, m-i);
  }
  // Shells
  int ncs = 0;
  if (tmp_field -> afp[10])
  {
    tmp_fshell = fmol -> first_shell;
    while (tmp_fshell)
    {
      if (tmp_fshell -> use)
      {
        if (tmp_fshell -> ia[0] && tmp_fshell -> ia[1]) ncs ++;
      }
      tmp_fshell = tmp_fshell -> next;
    }
  }
  if (ncs)
  {
    print_info ("SHELLS\t", "bold", buf);
    str = g_strdup_printf ("%d\n", ncs);
    print_info (str, "bold", buf);
    g_free (str);
    tmp_fshell = fmol -> first_shell;
    while (tmp_fshell)
    {
      if (tmp_fshell -> use && tmp_fshell -> ia[0] && tmp_fshell -> ia[1])
      {
        print_dlp_shell (buf, fmol, tmp_fshell);
      }
      tmp_fshell = tmp_fshell -> next;
    }
  }

  // Constraints
  if (tmp_field -> afp[11])
  {
    j = 0;
    tmp_fcons = fmol -> first_constraint;
    while (tmp_fcons)
    {
      if (tmp_fcons -> use) j ++;
      tmp_fcons = tmp_fcons -> next;
    }
    if (j > 0)
    {
      print_info ("CONSTRAINTS\t", "bold", buf);
      str = g_strdup_printf ("%d\n", j);
      print_info (str, "bold", buf);
      g_free (str);
      tmp_fcons = fmol -> first_constraint;
      while (tmp_fcons)
      {
        if (tmp_fcons -> use) print_dlp_cons (buf, tmp_fcons);
        tmp_fcons = tmp_fcons -> next;
      }
    }
  }

  // PMFs
  if (tmp_field -> afp[12])
  {
    tmp_fpmf = fmol -> first_pmf;
    while (tmp_fpmf)
    {
      if (tmp_fpmf -> use)
      {
        print_dlp_pmf (buf, tmp_fpmf);
        break;
      }
      tmp_fpmf = tmp_fpmf -> next;
    }
  }

  // Rigid
  if (tmp_field -> afp[13])
  {
    j = 0;
    k = 0;
    tmp_frig = fmol -> first_rigid;
    while (tmp_frig)
    {
      if (tmp_frig -> use) j ++;
      tmp_frig = tmp_frig -> next;
    }
    if (j > 0)
    {
      print_info ("RIGID\t", "bold", buf);
      str = g_strdup_printf ("%d\n", j);
      print_info (str, "bold", buf);
      g_free (str);
      tmp_frig = fmol -> first_rigid;
      while (tmp_frig)
      {
        if (tmp_frig -> use)
        {
          print_dlp_rigid (buf, tmp_frig);
        }
        tmp_frig = tmp_frig -> next;
      }
    }
  }

  // Tethering
  if (tmp_field -> afp[14])
  {
    j = 0;
    tmp_ftet = fmol -> first_tethered;
    while (tmp_ftet)
    {
      if (tmp_ftet -> use && tmp_ftet -> num) j ++;
      tmp_ftet = tmp_ftet -> next;
    }
    if (j > 0)
    {
      print_info ("TETH\t", "bold", buf);
      str = g_strdup_printf ("%d\n", j);
      print_info (str, "bold", buf);
      g_free (str);
      tmp_ftet = fmol -> first_tethered;
      while (tmp_ftet)
      {
        if (tmp_ftet -> use) print_dlp_tet (buf, tmp_ftet);
        tmp_ftet = tmp_ftet -> next;
      }
    }
  }
  gchar * str_title[8] = {"BONDS ", "BONDS ", "ANGLES ", "ANGLES ", "DIHEDRALS ", "DIHEDRALS ", "DIHEDRALS ", "INVERSIONS "};
  gboolean doprint;
  for (i=0; i<8; i++)
  {
    if (tmp_field -> afp[i+15])
    {
      j = get_num_struct_to_print (fmol, i);
      if ((i == 0 || i == 2 || i == 4) && tmp_field -> afp[i+16])
      {
        // To add the number of constraints
        j += get_num_struct_to_print (fmol, i+1);
      }
      if ((i == 4 && tmp_field -> afp[i+17]) || (i == 5 && ! tmp_field -> afp[i+14] && tmp_field -> afp[i+16]))
      {
        // To add the number of impropers
        k = (i == 4) ? 2 : 1;
        j += get_num_struct_to_print (fmol, i+k);
      }
      doprint = (i == 0 || i == 2 || i == 4 || i == 7) ? TRUE : FALSE;
      if ((i == 1 || i == 3 || i == 5) && ! tmp_field -> afp[i+14])
      {
        doprint = TRUE;
      }
      else if (i == 5 && ! tmp_field -> afp[i+14])
      {
        doprint = TRUE;
      }
      else if (i == 6 && ! tmp_field -> afp[i+13] && ! tmp_field -> afp[i+14])
      {
        doprint = TRUE;
      }

      if (j > 0)
      {
        if (doprint)
        {
          print_info (str_title[i], "bold", buf);
          str = g_strdup_printf ("%d\n", j);
          print_info (str, "bold_blue", buf);
          g_free (str);
        }
        tmp_fstr = fmol -> first_struct[i];
        while (tmp_fstr)
        {
          tmp_fat = get_active_atom (fmol -> id, tmp_fstr -> aid[0]);
          tmp_fbt = get_active_atom (fmol -> id, tmp_fstr -> aid[1]);
          if (i > 1) tmp_fct = get_active_atom (fmol -> id, tmp_fstr -> aid[2]);
          if (i > 3) tmp_fdt = get_active_atom (fmol -> id, tmp_fstr -> aid[3]);
          if (i == 0 || i == 1) print_dlp_bond (i, buf, tmp_fstr, fmol -> fragments[0], NULL, NULL);
          if (i == 2 || i == 3) print_dlp_angle (i, buf, tmp_fstr, fmol -> fragments[0], NULL, NULL);
          if (i == 4 || i == 5) print_dlp_dihedral (i, buf, tmp_fstr, fmol -> fragments[0], NULL, NULL);
          if (i == 6 || i == 7) print_dlp_improper_inversion (i, buf, tmp_fstr, fmol -> fragments[0], NULL, NULL);
          tmp_fstr = tmp_fstr -> next;
        }
      }
    }
  }

  print_info ("FINISH\n", "bold_orange", buf);
}

/*!
  \fn void print_dlp_body (GtkTextBuffer * buf, field_nth_body * body)

  \brief print force field non bonded potential

  \param buf the GtkTextBuffer to print into
  \param body the non bonded potential to print
*/
void print_dlp_body (GtkTextBuffer * buf, field_nth_body * body)
{
  gchar * str;
  int i, j;
  j = body_at (body -> bd);
  if (! body -> bd)
  {
    for (i=0; i<j; i++) print_info (g_strdup_printf ("%8s\t", get_body_element_name (body, i, 0)), NULL, buf);
  }
  else
  {
    for (i=0; i<j; i++) print_info (g_strdup_printf ("%8s\t", get_active_atom(body -> ma[i][0], body -> a[i][0]) -> name), NULL, buf);
  }
  str = g_strdup_printf ("%4s",fkeysw[activef][10+ body -> bd][body -> key]);
  print_info (str, NULL, buf);
  g_free (str);
  for (i=0; i<fvalues[activef][9+ body -> bd][body -> key]; i++)
  {
    str = g_strdup_printf ("\t%15.10f", body -> val[i]);
    print_info (str, NULL, buf);
    g_free (str);
  }
  print_info ("\n", NULL, buf);
}

/*!
  \fn void print_dlp_tersoff_cross (GtkTextBuffer * buf, field_nth_body * body_a, field_nth_body * body_b)

  \brief print Tersoff potential cross term

  \param buf the GtkTextBuffer to print into
  \param body_a 1st non bonded potential
  \param body_b 2nd non bonded potential
*/
void print_dlp_tersoff_cross (GtkTextBuffer * buf, field_nth_body * body_a, field_nth_body * body_b)
{
  gchar * str;
  int j;
  print_info (g_strdup_printf ("%8s\t", get_active_atom(body_a -> ma[0][0], body_a -> a[0][0]) -> name), NULL, buf);
  print_info (g_strdup_printf ("%8s\t", get_active_atom(body_b -> ma[0][0], body_b -> a[0][0]) -> name), NULL, buf);
  for (j=0; j<3; j++)
  {

    str = g_strdup_printf ("%15.10f", tmp_field -> cross[body_a -> id][body_b -> id][j]);
    print_info (str, NULL, buf);
    g_free (str);
    if (j<2) print_info ("\t", NULL, buf);
  }
  print_info ("\n", NULL, buf);
}

/*!
  \fn void print_dlp_tersoff (GtkTextBuffer * buf, field_nth_body * body)

  \brief print force field Tersoff potential

  \param buf the GtkTextBuffer to print into
  \param body the non bonded (Tersoff potential) to print
*/
void print_dlp_tersoff (GtkTextBuffer * buf, field_nth_body * body)
{
  gchar * str;
  int i, j, k;
  int num[2]={2, 3};
  int nc[2][3]={{5, 6, 3}, {5, 6, 5}};
  j = body_at (body -> bd);
  k = 0;
  for (i=0; i<num[body -> key]; i++)
  {
    if (i==0)
    {
      print_info (g_strdup_printf ("%8s\t", get_active_atom(body -> ma[0][0], body -> a[0][0]) -> name), NULL, buf);
      str = g_strdup_printf ("%4s\t",fkeysw[activef][10+body -> bd][body -> key]);
      print_info (str, NULL, buf);
      g_free (str);
    }
    else
    {
      print_info ("        \t    \t", NULL, buf);
    }
    for (j=0; j<nc[body -> key][i]; j++)
    {
      if (j > 0) print_info ("\t", NULL, buf);
      str = g_strdup_printf ("%15.10f", body -> val[j+k]);
      print_info (str, NULL, buf);
      g_free (str);
    }
    print_info ("\n", NULL, buf);
    k += nc[body -> key][i];
  }
  if (! body -> key)
  {
    field_nth_body * tmp_fbo;
    tmp_fbo = tmp_field -> first_body[2];
    while (tmp_fbo)
    {
      print_dlp_tersoff_cross (buf, body, tmp_fbo);
      tmp_fbo = tmp_fbo -> next;
    }
  }
}

/*!
  \fn void print_dlp_field (GtkTextBuffer * buf)

  \brief print DL-POLY classical force field

  \param buf the GtkTextBuffer to print into
*/
void print_dlp_field (GtkTextBuffer * buf)
{
  int i, j;
  gchar * str;

  GtkTextIter bStart;
  GtkTextIter bEnd;

  gtk_text_buffer_get_start_iter (buf, & bStart);
  gtk_text_buffer_get_end_iter (buf, & bEnd);
  gtk_text_buffer_delete (buf, & bStart, & bEnd);

  str = g_strdup_printf ("# This file was created using %s\n", PACKAGE);
  print_info (str, NULL, buf);
  g_free (str);
  str = g_strdup_printf ("# %s contains:\n", prepare_for_title(tmp_proj -> name));
  print_info (str, NULL, buf);
  g_free (str);
  i = 0;
  for (j=0; j<tmp_proj -> modelfc -> mol_by_step[0]; j++)
  {
    i += tmp_proj -> modelfc -> mols[0][j].multiplicity;
  }
  str = g_strdup_printf ("#  - %d atoms\n"
                         "#  - %d isolated molecular fragments\n"
                         "#  - %d distinct molecules\n",
                         tmp_proj -> natomes, i, tmp_proj -> modelfc -> mol_by_step[0]);
  print_info (str, NULL, buf);
  g_free (str);

  print_info ("# Energy unit:\n", NULL, buf);
  print_info ("UNITS ", "bold", buf);
  str = g_strdup_printf ("%s\n", fkeysw[activef][0][tmp_field -> energy_unit]);
  print_info (str, "bold_green", buf);
  g_free (str);
  print_info ("# Number of field molecules:\n", NULL, buf);
  print_info ("MOLECULES ", "bold", buf);
  str = g_strdup_printf ("%d\n", tmp_field -> molecules);
  print_info (str, "bold_red", buf);
  g_free (str);
  tmp_fmol = tmp_field -> first_molecule;
  for (i=0; i<tmp_field -> molecules; i++)
  {
    str = g_strdup_printf ("# Begin molecule %d\n", i+1);
    print_info (str, NULL, buf);
    g_free (str);
    print_dlp_molecule (buf, tmp_fmol);
    str = g_strdup_printf ("# End molecule %d\n", i+1);
    print_info (str, NULL, buf);
    g_free (str);
    if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
  }

  // Non bonded
  gchar * nd_title[5] = {"VDW", "METAL", "TERSOFF", "TBP", "FBP"};
  gchar * com_ndb[5] = {"Van der Walls pair", "metal", "Tersoff", "three-body", "four-body"};
  for (i=0; i<5; i++)
  {
    if (tmp_field -> afp[i+23])
    {
      j=0;
      tmp_fbody = tmp_field -> first_body[i];
      while (tmp_fbody)
      {
        if (tmp_fbody -> use) j++;
        tmp_fbody = tmp_fbody -> next;
      }
      if (j > 0)
      {
        str = g_strdup_printf ("# Non-bonded: %s potential(s)\n", com_ndb[i]);
        print_info (str, NULL, buf);
        g_free (str);
        print_info (nd_title[i], "bold", buf);
        str = g_strdup_printf (" %d\n", j);
        print_info (str, "bold_red", buf);
        tmp_fbody = tmp_field -> first_body[i];
        while (tmp_fbody)
        {
          if (tmp_fbody -> use)
          {
            if (i == 2)
            {
              print_dlp_tersoff (buf, tmp_fbody);
            }
            else
            {
              print_dlp_body (buf, tmp_fbody);
            }
          }
          tmp_fbody = tmp_fbody -> next;
        }
      }
    }
  }

  // External fields
  if (tmp_field -> afp[28])
  {
    i = 0;
    tmp_fext = tmp_field -> first_external;
    while (tmp_fext)
    {
      if (tmp_fext -> use) i ++;
      tmp_fext = tmp_fext -> next;
    }
    if (i == 1)
    {
      print_info ("EXTERN", "bold", buf);
      tmp_fext = tmp_field -> first_external;
      while (tmp_fext)
      {
        if (tmp_fext -> use)
        {
          str = g_strdup_printf ("\n%4s",fkeysw[activef][15][tmp_fext -> key]);
          print_info (str, NULL, buf);
          g_free (str);
          for (j=0; j<fvalues[activef][SEXTERN-6][tmp_fext -> key]; j++)
          {
            print_info (g_strdup_printf ("\t%15.10f", tmp_fext -> val[j]), NULL, buf);
          }
          print_info ("\n", NULL, buf);
          break;
        }
        tmp_fext = tmp_fext -> next;
      }
    }
  }
  print_info ("CLOSE", "bold", buf);
}

/*!
  \fn int get_pbc ()

  \brief get the PBC DL-POLY lattice type
*/
int get_pbc ()
{
  box_info * box = & tmp_proj -> cell.box[0];
  if (box -> param[1][0] == box -> param[1][1] && box -> param[1][0] == box -> param[1][2] && box -> param[1][0] == 90.0)
  {
    if (box -> param[0][0] == box -> param[0][1] && box -> param[0][0] == box -> param[0][2])
    {
      return 1;
    }
    else
    {
      return 2;
    }
  }
  else if (box -> vect[0][1] == 0.0 && box -> vect[0][2] == 0.0 && box -> vect[1][0] == 0.0
        && box -> vect[1][2] == 0.0 && box -> vect[2][0] == 0.0 && box -> vect[2][1] == 0.0)
  {
    if (box -> vect[0][0] == box -> vect[1][1] && box -> vect[0][0] == box -> vect[2][2])
    {
      return 1;
    }
    else
    {
      return 2;
    }
  }
  else
  {
    return 3;
  }
}

/*!
  \fn void print_dlp_config (GtkTextBuffer * buf)

  \brief print DL-POLY CONFIG file

  \param buf the GtkTextBuffer to print into
*/
void print_dlp_config (GtkTextBuffer * buf)
{
  int h, i, j, k, l, m, n;
  int pbc;
  gchar * str;

  GtkTextIter bStart;
  GtkTextIter bEnd;

  gtk_text_buffer_get_start_iter (buf, & bStart);
  gtk_text_buffer_get_end_iter (buf, & bEnd);
  gtk_text_buffer_delete (buf, & bStart, & bEnd);

  str = g_strdup_printf ("# DL-POLY CONFIG file created by %s, %s - %d atoms\n",
                         PACKAGE,
                         prepare_for_title(tmp_proj -> name),
                         tmp_proj -> natomes);
  print_info (str, "bold", buf);
  g_free (str);
  if (tmp_proj -> cell.pbc)
  {
    pbc = get_pbc ();
  }
  else
  {
    pbc = 0;
  }
  str = g_strdup_printf ("%d", 0);
  print_info (str, "bold_red", buf);
  g_free (str);
  str = g_strdup_printf ("\t%d", pbc);
  print_info (str, "bold_green", buf);
  g_free (str);
  str = g_strdup_printf ("\t%d\n", tmp_proj -> natomes);
  print_info (str, "bold_blue", buf);
  g_free (str);
  if (pbc > 0)
  {
    for (i=0; i<3; i++)
    {
      str = g_strdup_printf ("%f\t%f\t%f\n",
                             tmp_proj -> cell.box[0].vect[i][0],
                             tmp_proj -> cell.box[0].vect[i][1],
                             tmp_proj -> cell.box[0].vect[i][2]);
      print_info (str, NULL, buf);
      g_free (str);

    }
  }
  tmp_fmol = tmp_field -> first_molecule;
  h = 0;
  for (i=0; i<tmp_field -> molecules; i++)
  {
    for (j=0; j<tmp_fmol -> multi; j++)
    {
      for (k=0; k<tmp_fmol -> mol -> natoms; k++)
      {
        l = tmp_fmol -> atoms_id[k][j].a;
        m = tmp_fmol -> atoms_id[k][j].b;
        tmp_fat = get_active_atom (tmp_fmol -> id, l);
        str = g_strdup_printf ("%8s", tmp_fat -> name);
        print_info (str, "bold", buf);
        g_free (str);
        if (tmp_field -> sys_opts[2])
        {
          print_info ("\n", NULL, buf);
        }
        else
        {
          str = g_strdup_printf ("     %d\n", h+1);
          print_info (str, "bold_red", buf);
          g_free (str);
          h ++;
        }
        n = tmp_fat -> list[m];
        str = g_strdup_printf ("%f\t%f\t%f\n", tmp_proj -> atoms[0][n].x, tmp_proj -> atoms[0][n].y, tmp_proj -> atoms[0][n].z);
        print_info (str, NULL, buf);
        g_free (str);
      }
    }
    if (tmp_fmol -> next != NULL) tmp_fmol = tmp_fmol -> next;
  }
}


gchar * ens_keyw[4] = {"nve", "nvt", "npt", "nst"};
gchar * thermo_keyw[10] = {"evans", "lang", "ander", "ber", "hoover", "gst", "dpd", "mtk", "ttm", "inhomo"};
gchar * pseudo_thermo[3] = {"langevin", "gauss", "direct"};
gchar * area_keyw[5]={"area", "tens", "tens", "orth", "orth"};
gchar * md_keyw[4]={"temp               ", "steps              ", "integrat           ", "pres               "};
gchar * md_text[4]={"# Target temperature in K\n", "# Number of MD steps\n", "# Integration time step in ps\n", "# Target presssure in k atms\n"};
gchar * min_key[3]={"force", "energy", "distance"};
//gchar * md_legend[3]={"# Target temperature", "# Number of MD steps", "# MD time step d(t)"};

/*!
  \fn gboolean print_ana ()

  \brief determine if the analysis information section is required
*/
gboolean print_ana ()
{
  if ((int)tmp_field -> ana_opts[0] || (int)tmp_field -> ana_opts[4] || (int)tmp_field -> ana_opts[8] || (int)tmp_field -> ana_opts[11] || (int)tmp_field -> ana_opts[14])
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

extern gchar * eval_m[10];
extern gchar * eval_vdw[6];
extern gchar * io_rw_m[4];
extern gchar * io_pres[2];

gchar * elec_key[10]={"coul               ",
                      "distan             ",
                      "ewald precision    ",
                      "ewald              ",
                      "reaction           ",
                      "reaction damp      ",
                      "reaction precision ",
                      "shift              ",
                      "shift damp         ",
                      "shift precision    "};
gchar * vdw_key[6]={"lor ", "fend", "hoge",
                    "halg", "tang", "func"};
gchar * sys_info[8]={"\n# Ignore particle indices from CONFIG file and set indices by order of reading",
                     "\n# Ignore strict checks when reading CONFIG file, warning messages and assume safe simulation parameters",
                     "\n# Skip detailed topology reporting when reading FIELD file",
                     "\n# Ignore center of mass momentum removal during the simulation",
                     "\n# Tolerance for the relaxed shell model\n",
                     "\n# Subcelling threshold density of particle per link cell\n",
                     "\n# Create an expanded version of the current model\n",
                     "\n# Restart job from previous run: "};
gchar * sys_key[8]={NULL, NULL, NULL, NULL, "rlxtol             ", "subcell            ", "nfold              ", "restart            "};
gchar * sys_string[8]={"ind", "str", "top", "vom", NULL, NULL, NULL, NULL};
gchar * rest_inf[3]={"\n# Continue current simulation - require REVOLD file",
                     "\n# Start new simulation from older run without temperature reset",
                     "\n# Start new simulation from older run with temperature reset"};
gchar * rest_key[3]={NULL, "noscale", "scale"};

gchar * time_inf[2]={"\n\n# Set job time to ", "\n\n# Set job closure time to "};
gchar * time_key[2]={"job time           ", "close time         "};
gchar * io_inf[2]={"\n# I/O read interface, with:\n", "\n# I/O write interface, with:\n"};
gchar * io_key[2]={"\nio read             ", "\nio writ             "};
gchar * io_meth[4]={"mpiio", "direct", "master", "netcdf"};
gchar * io_pec[2]={"off", "on"};
gchar * io_typ[2]={"sorted", "unsorted"};

/*!
  \fn void print_int (GtkTextBuffer * buf, int data)

  \brief print integer value

  \param buf the GtkTextBuffer to print into
  \param data the integer value to print
*/
void print_int (GtkTextBuffer * buf, int data)
{
  gchar * str = g_strdup_printf (" %d", data);
  print_info (str, "bold_blue", buf);
  g_free (str);
}

/*!
  \fn void print_control_int (GtkTextBuffer * buf, int data, gchar * info_a, gchar * info_b, gchar * key)

  \brief print CONTROL file print integer value

  \param buf the GtkTextBuffer to print into
  \param data the integer value to print
  \param info_a 1st string to print
  \param info_b 2nd string to print, if any
  \param key DL-POLY key
*/
void print_control_int (GtkTextBuffer * buf, int data, gchar * info_a, gchar * info_b, gchar * key)
{
  gchar * str = g_strdup_printf ("%d", data);
  print_info (info_a, NULL, buf);
  print_info (str, NULL, buf);
  g_free (str);
  if (info_b != NULL) print_info (info_b, NULL, buf);
  print_info ("\n", NULL, buf);
  print_info (key, "bold", buf);
  print_int (buf, data);
}

/*!
  \fn void print_float (GtkTextBuffer * buf, double data)

  \brief print float value

  \param buf the GtkTextBuffer to print into
  \param data the float value to print
*/
void print_float (GtkTextBuffer * buf, double data)
{
  gchar * str = g_strdup_printf (" %f", data);
  print_info (str, "bold_red", buf);
  g_free (str);
}

/*!
  \fn void print_control_float (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key)

  \brief print CONTROL file print float value

  \param buf the GtkTextBuffer to print into
  \param data the float value to print
  \param info_a 1st string to print
  \param info_b 2nd string to print, if any
  \param key DL-POLY key
*/
void print_control_float (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key)
{
  gchar * str = g_strdup_printf ("%f", data);
  print_info (info_a, NULL, buf);
  print_info (str, NULL, buf);
  g_free (str);
  if (info_b != NULL) print_info (info_b, NULL, buf);
  print_info ("\n", NULL, buf);
  print_info (key, "bold", buf);
  print_float (buf, data);
}

/*!
  \fn void print_sci (GtkTextBuffer * buf, double data)

  \brief print float in scientific format

  \param buf the GtkTextBuffer to print into
  \param data the float value to print
*/
void print_sci (GtkTextBuffer * buf, double data)
{
  gchar * str = g_strdup_printf (" %e", data);
  print_info (str, "bold_orange", buf);
  g_free (str);
}

/*!
  \fn void print_control_sci (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key)

  \brief print CONTROL file print float value in scientific format

  \param buf the GtkTextBuffer to print into
  \param data the float value to print
  \param info_a 1st string to print
  \param info_b 2nd string to print, if any
  \param key DL-POLY key
*/
void print_control_sci (GtkTextBuffer * buf, double data, gchar * info_a, gchar * info_b, gchar * key)
{
  gchar * str = g_strdup_printf ("%e", data);
  print_info (info_a, NULL, buf);
  print_info (str, NULL, buf);
  if (info_b != NULL) print_info (info_b, NULL, buf);
  print_info ("\n", NULL, buf);
  print_info (key, "bold", buf);
  print_info (str, "bold_orange", buf);
  g_free (str);
}

/*!
  \fn void print_string (GtkTextBuffer * buf, gchar * string)

  \brief print string

  \param buf the GtkTextBuffer to print into
  \param string the string to print
*/
void print_string (GtkTextBuffer * buf, gchar * string)
{
  print_info (" ", NULL, buf);
  print_info (string, "bold_green", buf);
}

/*!
  \fn void print_control_string (GtkTextBuffer * buf, gchar * string, gchar * info_a, gchar * info_b, gchar * key)

  \brief print CONTROL file print string

  \param buf the GtkTextBuffer to print into
  \param string the string to print
  \param info_a 1st string to print, if any
  \param info_b 2nd string to print, if any
  \param key DL-POLY key
*/
void print_control_string (GtkTextBuffer * buf, gchar * string, gchar * info_a, gchar * info_b, gchar * key)
{
  if (info_a != NULL) print_info (info_a, NULL, buf);
  if (info_b != NULL) print_info (info_b, NULL, buf);
  if (info_a != NULL) print_info ("\n", NULL, buf);
  print_info (key, "bold", buf);
  if (string) print_string (buf, string);
}

/*!
  \fn void print_control_key (GtkTextBuffer * buf, gchar * info, gchar * key)

  \brief print CONTROL file print key

  \param buf the GtkTextBuffer to print into
  \param info the information string, if any
  \param key DL-POLY key to print
*/
void print_control_key (GtkTextBuffer * buf, gchar * info, gchar * key)
{
  if (info != NULL) print_info (info, NULL, buf);
  print_info (key, "bold", buf);
}


/*!
  \fn void print_dlp_control (GtkTextBuffer * buf)

  \brief print DL-POLY CONTROL file

  \param buf the GtkTextBuffer to print into
*/
void print_dlp_control (GtkTextBuffer * buf)
{
  int i, j, k;
  gchar * str;
  gchar * str_a, * str_b, * str_c;

  GtkTextIter bStart;
  GtkTextIter bEnd;

  gtk_text_buffer_get_start_iter (buf, & bStart);
  gtk_text_buffer_get_end_iter (buf, & bEnd);
  gtk_text_buffer_delete (buf, & bStart, & bEnd);

  str = g_strdup_printf ("# DL-POLY CONTROL file created by %s, %s - %d atoms\n\n",
                         PACKAGE,
                         prepare_for_title(tmp_proj -> name),
                         tmp_proj -> natomes);
  print_info (str, "bold", buf);
  g_free (str);


  if (tmp_field -> sys_opts[0] != 1.0)
  {
    print_control_float (buf, tmp_field -> sys_opts[0], "# Relative dielectric constant = ", NULL, "eps                 ");
  }
  if (tmp_field -> sys_opts[1] != 0.0)
  {
    print_control_float (buf, tmp_field -> sys_opts[1], "\n# Allowing local variation of system density of : ", " \%", "densvar            ");
  }
  for (i=2; i<10; i++)
  {
    j = (i < 7) ? i : (i == 7) ? 8 : (i == 8) ? 10 : 14;
    if (tmp_field -> sys_opts[j] == 1.0)
    {
      if (i == 9)
      {
        k = (int)tmp_field -> sys_opts[15];
        print_control_string (buf, rest_key[k], sys_info[i-2], rest_inf[k], sys_key[i-2]);
      }
      else if (i < 6)
      {
        print_control_string (buf, sys_string[i-2], sys_info[i-2], NULL, "no                 ");
      }
      else
      {
        print_control_key (buf, sys_info[i-2], sys_key[i-2]);
      }
      if (i == 6 || i == 7)
      {
        print_float (buf, tmp_field -> sys_opts[j+1]);
      }
      else if (i == 8)
      {
        for (k=1; k<4; k++) print_int (buf, (int)tmp_field -> sys_opts[j+k]);
      }
      print_info ("\n", NULL, buf);
    }
  }

  if (tmp_field -> vdw_opts[0] == 1.0)
  {
    print_info ("\n# Non bonded short range interactions - type vdW", NULL, buf);
    print_control_float (buf, tmp_field -> vdw_opts[1], "\n# van Der Waals short range cutoff = ", " Ang.", "rvdw               ");
    if (tmp_field -> vdw_opts[2] == 1.0)
    {
      print_control_string (buf, "direct", "\n# Enforce direct calculation of vdW interactions",
                          "\n# Do not work with system using tabulated potentials", "vdw                ");
    }
    if (tmp_field -> vdw_opts[3] == 1.0)
    {
      print_control_string (buf,  "shift", "\n# Apply force-shifting for vdW interactions", NULL, "vdw                ");
    }
    if (tmp_field -> vdw_opts[4] == 1.0)
    {
      print_control_string (buf, vdw_key[(int)tmp_field -> vdw_opts[5]], "\n# Apply mixing rule of type: ", eval_vdw[(int)tmp_field -> vdw_opts[5]], "vdw mix            ");
    }
  }
  else
  {
    print_control_string (buf, "vdw", "\n# No van der Waals interactions (short range)", NULL, "no                 ");
  }
  print_info ("\n\n", NULL, buf);

  if (tmp_field -> elec_opts[0] == 1.0)
  {
    print_info ("\n# Non bonded long range interactions", NULL, buf);
    print_control_float (buf, tmp_field -> elec_opts[1], "\n# Electrostatics long range cutoff = ", " Ang.", "cut                ");
    if (tmp_field -> elec_opts[2] == 1.0)
    {
      print_control_float (buf, tmp_field -> elec_opts[3], "\n# Use optional padding to the cutoff = ", " Ang.", "pad                ");
    }
    if (tmp_field -> elec_opts[4] == 1.0)
    {
      print_control_key (buf, "\n# Use extended coulombic exclusion\n", "exclu");
    }
    print_info ("\n# Electrostatics calculated using ", NULL, buf);
    print_info (eval_m[(int)tmp_field -> elec_opts[5]], NULL, buf);
    print_info ("\n", NULL, buf);
    print_info (elec_key[(int)tmp_field -> elec_opts[5]], "bold", buf);
    if (tmp_field -> elec_opts[5] == 2.0 || tmp_field -> elec_opts[5] == 6.0 || tmp_field -> elec_opts[5] == 9.0)
    {
      print_sci (buf, tmp_field -> elec_opts[6]);
    }
    else if (tmp_field -> elec_opts[5] == 3.0)
    {
      print_float (buf,  tmp_field -> elec_opts[6]);
      for (k=7; k<10; k++) print_int (buf, (int)tmp_field -> elec_opts[k]);
    }
    else if (tmp_field -> elec_opts[5] == 5.0 || tmp_field -> elec_opts[5] == 8.0)
    {
      print_float (buf, tmp_field -> elec_opts[6]);
    }

    if (tmp_field -> elec_opts[5] == 2.0 || tmp_field -> elec_opts[5] == 3.0)
    {
      print_control_int (buf, (int)tmp_field -> elec_opts[10], "\n# Evaluate k space contribution to the Ewald sum every: ", " MD step(s)", "ewald evalu        ");
    }
  }
  else
  {
    print_control_string (buf, "elec", "# No electrostatics interactions (long range)", NULL, "no                 ");
  }
  print_info ("\n", NULL, buf);

  if (tmp_field -> met_opts[0] == 1.0 || tmp_field -> met_opts[1] == 1.0)
  {
    print_info ("\n# Metallic interactions", NULL, buf);
  }
  if (tmp_field -> met_opts[0] == 1.0)
  {
    print_control_string (buf, "direct", "\n# Enforce direct calculation of metal interactions", "\n# This does not work with metal alloy systems using the *EAM* potentials", "metal              ");
  }
  if (tmp_field -> met_opts[1] == 1.0)
  {
    print_control_string (buf, "sqrtrho", "\n# Switch the TABEAM default embedding functions, F, from F(ρ) to F(√ρ)", NULL, "metal              ");
  }
  if (tmp_field -> met_opts[0] == 1.0 || tmp_field -> met_opts[1] == 1.0) print_info ("\n", NULL, buf);

  print_control_string (buf, ens_keyw[tmp_field -> ensemble], "\n# Thermostat information", NULL, "ensemble           ");
  if (tmp_field -> ensemble)
  {
    switch (tmp_field -> ensemble)
    {
      case 1:
        i = (tmp_field -> thermostat > 6) ? tmp_field -> thermostat + 1 : tmp_field -> thermostat;
        break;
      default:
        i = (! tmp_field -> thermostat) ? 1 : (tmp_field -> thermostat == 3) ? 7 : tmp_field -> thermostat + 2;
        break;
    }
    print_string (buf, thermo_keyw[i]);
    if (tmp_field -> ensemble > 1 || tmp_field -> thermostat)
    {
      if (tmp_field -> ensemble == 1 && tmp_field -> thermostat == 6)
      {
        str = g_strdup_printf ("s%1d", (int)tmp_field -> thermo_opts[0]+1);
        print_string (buf, str);
        g_free (str);
        print_float (buf, tmp_field -> thermo_opts[1]);
      }
      else
      {
        if (tmp_field -> ensemble == 3 && tmp_field -> thermo_opts[3] > 0.0) print_string (buf, "Q");
        print_float (buf, tmp_field -> thermo_opts[0]);
        if (tmp_field -> ensemble != 1 || (tmp_field -> thermostat == 2 || tmp_field -> thermostat == 5 || tmp_field -> thermostat > 6))
        {
          print_float (buf, tmp_field -> thermo_opts[1]);
          if (tmp_field -> ensemble == 1 && tmp_field -> thermostat > 6) print_float (buf, tmp_field -> thermo_opts[2]);
        }
      }
      if (tmp_field -> ensemble == 3 && tmp_field -> thermo_opts[3] > 0.0)
      {
        i = (int)tmp_field -> thermo_opts[3] - 1;
        print_string (buf,  area_keyw[i]);
        if (tmp_field -> thermo_opts[3] == 2.0) print_float (buf, tmp_field -> thermo_opts[4]);
      }
      if (tmp_field -> thermo_opts[5] == 1.0) print_string (buf, "semi");
    }
  }

  print_info ("\n\n", NULL, buf);
  if (tmp_field -> thermo_opts[6] == 1.0)
  {
    print_info ("# Attach a pseudo thermal bath with:\n", NULL, buf);
    if (tmp_field -> thermo_opts[7] > 0.0)
    {
      str = g_strdup_printf ("# - thermostat of type: %s\n", pseudo_thermo[(int)tmp_field -> thermo_opts[7] - 1]);
    }
    else
    {
      str = g_strdup_printf ("# - thermostats of type Langevin and Direct applied successively\n");
    }
    print_info (str, NULL, buf);
    g_free (str);
    str = g_strdup_printf ("# - thickness of thermostat layer to MD cell boundaries: %f Ang.\n", tmp_field -> thermo_opts[8]);
    print_info (str, NULL, buf);
    g_free (str);
    if (tmp_field -> thermo_opts[9] > 0.0)
    {
      str = g_strdup_printf ("# - Target temperature: %f K\n", tmp_field -> thermo_opts[9]);
      print_info (str, NULL, buf);
      g_free (str);
    }
    else
    {
      print_info ("# - Target temperature: system target temperature\n", NULL, buf);
    }
    print_info ("pseudo              ", "bold", buf);
    if (tmp_field -> thermo_opts[7] > 0.0)
    {
      print_info (pseudo_thermo[(int)tmp_field -> thermo_opts[7] - 1], "bold_green", buf);
    }
    print_float (buf, tmp_field -> thermo_opts[8]);
    if (tmp_field -> thermo_opts[9] > 0.0) print_float (buf, tmp_field -> thermo_opts[9]);
    print_info ("\n\n", NULL, buf);
  }

  // MD information
  print_info ("# Molecular dynamics information\n", NULL, buf);
  for (i=0; i<2+(int)tmp_field -> md_opts[1]; i++)
  {
    print_control_key (buf, md_text[i],  md_keyw[i]);
    switch (i)
    {
      case 0:
        print_float (buf, tmp_field -> md_opts[0]);
        if (tmp_field -> ensemble > 1)
        {
          print_info ("\n", NULL, buf);
          print_info (md_keyw[3], "bold", buf);
          print_float (buf, tmp_field -> md_opts[5]);
        }
        break;
      case 1:
        print_int (buf, (int)tmp_field -> md_opts[2]);
        break;
      case 2:
        print_info ("leapfrog", "bold_green", buf);
        break;
    }
    print_info ("\n", NULL, buf);
  }

  if (tmp_field -> md_opts[3] == 1.0)
  {
    print_control_float (buf, tmp_field -> md_opts[4], "# Variable time step, initial time step = ", " ps", "variable timestep  ");
    print_control_float (buf, tmp_field -> md_opts[6], "\n# Maximum time step allowed = ", " ps", "mxstep             ");
    print_control_float (buf, tmp_field -> md_opts[7], "\n# Maximum move allowed = ", " Ang.", "maxdis             ");
    print_control_float (buf, tmp_field -> md_opts[8], "\n# Minimum move allowed = ", " Ang.", "mindis             ");
  }
  else
  {
    print_control_float (buf, tmp_field -> md_opts[4], "# MD time step = ", " fs", "timestep           ");
  }

  print_control_int (buf, (int)tmp_field -> md_opts[9], "\n# Shake / Rattle iterations limit: ", " cycle(s)", "mxshak             ");
  print_control_sci (buf, tmp_field -> md_opts[10], "\n# Shake / Rattle tolerance: ", NULL, "shake               ");

  if (tmp_field -> md_opts[1] == 1.0)
  {
    print_control_int (buf, (int)tmp_field -> md_opts[11], "\n# FIQA iterations limit: ", " cycle(s)", "mxquat             ");
    print_control_sci (buf, tmp_field -> md_opts[12], "\n# Quaternion tolerance: ", NULL, "quater              ");
  }

  if (tmp_field -> md_opts[13] == 1.0)
  {
      print_info ("\n\n# Initiate impact on particle\n#  - with particle index: ", NULL, buf);
      str = g_strdup_printf ("%d", (int)tmp_field -> md_opts[14]);
      print_info (str, NULL, buf);
      print_info ("\n#  - at MD step: ", NULL, buf);
      str = g_strdup_printf ("%d", (int)tmp_field -> md_opts[15]);
      print_info (str, NULL, buf);
      g_free (str);
      print_info ("\n#  - with energy (k eV): ", NULL, buf);
      str = g_strdup_printf ("%f", tmp_field -> md_opts[16]);
      print_info (str, NULL, buf);
      g_free (str);
      print_info ("\n#  - direction (x, y, z): ", NULL, buf);
      str = g_strdup_printf ("%f %f %f", tmp_field -> md_opts[17], tmp_field -> md_opts[18], tmp_field -> md_opts[19]);
      print_info (str, NULL, buf);
      g_free (str);
      print_info ("\n", NULL, buf);
      print_info ("impact             ", "bold", buf);
      for (k=14; k<16; k++) print_int (buf, (int)tmp_field -> md_opts[k]);
      for (k=16; k<20; k++) print_float (buf, tmp_field -> md_opts[k]);
  }


  if (tmp_field -> equi_opts[0] == 1.0)
  {
    // Equilibration information
    print_info ("\n\n# Equilibration information", NULL, buf);
    print_control_int (buf, (int)tmp_field -> equi_opts[1], "\n# Equilibrate during: ", " MD step(s)", "equil              ");
    if (tmp_field -> equi_opts[2] == 1.0)
    {
      print_control_int (buf, (int)tmp_field -> equi_opts[3], "\n# During equilibration: rescale system temperature every: ", " MD step(s)", "scale              ");
    }
    if (tmp_field -> equi_opts[4] == 1.0)
    {
      print_control_float (buf, tmp_field -> equi_opts[5], "\n# During equilibration: cap forces, with fmax= ", " Kb T Ang-1", "cap                ");
    }
    if (tmp_field -> equi_opts[6] == 1.0)
    {
      print_control_int (buf, (int)tmp_field -> equi_opts[7], "\n# During equilibration: resample the instantaneous momenta distribution every: ", " MD step(s)", "regaus             ");
    }
    if (tmp_field -> equi_opts[8] == 1.0)
    {
      str = g_strdup_printf ("\n# Every %d step(s) during equilibration: minimize %s with target %s= %f\n",
                             (int)tmp_field -> equi_opts[11], min_key[(int)tmp_field -> equi_opts[9]], min_key[(int)tmp_field -> equi_opts[9]], tmp_field -> equi_opts[10]);
      print_control_key (buf, str, "minim              ");
      g_free (str);
      print_string (buf, min_key[(int)tmp_field -> equi_opts[9]]);
      print_int (buf, (int)tmp_field -> equi_opts[11]);
      print_float (buf, tmp_field -> equi_opts[10]);
      print_info ("\n", NULL, buf);
    }
    if (tmp_field -> equi_opts[12] == 1.0)
    {
      str = g_strdup_printf ("# At the start of the equilibration: minimize %s with target %s= %f\n",
                             min_key[(int)tmp_field -> equi_opts[13]], min_key[(int)tmp_field -> equi_opts[13]], tmp_field -> equi_opts[14]);
      print_control_key (buf, str, "optim              ");
      g_free (str);
      print_string (buf, min_key[(int)tmp_field -> equi_opts[13]]);
      print_float (buf, tmp_field -> equi_opts[14]);
      print_info ("\n", NULL, buf);
    }
    if (tmp_field -> equi_opts[15] == 1.0)
    {
      print_control_key (buf, "# During equilibration: perform a zero temperature MD minimization\n", "zero");
      print_info ("\n", NULL, buf);
    }
    if (tmp_field -> equi_opts[16] == 1.0)
    {
      print_control_key (buf, "# Include equilibration data in overall statistics\n", "collect");
      print_info ("\n", NULL, buf);
    }
  }

  if (print_ana())
  {
    print_info ("\n# Analysis information", NULL, buf);
    if (tmp_field -> ana_opts[0] == 1.0)
    {
      print_control_string (buf, "all", "\n# Calculate and collect all intra-molecular PDFs", NULL, "ana                ");
      for (k=1; k<3; k++) print_int (buf, (int)tmp_field -> ana_opts[k]);
      print_float (buf, tmp_field -> ana_opts[3]);
    }
    if (tmp_field -> ana_opts[4] == 1.0)
    {
      print_control_string (buf, "bon", "\n# Calculate and collect bonds PDFs", NULL, "ana                ");
      for (k=5; k<6; k++) print_int (buf, (int)tmp_field -> ana_opts[k]);
      print_float (buf, tmp_field -> ana_opts[7]);
    }
    if (tmp_field -> ana_opts[8] == 1.0)
    {
      print_control_string (buf, "ang", "\n# Calculate and collect angles PDFs", NULL, "ana                ");
      for (k=9; k<11; k++) print_int (buf, (int)tmp_field -> ana_opts[k]);
    }
    if (tmp_field -> ana_opts[11] == 1.0)
    {
      print_control_string (buf, "dih", "\n# Calculate and collect dihedrals PDFs", NULL, "ana                ");
      for (k=12; k<14; k++) print_int (buf, (int)tmp_field -> ana_opts[k]);
    }
    if (tmp_field -> ana_opts[14] == 1.0)
    {
      print_control_string (buf, "inv", "\n# Calculate and collect inversions PDFs", NULL, "ana                ");
      for (k=15; k<17; k++) print_int (buf, (int)tmp_field -> ana_opts[k]);
    }
    print_control_string (buf, "ana", "\n# Print any opted for analysis inter and intra-molecular PDFs", NULL, "print              ");
  }

  print_info ("\n", NULL, buf);

  if (tmp_field -> out_opts[21] == 1.0 || tmp_field -> out_opts[27] == 1.0)
  {
    print_control_float (buf, tmp_field -> out_opts[23], "\n# Bin size for RDfs and Z-density distribution: ", " Ang.", "binsize            ");
  }
  if (tmp_field -> out_opts[21] == 1.0)
  {
    print_control_int (buf, (int)tmp_field -> out_opts[22], "\n# Calculate and collect radial distribution functions every: ", " MD step(s)", "rdf                ");
    print_info ("\n", NULL, buf);
    print_control_string (buf, "rdf", NULL, NULL, "print              ");
  }
  if (tmp_field -> out_opts[27] == 1.0)
  {
    print_control_int (buf, (int)tmp_field -> out_opts[28], "\n# Calculate and collect Z-density profile every: ", " MD step(s)", "zden               ");
    print_info ("\n", NULL, buf);
    print_control_string (buf, "zden", NULL, NULL, "print              ");
  }
  if (tmp_field -> out_opts[24] == 1.0)
  {
    print_control_key (buf, "\n# Velocity autocorrelation functions, VAFs\n", "vaf                ");
    for (k=25; k<27; k++) print_int (buf, (int)tmp_field -> out_opts[k]);
    print_info ("\n", NULL, buf);
    print_control_string (buf, "vaf", NULL, NULL, "print              ");
    if (tmp_field -> out_opts[29] == 1.0)
    {
      print_control_string (buf, "vafav", "\n# Ignore time averaging for the VAFs", NULL, "no                 ");
    }
  }

  if ((int)tmp_field -> out_opts[0] || (int)tmp_field -> out_opts[4] || (int)tmp_field -> out_opts[8]
   || (int)tmp_field -> out_opts[12] || (int)tmp_field -> out_opts[15] || (int)tmp_field -> out_opts[17] || (int)tmp_field -> out_opts[19])
  {
    print_info ("\n\n# Output information", NULL, buf);
    if ((int)tmp_field -> out_opts[0])
    {
      print_control_key (buf, "\n# Write defects trajectory file, DEFECTS\n", "defe               ");
      for (k=1; k<3; k++) print_int (buf, (int)tmp_field -> out_opts[k]);
      print_float (buf, tmp_field -> out_opts[3]);
    }
    if ((int)tmp_field -> out_opts[4])
    {
      print_control_key (buf, "\n# Write displacement trajectory file, RSDDAT\n", "disp               ");
      for (k=5; k<7; k++) print_int (buf, (int)tmp_field -> out_opts[k]);
      print_float (buf, tmp_field -> out_opts[7]);
    }
    if ((int)tmp_field -> out_opts[8])
    {
      print_control_key (buf, "\n# Write HISTORY file\n", "traj               ");
      for (k=9; k<11; k++) print_int (buf, (int)tmp_field -> out_opts[k]);
      print_float (buf, tmp_field -> out_opts[11]);
    }
    if ((int)tmp_field -> out_opts[12])
    {
      print_control_key (buf, "\n# Write MSDTMP file\n", "msdtmp             ");
      for (k=13; k<15; k++) print_int (buf, (int)tmp_field -> out_opts[k]);
    }
    if ((int)tmp_field -> out_opts[15])
    {
      print_control_int (buf, (int)tmp_field -> out_opts[16], "\n# Print system data every: ", " MD step(s)", "print              ");
    }
    if ((int)tmp_field -> out_opts[17])
    {
      print_control_int (buf, (int)tmp_field -> out_opts[18], "\n# Accumulate statics data every: ", " MD step(s)", "stats              ");
    }
    if ((int)tmp_field -> out_opts[19])
    {
      print_control_int (buf, (int)tmp_field -> out_opts[20], "\n# Set rolling average stack to: ", " MD step(s)", "stack              ");
    }
  }

  print_control_int (buf, (int)tmp_field -> out_opts[30], "\n# Dump restart information every: ", " MD step(s)", "dump               ");

  for (i=0; i<2; i++)
  {
    if (tmp_field -> io_opts[2*i] == 1.0)
    {
      print_control_float (buf, tmp_field -> io_opts[2*i+1], time_inf[i], " s", time_key[i]);
    }
  }
  print_info ("\n", NULL, buf);
  for (i=0; i<2; i++)
  {
    j=4 + i*6;
    if (tmp_field -> io_opts[j] == 1.0)
    {
      j ++;
      print_info (io_inf[i], NULL, buf);
      print_info ("#  - method = ", NULL, buf);
      print_info (io_rw_m[(int)tmp_field -> io_opts[j]], NULL, buf);
      j++;
      if (i)
      {
        if (tmp_field -> io_opts[j-1] == 3.0)
        {
          print_info ("\n#  - precision = ", NULL, buf);
          print_info (io_pres[(int)tmp_field -> io_opts[j]], NULL, buf);
        }
        j ++;
        print_info ("\n#  - type = ", NULL, buf);
        print_info (io_typ[(int)tmp_field -> io_opts[j]], NULL, buf);
        j++;
      }
      if (tmp_field -> io_opts[4+7*i] != 2.0)
      {
        print_info ("\n#  - j, reader count = ", NULL, buf);
        str_a = g_strdup_printf ("%d", (int)tmp_field -> io_opts[j]);
        print_info (str_a, NULL, buf);
      }
      j++;
      if (tmp_field -> io_opts[4+7*i] != 2.0)
      {
        print_info ("\n#  - k, batch size = ", NULL, buf);
        str_b = g_strdup_printf ("%d", (int)tmp_field -> io_opts[j]);
        print_info (str_b, NULL, buf);
      }
      j++;
      print_info ("\n#  - l, buffer size = ", NULL, buf);
      str_c = g_strdup_printf ("%d", (int)tmp_field -> io_opts[j]);
      print_info (str_c, NULL, buf);
      j++;
      if (tmp_field -> io_opts[4+7*i] != 2.0)
      {
        print_info ("\n#  - e, parallel error check is ", NULL, buf);
        print_info (io_pec[(int)tmp_field -> io_opts[j]], NULL, buf);
      }
      print_info (io_key[i], "bold", buf);
      print_info (io_meth[(int)tmp_field -> io_opts[5+6*i]], "bold_green", buf);
      if (i)
      {
        if (tmp_field -> io_opts[11] == 3.0)
        {
          print_info (io_pres[(int)tmp_field -> io_opts[12]], "bold_green", buf);
        }
        print_info (" ", NULL, buf);
        print_info (io_typ[(int)tmp_field -> io_opts[13]], "bold_green", buf);
      }
      if (tmp_field -> io_opts[4+7*i] != 2.0)
      {
        print_info (" ", NULL, buf);
        print_info (str_a, "bold_blue", buf);
        g_free (str_a);
        print_info (" ", NULL, buf);
        print_info (str_b, "bold_blue", buf);
        g_free (str_b);
      }
      print_info (" ", NULL, buf);
      print_info (str_c, "bold_blue", buf);
      g_free (str_c);
      if (tmp_field -> io_opts[4+7*i] != 2.0)
      {
        (tmp_field -> io_opts[j] == 0.0) ? print_string (buf, "N") : print_string (buf, "Y");
      }
      print_info ("\n", NULL, buf);
      j++;
    }
  }
  if (tmp_field -> io_opts[18] == 1.0)
  {
    print_control_key (buf, "\n# Seeds for the random number generators\n", "seed               ");
    for (i=19; i<22; i++) print_int (buf, (int)tmp_field -> io_opts[i]);
    print_info ("\n", NULL, buf);
  }
  if (tmp_field -> io_opts[22] == 1.0)
  {
    print_control_key (buf, "\n# Limits to 2 the number of processors in z direction for slab simulations\n", "slab");
  }

  print_info ("\n\n", NULL, buf);
  print_info ("finish", "bold", buf); // Close the CONTROL file
}
