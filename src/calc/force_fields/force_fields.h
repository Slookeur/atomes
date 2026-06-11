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
* @file force_fields.h
* @short Variable declarations for the creation of the force field database
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This header file: 'force_fields.h'
*
* Contains:

 - Variable declarations for the creation of the force field database

*/

#ifndef FORCE_FIELDS_H_
#define FORCE_FIELDS_H_
/*
  Amber force fields
*/

// amber94
extern int amber94_objects[14];
extern int amber94_dim[14];
extern char * amber94_atoms[2][4];
extern char * amber94_equi[2][15];
extern char * amber94_bonds[83][5];
extern char * amber94_angles[191][6];
extern char * amber94_dihedrals[81][9];
extern char * amber94_impropers[31][8];
extern char * amber94_vdw[34][4];
// amber96
extern int amber96_objects[14];
extern int amber96_dim[14];
extern char * amber96_atoms[2][4];
extern char * amber96_equi[2][15];
extern char * amber96_bonds[83][5];
extern char * amber96_angles[191][6];
extern char * amber96_dihedrals[81][9];
extern char * amber96_impropers[31][8];
extern char * amber96_vdw[35][4];
// amber98
extern int amber98_objects[14];
extern int amber98_dim[14];
extern char * amber98_atoms[2][4];
extern char * amber98_equi[2][15];
extern char * amber98_bonds[83][5];
extern char * amber98_angles[191][6];
extern char * amber98_dihedrals[83][9];
extern char * amber98_impropers[31][8];
extern char * amber98_vdw[35][4];
// amber99
extern int amber99_objects[14];
extern int amber99_dim[14];
extern char * amber99_atoms[2][5];
extern char * amber99_equi[2][15];
extern char * amber99_bonds[116][5];
extern char * amber99_angles[281][6];
extern char * amber99_dihedrals[164][9];
extern char * amber99_impropers[38][8];
extern char * amber99_vdw[42][4];

/*
  CHARMM force fields
*/

// CHARMM-charmm22_prot
extern int charmm22_prot_objects[14];
extern int charmm22_prot_dim[14];
extern char * charmm22_prot_atoms[47][5];
extern char * charmm22_prot_bonds[122][5];
extern char * charmm22_prot_angles[324][8];
extern char * charmm22_prot_dihedrals[445][8];
extern char * charmm22_prot_impropers[34][8];
extern char * charmm22_prot_vdw[47][6];
// CHARMM-charmm22_prot_metals
extern int charmm22_prot_metals_objects[14];
extern int charmm22_prot_metals_dim[14];
extern char * charmm22_prot_metals_atoms[98][5];
extern char * charmm22_prot_metals_bonds[139][5];
extern char * charmm22_prot_metals_angles[345][8];
extern char * charmm22_prot_metals_dihedrals[452][8];
extern char * charmm22_prot_metals_impropers[43][8];
extern char * charmm22_prot_metals_vdw[98][6];
// CHARMM-charmm35_ethers
extern int charmm35_ethers_objects[14];
extern int charmm35_ethers_dim[14];
extern char * charmm35_ethers_atoms[13][5];
extern char * charmm35_ethers_bonds[22][5];
extern char * charmm35_ethers_angles[55][8];
extern char * charmm35_ethers_dihedrals[106][8];
extern char * charmm35_ethers_vdw[13][6];
// CHARMM-charmm36_carb
extern int charmm36_carb_objects[14];
extern int charmm36_carb_dim[14];
extern char * charmm36_carb_atoms[59][5];
extern char * charmm36_carb_bonds[153][5];
extern char * charmm36_carb_angles[438][8];
extern char * charmm36_carb_dihedrals[1354][8];
extern char * charmm36_carb_impropers[14][8];
extern char * charmm36_carb_vdw[57][6];
// CHARMM-charmm36_cgenff
extern int charmm36_cgenff_objects[14];
extern int charmm36_cgenff_dim[14];
extern char * charmm36_cgenff_atoms[163][5];
extern char * charmm36_cgenff_bonds[506][5];
extern char * charmm36_cgenff_angles[1561][8];
extern char * charmm36_cgenff_dihedrals[3937][8];
extern char * charmm36_cgenff_impropers[125][8];
extern char * charmm36_cgenff_vdw[156][6];
// CHARMM-charmm36_lipid
extern int charmm36_lipid_objects[14];
extern int charmm36_lipid_dim[14];
extern char * charmm36_lipid_atoms[29][5];
extern char * charmm36_lipid_bonds[50][5];
extern char * charmm36_lipid_angles[131][8];
extern char * charmm36_lipid_dihedrals[180][8];
extern char * charmm36_lipid_impropers[4][8];
extern char * charmm36_lipid_vdw[29][6];
// CHARMM-charmm36m_prot
extern int charmm36m_prot_objects[14];
extern int charmm36m_prot_dim[14];
extern char * charmm36m_prot_atoms[53][5];
extern char * charmm36m_prot_bonds[132][5];
extern char * charmm36m_prot_angles[364][8];
extern char * charmm36m_prot_dihedrals[706][8];
extern char * charmm36m_prot_impropers[35][8];
extern char * charmm36m_prot_vdw[53][6];
// CHARMM-charmm36_na
extern int charmm36_na_objects[14];
extern int charmm36_na_dim[14];
extern char * charmm36_na_atoms[42][5];
extern char * charmm36_na_bonds[89][5];
extern char * charmm36_na_angles[226][8];
extern char * charmm36_na_dihedrals[502][8];
extern char * charmm36_na_impropers[15][8];
extern char * charmm36_na_vdw[42][6];
// CHARMM-charmm36_prot
extern int charmm36_prot_objects[14];
extern int charmm36_prot_dim[14];
extern char * charmm36_prot_atoms[53][5];
extern char * charmm36_prot_bonds[132][5];
extern char * charmm36_prot_angles[364][8];
extern char * charmm36_prot_dihedrals[705][8];
extern char * charmm36_prot_impropers[35][8];
extern char * charmm36_prot_vdw[53][6];
// CHARMM-charmm_silicates
extern int charmm_silicates_objects[14];
extern int charmm_silicates_dim[14];
extern char * charmm_silicates_atoms[13][5];
extern char * charmm_silicates_bonds[14][5];
extern char * charmm_silicates_angles[27][8];
extern char * charmm_silicates_dihedrals[34][8];
extern char * charmm_silicates_vdw[13][6];

/*
  FF force fields
*/

// Compass
extern int Compass_objects[14];
extern int Compass_dim[14];
extern char * Compass_atoms[45][5];
extern char * Compass_equivalence[46][7];
extern char * Compass_bonds[54][7];
extern char * Compass_angles[94][8];
extern char * Compass_torsions[95][11];
extern char * Compass_inversions[22][7];
extern char * Compass_vdw[46][4];
extern char * Compass_bond_increments[55][5];
// PCFF
extern int PCFF_objects[14];
extern int PCFF_dim[14];
extern char * PCFF_atoms[133][5];
extern char * PCFF_equivalence_auto[108][11];
extern char * PCFF_equivalence[134][7];
extern char * PCFF_bonds_auto[627][5];
extern char * PCFF_bonds[126][7];
extern char * PCFF_angles_auto[329][6];
extern char * PCFF_angles[302][8];
extern char * PCFF_torsions_auto[216][8];
extern char * PCFF_torsions[492][11];
extern char * PCFF_inversions[83][7];
extern char * PCFF_vdw[94][4];
extern char * PCFF_bond_increments[564][5];
// CFF91
extern int CFF91_objects[14];
extern int CFF91_dim[14];
extern char * CFF91_atoms[93][5];
extern char * CFF91_equivalence_auto[97][11];
extern char * CFF91_equivalence[95][7];
extern char * CFF91_bonds_auto[667][5];
extern char * CFF91_bonds[58][7];
extern char * CFF91_angles_auto[330][6];
extern char * CFF91_angles[155][8];
extern char * CFF91_torsions_auto[216][8];
extern char * CFF91_torsions[294][11];
extern char * CFF91_inversions[70][7];
extern char * CFF91_vdw[40][4];
extern char * CFF91_bond_increments[560][5];
// CVFF
extern int CVFF_objects[14];
extern int CVFF_dim[14];
extern char * CVFF_atoms[133][5];
extern char * CVFF_equivalence_auto[123][11];
extern char * CVFF_equivalence[129][7];
extern char * CVFF_bonds_auto[776][5];
extern char * CVFF_morse_bonds[775][6];
extern char * CVFF_angles_auto[563][6];
extern char * CVFF_torsions_auto[295][8];
extern char * CVFF_impropers[41][8];
extern char * CVFF_vdw[45][4];
extern char * CVFF_bond_increments[683][5];
// CVFF_aug
extern int CVFF_aug_objects[14];
extern int CVFF_aug_dim[14];
extern char * CVFF_aug_atoms[172][5];
extern char * CVFF_aug_equivalence_auto[162][11];
extern char * CVFF_aug_equivalence[168][7];
extern char * CVFF_aug_bonds_auto[798][5];
extern char * CVFF_aug_morse_bonds[752][6];
extern char * CVFF_aug_angles_auto[640][6];
extern char * CVFF_aug_torsions_auto[342][8];
extern char * CVFF_aug_impropers[41][8];
extern char * CVFF_aug_vdw[86][4];
extern char * CVFF_aug_bond_increments[717][5];
// OPLSAAM
extern int OPLSAAM_objects[14];
extern int OPLSAAM_dim[14];
extern char * OPLSAAM_atoms[78][4];
extern char * OPLSAAM_bonds[159][5];
extern char * OPLSAAM_angles[437][5];
extern char * OPLSAAM_dihedrals[1446][7];
extern char * OPLSAAM_impropers[105][7];
extern char * OPLSAAM_vdw[78][7];
// OPLSAAR
extern int OPLSAAR_objects[14];
extern int OPLSAAR_dim[14];
extern char * OPLSAAR_atoms[145][4];
extern char * OPLSAAR_bonds[242][5];
extern char * OPLSAAR_angles[593][5];
extern char * OPLSAAR_dihedrals[2482][7];
extern char * OPLSAAR_impropers[131][7];
extern char * OPLSAAR_vdw[145][7];
#endif
