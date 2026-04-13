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
* @file force_fields.c
* @short Functions to prepare the force field XML files from the sources in atomes \n
         Functions to read the force field XML files
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'force_fields.c'
*
* Contains:
*

 - The functions to prepare the force field XML files from the sources in atomes
 - The functions to read the force field XML files

*
* List of functions:

  int get_pdim(int fid, int prop, int col);
  int get_fkey(int fid, int prop, int col);
  int find_atom_id (int print, char * keyw);
  int find_atom_z (char * keyw);
  int get_z (int faid);
  int get_atoms (int z);
  int get_bond (int faid, int bid);
  int get_angle (int faid, int aid);
  int get_apex (int faid, int aid);
  int get_torsion (int faid, int tid, int a, int b);
  int get_improper (int faid, int a, int b);
  int get_vdw (int faid);
  int get_bi (int faid);
  int field_find_atoms ();

  float get_force_field_atom_mass (int sp, int num);

  gboolean not_done (int eid, int a, int b);
  gboolean not_done_an (int eid, int a, int b, int c);
  gboolean is_a_match (int * data, int num, int val[4]);

  gchar * find_atom_key (int fid, int prop, char * keyw);
  gchar * open_field_file (int field);

  void associate_pointers_to_field_data (int id);
  void print_object_dim_and_key_tables (int fid);
  void set_data (int pid, int obj, int oid, int faid);
  void field_has_element (int aid);
  void print_atom_table (int fid);
  void print_this_bond (int eid, int h, int fid, int inum, char * at_a, char * at_b, char ** the_bond);
  void print_bond_table (int fid, int inum);
  void print_this_angle (int eid, int h, int fid, int inum, int ub, char * at_a, char * at_b, char * at_c, char ** the_angle);
  void print_angle_table (int fid, int inum);
  void print_dihedral_table (int fid, int inum);
  void print_improper_table (int fid, int inum);
  void print_inversion_table (int fid, int inum);
  void print_vdw_table (int fid, int inum);
  void find_object_ijkl (int hid, int foid, int oid, int sa, int za, int sb, int zb, int sc, int zc, int sd, int zd);
  void field_find_bonds ();
  void field_find_angles ();
  void field_find_dihedrals (int id);
  void field_find_vdw ();
  void print_all (int oid);
  void clean_this_field_data (xmlDoc * doc, xmlTextReaderPtr reader);

  G_MODULE_EXPORT void setup_this_force_field (int id);

*/

#include "global.h"
#include "interface.h"
#include "dlp_field.h"
#include "force_fields.h"
#include <libxml/xmlreader.h>

extern xmlNodePtr findnode (xmlNodePtr startnode, char * nname);
extern int clean_xml_data (xmlDoc * doc, xmlTextReaderPtr reader);

#define N_FIELDS 21
#define N_PARAMS
#define ALL_PARAMS 9

// Define Fields order and id

#define FATS 0
#define FEQI 1
#define FBDS 2
#define FANG 3
#define FDIH 4
#define FIMP 5
#define FINV 6
#define FNBD 7
#define FINC 8

#define USE_ATOMS

/*

AMBER*: In atoms check label (1 letter only)

CHARMM*: In atoms check label (1 letter only)

CVFF/CVFF_aug/CFF91/PCFF:
  - Bonds: 'c=_' in 'c=', 'n=_' in 'n=', 'd_' in 'dw', 'ci_' in 'ci', 'ni_' in 'ni'
  - Quadratic angles: 's3m_' in 's3e_', 's4m_' in 's4e_', 'o4m_' in 'o4e_', '*?' in '*'

CVFF:
  - Atoms: 'mg' in 'Mg'

CVFF_aug:
  - Non-bonded : 'Al' in 'al'

Compass :

OPLSAA?:
  - In atoms : 'NA' to 'Na', 'CLA' to 'Cl', 'MG' to 'Mg'

*/

char * field_keyw[N_FIELDS] = {"amber94",
                               "amber96",
                               "amber98",
                               "amber99",
                               "prot_22",
                               "prot_metals_22",
                               "ethers_35",
                               "carb_36",
                               "cgenff_36",
                               "lipid_36",
                               "na_36",
                               "prot_36",
                               "prot_metals_36",
                               "silicates",
                               "CVFF",
                               "CVFF_Aug",
                               "CFF91",
                               "PCFF",
                               "Compass",
                               "OPLSAAP",
                               "OPLSAAR"};

char * field_ffl[N_FIELDS] = {"amber94",
                              "amber96",
                              "amber98",
                              "amber99",
                              "charmm-prot_22",
                              "charmm-prot_metals_22",
                              "charmm-ethers_35",
                              "charmm-carb_36",
                              "charmm-cgenff_36",
                              "charmm-lipid_36",
                              "charmm-na_36",
                              "charmm-prot_36",
                              "charmm-prot_metals_36",
                              "charmm-silicates",
                              "cvff",
                              "cvff_aug",
                              "cff91",
                              "pcff",
                              "compass",
                              "oplsaap",
                              "oplsaar"};

char * field_name[N_FIELDS] = {"Assisted Model Building with Energy Refinement 94",
                               "Assisted Model Building with Energy Refinement 96",
                               "Assisted Model Building with Energy Refinement 98",
                               "Assisted Model Building with Energy Refinement 99",
                               "Chemistry at HARvard Macromolecular Mechanics 22 Proteins",
                               "Chemistry at HARvard Macromolecular Mechanics 22 Proteins and metals",
                               "Chemistry at HARvard Macromolecular Mechanics 35 Ethers",
                               "Chemistry at HARvard Macromolecular Mechanics 36 Carbohydrates",
                               "Chemistry at HARvard Macromolecular Mechanics 36 General",
                               "Chemistry at HARvard Macromolecular Mechanics 36 Lipids",
                               "Chemistry at HARvard Macromolecular Mechanics 36 Nucleid acids",
                               "Chemistry at HARvard Macromolecular Mechanics 36 Proteins",
                               "Chemistry at HARvard Macromolecular Mechanics 36 proteins and metals",
                               "Chemistry at HARvard Macromolecular Mechanics Silicates",
                               "Consistent Valence Force Field",
                               "Consistent Valence Force Field Augmented",
                               "Consistent Force Field 91",
                               "Polymer Consistent Force Field",
                               "Condensed-phase Optimized Molecular Potentials for Atomistic Simulation Studies",
                               "Optimized Potentials for Liquid Simulation All-atoms for Proteins",
                               "Optimized Potentials for Liquid Simulation All-atoms for Proteins, Nucleosides, and Nucleotides"};

char * field_acro[N_FIELDS] = {"AMBER 94",
                               "AMBER 96",
                               "AMBER 98",
                               "AMBER 99",
                               "CHARMM 22 Proteins",
                               "CHARMM 22 Proteins and metals",
                               "CHARMM 35 Ethers",
                               "CHARMM 36 Carbohydrates",
                               "CHARMM 36 General",
                               "CHARMM 36 Lipids",
                               "CHARMM 36 Nucleid acids",
                               "CHARMM 36 Proteins",
                               "CHARMM 36 Proteins and metals",
                               "CHARMM Silicates",
                               "CVFF",
                               "CVFF Augmented",
                               "CFF 91",
                               "PCFF",
                               "COMPASS",
                               "OPLS All-atoms for proteins",
                               "OPLS All-atoms for RNA"};

char *** field_atoms;
char *** field_equi[2];
// 0 = Quadratic, 1 = Quartic, 2 = Morse
char *** field_bonds[3];
// 0 = Quadratic, 1 = Quartic
char *** field_angles[2];
char *** field_dihedrals[2];
char *** field_inversions;
char *** field_impropers;
char *** field_vdw;

int * field_objects;
int * field_dim;

gboolean is_99;

int * atoms_id;
int ** atoms_id_list;
int ** extraz_id;

FILE * fp;

gchar * ffo_file[21]={"parm94.dat", "parm96.dat", "parm98.dat", "parm99.dat",
                      "par_all22_prot.prm", "par_all22_prot_metals.prm", "par_all35_ethers.prm",
                      "par_all36_carb.prm", "par_all36_cgenff.prm", "par_all36_lipid.prm", "par_all36_na.prm",
                      "par_all36_prot.prm", "par_all36m_prot.prm", "par_all_silicates.prm",
                      "CVFF.frc", "CVFF_aug.frc", "CFF91.frc", "PCFF.frc", "compass.frc",  "par_opls_aam.inp", "par_opls_rna.inp"};

gchar * prop_name[11]= {"Quadratic bonds", "Quartic bonds", "Morse bonds", "Quadratic angles", "Quartic angles",
                        "Dihedrals", "Torsions 3", "Inversions", "Impropers", "Non-bonded", "Bond increments"};

gchar * table_name[11]= {"qbonds", "sbonds", "mbonds", "qangles", "sangles", "dih", "tor", "inv", "imp", "vdw", "bdi"};

int ffdim[14][2] = {{1, 0}, {1, 1}, {1, 1}, {2, 2}, {2, 4}, {2, 3}, {3, 2}, {3, 4}, {4, 3}, {4, 4}, {4, 2}, {4, 3}, {1, 2}, {2, 0}};
// Pot +1 ou (-Pot+1) si special
int ffpot[N_FIELDS][10]={{1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 0, -1, 0, 1, 0, 2, 0, -1},
                        {1, 0, 2, 1, 0, 1, -4, 1, 0, 1},
                        {1, 0, 2, 1, 0, 1, -4, 1, 0, 0},
                        {1, 6, 0, 0, 2, 1, -4, 0, 2, 0},
                        {1, 6, 0, 1, 2, 1, -4, 0, 2, 0},
                        {0, 6, 0, 0, 2, 0, -4, 0, 2, -2},
                        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1},
                        {1, 0, 0, 1, 0, 1, 0, 1, 0, 1}};


/*!
  \fn int get_pdim(int fid, int prop, int col)

  \brief retrieve number of parameters for property in force field database

  \param fid the force field id
  \param prop the type of property
  \param col the column id
*/
int get_pdim(int fid, int prop, int col)
{
  if (prop == 0) return field_dim[0];
  if (field_dim[prop] == 0) return 0;
  if (prop == 6 && col == 1 && (fid >= CHARMM22P && fid <= CHARMMSI)) return 4;
  if (prop == 12 && col == 1 && ((fid >= CHARMM22P && fid <= CHARMMSI) || (fid >= OPLSAAP))) return 4;
  return ffdim[prop][col];
}

/*!
  \fn int get_fkey(int fid, int prop, int col)

  \brief retrieve key for property in force field database

  \param fid the force field id
  \param prop the property id
  \param col the column id
*/
int get_fkey(int fid, int prop, int col)
{
  return ffdim[prop][col];
}

#ifndef USE_ATOMS
void allocate_field_data (int * objects, int * dim,
                       char * atoms[objects[0]][dim[0]], char * equi_a[objects[1]][dim[1]], char * equi_b[objects[2]][dim[2]],
                       char * bonds[objects[3]][dim[3]], char * q_bonds[objects[4]][dim[4]], char * m_bonds[objects[5]][dim[5]],
                       char * angles[objects[6]][dim[6]], char * q_angles[objects[7]][dim[7]],
                       char * dihedrals[objects[8]][dim[8]], char * q_dihedrals[objects[9]][dim[9]],
                       char * inversions[objects[10]][dim[10]], char * impropers[objects[11]][dim[11]],
                       char * vdw[objects[12]][dim[12]], char * bond_inc[objects[13]][dim[13]])

{
  int h, i;
  field_objects = objects;
  field_dim = dim;
  field_atoms = g_malloc0(field_objects[0]*sizeof*field_atoms);
  for (i=0; i<field_objects[0]; i++)
  {
    field_atoms[i] = g_malloc0(field_dim[0]*sizeof*field_atoms[i]);
    field_atoms[i] = atoms[i];
  }
  for (h=0; h<2; h++)
  {
    if (field_objects[1+h])
    {
      field_equi[h] = g_malloc0(field_objects[1+h]*sizeof*field_equi[h]);
      for (i=0; i<field_objects[1+h]; i++)
      {
        field_equi[h][i] = g_malloc0(field_dim[h+1]*sizeof*field_equi[h][i]);
        switch (h)
        {
          case 0:
            field_equi[h][i] = equi_a[i];
            break;
          case 1:
            field_equi[h][i] = equi_b[i];
            break;
        }
      }
    }
  }
  for (h=0; h<3; h++)
  {
    if (field_objects[3+h])
    {
      field_bonds[h] = g_malloc0(field_objects[3+h]*sizeof*field_bonds[h]);
      for (i=0; i<field_objects[3+h]; i++)
      {
        field_bonds[h][i] = g_malloc0(field_dim[3+h]*sizeof*field_bonds[h][i]);
        switch (h)
        {
          case 0:
            field_bonds[h][i] = bonds[i];
            break;
          case 1:
            field_bonds[h][i] = q_bonds[i];
            break;
          case 2:
            field_bonds[h][i] = m_bonds[i];
            break;
        }
      }
    }
    else
    {
      field_bonds[h] = NULL;
    }
  }
  for (h=0; h<2; h++)
  {
    if (field_objects[6+h])
    {
      field_angles[h] = g_malloc0(field_objects[6+h]*sizeof*field_angles[h]);
      for (i=0; i<field_objects[6+h]; i++)
      {
        field_angles[h][i] = g_malloc0(field_dim[6+h]*sizeof*field_angles[h][i]);
        switch (h)
        {
          case 0:
            field_angles[h][i] = angles[i];
            break;
          case 1:
            field_angles[h][i] = q_angles[i];
            break;
        }
      }
    }
    else
    {
      field_angles[h] = NULL;
    }
  }
  for (h=0; h<2; h++)
  {
    if (field_objects[8+h])
    {
      field_dihedrals[h] = g_malloc0(field_objects[8+h]*sizeof*field_dihedrals[h]);
      for (i=0; i<field_objects[8+h]; i++)
      {
        field_dihedrals[h][i] = g_malloc0(field_dim[8+h]*sizeof*field_dihedrals[h][i]);
        switch (h)
        {
          case 0:
            field_dihedrals[h][i] = dihedrals[i];
            break;
          case 1:
            field_dihedrals[h][i] = q_dihedrals[i];
            break;
        }
      }
    }
    else
    {
      field_dihedrals[h] = NULL;
    }
  }

  if (field_objects[10])
  {
    field_inversions = g_malloc0(field_objects[10]*sizeof*field_inversions);
    for (i=0; i<field_objects[10]; i++)
    {
      field_inversions[i] = g_malloc0(field_dim[10]*sizeof*field_inversions[i]);
      field_inversions[i] = inversions[i];
    }
  }
  else
  {
    field_inversions = NULL;
  }


  if (field_objects[11])
  {
    field_impropers = g_malloc0(field_objects[11]*sizeof*field_impropers);
    for (i=0; i<field_objects[11]; i++)
    {
      field_impropers[i] = g_malloc0(field_dim[11]*sizeof*field_impropers[i]);
      field_impropers[i] = impropers[i];
    }
  }
  else
  {
    field_impropers = NULL;
  }


  if (field_objects[12])
  {
    field_vdw = g_malloc0(field_objects[12]*sizeof*field_vdw);
    for (i=0; i<field_objects[12]; i++)
    {
      field_vdw[i] = g_malloc0(field_dim[12]*sizeof*field_vdw[i]);
      field_vdw[i] = vdw[i];
    }
  }
  else
  {
    field_vdw = NULL;
  }
}

/*!
  \fn void associate_pointers_to_field_data (int id)

  \brief setup force field pointers

  \param id the force field id
*/
void associate_pointers_to_field_data (int id)
{
  switch (id)
  {
    case AMBER94:
      allocate_field_data ((int *)amber94_objects, (int *)amber94_dim, amber94_atoms, amber94_equi, NULL,
                           amber94_bonds, NULL, NULL, amber94_angles, NULL,
                           amber94_dihedrals, NULL, NULL, amber94_impropers, amber94_vdw, NULL);
      break;
    case AMBER96:
      allocate_field_data ((int *)amber96_objects, (int *)amber96_dim, amber96_atoms, amber96_equi, NULL,
                           amber96_bonds, NULL, NULL, amber96_angles, NULL,
                           amber96_dihedrals, NULL, NULL, amber96_impropers, amber96_vdw, NULL);
      break;
    case AMBER98:
      allocate_field_data ((int *)amber98_objects, (int *)amber98_dim, amber98_atoms, amber98_equi, NULL,
                           amber98_bonds, NULL, NULL, amber98_angles, NULL,
                           amber98_dihedrals, NULL, NULL, amber98_impropers, amber98_vdw, NULL);
      break;
    case AMBER99:
      allocate_field_data ((int *)amber99_objects, (int *)amber99_dim, amber99_atoms, amber99_equi, NULL,
                           amber99_bonds, NULL, NULL, amber99_angles, NULL,
                           amber99_dihedrals, NULL, NULL, amber99_impropers, amber99_vdw, NULL);
      break;
    case CHARMM22P:
      allocate_field_data ((int *)charmm22_prot_objects, (int *)charmm22_prot_dim, charmm22_prot_atoms, NULL, NULL,
                           charmm22_prot_bonds, NULL, NULL, charmm22_prot_angles, NULL,
                           charmm22_prot_dihedrals, NULL, NULL, charmm22_prot_impropers, charmm22_prot_vdw, NULL);
      break;
    case CHARMM22M:
      allocate_field_data ((int *)charmm22_prot_metals_objects, (int *)charmm22_prot_metals_dim, charmm22_prot_metals_atoms, NULL, NULL,
                           charmm22_prot_metals_bonds, NULL, NULL, charmm22_prot_metals_angles, NULL,
                           charmm22_prot_metals_dihedrals, NULL, NULL, charmm22_prot_metals_impropers, charmm22_prot_metals_vdw, NULL);
      break;
    case CHARMM35E:
      // No impropers
      allocate_field_data ((int *)charmm35_ethers_objects, (int *)charmm35_ethers_dim, charmm35_ethers_atoms, NULL, NULL,
                           charmm35_ethers_bonds, NULL, NULL, charmm35_ethers_angles, NULL,
                           charmm35_ethers_dihedrals, NULL, NULL, NULL, charmm35_ethers_vdw, NULL);
      break;
    case CHARMM36C:
      allocate_field_data ((int *)charmm36_carb_objects, (int *)charmm36_carb_dim, charmm36_carb_atoms, NULL, NULL,
                           charmm36_carb_bonds, NULL, NULL, charmm36_carb_angles, NULL,
                           charmm36_carb_dihedrals, NULL, NULL, charmm36_carb_impropers, charmm36_carb_vdw, NULL);
      break;
    case CHARMM36G:
      allocate_field_data ((int *)charmm36_cgenff_objects, (int *)charmm36_cgenff_dim, charmm36_cgenff_atoms, NULL, NULL,
                           charmm36_cgenff_bonds, NULL, NULL, charmm36_cgenff_angles, NULL,
                           charmm36_cgenff_dihedrals, NULL, NULL, charmm36_cgenff_impropers, charmm36_cgenff_vdw, NULL);
      break;
    case CHARMM36L:
      allocate_field_data ((int *)charmm36_lipid_objects, (int *)charmm36_lipid_dim, charmm36_lipid_atoms, NULL, NULL,
                           charmm36_lipid_bonds, NULL, NULL, charmm36_lipid_angles, NULL,
                           charmm36_lipid_dihedrals, NULL, NULL, charmm36_lipid_impropers, charmm36_lipid_vdw, NULL);
      break;
    case CHARMM36N:
      allocate_field_data ((int *)charmm36_na_objects, (int *)charmm36_na_dim, charmm36_na_atoms, NULL, NULL,
                           charmm36_na_bonds, NULL, NULL, charmm36_na_angles, NULL,
                           charmm36_na_dihedrals, NULL, NULL, charmm36_na_impropers, charmm36_na_vdw, NULL);
      break;
    case CHARMM36P:
      allocate_field_data ((int *)charmm36_prot_objects, (int *)charmm36_prot_dim, charmm36_prot_atoms, NULL, NULL,
                           charmm36_prot_bonds, NULL, NULL, charmm36_prot_angles, NULL,
                           charmm36_prot_dihedrals, NULL, NULL, charmm36_prot_impropers, charmm36_prot_vdw, NULL);
      break;
    case CHARMM36M:
      allocate_field_data ((int *)charmm36m_prot_objects, (int *)charmm36m_prot_dim, charmm36m_prot_atoms, NULL, NULL,
                           charmm36m_prot_bonds, NULL, NULL, charmm36m_prot_angles, NULL,
                           charmm36m_prot_dihedrals, NULL, NULL, charmm36m_prot_impropers, charmm36m_prot_vdw, NULL);
      break;
    case CHARMMSI:
      // No impropers
      allocate_field_data ((int *)charmm_silicates_objects, (int *)charmm_silicates_dim, charmm_silicates_atoms, NULL, NULL,
                           charmm_silicates_bonds, NULL, NULL, charmm_silicates_angles, NULL,
                           charmm_silicates_dihedrals, NULL, NULL, NULL, charmm_silicates_vdw, NULL);
      break;
    case CVFF:
      allocate_field_data ((int *)CVFF_objects, (int *)CVFF_dim, CVFF_atoms, CVFF_equivalence_auto, CVFF_equivalence,
                           CVFF_bonds_auto, NULL, CVFF_morse_bonds, CVFF_angles_auto, NULL,
                           CVFF_torsions_auto, NULL, NULL, CVFF_impropers, CVFF_vdw, NULL);
      break;
    case CVFF_AUG:
      allocate_field_data ((int *)CVFF_aug_objects, (int *)CVFF_aug_dim, CVFF_aug_atoms, CVFF_aug_equivalence_auto, CVFF_aug_equivalence,
                           CVFF_aug_bonds_auto, NULL, CVFF_aug_morse_bonds, CVFF_aug_angles_auto, NULL,
                           CVFF_aug_torsions_auto, NULL, NULL, CVFF_aug_impropers, CVFF_aug_vdw, NULL);
      break;
    case CFF91:
      allocate_field_data ((int *)CFF91_objects, (int *)CFF91_dim, CFF91_atoms, CFF91_equivalence_auto, CFF91_equivalence,
                           CFF91_bonds_auto, CFF91_bonds, NULL, CFF91_angles_auto, CFF91_angles,
                           CFF91_torsions_auto, CFF91_torsions, CFF91_inversions, NULL, CFF91_vdw, NULL);
      break;
    case PCFF:
      allocate_field_data ((int *)PCFF_objects, (int *)PCFF_dim, PCFF_atoms, PCFF_equivalence_auto, PCFF_equivalence,
                           PCFF_bonds_auto, PCFF_bonds, NULL, PCFF_angles_auto, PCFF_angles,
                           PCFF_torsions_auto, PCFF_torsions, PCFF_inversions, NULL, PCFF_vdw, NULL);
      break;
    case COMPASS:
      allocate_field_data ((int *)Compass_objects, (int *)Compass_dim, Compass_atoms, NULL, Compass_equivalence,
                           NULL, Compass_bonds, NULL, NULL, Compass_angles,
                           NULL, Compass_torsions, Compass_inversions, NULL, Compass_vdw, NULL);
      break;
    case OPLSAAP:
      allocate_field_data ((int *)OPLSAAM_objects, (int *)OPLSAAM_dim, OPLSAAM_atoms, NULL, NULL,
                           OPLSAAM_bonds, NULL, NULL, OPLSAAM_angles, NULL,
                           OPLSAAM_dihedrals, NULL, NULL,  OPLSAAM_impropers, OPLSAAM_vdw, NULL);
      break;
    case OPLSAAR:
      allocate_field_data ((int *)OPLSAAR_objects, (int *)OPLSAAR_dim, OPLSAAR_atoms, NULL, NULL,
                           OPLSAAR_bonds, NULL, NULL, OPLSAAR_angles, NULL,
                           OPLSAAR_dihedrals, NULL, NULL,  OPLSAAR_impropers, OPLSAAR_vdw, NULL);
      break;
    default:
      field_objects = NULL;
      field_atoms = NULL;
      field_bonds[0] = field_bonds[1] = field_bonds[2]= NULL;
      field_angles[0] = field_angles[1] = NULL;
      field_dihedrals[0] = field_dihedrals[1] = NULL;
      field_inversions = NULL;
      field_impropers = NULL;
      field_vdw = NULL;
      break;
  }
}
#endif

/*!
  \fn int find_atom_id (int print, char * keyw)

  \brief find atom id in force field database

  \param print print debug information ?
  \param keyw the key entry for atom in the force field database
*/
int find_atom_id (int print, char * keyw)
{
  int i;
  for (i=0; i<field_objects[0]; i++)
  {
    if (g_strcmp0 (keyw, field_atoms[i][2]) == 0) return i;
  }
  if (g_strcmp0 (keyw, "X") == 0) return -1;
  if (g_strcmp0 (keyw, "*") == 0) return -1;
#ifdef DEBUG
  if (print) g_debug ("ID:: -10:: key= '%s'", keyw);
#endif
  return -10;
}

/*!
  \fn gchar * find_atom_key (int fid, int prop, char * keyw)

  \brief retrieve property key in force field database for atom

  \param fid the force field id
  \param prop the type of property
  \param keyw the key entry for atom in the force field database
*/
gchar * find_atom_key (int fid, int prop, char * keyw)
{
  if (fid > CHARMMSI && fid < OPLSAAP)
  {
    if (find_atom_id(0, keyw) == -10)
    {
      int i;
      for (i=0; i<field_objects[2]; i++)
      {
        if (g_strcmp0 (keyw, field_equi[1][i][prop]) == 0) return field_equi[1][i][0];
      }
      for (i=0; i<field_objects[1]; i++)
      {
        if (g_strcmp0 (keyw, field_equi[0][i][prop+prop/2+prop/5]) == 0) return field_equi[0][i][0];
      }
      if (prop > 2)
      {
        for (i=0; i<field_objects[1]; i++)
        {
          if (g_strcmp0 (keyw, field_equi[0][i][prop+prop/2+prop/5+1]) == 0) return field_equi[0][i][0];
        }
      }
#ifdef DEBUG
      g_debug ("Nothing found in equi:: prop= %d, keyw= %s", prop, keyw);
#endif
    }
    return keyw;
  }
  else
  {
    return keyw;
  }
}

/*!
  \fn int find_atom_z (char * keyw)

  \brief find atom Z

  \param keyw the element name
*/
int find_atom_z (char * keyw)
{
  int i, j;
  for (i=0; i<field_objects[0]; i++)
  {
    if (g_strcmp0 (keyw, field_atoms[i][2]) == 0)
    {
      for (j=1; j<120; j++)
      {
        if (g_strcmp0 (periodic_table_info[j].lab, field_atoms[i][0]) == 0) return periodic_table_info[j].Z;
        //if (g_strcmp0 (exact_name(periodic_table_info[j].lab), exact_name(field_atoms[i][0])) == 0) return periodic_table_info[j].Z;
      }
    }
  }
  if (g_strcmp0 (keyw, "X") == 0) return -1;
  if (g_strcmp0 (keyw, "*") == 0) return -1;
  if (g_strcmp0 (keyw, "L") == 0 || g_strcmp0 (keyw, "lp") == 0 || g_strcmp0 (keyw, "LP") == 0 || g_strcmp0 (keyw, "LPH") == 0) return -2;
  if (g_strcmp0 (keyw, "EP") == 0) return -3;
  if (g_strcmp0 (keyw, "DUM") == 0) return -4;
#ifdef DEBUG
  g_debug ("Z:: -10:: key= '%s'", keyw);
#endif
  return -10;
}

/*!
  \fn void print_object_dim_and_key_tables (int fid)

  \brief print tables dimensions and keys to file

  \param fid the force field id
*/
void print_object_dim_and_key_tables (int fid)
{
  gchar * nodes[11]={"atoms", "bonds-h", "bonds-q", "bonds-m", "angles-h", "angles-q",
                     "dihedrals-c", "dihedrals-ccc", "impropers", "inversions", "non-bonded"};
  fprintf (fp, "  <ff-data>\n");
  int i, j, k, l, n, m;
  k = -1;
  for (i=0; i<13; i++)
  {
    if (i != 1 && i != 2)
    {
      k ++;
      j = (i == 10) ? 11 : (i == 11) ? 10 : i;
      if (i == 0)
      {
        fprintf (fp, "    <%s dim=\"%d\">%d</%s>\n", nodes[k], (field_objects[j]) ? get_pdim(fid, j, 1) : 0, field_objects[j], nodes[k]);
      }
      else if (i < 12)
      {
        fprintf (fp, "    <%s dim=\"%d\" pot=\"%d\">%d</%s>\n", nodes[k], (field_objects[j]) ? get_pdim(fid, j, 1) : 0, ffpot[fid][k-1], field_objects[j], nodes[k]);
      }
      else
      {
        l = 0;
        if (fid <= AMBER99)
        {
          for (n=0; n<field_objects[1]; n++)
          {
            for (m=1; m<field_dim[1]; m++)
            {
              if(g_strcmp0 (field_equi[0][n][m], " ") != 0)
              {
                l ++;
              }
            }
          }
        }
        fprintf (fp, "    <%s dim=\"%d\" pot=\"%d\">%d</%s>\n", nodes[k], (field_objects[j]) ? get_pdim(fid, j, 1) : 0, ffpot[fid][k-1], field_objects[j]+l, nodes[k]);
      }
    }
  }
  fprintf (fp, "  </ff-data>\n");
}

field_object_match ** all_data[10];
field_object_match * om_tmp;

/*!
  \fn void set_data (int pid, int obj, int oid, int faid)

  \brief save retrieved force field data

  \param pid the type of structural element
  \param obj the element id
  \param oid the parameter id the force field database
  \param faid the field atom id
*/
void set_data (int pid, int obj, int oid, int faid)
{
  if (all_data[pid][faid] == NULL)
  {
    all_data[pid][faid] = g_malloc0(sizeof*all_data[pid][faid]);
    om_tmp = all_data[pid][faid];
  }
  else
  {
    om_tmp -> next = g_malloc0(sizeof*om_tmp -> next);
    om_tmp -> next -> id = om_tmp -> id + 1;
    om_tmp -> next -> prev = om_tmp;
  }
  om_tmp -> obj = obj;
  om_tmp -> oid = oid;
}

/*!
  \fn int get_z (int faid)

  \brief get field atom Z

  \param faid the field atom id
*/
int get_z (int faid)
{
  int i;
  for (i=0; i<120; i++)
  {
    if (g_strcmp0 (periodic_table_info[i].lab, field_atoms[faid][0]) == 0) return periodic_table_info[i].Z;
  }
  return -1;
}

/*!
  \fn int get_atoms (int z)

  \brief retrieve all field atom(s) with matching Z

  \param z the target Z
*/
int get_atoms (int z)
{
  int i, j;
  j = 0;
  for (i=0; i<field_objects[0]; i++)
  {
    if (get_z(i) == z)
    {
      set_data (0, 0, j, z);
      j ++;
    }
  }
  return j;
}

/*!
  \fn int get_bond (int faid, int bid)

  \brief retrieve all bonding parameter(s) for this field atom

  \param faid the field atom id
  \param bid the structural element id
*/
int get_bond (int faid, int bid)
{
  int i, j;
  j = 0;
  if (field_objects[3+bid])
  {
    for (i=0; i<field_objects[3+bid]; i++)
    {
      if ((g_strcmp0 (field_bonds[bid][i][0], field_atoms[faid][2]) == 0) ||  (g_strcmp0 (field_bonds[bid][i][1], field_atoms[faid][2]) == 0))
      {
        set_data (1, bid, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_angle (int faid, int aid)

  \brief retrieve all angle parameter(s) for this field atom

  \param faid the field atom id
  \param aid the structural element id
*/
int get_angle (int faid, int aid)
{
  int i, j;
  j = 0;
  if (field_objects[6+aid])
  {
    for (i=0; i<field_objects[6+aid]; i++)
    {
      if ((g_strcmp0 (field_angles[aid][i][0], field_atoms[faid][2]) == 0) ||  (g_strcmp0 (field_angles[aid][i][2], field_atoms[faid][2]) == 0))
      {
        set_data (2, aid, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_apex (int faid, int aid)

  \brief  retrieve all angle apex parameter(s) for this field atom

  \param faid the field atom id
  \param aid the structural element id
*/
int get_apex (int faid, int aid)
{
  int i, j;
  j = 0;
  if (field_objects[6+aid])
  {
    for (i=0; i<field_objects[6+aid]; i++)
    {
      if (g_strcmp0 (field_angles[aid][i][1], field_atoms[faid][2]) == 0)
      {
        set_data (3, aid, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_torsion (int faid, int tid, int a, int b)

  \brief retrieve all torsion parameter(s) for this field atom

  \param faid the field atom id
  \param tid the structural element id
  \param a 1st atom position
  \param b 2nd atom position
*/
int get_torsion (int faid, int tid, int a, int b)
{
  int i, j;
  j = 0;
  if (field_objects[8+tid])
  {
    for (i=0; i<field_objects[8+tid]; i++)
    {
      if ((g_strcmp0 (field_dihedrals[tid][i][a], field_atoms[faid][2]) == 0) ||  (g_strcmp0 (field_dihedrals[tid][i][b], field_atoms[faid][2]) == 0))
      {
        set_data (4+a, tid, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_improper (int faid, int a, int b)

  \brief retrieve all improper parameter(s) for this field atom

  \param faid the field atom id
  \param a 1st atom position
  \param b 2nd atom position
*/
int get_improper (int faid, int a, int b)
{
  int i, j;
  j = 0;
  if (field_objects[11])
  {
    for (i=0; i<field_objects[11]; i++)
    {
      if ((g_strcmp0 (field_impropers[i][a], field_atoms[faid][2]) == 0) ||  (g_strcmp0 (field_impropers[i][b], field_atoms[faid][2]) == 0))
      {
        set_data (6+a, 0, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_vdw (int faid)

  \brief retrieve all VdW parameter(s) for this field atom

  \param faid the field atom id
*/
int get_vdw (int faid)
{
  int i, j;
  j = 0;
  if (field_objects[12])
  {
    for (i=0; i<field_objects[12]; i++)
    {
      if (g_strcmp0 (field_vdw[i][0], field_atoms[faid][2]) == 0)
      {
        set_data (8, 0, i, faid);
        j ++;
      }
    }
  }
  return j;
}

/*!
  \fn int get_bi (int faid)

  \brief NOT USED !

  \param faid
* /
int get_bi (int faid)
{
  int i, j;
  j = 0;
  if (field_objects[13])
  {
    for (i=0; i<field_objects[13]; i++)
    {
      / *if (g_strcmp0 (field_[i][0], field_atoms[faid][2]) == 0)
      {
        set_data (9, 0, i, faid);
        j ++;
      }* /
    }
  }
  return j;
}
*/

/*!
  \fn void field_has_element (int aid)

  \brief retrieve force field parameters for this atom

  \param aid the target atom id
*/
void field_has_element (int aid)
{
  int h, i, j, k, l, m, n, o, p, q, r, s;
  j = k = l = m = n = o = p = q = r = s = 0;
  for (i=0; field_objects[0]; i++)
  {
    if (g_strcmp0 (periodic_table_info[aid].lab, field_atoms[i][0]) == 0)
    {
      j ++;
      for (h=0; h<3; h++) k += get_bond (i, h);
      for (h=0; h<2; h++) l += get_angle(i, h);
      for (h=0; h<2; h++) m += get_apex(i, h);
      for (h=0; h<2; h++) n += get_torsion (i, h, 0, 3);
      for (h=0; h<2; h++) o += get_torsion (i, h, 1, 2);
      p += get_improper (i, 0, 3);
      q += get_improper (i, 1, 2);
      r += get_vdw(i);
      //s += get_bi(i);
    }
  }
  // Number of objects by atomic number, 10 columns
  // 0 = Elem, 1 = Tot bd, 2 = Tot ang AC, 3 = Tot ang apex, 4 = Tot dih 14, 5 Tot dih 23, 6 Tot imp ab, 7 = Tot imp cd, 8 = Tot wdv, 9 = Tot BI
  fprintf (fp, "{%3i,%3i,%3i,%3i,%3i,%3i,%3i,%3i,%3i,%3i}", j, k, l, m, n, o, p, q, r, s);
}

/*!
  \fn void print_atom_table (int fid)

  \brief print force field atom data to file

  \param fid the force field id
*/
void print_atom_table (int fid)
{
  int i, j;
  gchar * nodes[4]={"label", "mass", "key", "info"};
    fprintf (fp, "  <atoms>\n");
  for (i=0; i<field_objects[0]; i++)
  {
    fprintf (fp, "    <at");
    for (j=0; j<4; j++)
    {
       fprintf (fp, " %s=\"%s\"", nodes[j], field_atoms[i][j]);
    }
    fprintf (fp, "/>\n");
  }
  fprintf (fp, "  </atoms>\n");
}

typedef struct f_bond f_bond;
struct f_bond
{
  int a;
  int b;
  float v[2];
  f_bond * next;
  f_bond * prev;
};

f_bond * the_bonds[2];
f_bond * tmpbd;

/*!
  \fn gboolean not_done (int eid, int a, int b)

  \brief was this case already considered ?

  \param eid equivalence id (not used for the time being)
  \param a 1st atom id
  \param b 2nd atom id
*/
gboolean not_done (int eid, int a, int b)
{
  tmpbd = the_bonds[eid];
  while (tmpbd)
  {
    if (tmpbd -> a == a && tmpbd -> b == b) return FALSE;
    if (tmpbd -> a == b && tmpbd -> b == a) return FALSE;
    tmpbd = tmpbd -> next;
  }
  return TRUE;
}

/*!
  \fn void print_this_bond (int eid, int h, int fid, int inum, char * at_a, char * at_b, char ** the_bond)

  \brief print this bond to file

  \param eid equivalence id (not used for the time being)
  \param h the bond type
  \param fid the force field id
  \param inum the structural element id
  \param at_a 1st atom name
  \param at_b 2nd atom name
  \param the_bond the force field parameter(s) for this bond
*/
void print_this_bond (int eid, int h, int fid, int inum, char * at_a, char * at_b, char ** the_bond)
{
  int j;
  gchar * node[3] = {"bd-h", "bd-q", "bd-m"};
  gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 2, at_a));
  gchar * bbb = g_strdup_printf ("%s", find_atom_key(fid, 2, at_b));
  if (eid < 0 || not_done (eid, find_atom_id (1, aaa), find_atom_id (1, bbb)))
  {
    if (eid > -1)
    {
      if (the_bonds[eid])
      {
        tmpbd = the_bonds[eid];
        while (tmpbd -> next) tmpbd = tmpbd -> next;
        tmpbd -> next = g_malloc0(sizeof*tmpbd -> next);
        tmpbd -> next -> prev = tmpbd;
        tmpbd = tmpbd -> next;
      }
      else
      {
        the_bonds[eid] = g_malloc0(sizeof*the_bonds[eid]);
        tmpbd = the_bonds[eid];
      }
      tmpbd -> a = find_atom_id (0, aaa);
      tmpbd -> b = find_atom_id (0, bbb);
      tmpbd -> v[0] = string_to_double ((gpointer)the_bond[2]);
      tmpbd -> v[1] = string_to_double ((gpointer)the_bond[3]);
    }
    fprintf (fp, "    <%s a=\"%d\" b=\"%d\" z_a=\"%d\" z_b=\"%d\"",
                       node[h], find_atom_id (0, aaa), find_atom_id (0, bbb),
                                find_atom_z (aaa), find_atom_z (bbb));
    switch (h)
    {
      case 0:
        if (fid == CVFF || fid == CVFF_AUG || fid == CFF91 || fid == PCFF)
        {
          fprintf (fp, " K=\"%f\" R_zero=\"%f\"", string_to_double ((gpointer)the_bond[3]), string_to_double ((gpointer)the_bond[2]));
        }
        else
        {
          fprintf (fp, " K=\"%f\" R_zero=\"%f\"", string_to_double ((gpointer)the_bond[2]), string_to_double ((gpointer)the_bond[3]));
        }
        j = 4;
        break;
      case 1:
        fprintf (fp, " K=\"%f\" R_zero=\"%f\" KK=\"%f\" KKK=\"%f\"", string_to_double ((gpointer)the_bond[3]), string_to_double ((gpointer)the_bond[2]), string_to_double ((gpointer)the_bond[4]), string_to_double ((gpointer)the_bond[5]));
        j = 6;
        break;
      case 2:
        fprintf (fp, " D=\"%f\" R_zero=\"%f\" Alpha=\"%f\"", string_to_double ((gpointer)the_bond[3]), string_to_double ((gpointer)the_bond[2]), string_to_double ((gpointer)the_bond[4]));
        j = 5;
        break;
    }
    if (field_dim[inum+h] > j)
    {
      fprintf (fp, " info=\"%s\"", the_bond[j]);
    }
    fprintf (fp, "/>\n");
  }
  else if (eid > -1)
  {
    g_debug ("Done:: tmpbd -> a= %d, tmpbd -> b= %d, tmpbd -> v[0]= %f, tmpbd -> v[1]= %f, this.v[0]= %f, this.v[1]=%f",
                     tmpbd -> a, tmpbd -> b, tmpbd -> v[0], tmpbd -> v[1], string_to_double ((gpointer)the_bond[2]), string_to_double ((gpointer)the_bond[3]));
  }
  g_free (aaa);
  g_free (bbb);
}

/*!
  \fn void print_bond_table (int fid, int inum)

  \brief print force field bond table to file

  \param fid the force field id
  \param inum the structural element id
*/
void print_bond_table (int fid, int inum)
{
  int h, i;//, j, k, l, m;
  gchar * bode[3] = {"bonds-h", "bonds-q", "bonds-m"};
  gchar * desc[3] = {"Quadratic bonds:\n\n    V(R) = K x (R - R0)^2\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 A^-2)\n      - R = bond distance (A)\n      - R0 = equilibrium distance (A)\n",
                     "Quartic bonds:\n\n    V(R) = K x (R - R0)^2  +  K' x (R - R0)^3  +  K'' x (R - R0)^4\n"
                     "    With:\n      - K, K' and K'' = nth order spring constants (kcal mol^-1 A^-2)\n"
                     "      - R = bond distance (A)\n      - R0 = equilibrium distance (A)",
                     "Morse bonds:\n\n    V(R) = D x (1 - exp(-ALPHA x (R - R0)))^2\n"
                     "    With:\n      - D = well depth\n      - ALPHA = well width\n      - R = bond distance\n      - R0 = equilibrium distance (A)"};
  for (h=0; h<3; h++)
  {
    the_bonds[0] = the_bonds[1] = NULL;
    if (field_objects[inum+h])
    {
      fprintf (fp, "  <%s>\n", bode[h]);
      fprintf (fp, "  <!-- %s   -->\n", desc[h]);
      /*if (fid > CHARMMSI && fid < OPLSAAP)
      {
        for (i=0; i<field_objects[inum+h]; i++)
        {
          if (find_atom_id (1, field_bonds[h][i][0]) == -10 && find_atom_id (1, field_bonds[h][i][1]) == -10)
          {
            for (j=1; j<3; j++)
            {
              for (k=0; k<field_objects[j]; k++)
              {
                if (g_strcmp0 (field_equi[j-1][k][4-j], field_bonds[h][i][0]) == 0)
                {
                  l = (g_strcmp0 (field_bonds[h][i][0], field_bonds[h][i][1]) == 0) ? k : 0;
                  for (m=l; m<field_objects[j]; m++)
                  {
                    if (g_strcmp0 (field_equi[j-1][m][4-j], field_bonds[h][i][1]) == 0)
                    {
                      print_this_bond (j-1, h, fid, inum, field_equi[j-1][k][0], field_equi[j-1][m][0], field_bonds[h][i]);
                    }
                  }
                }
              }
            }
          }
          else if (find_atom_id (1, field_bonds[h][i][0]) == -10)
          {
            for (j=1; j<3; j++)
            {
              for (k=0; k<field_objects[j]; k++)
              {
                if (g_strcmp0 (field_equi[j-1][k][4-j], field_bonds[h][i][0]) == 0)
                {
                  print_this_bond (j-1, h, fid, inum, field_equi[j-1][k][0], field_bonds[h][i][1], field_bonds[h][i]);
                }
              }
            }
          }
          else if (find_atom_id (1, field_bonds[h][i][1]) == -10)
          {
            for (j=1; j<3; j++)
            {
              for (k=0; k<field_objects[j]; k++)
              {
                if (g_strcmp0 (field_equi[j-1][k][4-j], field_bonds[h][i][1]) == 0)
                {
                  print_this_bond (j-1, h, fid, inum, field_bonds[h][i][0], field_equi[j-1][k][0], field_bonds[h][i]);
                }
              }
            }
          }
          else
          {
            print_this_bond (-1, h, fid, inum, field_bonds[h][i][0], field_bonds[h][i][1], field_bonds[h][i]);
          }
          for (j=0; j<2; j++)
          {
            if (the_bonds[j])
            {
              tmpbd = the_bonds[j];
              while (tmpbd -> next)
              {
                tmpbd = tmpbd -> next;
                g_free (tmpbd -> prev);
              }
              g_free (tmpbd);
              the_bonds[j] = NULL;
            }
          }
        }
        fprintf (fp, "  </%s>\n", bode[h]);
      }
      else*/
      {
        for (i=0; i<field_objects[inum+h]; i++)
        {
          print_this_bond (-1, h, fid, inum, field_bonds[h][i][0], field_bonds[h][i][1], field_bonds[h][i]);
        }
        fprintf (fp, "  </%s>\n", bode[h]);
      }
    }
    /*if (the_bonds[0] || the_bonds[1])
    {
      for (i=0; i<2; i++)
      {
        if (the_bonds[i])
        {
          tmpbd = the_bonds[i];
          while (tmpbd -> next)
          {
            tmpbd = tmpbd -> next;
            g_free (tmpbd -> prev);
          }
          g_free (tmpbd);
        }
      }
    }*/
  }
}

typedef struct f_angl f_angl;
struct f_angl
{
  int a;
  int b;
  int c;
  float v[2];
  f_angl * next;
  f_angl * prev;
};

f_angl * the_angles[2];
f_angl * tmpan;

/*!
  \fn gboolean not_done_an (int eid, int a, int b, int c)

  \brief was this case already considered ?

  \param eid equivalence id (not used for the time being)
  \param a 1st atom id
  \param b 2nd atom id
  \param c 3rd atom id
*/
gboolean not_done_an (int eid, int a, int b, int c)
{
  tmpan = the_angles[eid];
  while (tmpan)
  {
    if (tmpan -> a == a && tmpan -> b == b && tmpan -> c == c) return FALSE;
    if (tmpan -> a == c && tmpan -> b == b && tmpan -> c == a) return FALSE;
    tmpan = tmpan -> next;
  }
  return TRUE;
}

/*!
  \fn void print_this_angle (int eid, int h, int fid, int inum, int ub, char * at_a, char * at_b, char * at_c, char ** the_angle)

  \brief print this angle to file

  \param eid equivalence id (not used for the time being)
  \param h 0 = angle, 1 = angle restraints
  \param fid the force field id
  \param inum the structural element id
  \param ub Urey-Bradley parameter for the CHARRM FF, 2 for CHARMM FF, 0 otherwise
  \param at_a 1st atom name
  \param at_b 2nd atom name
  \param at_c 3rd atom name
  \param the_angle the force field parameter(s) for this angle
*/
void print_this_angle (int eid, int h, int fid, int inum, int ub, char * at_a, char * at_b, char * at_c, char ** the_angle)
{
  int j;
  gchar * node[2] = {"ang-h", "ang-q"};
  gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 3, at_a));
  gchar * bbb = g_strdup_printf ("%s", find_atom_key(fid, 3, at_b));
  gchar * ccc = g_strdup_printf ("%s", find_atom_key(fid, 3, at_c));
  if (eid < 0 || not_done_an (eid, find_atom_id (1, aaa), find_atom_id (1, bbb), find_atom_id (1, ccc)))
  {
    if (eid > -1)
    {
      if (the_angles[eid])
      {
        tmpan = the_angles[eid];
        while (tmpan -> next) tmpan = tmpan -> next;
        tmpan -> next = g_malloc0(sizeof*tmpan -> next);
        tmpan -> next -> prev = tmpan;
        tmpan = tmpan -> next;
      }
      else
      {
        the_angles[eid] = g_malloc0(sizeof*the_angles[eid]);
        tmpan = the_angles[eid];
      }
      tmpan -> a = find_atom_id (0, aaa);
      tmpan -> b = find_atom_id (0, bbb);
      tmpan -> c = find_atom_id (0, ccc);
      tmpan -> v[0] = string_to_double ((gpointer)the_angle[3]);
      tmpan -> v[1] = string_to_double ((gpointer)the_angle[4]);
    }
    fprintf (fp, "    <%s a=\"%d\" b=\"%d\" c=\"%d\" z_a=\"%d\" z_b=\"%d\" z_c=\"%d\"",
             node[h], find_atom_id (0, aaa), find_atom_id (0, bbb), find_atom_id (0, ccc),
                      find_atom_z (aaa), find_atom_z (bbb), find_atom_z (ccc));
    switch (h)
    {
      case 0:
        if (fid >= CVFF && fid < COMPASS)
        {
          fprintf (fp, " K=\"%f\" Theta_zero=\"%f\"", string_to_double ((gpointer)the_angle[4]), string_to_double ((gpointer)the_angle[3]));
        }
        else
        {
          fprintf (fp, " K=\"%f\" Theta_zero=\"%f\"", string_to_double ((gpointer)the_angle[3]), string_to_double ((gpointer)the_angle[4]));
        }
        if (ub)
        {
          fprintf (fp, " Kub=\"%f\" S_zero=\"%f\"", string_to_double ((gpointer)the_angle[5]), string_to_double ((gpointer)the_angle[6]));
        }
        j = (ub) ? 7 : 5;
        break;
      case 1:
        fprintf (fp, " K=\"%f\" Theta_zero=\"%f\" KK=\"%f\" KKK=\"%f\"", string_to_double ((gpointer)the_angle[4]), string_to_double ((gpointer)the_angle[3]),
                                                                         string_to_double ((gpointer)the_angle[5]), string_to_double ((gpointer)the_angle[6]));
        j = 7;
        break;
    }
    if (field_dim[inum+h] > j)
    {
      fprintf (fp, " info=\"%s\"", the_angle[j]);
    }
    fprintf (fp, "/>\n");
  }
  else if (eid > -1)
  {
    g_debug ("Done:: tmpan -> a= %d, tmpan -> b= %d, tmpan -> c= %d, tmpan -> v[0]= %f, tmpan -> v[1]= %f, this.v[0]= %f, this.v[1]=%f",
                     tmpan -> a, tmpan -> b, tmpan -> c, tmpan -> v[0], tmpan -> v[1], string_to_double ((gpointer)the_angle[3]), string_to_double ((gpointer)the_angle[4]));
  }
  g_free (aaa);
  g_free (bbb);
  g_free (ccc);
}

/*!
  \fn void print_angle_table (int fid, int inum)

  \brief print angle(s) table to file

  \param fid the field id
  \param inum the object id
*/
void print_angle_table (int fid, int inum)
{
  int h, i;//, j, k, l;
  int ub = (fid > AMBER99 && fid <= CHARMMSI) ? 2 : 0; // Urey-Bradley parameter for the CHARMM FF
  gchar * aode[2] = {"angles-h", "angles-q"};

  gchar * desc[3] = {"Quadratic angles:\n\n    V(Theta) = K x (Theta - Theta0)^2\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 rad^-2)\n      - Theta = angle (°)\n      - Theta0 = equilibrium angle (°)",
                     "Quartic angles:\n\n    V(Theta) = K x (Theta - Theta0)^2  +  K' x (Theta - Theta0)^3  +  K' x (Theta - Theta0)^4\n"
                     "    With:\n      - K, K' and K'' = nth order spring constant (kcal mol^-1 rad^-nth)\n      - Theta = angle (°)\n      - Theta0 = equilibrium angle (°)",
                     "Quadratic angles + Urey-Bradley (UB) parameters:\n\n    V(Theta) = K x (Theta - Theta0)^2\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 rad^-2)\n      - Theta = angle (°)\n      - Theta0 = equilibrium angle(°)\n"
                     "    V(S) = Kub x (R - Rub)^2\n"
                     "    With:\n      - Kub = spring constant (kcal mol^-1 A^-2)\n      - S = distance 1-3 (A)\n      - S0 = equilibrium distance 1-3 (A)"};
  for (h=0; h<2; h++)
  {
    the_angles[0] = the_angles[1] = NULL;
    if (field_objects[inum+h])
    {
      fprintf (fp, "  <%s>\n", aode[h]);
      fprintf (fp, "  <!-- %s   -->\n", desc[h+ub]);
      /*if (fid > CHARMMSI && fid < OPLSAAP)
      {
        for (i=0; i<field_objects[inum+h]; i++)
        {
          if (find_atom_id (1, field_angles[h][i][0]) == -10 && find_atom_id (1, field_angles[h][i][1]) == -10 && find_atom_id (1, field_angles[h][i][2]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[2]; k++)
                {
                  if (g_strcmp0 (field_equi[1][k][3], field_angles[h][i][1]) == 0)
                  {
                    for (l=0; l<field_objects[2]; l++)
                    {
                      if (g_strcmp0 (field_equi[1][l][3], field_angles[h][i][2]) == 0)
                      {
                        print_this_angle (1, h, fid, inum, ub, field_equi[1][j][0], field_equi[1][k][0], field_equi[1][l][0], field_angles[h][i]);
                      }
                    }
                  }
                }
              }
            }
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][4], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[1]; k++)
                {
                  if (g_strcmp0 (field_equi[0][k][5], field_angles[h][i][1]) == 0)
                  {
                    for (l=0; l<field_objects[1]; l++)
                    {
                      if (g_strcmp0 (field_equi[0][l][4], field_angles[h][i][2]) == 0)
                      {
                        print_this_angle (0, h, fid, inum, ub, field_equi[0][j][0], field_equi[0][k][0], field_equi[0][l][0], field_angles[h][i]);
                      }
                    }
                  }
                }
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][0]) == -10 && find_atom_id (1, field_angles[h][i][1]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[2]; k++)
                {
                  if (g_strcmp0 (field_equi[1][k][3], field_angles[h][i][1]) == 0)
                  {
                    print_this_angle (1, h, fid, inum, ub, field_equi[1][j][0], field_equi[1][k][0], field_angles[h][i][2], field_angles[h][i]);
                  }
                }
              }
            }
            // End - Appex
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][4], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[1]; k++)
                {
                  if (g_strcmp0 (field_equi[0][k][5], field_angles[h][i][1]) == 0)
                  {
                    print_this_angle (0, h, fid, inum, ub, field_equi[0][j][0], field_equi[0][k][0], field_angles[h][i][2], field_angles[h][i]);
                  }
                }
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][0]) == -10 && find_atom_id (1, field_angles[h][i][2]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[2]; k++)
                {
                  if (g_strcmp0 (field_equi[1][k][3], field_angles[h][i][2]) == 0)
                  {
                    print_this_angle (1, h, fid, inum, ub, field_equi[1][j][0], field_angles[h][i][1], field_equi[1][k][0], field_angles[h][i]);
                  }
                }
              }
            }
            // End - End
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][4], field_angles[h][i][0]) == 0)
              {
                for (k=0; k<field_objects[1]; k++)
                {
                  if (g_strcmp0 (field_equi[0][k][4], field_angles[h][i][2]) == 0)
                  {
                    print_this_angle (0, h, fid, inum, ub, field_equi[0][j][0], field_angles[h][i][1], field_equi[0][k][0], field_angles[h][i]);
                  }
                }
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][1]) == -10 && find_atom_id (1, field_angles[h][i][2]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][1]) == 0)
              {
                for (k=0; k<field_objects[2]; k++)
                {
                  if (g_strcmp0 (field_equi[1][k][3], field_angles[h][i][2]) == 0)
                  {
                    print_this_angle (1, h, fid, inum, ub, field_angles[h][i][0], field_equi[1][j][0], field_equi[1][k][0], field_angles[h][i]);
                  }
                }
              }
            }
            // Appex - End
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][5], field_angles[h][i][1]) == 0)
              {
                for (k=0; k<field_objects[1]; k++)
                {
                  if (g_strcmp0 (field_equi[0][k][4], field_angles[h][i][2]) == 0)
                  {
                    print_this_angle (0, h, fid, inum, ub, field_angles[h][i][0], field_equi[0][j][0], field_equi[0][k][0], field_angles[h][i]);
                  }
                }
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][0]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][0]) == 0)
              {
                print_this_angle (1, h, fid, inum, ub, field_equi[1][j][0], field_angles[h][i][1], field_angles[h][i][2], field_angles[h][i]);
              }
            }
            // End
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][4], field_angles[h][i][0]) == 0)
              {
                print_this_angle (0, h, fid, inum, ub, field_equi[0][j][0], field_angles[h][i][1], field_angles[h][i][2], field_angles[h][i]);
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][1]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][1]) == 0)
              {
                print_this_angle (1, h, fid, inum, ub, field_angles[h][i][0], field_equi[1][j][0], field_angles[h][i][2], field_angles[h][i]);
              }
            }
            // Appex
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][5], field_angles[h][i][1]) == 0)
              {
                print_this_angle (0, h, fid, inum, ub, field_angles[h][i][0], field_equi[0][j][0], field_angles[h][i][2], field_angles[h][i]);
              }
            }
          }
          else if (find_atom_id (1, field_angles[h][i][2]) == -10)
          {
            for (j=0; j<field_objects[2]; j++)
            {
              if (g_strcmp0 (field_equi[1][j][3], field_angles[h][i][2]) == 0)
              {
                print_this_angle (1, h, fid, inum, ub, field_angles[h][i][0], field_angles[h][i][1], field_equi[1][j][0], field_angles[h][i]);
              }
            }
            // End
            for (j=0; j<field_objects[1]; j++)
            {
              if (g_strcmp0 (field_equi[0][j][4], field_angles[h][i][2]) == 0)
              {
                print_this_angle (0, h, fid, inum, ub, field_angles[h][i][0], field_angles[h][i][1], field_equi[0][j][0], field_angles[h][i]);
              }
            }
          }
          else
          {
            print_this_angle (-1, h, fid, inum, ub, field_angles[h][i][0], field_angles[h][i][1], field_angles[h][i][2], field_angles[h][i]);
          }
          for (j=0; j<2; j++)
          {
            if (the_angles[j])
            {
              tmpan = the_angles[j];
              while (tmpan -> next)
              {
                tmpan = tmpan -> next;
                g_free (tmpan -> prev);
              }
              g_free (tmpan);
              the_angles[j] = NULL;
            }
          }
        }
      }
      else*/
      {
        for (i=0; i<field_objects[inum+h]; i++)
        {
          print_this_angle (-1, h, fid, inum, ub, field_angles[h][i][0], field_angles[h][i][1], field_angles[h][i][2], field_angles[h][i]);
        }
      }
      fprintf (fp, "  </%s>\n", aode[h]);
    }
  }
}

/*!
  \fn void print_dihedral_table (int fid, int inum)

  \brief print dihedral(s) table to file

  \param fid the force field id
  \param inum the object id
*/
void print_dihedral_table (int fid, int inum)
{
  int h, i, j;
  gchar * dode[2] = {"dihedrals-c", "dihedrals-ccc"};
  gchar * node[2] = {"dih-c", "dih-ccc"};
  gchar * desc[2] = {"Dihedrals / torsions:\n\n    V(Phi) = K x (1 + cos(n x Phi - Phi0))\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 rad^-2)\n      - Phi = dihedral angle (°)\n      - n = multiplicity\n      - Phi0 = phase shift angle (°)",
                     "Dihedrals / torsions\n\n    V(Phi) = K x (Phi - Phi0)^2  +  K' x (Phi - Phi0)^3  +  K'' x (Phi - Phi0)^4\n"
                     "    With:\n      - K, K' and K'' = nth order spring constant (kcal mol^-1 rad^-nth)\n      - Phi = dihedral angle (°)\n      - Phi0 = phase shift angle (°)"};
  for (h=0; h<2; h++)
  {
    if (field_objects[inum+h])
    {
      fprintf (fp, "  <%s>\n", dode[h]);
      fprintf (fp, "  <!-- %s  -->\n", desc[h]);
      for (i=0; i<field_objects[inum+h]; i++)
      {
        gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 4, field_dihedrals[h][i][0]));
        gchar * bbb = g_strdup_printf ("%s", find_atom_key(fid, 4, field_dihedrals[h][i][1]));
        gchar * ccc = g_strdup_printf ("%s", find_atom_key(fid, 4, field_dihedrals[h][i][2]));
        gchar * ddd = g_strdup_printf ("%s", find_atom_key(fid, 4, field_dihedrals[h][i][3]));
        fprintf (fp, "    <%s a=\"%d\" b=\"%d\" c=\"%d\" d=\"%d\" z_a=\"%d\" z_b=\"%d\" z_c=\"%d\" z_d=\"%d\"",
                 node[h], find_atom_id (1, aaa), find_atom_id (1, bbb), find_atom_id (1, ccc), find_atom_id (1, ddd),
                          find_atom_z (aaa), find_atom_z (bbb), find_atom_z (ccc), find_atom_z (ddd));
        g_free (aaa);
        g_free (bbb);
        g_free (ccc);
        g_free (ddd);
        switch (h)
        {
          case 0:
            if (fid < 4)
            {
              fprintf (fp, " K=\"%f\" Phi_zero=\"%f\" n=\"%f\"", string_to_double ((gpointer)field_dihedrals[h][i][5]) / string_to_double ((gpointer)field_dihedrals[h][i][4]),
                                                                 string_to_double ((gpointer)field_dihedrals[h][i][6]), string_to_double ((gpointer)field_dihedrals[h][i][7]));
              j = 8;
            }
            else
            {
              fprintf (fp, " K=\"%f\" Phi_zero=\"%f\" n=\"%f\"", string_to_double ((gpointer)field_dihedrals[h][i][4]), string_to_double ((gpointer)field_dihedrals[h][i][6]),
                                                                 string_to_double ((gpointer)field_dihedrals[h][i][5]));
              j = 7;
            }
            break;
          case 1:
            fprintf (fp, " K=\"%f\" Phi_zero=\"%f\" KK=\"%f\" KKK=\"%f\"", string_to_double ((gpointer)field_dihedrals[h][i][4]), string_to_double ((gpointer)field_dihedrals[h][i][5]),
                                                                           string_to_double ((gpointer)field_dihedrals[h][i][6]), string_to_double ((gpointer)field_dihedrals[h][i][8]));
            j = 10;
            break;
        }
        if (field_dim[inum+h] > j)
        {
          fprintf (fp, " info=\"%s\"", field_dihedrals[h][i][j]);
        }
        fprintf (fp, "/>\n");
      }
      fprintf (fp, "  </%s>\n", dode[h]);
    }
  }
}

/*!
  \fn void print_improper_table (int fid, int inum)

  \brief print improper(s) table to file

  \param fid the force field id
  \param inum the object id
*/
void print_improper_table (int fid, int inum)
{
  int i, j;
  gchar * desc[2] = {"Impropers:\n\n    V(Phi) = K x (1 + cos(n x Phi - Phi0))\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 rad^-2)\n      - Phi = angle (°)\n      - n = multiplicity\n      - Phi0 = equilibrium angle (°)",
                     "Impropers:  V(Phi) = K x (Phi - Phi0)^2\n\n"
                     "    With:\n      - K = spring constant (kcal mol^-1 rad^-2)\n      - Phi = angle (°)\n      - Phi0 = equilibrium angle (°)"};
  int wid = (fid < 4 || fid == OPLSAAP || fid == OPLSAAR) ? 3 : 2;
  if (field_objects[inum])
  {
    fprintf (fp, "  <impropers>\n");
    fprintf (fp, "  <!-- %s  -->\n", desc[wid == 3 ? 0 : 1]);
    for (i=0; i<field_objects[inum]; i++)
    {
      gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 5, (fid > AMBER99 && fid < CVFF) ? field_impropers[i][0]: field_impropers[i][1]));
      gchar * bbb = g_strdup_printf ("%s", find_atom_key(fid, 5, (fid > AMBER99 && fid < CVFF) ? field_impropers[i][1]: field_impropers[i][0]));
      gchar * ccc = g_strdup_printf ("%s", find_atom_key(fid, 5, field_impropers[i][2]));
      gchar * ddd = g_strdup_printf ("%s", find_atom_key(fid, 5, field_impropers[i][3]));
      fprintf (fp, "    <imp a=\"%d\" b=\"%d\" c=\"%d\" d=\"%d\" z_a=\"%d\" z_b=\"%d\" z_c=\"%d\" z_d=\"%d\"",
               find_atom_id (1, aaa), find_atom_id (1, bbb), find_atom_id (1, ccc), find_atom_id (1, ddd),
               find_atom_z (aaa), find_atom_z (bbb), find_atom_z (ccc), find_atom_z (ddd));
      g_free (aaa);
      g_free (bbb);
      g_free (ccc);
      g_free (ddd);
      if (fid < 4)
      {
        fprintf (fp, " K=\"%f\" Phi_zero=\"%f\" n=\"%f\"", string_to_double ((gpointer)field_impropers[i][4]), string_to_double ((gpointer)field_impropers[i][5]), string_to_double ((gpointer)field_impropers[i][6]));
        j = 7;
      }
      else if (wid == 3)
      {
        fprintf (fp," K=\"%f\" Phi_zero=\"%f\" n=\"%f\"", string_to_double ((gpointer)field_impropers[i][4]), string_to_double ((gpointer)field_impropers[i][6]), string_to_double ((gpointer)field_impropers[i][5]));
        j = 7;
      }
      else if (fid > AMBER99 && fid < CVFF)
      {
        fprintf (fp, " K=\"%f\" Phi_zero=\"%f\"", string_to_double ((gpointer)field_impropers[i][4]), string_to_double ((gpointer)field_impropers[i][6]));
        j = 7;
      }
      else
      {
        fprintf (fp, " K=\"%f\" Phi_zero=\"%f\"", string_to_double ((gpointer)field_impropers[i][4]), string_to_double ((gpointer)field_impropers[i][5]));
        j = 6;
      }
      if (field_dim[inum] > j)
      {
        fprintf (fp, " info=\"%s\"", field_impropers[i][j]);
      }
      fprintf (fp, "/>\n");
    }
    fprintf (fp, "  </impropers>\n");
  }
}

/*!
  \fn void print_inversion_table (int fid, int inum)

  \brief print inversion(s) table to file

  \param fid the force field id
  \param inum the object id
*/
void print_inversion_table (int fid, int inum)
{
  int i;
  gchar * desc = {"Inversions\n  0= Key_a, 1= Key_b, 2= Key_c, 3= Key_d, 4= K, 5= Phi0, 7= FF info\n\n  V(φ) = K x (Phi - Phi0)^2\n"
                  "  With:\n    - K = spring constant (kcal mol^-1 rad^-2)\n    - Phi = angle (°)\n    - Phi0 = equilibrium angle (°)"};
  if (field_objects[inum])
  {
    fprintf (fp, "  <inversions>\n");
    fprintf (fp, "  <!-- %s  -->\n", desc);
    for (i=0; i<field_objects[inum]; i++)
    {
      gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 5, field_inversions[i][1]));
      gchar * bbb = g_strdup_printf ("%s", find_atom_key(fid, 5, field_inversions[i][0]));
      gchar * ccc = g_strdup_printf ("%s", find_atom_key(fid, 5, field_inversions[i][2]));
      gchar * ddd = g_strdup_printf ("%s", find_atom_key(fid, 5, field_inversions[i][3]));
      fprintf (fp, "    <inv a=\"%d\" b=\"%d\" c=\"%d\" d=\"%d\" z_a=\"%d\" z_b=\"%d\" z_c=\"%d\" z_d=\"%d\" K=\"%f\" Phi_zero=\"%f\"",
               find_atom_id (1, aaa), find_atom_id (1, bbb), find_atom_id (1, ccc), find_atom_id (1, ddd),
               find_atom_z (aaa), find_atom_z (bbb), find_atom_z (ccc), find_atom_z (ddd),
               string_to_double ((gpointer)field_inversions[i][4]), string_to_double ((gpointer)field_inversions[i][5]));
      g_free (aaa);
      g_free (bbb);
      g_free (ccc);
      g_free (ddd);
      if (field_dim[inum] > 6)
      {
        fprintf (fp, " info=\"%s\"", field_inversions[i][6]);
      }
      fprintf (fp, "/>\n");
    }
    fprintf (fp, "  </inversions>\n");
  }
}

/*!
  \fn void print_vdw_table (int fid, int inum)

  \brief print VdW table to file

  \param fid the force field id
  \param inum the object id
*/
void print_vdw_table (int fid, int inum)
{
  int i, j, k, l;
  gchar * desc[3] = {"Non-bonded 12-6\n  0= Key_a, 1= Epslion, 2= R0 (A), 3=  FF info\n\n  Non-bonded (12-6): V(rij) =  Epslion(ij) x [(R0(ij)/rij)^12 - 2 x (R0(ij)/rij)^6]\n"
                     "  With:\n"
                     "  Epslion(ij) = sqrt(Epslion(i) x Epslion(j))  and Epslion(i) (kcal mol^-1)\n"
                     "  R0(ij)= (R0(i) + R0(j))/2  and R0(i) (A)\n",
                     "Non-bonded 12-6\n  0= Key_a, 1= A, 2= B, 3=  FF info\n\n  Non-bonded (12-6): V(rij) =  A(ij)/rij^12 - B(ij)/rij^6\n"
                     "  With:\n"
                     "  A(ij) = sqrt(Ai x Aj)\n"
                     "  B(ij) = sqrt(Bi x Bj)\n",
                     "Non-bonded 9-6\n  0= Key_a, 1= Epslion, 2= R0 (A), 3=  FF info\n\n  Non-bonded (9-6): V(rij) =  Epslion(ij) x [2 x (R0(ij)/rij)^9 - 3 x (R0(ij)/rij)^6]\n"
                     "  With:\n"
                     "  Epslion(ij) = 2 x sqrt(Epslion(i) x Epslion(j)) x R0(i)^3 x R0(j)^3 / [ R0(i)^6 + R0(j)^6 ]      and Epslion(i) (kcal mol^-1)\n"
                     "  R0(ij)= ((R0(i)^6 + R0(j)^6)/2) ^ 1/6      and R0(i) (A)\n"};

  int wid = ((fid >= CHARMM22P && fid <= CHARMMSI) || (fid >= OPLSAAP)) ? 1 : 0;
  int did = (fid <= CHARMMSI || fid >= OPLSAAP) ? 0 : (fid == CVFF || fid == CVFF_AUG) ? 1 : 2;
  int cid = (fid > COMPASS) ? 1 : 0;
  int wdi[2] = {2, 4};
  if (field_objects[inum])
  {
    fprintf (fp, "  <non-bonded>\n");
    fprintf (fp, "  <!-- %s  -->\n", desc[did]);
    for (i=0; i<field_objects[inum]; i++)
    {
      gchar * aaa = g_strdup_printf ("%s", find_atom_key(fid, 1, field_vdw[i][0]));
      fprintf (fp, "    <non-bd a=\"%d\" z_a=\"%d\"", find_atom_id (0, aaa), find_atom_z (aaa));
      g_free (aaa);
      if (did == 1)
      {
        fprintf (fp, " A=\"%f\" B= \"%f\"", string_to_double ((gpointer)field_vdw[i][1]), string_to_double ((gpointer)field_vdw[i][2]));
      }
      else
      {
        if (fid <= AMBER99)
        {
          fprintf (fp, " Ei=\"%f\" Ri=\"%f\"", string_to_double ((gpointer)field_vdw[i][2]), string_to_double ((gpointer)field_vdw[i][1]));
        }
        else
        {
          fprintf (fp, " Ei=\"%f\" Ri=\"%f\"", string_to_double ((gpointer)field_vdw[i][1+cid]), string_to_double ((gpointer)field_vdw[i][2+cid]));
        }
        if ((fid >= CHARMM22P && fid <= CHARMMSI) || fid > COMPASS)
        {
          fprintf (fp, " Eii=\"%f\" Rii=\"%f\"", string_to_double ((gpointer)field_vdw[i][3+2*cid]), string_to_double ((gpointer)field_vdw[i][4+2*cid]));
        }
      }
      j = wdi[wid]+1+2*cid;

      if (field_dim[inum] > j)
      {
        fprintf (fp, " info=\"%s\"", field_vdw[i][j]);
      }
      fprintf (fp, "/>\n");
      if (fid <= AMBER99)
      {
        for (k=0; k<field_objects[1]; k++)
        {
          if (g_strcmp0 (field_equi[0][k][0], field_vdw[i][0]) == 0)
          {
            for (l=1; l<field_dim[1]; l++)
            {
              if(g_strcmp0 (field_equi[0][k][l], " ") != 0)
              {
                fprintf (fp, "    <non-bd a=\"%d\" z_a=\"%d\"", find_atom_id (0, field_equi[0][k][l]), find_atom_z (field_vdw[i][0]));
                fprintf (fp, " Ei=\"%f\" Ri=\"%f\"", string_to_double ((gpointer)field_vdw[i][2]), string_to_double ((gpointer)field_vdw[i][1]));
                if (field_dim[inum] > j)
                {
                  fprintf (fp, " info=\"Equi. to %s\"", field_vdw[i][0]);
                }
                fprintf (fp, "/>\n");
              }
            }
          }
        }
      }
    }
    fprintf (fp, "  </non-bonded>\n");
  }
}


field_object_match * field_objects_id[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
field_object_match * tmp_obj_id;

char *** ff_atoms;
// 0 = Quadratic, 1 = Quartic, 2 = Morse

field_data * ff_bonds[3];
field_data * ff_angles[2];
field_data * ff_dih[2];
field_data * ff_imp;
field_data * ff_inv;
field_data * ff_vdw;

int ff_unit;
int * ff_objects;
int * ff_dim;
int * ff_info;
int * ff_key;

/*!
  \fn int field_find_atoms ()

  \brief retrieve matching atom from force field data base
*/
int field_find_atoms ()
{
  int i, j, k, l;
  atoms_id = allocint (tmp_proj -> nspec);
  extraz_id = allocdint (5, tmp_proj -> nspec);
  atoms_id_list = g_malloc0(tmp_proj -> nspec*sizeof*atoms_id_list);
  k = 0;
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    atoms_id[i] = 0;
#ifdef DEBUG
    g_debug ("Searching field atoms for spec: %d, label= %s, z= %d", i, tmp_proj -> chemistry -> label[i], (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i]);
#endif
    for (j=0; j<ff_objects[0]; j++)
    {
      if (g_strcmp0 (exact_name(tmp_proj -> chemistry -> label[i]), exact_name(ff_atoms[j][0])) == 0)
      {
        atoms_id[i] ++;
      }
    }
    if (atoms_id[i] > 0) k ++;
  }
  for (i=0; i<tmp_proj -> nspec; i++)
  {
#ifdef DEBUG
    g_debug ("For spec: %d, number of match = %d", i, atoms_id[i]);
#endif
    if (atoms_id[i])
    {
      atoms_id_list[i] = g_malloc0(atoms_id[i]*sizeof*atoms_id_list[i]);
      l = 0;
      for (j=0; j<ff_objects[0]; j++)
      {
        if (g_strcmp0 (exact_name(tmp_proj -> chemistry -> label[i]), exact_name(ff_atoms[j][0])) == 0)
        {
          atoms_id_list[i][l] = j;
          l ++;
        }
      }
    }
  }
  return k;
}

int is_extra;

/*!
  \fn gboolean is_a_match (int * data, int num, int val[4])

  \brief is this a match ?

  \param data the set of parameter(s)
  \param num the number of parameter
  \param val the target value(s)
*/
gboolean is_a_match (int * data, int num, int val[4])
{
  int i;
  for (i=0; i<num; i++)
  {
    if (data[i] == -1) is_extra = 1;
    if (data[i] != val[i] && data[i] != -1) return FALSE;
  }
  return TRUE;
}

/*!
  \fn void find_object_ijkl (int hid, int foid, int oid, int sa, int za, int sb, int zb, int sc, int zc, int sd, int zd)

  \brief retrieve parameter from force field database

  \param hid iteration parameter
  \param foid force table id
  \param oid type of structural element
  \param sa 1st chemical species
  \param za 1st Z
  \param sb 2nd chemical species
  \param zb 2nd Z
  \param sc 3rd chemical species, if any
  \param zc 3rd Z, if any
  \param sd 4th chemical species, if any
  \param zd 4th Z, if any
*/
void find_object_ijkl (int hid, int foid, int oid, int sa, int za, int sb, int zb, int sc, int zc, int sd, int zd)
{
  int h, i;
  int val[4];
  val[0] = za;
  val[1] = zb;
  val[2] = zc;
  val[3] = zd;
  gboolean do_this_id, save_this_id;
  field_object_match * test_obj_id;
  for (h=0; h<2+hid; h++)
  {
    if (ff_objects[h+foid] > 0)
    {
      for (i=0; i<ff_objects[h+foid]; i++)
      {
        do_this_id = FALSE;
        is_extra = 0;
        switch (oid+2)
        {
          case FBDS:
            do_this_id = is_a_match (ff_bonds[h] -> atoms_z[i], 2, val);
            break;
          case FANG:
            do_this_id = is_a_match (ff_angles[h] -> atoms_z[i], 3, val);
            break;
          case FDIH:
            do_this_id = is_a_match (ff_dih[h] -> atoms_z[i], 4, val);
            break;
          case FIMP:
            do_this_id = is_a_match (ff_imp -> atoms_z[i], 4, val);
            break;
          case FINV:
            do_this_id = is_a_match (ff_inv -> atoms_z[i], 4, val);
            break;
          case FNBD:
            do_this_id = is_a_match (ff_vdw -> atoms_z[i], 1, val);
            break;
          default:
            break;
        }
        if (do_this_id)
        {
          save_this_id = TRUE;
          if (field_objects_id[oid])
          {
            test_obj_id = field_objects_id[oid];
            while (test_obj_id != NULL)
            {
              if (test_obj_id -> type == h && test_obj_id -> oid == i)
              {
                save_this_id = FALSE;
                break;
              }
              test_obj_id = test_obj_id -> next;
            }
            if (save_this_id)
            {
              tmp_obj_id -> next = g_malloc0(sizeof*tmp_obj_id -> next);
              tmp_obj_id -> next -> id = tmp_obj_id -> id + 1;
              tmp_obj_id = tmp_obj_id -> next;
            }
          }
          else
          {
            field_objects_id[oid] = g_malloc0(sizeof*field_objects_id[oid]);
            tmp_obj_id = field_objects_id[oid];
          }
          if (save_this_id)
          {
            // if (oid == 3) g_debug ("Saving Impropers, sa= %d, sb= %d, sc= %d, sd= %d", sa, sb, sc, sd);
            tmp_obj_id -> obj = oid;
            tmp_obj_id -> type = h;
            tmp_obj_id -> oid = i;
            if (oid < 5)
            {
              if (sa > -1) extraz_id[oid][sa] += is_extra;
              if (sb > -1) extraz_id[oid][sb] += is_extra;
              if (sc > -1) extraz_id[oid][sc] += is_extra;
              if (sd > -1) extraz_id[oid][sd] += is_extra;
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void field_find_bonds ()

  \brief retrieve available bond parameters in force field database
*/
void field_find_bonds ()
{
  int i, j, k, l;
  for (i=0; i<tmp_proj -> nspec-1; i++)
  {
    if (atoms_id[i] > 0)
    {
      for (j=i+1; j<tmp_proj -> nspec; j++)
      {
        if (atoms_id[j] > 0)
        {
          k = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i];
          l = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][j];
          find_object_ijkl (1, 1, 0, i, k, j, l, -1, -1, -1, -1);
          find_object_ijkl (1, 1, 0, j, l, i, k, -1, -1, -1, -1);
        }
      }
    }
  }
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    if (atoms_id[i] > 0)
    {
      k = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i];
      find_object_ijkl (1, 1, 0, i, k, i, k, -1, -1, -1, -1);
    }
  }
}

/*!
  \fn void field_find_angles ()

  \brief retrieve available angle parameters in force field database
*/
void field_find_angles ()
{
  int i, j, k, l, m, n;
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    if (atoms_id[i] > 0)
    {
      l = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i];
      for (j=0; j<tmp_proj -> nspec; j++)
      {
        if (atoms_id[j] > 0)
        {
          m = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][j];
          for (k=0; k<tmp_proj -> nspec; k++)
          {
            if (atoms_id[k] > 0)
            {
              n = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][k];
              find_object_ijkl (0, 4, 1, i, l, j, m, k, n, -1, -1);
            }
          }
        }
      }
    }
  }
}

/*!
  \fn void field_find_dihedrals (int id)

  \brief  retrieve available dihedral parameters in force field database

  \param id 0 = dihedral(s), 1 = improper(s), 2 = inversion(s)
*/
void field_find_dihedrals (int id)
{
  int i, j, k, l, m, n, o, p, q;
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    if (atoms_id[i] > 0)
    {
      m = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i];
      for (j=0; j<tmp_proj -> nspec; j++)
      {
        if (atoms_id[j] > 0)
        {
          n = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][j];
          for (k=0; k<tmp_proj -> nspec; k++)
          {
            if (atoms_id[k] > 0)
            {
              o = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][k];
              for (l=0; l<tmp_proj -> nspec; l++)
              {
                if (atoms_id[l] > 0)
                {
                  p = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][l];
                  q = (id) ? 1 : 0;
                  find_object_ijkl (-q, 6+q*2+q*(id-1), 2+id, i, m, j, n, k, o, l, p);
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
  \fn void field_find_vdw ()

  \brief retrieve available VdW parameters in force field database
*/
void field_find_vdw ()
{
  int i, j;
  for (i=0; i<tmp_proj -> nspec; i++)
  {
    if (atoms_id[i] > 0)
    {
      j = (int)tmp_proj -> chemistry -> chem_prop[CHEM_Z][i];
      find_object_ijkl (-1, 10, 5, i, j, -1, -1, -1, -1, -1, -1);
    }
  }
}


#ifdef DEBUG
/*!
  \fn void print_all (int oid)

  \brief print found force field data

  \param oid the type of structural element
*/
void print_all (int oid)
{
  gchar * prop[7] = {"atom", "bond", "angle", "dihedral", "improper", "inversion", "vdw"};
  g_debug ("Field %s found: %d", prop[oid-1], tmp_obj_id -> id+1);
  while (tmp_obj_id != NULL)
  {
    g_debug ("     %s N° %d: h= %d, oid= %d", prop[oid-1], tmp_obj_id -> id+1, tmp_obj_id -> type, tmp_obj_id -> oid);
    tmp_obj_id = tmp_obj_id -> next;
  }
}
#endif

/*!
  \fn float get_force_field_atom_mass (int sp, int num)

  \brief get force field atomic mass

  \param sp the target chemical species
  \param num the atom id
*/
float get_force_field_atom_mass (int sp, int num)
{
  g_debug ("sp= %d, atoms_id[%d]= %d, num= %d", sp, sp, atoms_id[sp], num);
  if (atoms_id[sp] > 0 && num >= 0 && num <= atoms_id[sp])
  {
    return string_to_double ((gpointer)ff_atoms[atoms_id_list[sp][num]][1]);
  }
  else
  {
    return 0.0;
  }
}

#ifdef USE_ATOMS

gchar * filetoread;

/*!
  \fn void clean_this_field_data (xmlDoc * doc, xmlTextReaderPtr reader)

  \brief free force field XML file data

  \param doc the xmlDoc pointer to free
  \param reader the XML reader to free
*/
void clean_this_field_data (xmlDoc * doc, xmlTextReaderPtr reader)
{
  clean_xml_data (doc, reader);
  if (filetoread) g_free (filetoread);
  filetoread = NULL;
  if (ff_objects) g_free (ff_objects);
  ff_objects = NULL;
  if (ff_dim) g_free (ff_dim);
  ff_dim = NULL;
  if (ff_key) g_free (ff_key);
  ff_key = NULL;
  if (ff_info) g_free (ff_info);
  ff_info = NULL;
  int i;
  for (i=0; i<3; i++)
  {
    if (i < 2)
    {
      if (ff_angles[i])
      {
        g_free (ff_angles[i]);
        ff_angles[i] = NULL;
      }
      if (ff_dih[i])
      {
        g_free (ff_dih[i]);
        ff_dih[i] = NULL;
      }
    }
    if (ff_bonds[i])
    {
      g_free (ff_bonds[i]);
      ff_bonds[i] = NULL;
    }
  }
  if (ff_imp)
  {
    g_free (ff_imp);
    ff_imp = NULL;
  }
  if (ff_inv)
  {
    g_free (ff_inv);
    ff_inv = NULL;
  }
  if (ff_vdw)
  {
    g_free (ff_vdw);
    ff_vdw = NULL;
  }
}

/*!
  \fn gchar * open_field_file (int field)

  \brief open force field XML file

  \param field the id of the force field to open
*/
gchar * open_field_file (int field)
{
  int i, j;
  xmlDoc * doc;
  xmlTextReaderPtr reader;
  xmlNodePtr racine, f_node;
  xmlNodePtr n_node, m_node, o_node;
  field_data * ff_data;
  xmlAttrPtr prop;
  xmlChar * content;
  const xmlChar fml[7]="ff-xml";
  gchar * data_nodes[11]={"atoms", "bonds-h", "bonds-q", "bonds-m", "angles-h", "angles-q",
                          "dihedrals-c", "dihedrals-ccc", "impropers", "inversions", "non-bonded"};
  gchar * data_n[11]={"at", "bd-h", "bd-q", "bd-m", "ang-h", "ang-q",
                      "dih-c", "dih-ccc", "imp", "inv", "non-bd"};
  gchar * str;
  filetoread = NULL;
  ff_objects = NULL;
  ff_dim = NULL;
  ff_key = NULL;
  ff_info = NULL;
  ff_unit = -1;
  gchar * force_field_name;
  gboolean setinfo;
  int odim[10] = {2, 2, 2, 3, 3, 4, 4, 4, 4, 1};
  /*
   * build an xmlReader for that file
   */

#ifdef G_OS_WIN32
  filetoread = g_strdup_printf ("%s\\force_fields\\%s.ffl", PACKAGE_LIB_DIR, field_ffl[field]);
#else
  filetoread = g_strdup_printf ("%s/force_fields/%s.ffl", PACKAGE_LIB_DIR, field_ffl[field]);
#endif

  reader = xmlReaderForFile(filetoread, NULL, 0);
  if (reader == NULL)
  {
    g_warning ("Error reading FF file %s, reader error ?!", filetoread);
    return NULL;
  }
  else
  {
    doc = xmlParseFile(filetoread);
    if (doc == NULL)
    {
      g_warning ("Error reading FF file %s, xmlParsing error ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    racine = xmlDocGetRootElement(doc);
    if (racine == NULL)
    {
      g_warning ("Error reading FF file %s, missing XML root node ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    if (g_strcmp0 ((char *)(racine -> name), (char *)fml) != 0)
    {
      g_warning ("Error reading FF file %s, missing ff-xml node ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    // nodes :: field-name, data, atoms, bonds, angles, dihedrals, impropers, inversons, non-bonded, bond-inc ?
    f_node = findnode (racine -> children, "force-field");
    if (f_node == NULL)
    {
      g_warning ("Error reading FF file %s, missing force field node ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    content = xmlNodeGetContent(f_node);
    force_field_name = g_strdup_printf ("%s", content);
    xmlFree (content);
    f_node = findnode (racine -> children, "units");
    if (f_node == NULL)
    {
      g_warning ("Error reading FF file %s, missing units node ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    content = xmlNodeGetContent(f_node);
    str = g_strdup_printf ("%s", content);
    xmlFree (content);
    for (i=0; i<4;i++)
    {
      if (g_strcmp0 (str, fnames[activef][0][i]) == 0)
      {
        ff_unit = i;
        break;
      }
    }
    if (ff_unit < 0)
    {
      g_warning ("Error reading FF file %s, units, ff_unit = %d < 0 ?!", filetoread, ff_unit);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    f_node = findnode (racine -> children, "ff-data");
    if (f_node == NULL)
    {
      g_warning ("Error reading FF file %s, missing ff-data node ?!", filetoread);
      clean_this_field_data (doc, reader);
      return NULL;
    }
    ff_objects = g_malloc0(11*sizeof*ff_objects);
    ff_dim = g_malloc0(11*sizeof*ff_dim);
    //ff_info = g_malloc0(11*sizeof*ff_key);
    ff_key = g_malloc0(10*sizeof*ff_key);
    for (i=0; i<11; i++)
    {
      m_node = findnode (f_node -> children, data_nodes[i]);
      if (m_node == NULL)
      {
        g_warning ("Error reading FF file %s, ff-data, i= %d, missing node ?!", filetoread, i);
        clean_this_field_data (doc, reader);
        return NULL;
      }
      content = xmlNodeGetContent(m_node);
      ff_objects[i] = (int)string_to_double ((gpointer)content);
      xmlFree (content);
      if (ff_objects[i] < 0)
      {
        g_warning ("Error reading FF file %s, ff-data, ff_objects[%d] = %d < 0 ?!", filetoread, i, ff_objects[i]);
        clean_this_field_data (doc, reader);
        return NULL;
      }
      else if (ff_objects[i])
      {
        prop = m_node -> properties;
        if (prop == NULL)
        {
          g_warning ("Error reading FF file %s, ff-data, i= %d, missing node prop ?!", filetoread, i);
          clean_this_field_data (doc, reader);
          return NULL;
        }
        while (prop)
        {
          o_node = prop -> children;
          if (o_node == NULL)
          {
            g_warning ("Error reading FF file %s, ff-data, i= %d, missing prop data ?!", filetoread, i);
            clean_this_field_data (doc, reader);
            return NULL;
          }
          content = xmlNodeGetContent(o_node);
          if (g_strcmp0 ("dim",(char *)prop -> name) == 0)
          {
            ff_dim[i] = (int) string_to_double ((gpointer)content);
          }
          /*else if (g_strcmp0 ("info",(char *)prop -> name) == 0)
          {
            ff_info[i] = (int)string_to_double ((gpointer)content);
          }*/
          else if (g_strcmp0 ("pot",(char *)prop -> name) == 0)
          {
            ff_key[i-1] = (int)string_to_double ((gpointer)content);
          }
          xmlFree (content);
          prop = prop -> next;
        }
      }
    }
    for (i=0; i<11; i++)
    {
      if (ff_objects[i])
      {
        f_node = findnode (racine -> children, data_nodes[i]);
        if (f_node == NULL)
        {
          g_warning ("Error reading FF file %s, i= %d, missing node ?!", filetoread, i);
          clean_this_field_data (doc, reader);
          return NULL;
        }
        n_node = f_node -> children;
        if (n_node == NULL)
        {
          g_warning ("Error reading FF file %s, i= %d, missing child node ?!", filetoread, i);
          clean_this_field_data (doc, reader);
          return NULL;
        }
        if (i == 0)
        {
          ff_atoms = g_malloc0(ff_objects[0]*sizeof*ff_atoms);
          for (j=0; j<ff_objects[0]; j++)
          {
            ff_atoms[j] = g_malloc0(ff_dim[0]*sizeof*ff_atoms[j]);
          }
        }
        else if (i < 4)
        {
          ff_bonds[i-1] = g_malloc0(sizeof*ff_bonds[i-1]);
          ff_data = ff_bonds[i-1];
        }
        else if (i < 6)
        {
          ff_angles[i-4] = g_malloc0(sizeof*ff_angles[i-4]);
          ff_data = ff_angles[i-4];
        }
        else if (i < 8)
        {
          ff_dih[i-6] = g_malloc0(sizeof*ff_dih[i-6]);
          ff_data = ff_dih[i-6];
        }
        else if (i == 8)
        {
          ff_imp = g_malloc0(sizeof*ff_imp);
          ff_data = ff_imp;
        }
        else if (i == 9)
        {
          ff_inv = g_malloc0(sizeof*ff_inv);
          ff_data = ff_inv;
        }
        else if (i == 10)
        {
          ff_vdw = g_malloc0(sizeof*ff_vdw);
          ff_data = ff_vdw;
        }
        if (i > 0)
        {
          ff_data -> atoms_z = g_malloc0(ff_objects[i]*sizeof*ff_data -> atoms_z);
          ff_data -> atoms_id = g_malloc0(ff_objects[i]*sizeof*ff_data -> atoms_id);
          ff_data -> npar = ff_dim[i];
          ff_data -> key = ff_key[i-1];
          ff_data -> param = g_malloc0(ff_objects[i]*sizeof*ff_data -> param);
          for (j=0; j<ff_objects[i]; j++)
          {
            ff_data -> atoms_z[j] = g_malloc0(odim[i-1]*sizeof*ff_data -> atoms_z[j]);
            ff_data -> atoms_id[j] = g_malloc0(odim[i-1]*sizeof*ff_data -> atoms_id[j]);
            ff_data -> param[j] = g_malloc0(ff_dim[i]*sizeof*ff_data -> param[j]);
          }
          ff_data -> info = g_malloc0(ff_objects[i]*sizeof*ff_data -> info);
        }
        setinfo = FALSE;
        for (j=0; j<ff_objects[i]; j++)
        {
          n_node = findnode (n_node, data_n[i]);
          if (n_node == NULL)
          {
             g_warning ("Error reading FF file %s, i= %d, j= %d, missing node ?!", filetoread, i, j);
             clean_this_field_data (doc, reader);
             return NULL;
           }
          prop = n_node -> properties;
          if (prop == NULL)
          {
             g_warning ("Error reading FF file %s, i= %d, j= %d, missing prop ?!", filetoread, i, j);
             clean_this_field_data (doc, reader);
             return NULL;
           }
          while (prop)
          {
            o_node = prop -> children;
            if (o_node == NULL)
            {
              g_warning ("Error reading FF file %s, i= %d, j= %d, missing prop node ?!", filetoread, i, j);
              clean_this_field_data (doc, reader);
              return NULL;
            }
            content = xmlNodeGetContent(o_node);
            switch (i)
            {
              case 0:
                if (g_strcmp0 ("label",(char *)prop -> name) == 0)
                {
                  ff_atoms[j][0] = g_strdup_printf ("%s", content);
                }
                else if (g_strcmp0 ("mass",(char *)prop -> name) == 0)
                {
                  ff_atoms[j][1] = g_strdup_printf ("%s", content);
                }
                else if (g_strcmp0 ("key",(char *)prop -> name) == 0)
                {
                  ff_atoms[j][2] = g_strdup_printf ("%s", content);
                }
                else if (g_strcmp0 ("info",(char *)prop -> name) == 0)
                {
                  ff_atoms[j][3] = g_strdup_printf ("%s", content);
                }
                else
                {
                  g_warning ("Error reading FF file %s, i= %d, j= %d, unexpected node ?!", filetoread, i, j);
                  clean_this_field_data (doc, reader);
                  return NULL;
                }
                break;
              default:
                if (g_strcmp0 ("a",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_id[j][0] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("b",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_id[j][1] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("c",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_id[j][2] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("d",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_id[j][3] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("z_a",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_z[j][0] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("z_b",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_z[j][1] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("z_c",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_z[j][2] = (int) string_to_double ((gpointer)content);
                }
                else if (g_strcmp0 ("z_d",(char *)prop -> name) == 0)
                {
                  ff_data -> atoms_z[j][3] = (int) string_to_double ((gpointer)content);
                }
                else if (i > 0 && g_strcmp0 ("info",(char *)prop -> name) == 0)
                {
                  ff_data -> info[j] = g_strdup_printf ("%s", content);
                  setinfo = TRUE;
                }
                else
                {
                  if (i == 10)
                  {
                    // Non-bonded: 12-6: E_i, R_i, E_14, R_14
                    if (g_strcmp0 ("Ei",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][0] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Ri",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][1] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Eii",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][2] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Rii",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][3] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("A",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][0] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("B",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][1] = string_to_double ((gpointer)content);
                    }
                    else
                    {
                      g_warning ("Error reading FF file %s, i= %d, j= %d, unexpected node ?! node name= %s", filetoread, i, j, (char *)prop -> name);
                      clean_this_field_data (doc, reader);
                      xmlFree (content);
                      return NULL;
                    }
                  }
                  else
                  {
                    // Bonds: Harm: K, R0
                    // Bonds: Quartic: K, R_0, K', K''
                    // Bonds: Morse: D, R_0, Alpha
                    // Angle: Quartic: K, Theta_0, K', K''
                    // Dihedral: Cosine: K, Phi_0, n
                    // Dihedral: Cosine 3: K, Phi_0, K', K''
                    // Improper: Cosine: K, n, Phi_0 / Harm: K, Phi_0
                    // Inversion: Harm: K, Phi_0
                    if (g_strcmp0 ("K",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][0] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("R_zero",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][1] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Theta_zero",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][1] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Phi_zero",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][1] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("KK",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][2] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("KKK",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][3] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Kub",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][2] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("S_zero",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][3] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("n",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][2] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("D",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][0] = string_to_double ((gpointer)content);
                    }
                    else if (g_strcmp0 ("Alpha",(char *)prop -> name) == 0)
                    {
                      ff_data -> param[j][2] = string_to_double ((gpointer)content);
                    }
                    else
                    {
                      g_warning ("Error reading FF file %s, i= %d, j= %d, unexpected node ?! node name= %s", filetoread, i, j, (char *)prop -> name);
                      xmlFree (content);
                      clean_this_field_data (doc, reader);
                      return NULL;
                    }
                  }
                }
                break;
            }
            xmlFree (content);
            prop = prop -> next;
          }
          n_node = n_node -> next;
        }
        if (i > 0 && ! setinfo)
        {
          g_free (ff_data -> info);
          ff_data -> info = NULL;
        }
        /*if (i==0)
        {
          for (j=0; j<ff_objects[i]; j++)
          {
            g_debug ("j= %d, ff_atoms[%d][0]= %s, ff_atoms[%d][1]= %s, ff_atoms[%d][2]= %s, ff_atoms[%d][3]= %s",
                     j, j, ff_atoms[j][0], j, ff_atoms[j][1], j, ff_atoms[j][2], j, ff_atoms[j][3]);
          }
        }*/
      }
    }
    xmlFreeDoc(doc);
    xmlFreeTextReader(reader);
    xmlCleanupParser();
    return force_field_name;
  }
}

#endif

/*!
  \fn G_MODULE_EXPORT void setup_this_force_field (int id)

  \brief setup force field parameters

  \param id the force field id
*/
G_MODULE_EXPORT void setup_this_force_field (int id)
{
#ifdef USE_ATOMS
  // associate_new_pointers_to_field_data (id);
  // Atoms
  int i;
  for (i=0; i<6; i++)
  {
    if (field_objects_id[i])
    {
      tmp_obj_id = field_objects_id[i];
      while (tmp_obj_id -> next)
      {
        tmp_obj_id = tmp_obj_id -> next;
        g_free (tmp_obj_id -> prev);
      }
      g_free (tmp_obj_id);
      field_objects_id[i] = NULL;
    }
  }
  gchar * name =  open_field_file (id);
  if (g_strcmp0 (name, field_acro[id]) != 0)
  {
    // Error in xml file ?!
    tmp_field -> type = -1;
  }
  else if (name)
  {
    int spf = field_find_atoms ();
    if (spf < tmp_proj -> nspec)
    {
      // Some atomic species are not described in this force field
      // Take action ?
    }

    int i;
    for (i=0; i<6; i++)
    {
      if (field_objects_id[i])
      {
        tmp_obj_id = field_objects_id[i];
        while (tmp_obj_id -> next)
        {
          tmp_obj_id = tmp_obj_id -> next;
          g_free (tmp_obj_id -> prev);
        }
        g_free (tmp_obj_id);
        field_objects_id[i] = NULL;
      }
    }

    // Bonds
    tmp_obj_id = NULL;
    field_find_bonds ();
#ifdef DEBUG
    if (tmp_obj_id) print_all (FBDS);
#endif

    // Angles
    tmp_obj_id = NULL;
    field_find_angles ();
#ifdef DEBUG
    if (tmp_obj_id) print_all (FANG);
#endif
    // Dihedrals
    tmp_obj_id = NULL;
    field_find_dihedrals (0);
#ifdef DEBUG
    if (tmp_obj_id) print_all (FDIH);
#endif
    // Impropers
    tmp_obj_id = NULL;
    field_find_dihedrals (1);
#ifdef DEBUG
    if (tmp_obj_id) print_all (FIMP);
#endif
    // Inversions
    tmp_obj_id = NULL;
    field_find_dihedrals (2);
#ifdef DEBUG
    if (tmp_obj_id) print_all (FINV);
#endif
    // Vdw
    tmp_obj_id = NULL;
    field_find_vdw ();
#ifdef DEBUG
    if (tmp_obj_id) print_all (FNBD);
#endif
  }
#else
  int i;
  gchar * str;
  //FILE * hp = fopen ("force_fields.h", "w");
  for (i=0; i<N_FIELDS; i++)
  {
    g_debug ("\n\nField id= %d, name= %s", i, field_keyw[i]);
    associate_pointers_to_field_data (i);
    is_99 = (i==CHARMM22M) ? TRUE : FALSE;
    str = g_strdup_printf ("%s/force_fields/%s.ffl", PACKAGE_LIB_DIR, field_ffl[i]);
    fp = fopen (str, "w");
    fprintf (fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf (fp, "<!-- Force Field Library XML file -->\n");
    fprintf (fp, "<ff-xml>\n");
    fprintf (fp, "  <force-field>%s</force-field>\n", field_acro[i]);
    fprintf (fp, "  <!--  data adapted from the file: %s -->\n", ffo_file[i]);
    fprintf (fp, "  <units>k-calories per mol</units>\n");
    g_free (str);
    print_object_dim_and_key_tables (i);
    print_atom_table (i);
    print_bond_table (i, 3);
    print_angle_table (i, 6);
    print_dihedral_table (i, 8);

    print_improper_table (i, 11);
    print_inversion_table (i, 10);
    print_vdw_table (i, 12);
    //print_increment_table (i, 13)
    fprintf (fp, "</ff-xml>\n");
    fclose (fp);
  }
  //fclose (hp);

#endif
}
