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
* @file la_print.c
* @short Functions to print the LAMMPS input file(s)
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'la_print.c'
*
* Contains:
*

 - The functions to print the LAMMPS input file(s)

*
* List of functions:

  int get_mol_id_from_model_id (int at);
  int print_lammps_dihedral (int did, int di, GtkTextBuffer * buf, field_struct * dh);
  int print_lammps_angle (int aid, int ai, GtkTextBuffer * buf, field_struct * an);
  int print_lammps_bond (int bid, int bi, GtkTextBuffer * buf, field_struct * bd);
  int is_this_new_prop (int sid, field_prop * init, field_prop * to_check);
  int get_type_struct_to_print (field_molecule * tfmol, int sid);
  int get_different_atoms ();

  gboolean are_different_field_atoms (field_atom* at, field_atom* bt);

  void print_lammps_mass (GtkTextBuffer * buf);
  void print_lammps_atoms (GtkTextBuffer * buf);
  void print_lammps_atom_file (GtkTextBuffer * buf);

  field_atom* get_print_atom (int aid);

*/

#include "dlp_field.h"
#include "interface.h"

/*! \enum lammps_atom_types */
enum lammps_atom_types { // comments = corresponding data file formats
  l_angle      =  0,     /*!< atom-ID molecule-ID atom-type x y z */
  l_atomic     =  1,     /*!< atom-ID atom-type x y z */
  l_body       =  2,     /*!< atom-ID atom-type bodyflag mass x y z */
  l_bond       =  3,     /*!< atom-ID molecule-ID atom-type x y z */
  l_charge     =  4,     /*!< atom-type q x y z */
  l_dipole     =  5,     /*!< atom-ID atom-type q x y z mux muy muz */
  l_dpd        =  6,     /*!< atom-ID atom-type theta x y z */
  l_edpd       =  7,     /*!< atom-ID atom-type edpd_temp edpd_cv x y z */
  l_electron   =  8,     /*!< atom-ID atom-type q spin eradius x y z */
  l_ellipsoid  =  9,     /*!< atom-ID atom-type ellipsoidflag density x y z */
  l_full       = 10,     /*!< atom-ID molecule-ID atom-type q x y z */
  l_line       = 11,     /*!< atom-ID molecule-ID atom-type lineflag density x y z */
  l_mdpd       = 12,     /*!< atom-ID atom-type rho x y z */
  l_molecular  = 13,     /*!< atom-ID molecule-ID atom-type x y z */
  l_peri       = 14,     /*!< atom-ID atom-type volume density x y z */
  l_smd        = 15,     /*!< atom-ID atom-type molecule volume mass kernel-radius contact-radius x0 y0 z0 x y z */
  l_sph        = 16,     /*!< atom-ID atom-type rho esph cv x y z */
  l_sphere     = 17,     /*!< atom-ID atom-type diameter density x y z */
  l_spin       = 18,     /*!< atom-ID atom-type x y z spx spy spz sp */
  l_tdpd       = 19,     /*!< atom-ID atom-type x y z cc1 cc2 … ccNspecies */
  l_template   = 20,     /*!< atom-ID atom-type molecule-ID template-index template-atom x y z */
  l_tri        = 21,     /*!< atom-ID molecule-ID atom-type triangleflag density x y z */
  l_wavepacket = 22,     /*!< atom-ID atom-type charge spin eradius etag cs_re cs_im x y z */
  l_hybrid     = 23      /*!< atom-ID atom-type x y z sub-style1 sub-style2 … */
};

extern gboolean in_bond (int at, int bd[2]);
extern int get_num_vdw_max ();
extern gchar * get_body_element_name (field_nth_body * body, int aid, int nbd);
extern int get_num_struct_to_print (field_molecule * fmol, int sid);
extern gboolean are_identical_prop (int ti, int ai, field_prop * pro_a, field_prop * pro_b);
extern void merging_atoms (field_atom* to_merge, field_atom* to_remove, gboolean upda);
extern char * vect_comp[3];

/*!
  \fn int get_mol_id_from_model_id (int at)

  \brief get field molecule id using atom id

  \param at the atom id
*/
int get_mol_id_from_model_id (int at)
{
  int i;
  tmp_fmol = tmp_field -> first_molecule;
  field_atom* fat;
  while (tmp_fmol)
  {
    fat = tmp_fmol -> first_atom;
    while (fat)
    {
      for (i=0; i< fat -> num; i++)
      {
        if (at == fat -> list[i]) return tmp_fmol -> id;
      }
      fat = fat -> next;
    }
    tmp_fmol = tmp_fmol -> next;
  }
  return -1;
}

field_prop * print_prop[8];

/*!
  \fn int print_lammps_dihedral (int did, int di, GtkTextBuffer * buf, field_struct * dh)

  \brief print LAMMPS dihedral

  \param did the dihedral id to print
  \param di the type of structural property = 2
  \param buf the GtkTextBuffer to print into
  \param dh the structural property to print
*/
int print_lammps_dihedral (int did, int di, GtkTextBuffer * buf, field_struct * dh)
{
  int a, c, i, j, k, l, m, n, o, p;
  gchar * str;
  field_prop * tp_prop;
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
    a = ids[0] = tmp_fat -> list_id[i];
    if (same_atom) already_done[a] = TRUE;
    for (k=0; k<tmp_proj -> atoms[0][j].numv; k++)
    {
      l = tmp_proj -> atoms[0][j].vois[k];
      if (tmp_proj -> atoms[0][l].faid == tmp_fbt -> id)
      {
        ids[1] = tmp_proj -> atoms[0][l].fid;
        for (m=0; m<tmp_proj -> atoms[0][l].numv; m++)
        {
          n = tmp_proj -> atoms[0][l].vois[m];
          if (n != j && tmp_proj -> atoms[0][n].faid == tmp_fct -> id)
          {
            ids[2] = tmp_proj -> atoms[0][n].fid;
            for (o=0; o<tmp_proj -> atoms[0][n].numv; o++)
            {
              p = tmp_proj -> atoms[0][n].vois[o];
              c = ids[3] = tmp_proj -> atoms[0][p].fid;
              if (p != j && p != l && tmp_proj -> atoms[0][p].faid == tmp_fdt -> id && (! same_atom || (same_atom && ! already_done[c])))
              {
                tmp_fprop = get_active_prop_using_atoms (dh -> other, 4, ids);
                if (tmp_fprop == NULL) tmp_fprop = dh -> def;
                tp_prop = print_prop[2*di];
                while (! are_identical_prop (2*di+1, 0, tmp_fprop, tp_prop) && tp_prop -> next) tp_prop = tp_prop -> next;
                if (tp_prop -> use)
                {
                  str = g_strdup_printf ("%5d %5d %10d %10d %10d %10d\n", did+1, tp_prop -> pid, j+1, l+1, n+1, p+1);
                  print_info (str, NULL, buf);
                  g_free (str);
                  did ++;
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
  return did;
}

/*!
  \fn int print_lammps_angle (int aid, int ai, GtkTextBuffer * buf, field_struct * an)

  \brief print LAMMPS angle

  \param aid the angle id to print
  \param ai the type of structural property = 1
  \param buf the GtkTextBuffer to print into
  \param an the structural property to print
*/
int print_lammps_angle (int aid, int ai, GtkTextBuffer * buf, field_struct * an)
{
  int i, j, k, l, m, o, p, q;
  gchar * str;
  field_prop * tp_prop;
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
    k = ids[0] = tmp_proj -> atoms[0][j].fid;
    if (same_atom) already_done[k] = TRUE;
    for (l=0; l<tmp_proj -> atoms[0][j].numv; l++)
    {
      m = tmp_proj -> atoms[0][j].vois[l];
      if (tmp_proj -> atoms[0][m].faid == tmp_fbt -> id)
      {
        ids[1] = tmp_proj -> atoms[0][m].fid;
        for (o=0; o<tmp_proj -> atoms[0][m].numv; o++)
        {
          p = tmp_proj -> atoms[0][m].vois[o];
          q = ids[2] = tmp_proj -> atoms[0][p].fid;
          if (p != j && tmp_proj -> atoms[0][p].faid == tmp_fct -> id && (! same_atom || (same_atom && ! already_done[q])))
          {
            tmp_fprop = get_active_prop_using_atoms (an -> other, 3, ids);
            if (tmp_fprop == NULL) tmp_fprop = an -> def;
            tp_prop = print_prop[2*ai];
            while (! are_identical_prop (2*ai+1, 0, tmp_fprop, tp_prop)) tp_prop = tp_prop -> next;
            if (tp_prop -> use)
            {
              str = g_strdup_printf ("%5d %5d %10d %10d %10d\n", aid+1, tp_prop -> pid, j+1, m+1, p+1);
              print_info (str, NULL, buf);
              g_free (str);
              aid ++;
            }
          }
        }
      }
    }
  }
  g_free (ids);
  if (same_atom) g_free (already_done);
  return aid;
}

/*!
  \fn int print_lammps_bond (int bid, int bi, GtkTextBuffer * buf, field_struct * bd)

  \brief print LAMMPS bond

  \param bid the bond id to print
  \param bi the type of structural property = 0
  \param buf the GtkTextBuffer to print into
  \param bd the structural property to print
*/
int print_lammps_bond (int bid, int bi, GtkTextBuffer * buf, field_struct * bd)
{
  int i, j, k, l, m, n;
  gchar * str;
  field_prop * tp_prop;
  int * ids = allocint (2);
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
    k = ids[0] = tmp_fat -> list_id[i];
    if (same_atom) already_done[k] = TRUE;
    for (l=0; l<tmp_proj -> atoms[0][j].numv; l++)
    {
      m = tmp_proj -> atoms[0][j].vois[l];
      n = ids[1] = tmp_proj -> atoms[0][m].fid;
      if (tmp_proj -> atoms[0][m].faid == tmp_fbt -> id && (! same_atom || (same_atom && ! already_done[n])))
      {
        tmp_fprop = get_active_prop_using_atoms (bd -> other, 2, ids);
        if (tmp_fprop == NULL) tmp_fprop = bd -> def;
        tp_prop = print_prop[2*bi];
        while (! are_identical_prop (2*bi+1, 0, tmp_fprop, tp_prop)) tp_prop = tp_prop -> next;
        if (tmp_fprop -> use)
        {
          str = g_strdup_printf ("%5d %5d %10d %10d\n", bid+1, tp_prop -> pid, j+1, m+1);
          print_info (str, NULL, buf);
          g_free (str);
          bid ++;
        }
      }
    }
  }
  g_free (ids);
  if (same_atom) g_free (already_done);
  return bid;
}

/*!
  \fn int is_this_new_prop (int sid, field_prop * init, field_prop * to_check)

  \brief check if field property exists, if not add it

  \param sid the type of structural property
  \param init the initial field property pointer
  \param to_check the field property to check
*/
int is_this_new_prop (int sid, field_prop * init, field_prop * to_check)
{
  field_prop * tmp_pr = init;
  gboolean add_prop = TRUE;
  while (tmp_pr)
  {
    if (are_identical_prop (sid+1, 0, to_check, tmp_pr))
    {
      add_prop = FALSE;
      break;
    }
    else if (tmp_pr -> next)
    {
      tmp_pr = tmp_pr -> next;
    }
    else
    {
      break;
    }
  }
  if (add_prop)
  {
    tmp_pr -> next = duplicate_field_prop (to_check, sid);
    tmp_pr -> next -> pid = tmp_pr -> pid + 1;
  }
  return (add_prop) ? 1 : 0;
}

/*!
  \fn int get_type_struct_to_print (field_molecule * tfmol, int sid)

  \brief build the list of 'sid' type structural property(ies) to print

  \param tfmol the field molecule
  \param sid the type of structural
*/
int get_type_struct_to_print (field_molecule * tfmol, int sid)
{
  int pid = 0;
  tmp_fstr = tfmol -> first_struct[sid];
  while (tmp_fstr)
  {
    if (tmp_fstr -> def -> use)
    {
      if (! print_prop[sid])
      {
        print_prop[sid] = duplicate_field_prop (tmp_fstr -> def, sid);
        pid = print_prop[sid] -> pid = 1;
      }
      else
      {
        pid += is_this_new_prop (sid, print_prop[sid], tmp_fstr -> def);
      }
    }
    if (tmp_fstr -> other)
    {
      tmp_fprop = tmp_fstr -> other;
      while (tmp_fprop)
      {
        if (tmp_fprop -> use && ! are_identical_prop (sid+1, 0, tmp_fstr -> def, tmp_fprop))
        {
          if (! print_prop[sid])
          {
            print_prop[sid] = duplicate_field_prop (tmp_fprop, sid);
            pid = print_prop[sid] -> pid = 1;
          }
          else
          {
            pid += is_this_new_prop (sid, print_prop[sid], tmp_fprop);
          }
        }
        tmp_fprop = tmp_fprop -> next;
      }
    }
    tmp_fstr = tmp_fstr -> next;
  }
  return pid;
}

field_atom* all_at;

/*!
  \fn gboolean are_different_field_atoms (field_atom* at, field_atom* bt)

  \brief check if two field atoms are different

  \param at 1st field atom
  \param bt 2nd field atom
*/
gboolean are_different_field_atoms (field_atom* at, field_atom* bt)
{
  if (g_strcmp0(at -> name, bt -> name) != 0) return TRUE;
  if (at -> sp != bt -> sp) return TRUE;
  if (at -> mass != bt -> mass) return TRUE;
  if (at -> charge != bt -> charge) return TRUE;
  return FALSE;
}

/*!
  \fn int get_different_atoms ()

  \brief create the list of all different field atoms
*/
int get_different_atoms ()
{
  int numat = 0;
  tmp_fmol = tmp_field -> first_molecule;
  all_at = NULL;
  gboolean append;
  while (tmp_fmol)
  {
    tmp_fat = tmp_fmol -> first_atom;
    while (tmp_fat)
    {
      if (! all_at)
      {
        all_at = duplicate_field_atom(tmp_fat);
        numat = all_at -> id = 1;
      }
      else
      {
        append = TRUE;
        tmp_fbt = all_at;
        while (tmp_fbt)
        {
          if (! are_different_field_atoms(tmp_fat, tmp_fbt))
          {
            merging_atoms (tmp_fbt, tmp_fat, FALSE);
            append = FALSE;
            break;
          }
          tmp_fbt = tmp_fbt -> next;
        }
        if (append)
        {
          tmp_fbt = all_at;
          while (tmp_fbt -> next) tmp_fbt = tmp_fbt -> next;
          tmp_fbt -> next = duplicate_field_atom (tmp_fat);
          numat = tmp_fbt -> next -> id = tmp_fbt -> id + 1;
        }
      }
      tmp_fat = tmp_fat -> next;
    }
    tmp_fmol = tmp_fmol -> next;
  }
  return numat;
}

/*!
  \fn void print_lammps_mass (GtkTextBuffer * buf)

  \brief pritn LAMMPS atomic masses

  \param buf the GtkTextBuffer to print into
*/
void print_lammps_mass (GtkTextBuffer * buf)
{
  gchar * str;
  print_info ("\nMasses\n\n", "bold", buf);
  tmp_fat = all_at;
  while (tmp_fat)
  {
    str = g_strdup_printf ("\t%d\t%f\n", tmp_fat -> id, tmp_fat -> mass);
    print_info (str, NULL, buf);
    g_free (str);
    tmp_fat = tmp_fat -> next;
  }
}

/*!
  \fn field_atom* get_print_atom (int aid)

  \brief get LAMMPS field atom

  \param aid the atom id to find
*/
field_atom* get_print_atom (int aid)
{
  int i;
  tmp_fat = all_at;
  while (tmp_fat)
  {
    for (i=0; i<tmp_fat -> num; i++)
    {
      if (tmp_fat -> list[i] == aid) return tmp_fat;
    }
    tmp_fat = tmp_fat -> next;
  }
  return NULL;
}

/*!
  \fn void print_lammps_atoms (GtkTextBuffer * buf)

  \brief print LAMMPS atoms

  \param buf the GtkTextBuffer to print into
*/
void print_lammps_atoms (GtkTextBuffer * buf)
{
  int i; //, j, k;
  // * field_molecule * la_mol;
  field_atom* la_ats;
  gchar * pos, * atid, * atype; //* molid, * amass;
  gchar * str;
  print_info ("\nAtoms\n\n", "bold", buf);
  for (i=0; i<tmp_proj -> natomes; i++)
  {
    atid = g_strdup_printf ("%10d", i+1);
    // * la_mol = get_active_field_molecule_from_model_id (tmp_proj, i);
    // molid = g_strdup_printf ("%5d", la_mol -> id+1);
    la_ats = get_print_atom (i);
    atype = g_strdup_printf ("%5d", la_ats -> id);
    pos = g_strdup_printf ("%f\t%f\t%f", tmp_proj -> atoms[0][i].x, tmp_proj -> atoms[0][i].y, tmp_proj -> atoms[0][i].z);
    // amass = g_strdup_printf ("%f", la_ats -> mass);
    /* switch ()
    {
      case l_angle:
        // atom-ID molecule-ID atom-type x y z
        str = g_strdup_printf ("%s\t%s\t%s\t%s\n", atid, molid, atype, pos);
        print_info (str, NULL, buf);
        g_free (str);
        break;
      case l_atomic: */
        // atom-ID atom-type x y z
        str = g_strdup_printf ("%s\t%s\t%s\n", atid, atype, pos);
        print_info (str, NULL, buf);
        g_free (str);
        /*break;
      case l_body:
        // atom-ID atom-type bodyflag mass x y z
        break;
      case l_bond:
        // atom-ID molecule-ID atom-type x y z
        str = g_strdup_printf ("%s\t%s\t%s\t%s\n", atid, molid, atype, pos);
        print_info (str, NULL, buf);
        g_free (str);
        break;
      case l_charge:
        // atom-type q x y z
        break;
      case l_dipole:
        // atom-ID atom-type q x y z mux muy muz
        break;
      case l_dpd:
        // atom-ID atom-type theta x y z
        break;
      case l_edpd:
        // atom-ID atom-type edpd_temp edpd_cv x y z
        break;
      case l_electron:
        // atom-ID atom-type q spin eradius x y z
        break;
      case l_ellipsoid:
        // atom-ID atom-type ellipsoidflag density x y z
        break;
      case l_full:
        // atom-ID molecule-ID atom-type q x y z
        break;
      case l_line:
        // atom-ID molecule-ID atom-type lineflag density x y z
        break;
      case l_mdpd:
        // atom-ID atom-type rho x y z
        break;
      case l_molecular:
        // atom-ID molecule-ID atom-type x y z
        break;
      case l_peri:
        // atom-ID atom-type volume density x y z
        break;
      case l_smd:
        // atom-ID atom-type molecule volume mass kernel-radius contact-radius x0 y0 z0 x y z
        break;
      case l_sph:
        // atom-ID atom-type rho esph cv x y z
        break;
      case l_sphere:
        // atom-ID atom-type diameter density x y z
        break;
      case l_spin:
        // atom-ID atom-type x y z spx spy spz sp
        break;
      case l_tdpd:
        // atom-ID atom-type x y z cc1 cc2 … ccNspecies
        break;
      case l_template:
        // atom-ID atom-type molecule-ID template-index template-atom x y z
        break;
      case l_tri:
        // atom-ID molecule-ID atom-type triangleflag density x y z
        break;
      case l_wavepacket:
        // atom-ID atom-type charge spin eradius etag cs_re cs_im x y z
        break;
    }*/
  }
}

/*!
  \fn void print_lammps_atom_file (GtkTextBuffer * buf)

  \brief print LAMMPS atom file

  \param buf the GtkTextBuffer to print into
*/
void print_lammps_atom_file (GtkTextBuffer * buf)
{
  int i, j; //, k, l;
  gchar * str;

  GtkTextIter bStart;
  GtkTextIter bEnd;

  gtk_text_buffer_get_start_iter (buf, & bStart);
  gtk_text_buffer_get_end_iter (buf, & bEnd);
  gtk_text_buffer_delete (buf, & bStart, & bEnd);

  //str = g_strdup_printf ("# This file was created using %s\n", PACKAGE);
  //print_info (str, NULL, buf);
  //g_free (str);
  print_info ("LAMMPS Atom File\n\n", NULL, buf);
  str = g_strdup_printf ("%12d", tmp_proj -> natomes);
  print_info (str, "bold_blue", buf);
  g_free (str);
  print_info ("  atoms", "bold", buf);
  print_info ("\n", NULL, buf);
  gchar * str_title[4] = {"  bond", "  angle", "  dihedral", "  improper"};

  for (i=0; i<4; i++)
  {
    tmp_fmol = tmp_field -> first_molecule;
    j = 0;
    while (tmp_fmol)
    {
      if (tmp_field -> afp[i*2+15])
      {
        j += get_num_struct_to_print (tmp_fmol, i*2);
      }
      tmp_fmol = tmp_fmol -> next;
    }
    if (j > 0)
    {
      str = g_strdup_printf ("%12d", j);
      print_info (str, "bold_blue", buf);
      g_free (str);
      print_info (str_title[i], "bold", buf);
      print_info ("s\n", "bold", buf);
    }
  }
  print_info ("\n", NULL, buf);
  int numat = get_different_atoms ();
  str = g_strdup_printf ("%12d", numat);
  print_info (str, "bold_red", buf);
  g_free (str);
  print_info ("  atom types", "bold", buf);
  print_info ("\n", NULL, buf);
  int ntypes[4];
  for (i=0; i<4; i++)
  {
    ntypes[i] = 0;
    print_prop[2*i] = NULL;
    if (tmp_field -> afp[i*2+15])
    {
      tmp_fmol = tmp_field -> first_molecule;
      while (tmp_fmol)
      {
        j = get_type_struct_to_print (tmp_fmol, 2*i);
        ntypes[i] += j;
        tmp_fmol = tmp_fmol -> next;
      }
      if (ntypes[i] > 0)
      {
        str = g_strdup_printf ("%12d", ntypes[i]);
        print_info (str, "bold_red", buf);
        g_free (str);
        print_info (str_title[i], "bold", buf);
        print_info (" types\n", "bold", buf);
      }
    }
  }

  // Lattice
  print_info ("\n", NULL, buf);
  /*xlo xhi
  ylo yhi
  zlo zhi*/
  if (tmp_proj -> cell.pbc)
  {
    j = 0;
    for (i=0; i<3; i++)
    {
      str = g_strdup_printf ("%f %f %slo %shi\n", 0.0, tmp_proj -> cell.box[0].param[0][i], vect_comp[i], vect_comp[i]);
      print_info (str, NULL, buf);
      g_free (str);
      if (tmp_proj -> cell.box[0].param[1][i] != 90.0) j=1;
    }
    if (j)
    {
      float lx, ly, lz;
      float xy, xz, yz;
      lx = tmp_proj -> cell.box[0].param[0][0];
      xy = tmp_proj -> cell.box[0].param[0][1] * cos(tmp_proj -> cell.box[0].param[1][2]*pi/180.0);
      xz = tmp_proj -> cell.box[0].param[0][2] * cos(tmp_proj -> cell.box[0].param[1][1]*pi/180.0);
      ly = sqrt(tmp_proj -> cell.box[0].param[0][1]*tmp_proj -> cell.box[0].param[0][1] -xy*xy);
      yz = (tmp_proj -> cell.box[0].param[0][1]*(tmp_proj -> cell.box[0].param[0][2]*cos(tmp_proj -> cell.box[0].param[1][0]*pi/180.0)) - xy*xz) / ly;
      lz = sqrt(tmp_proj -> cell.box[0].param[0][2]*tmp_proj -> cell.box[0].param[0][2] - xz*xz - yz*yz);
      str = g_strdup_printf ("%f %f %f\n", lx, ly, lz);
      print_info (str, NULL, buf);
      g_free (str);
    }
  }
  else
  {
    // min(pos), max(pos)
  }

  // Masses
  print_lammps_mass (buf);

  gchar * coeffs[13]={"Pair", "Bond", "Angle", "Dihedral", "Improper", "BondBond", "BondAngle", "MiddleBondTorsion",
                      "EndBondTorsion", "AngleTorsion", "AngleAngleTorsion", "BondBond13", "AngleAngle"};
  // > 5 = CVFF force field ?

  /* if (tmp_field -> afp[23])
  {
    j=0;
    tmp_fbody = tmp_field -> first_body[0];
    while (tmp_fbody)
    {
      if (tmp_fbody -> use) j++;
      tmp_fbody = tmp_fbody -> next;
    }
    if (j > 0)
    {
      k = get_num_vdw_max ();
      l = k * (k+1) / 2;
      str = g_strdup_printf ("%s Coeffs\n\n", coeffs[0]);
      print_info(str, "bold", buf);
      g_free (str);
      tmp_fbody = tmp_field -> first_body[0];
      while (tmp_fbody)
      {
        if (tmp_fbody -> use)
        {
          print_lam_body (buf, l, tmp_fbody);
        }
        tmp_fbody = tmp_fbody -> next;
      }
    }
  } */

  for (i=0; i<4; i++)
  {
    if (ntypes[i])
    {
      str = g_strdup_printf ("\n%s Coeffs\n\n", coeffs[i+1]);
      print_info (str, "bold", buf);
      g_free (str);
      tmp_fprop = print_prop[2*i];
      while (tmp_fprop)
      {
        str = g_strdup_printf (" %5d", tmp_fprop -> pid);
        print_info (str, NULL, buf);
        g_free (str);
        for (j=0; j<fvalues[activef][2*i+1][tmp_fprop -> key]; j++)
        {
          str = g_strdup_printf (" %15.10f", tmp_fprop -> val[j]);
          print_info (str, NULL, buf);
          g_free (str);
        }
        print_info ("\n", NULL, buf);
        tmp_fprop = tmp_fprop -> next;
      }
    }
  }

  // Atoms
  print_lammps_atoms (buf);

  for (i=0; i<4; i++)
  {
    j = 0;
    if (ntypes[i])
    {
      str = g_strdup_printf ("\n%ss\n\n", coeffs[i+1]);
      print_info(str, "bold", buf);
      g_free (str);
      tmp_fmol = tmp_field -> first_molecule;
      while (tmp_fmol)
      {
        if (get_num_struct_to_print (tmp_fmol, i*2))
        {
          tmp_fstr = tmp_fmol -> first_struct[i*2];
          while (tmp_fstr)
          {
            tmp_fat = get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[0]);
            tmp_fbt = get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[1]);
            if (i > 0) tmp_fct = get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[2]);
            if (i > 1) tmp_fdt = get_active_atom (tmp_fmol -> id, tmp_fstr -> aid[3]);
            if (i == 0) j = print_lammps_bond (j, i, buf, tmp_fstr);
            if (i == 1) j = print_lammps_angle (j, i, buf, tmp_fstr);
            if (i == 2) j = print_lammps_dihedral (j, i, buf, tmp_fstr);
            // if (i == 3) j = print_lammps_improper (j, i, buf, tmp_fstr);
            tmp_fstr = tmp_fstr -> next;
          }
        }
        tmp_fmol = tmp_fmol -> next;
      }
    }
  }
}
