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
* @file charmm35_ethers.c
* @short CHARMM-charmm35_ethers force field, from 'charmm35_ethers'
* @author Sébastien Le Roux <sebastien.leroux@ipcms.unistra.fr>
*/

/*
* This file: 'charmm35_ethers.c'
*
* Contains:
*

  CHARMM-charmm35_ethers force field, data from taken from:

       - the file: 'charmm35_ethers'
       - distributed in: 'toppar_c36_jul16_mass_orig.tgz'

  This file contains several tables:

    Atoms      : charmm35_ethers_atoms[13][5]
    Bonds      : charmm35_ethers_bonds[22][5]
    Angles     : charmm35_ethers_angles[55][8]
    Dihedrals  : charmm35_ethers_dihedrals[106][8]
    Non bonded : charmm35_ethers_vdw[13][6]
*/

#include "global.h"

int charmm35_ethers_objects[14] = {13, 0, 0, 22, 0, 0, 55, 0, 106, 0 , 0, 0, 13, 0};
int charmm35_ethers_dim[14] = {5, 0, 0, 5, 0, 0, 8, 0, 8, 0, 0, 0, 6, 0};

/*
 Atoms
  0= Element, 1= Mass, 2= Key, 3= Description, 4= charmm35_ethers info
*/
char * charmm35_ethers_atoms[13][5]= {{"H", "1.00800", "HCA1A", "Alkane H attached to C(sp3)H (eq. HA1)", "241"},
                               {"H", "1.00800", "HCA2A", "Alkane H attached to C(sp3)H2 (eq. HA2)", "242"},
                               {"H", "1.00800", "HCA3A", "Alkane H attached to C(sp3)H3 (eq. HA3)", "243"},
                               {"H", "1.00800", "HCA25A", "Alkane H attached to C(sp3)H2 in 5-membered ring", "244"},
                               {"C", "12.01100", "CC30A", "-C(sp3) Carbon (eq. CT)", "245"},
                               {"C", "12.01100", "CC31A", "-C(sp3)H Carbon (eq. CT1)", "246"},
                               {"C", "12.01100", "CC32A", "-C(sp3)H2 Carbon (eq. CT2)", "247"},
                               {"C", "12.01100", "CC33A", "-C(sp3)H3 Carbon (eq. CT3)", "248"},
                               {"C", "12.01100", "CC325A", "-C(sp3)H2 Carbon in 5-membered ring", "249"},
                               {"C", "12.01100", "CC325B", "-C(sp3)H2 Carbon in THF (tetrahydrofuran)", "250"},
                               {"C", "12.01100", "CC326A", "-C(sp3)H2 Carbon in THP (tetrahydropyran)", "251"},
                               {"O", "15.99940", "OC30A", "Ether Oxygen", "252"},
                               {"O", "15.99940", "OC305A", "Ether Oxygen in THF", "253"}};

/*
 Quadratic bonds
  0= Key_a, 1= Key_b, 2= R0 (A), 3= Kb (kcal mol^-1 A^2), 4= charmm35_ethers info

  V(R) = Kb x (R - R0)^2
*/
char * charmm35_ethers_bonds[22][5]= {{"CC31A", "HCA1A", "309.00", "1.111", "alkanes, 4/98"},
                                {"CC32A", "HCA2A", "309.00", "1.111", "alkanes, 4/98"},
                                {"CC33A", "HCA3A", "322.00", "1.111", "alkanes, 4/98"},
                                {"CC30A", "CC32A", "222.50", "1.538", "10/98"},
                                {"CC30A", "CC33A", "222.50", "1.538", "10/98"},
                                {"CC31A", "CC31A", "222.50", "1.500", "alkanes, 3/92"},
                                {"CC31A", "CC32A", "222.50", "1.538", "alkanes, 3/92"},
                                {"CC31A", "CC33A", "222.50", "1.538", "alkanes, 3/92"},
                                {"CC32A", "CC32A", "222.50", "1.530", "alkanes, 3/92"},
                                {"CC32A", "CC33A", "222.50", "1.528", "alkanes, 3/92"},
                                {"CC33A", "CC33A", "222.50", "1.530", "alkanes, 3/92"},
                                {"CC325A", "CC325A", "195.00", "1.548", "cyclopentane CPEN 10/17/05 viv"},
                                {"CC325A", "HCA25A", "307.00", "1.116", "cyclopentane CPEN 10/17/05 viv"},
                                {"CC325B", "OC305A", "350.00", "1.425", "THF, nucleotide CSD/NDB survey, 5/30/06,viv"},
                                {"CC325B", "CC325B", "195.00", "1.518", "THF, nucleotide CSD/NDB survey, 5/30/06,viv"},
                                {"CC325B", "HCA25A", "307.00", "1.100", "THF, THF neutron diffr., 5/30/06, viv"},
                                {"CC325B", "CC33A", "222.50", "1.528", "TF2M, viv"},
                                {"CC32A", "OC30A", "360.00", "1.415", "DEET, diethylether, alex"},
                                {"CC33A", "OC30A", "360.00", "1.415", "DEET, diethylether, alex"},
                                {"CC326A", "HCA2A", "309.00", "1.111", "THP, viv"},
                                {"CC326A", "CC326A", "222.50", "1.530", "THP, viv"},
                                {"CC326A", "OC30A", "360.00", "1.415", "DEET, diethylether, viv"}};

/*
 Quadratic angles:
  0= Key_a, 1= Key_b, 2= Key_c, 3= Ktheta (kcal mol^-1 rad^2), 4= Theta0 (deg)

  V(Theta) = Ktheta * (Theta - Theta0)^2

 Urey-Bradley angles:
  5= Kub (kcal mol^-1 A^2), 6= S0 (A), 7= charmm35_ethers info

  V(S) = Kub x (S - S0)^2
*/
char * charmm35_ethers_angles[55][8]= {{"HCA1A", "CC31A", "CC31A", "34.500", "110.10", "22.53", "2.179", "alkane, 3/92"},
                                 {"HCA1A", "CC31A", "CC32A", "34.500", "110.10", "22.53", "2.179", "alkane, 3/92"},
                                 {"HCA1A", "CC31A", "CC33A", "34.500", "110.10", "22.53", "2.179", "alkane, 3/92"},
                                 {"HCA2A", "CC32A", "CC30A", "26.500", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA2A", "CC32A", "CC31A", "26.500", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA2A", "CC32A", "CC32A", "26.500", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA2A", "CC32A", "CC33A", "34.600", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA3A", "CC33A", "CC30A", "33.430", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA3A", "CC33A", "CC31A", "33.430", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA3A", "CC33A", "CC32A", "34.600", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA3A", "CC33A", "CC33A", "37.500", "110.10", "22.53", "2.179", "alkane, 4/98"},
                                 {"HCA2A", "CC32A", "HCA2A", "35.50", "109.00", "5.40", "1.802", "alkane, 3/92"},
                                 {"HCA3A", "CC33A", "HCA3A", "35.50", "108.40", "5.40", "1.802", "alkane, 3/92"},
                                 {"CC30A", "CC32A", "CC32A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC30A", "CC32A", "CC33A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC31A", "CC31A", "CC31A", "53.350", "111.00", "8.00", "2.561", "alkane, 3/92"},
                                 {"CC31A", "CC31A", "CC32A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC31A", "CC31A", "CC33A", "53.350", "108.50", "8.00", "2.561", "alkane, 3/92"},
                                 {"CC31A", "CC32A", "CC31A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC31A", "CC32A", "CC32A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC31A", "CC32A", "CC33A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC32A", "CC30A", "CC32A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC32A", "CC31A", "CC32A", "58.350", "113.50", "11.16", "2.561", "glycerol"},
                                 {"CC32A", "CC32A", "CC32A", "58.350", "113.60", "11.16", "2.561", "alkane, 3/92"},
                                 {"CC32A", "CC32A", "CC33A", "58.000", "115.00", "8.00", "2.561", "alkane, 3/92"},
                                 {"CC33A", "CC30A", "CC33A", "53.350", "114.00", "8.00", "2.561", "alkane 3/2/92"},
                                 {"CC33A", "CC31A", "CC32A", "53.350", "114.00", "8.00", "2.561", "alkane 3/2/92"},
                                 {"CC33A", "CC31A", "CC33A", "53.350", "114.00", "8.00", "2.561", "alkane 3/2/92"},
                                 {"CC33A", "CC32A", "CC33A", "53.350", "114.00", "8.00", "2.561", "alkane 3/2/92"},
                                 {"CC325A", "CC325A", "CC325A", "58.00", "106.00", "11.16", "2.561", "CPEN 10/17/05 viv"},
                                 {"HCA25A", "CC325A", "CC325A", "35.00", "111.40", "22.53", "2.179", "CPEN 10/17/05 viv"},
                                 {"HCA25A", "CC325A", "HCA25A", "38.50", "106.80", "5.40", "1.802", "CPEN 10/17/05 viv"},
                                 {"HCA25A", "CC325B", "CC325B", "35.00", "111.40", "22.53", "2.179", "TF2M, viv"},
                                 {"HCA25A", "CC325B", "HCA25A", "38.50", "106.80", "5.40", "1.802", "THF, 10/17/05 viv"},
                                 {"CC325B", "CC325B", "CC325B", "58.00", "109.50", "11.16", "2.561", "THF, nucleotide CSD/NDB survey, 05/30/06, viv"},
                                 {"OC305A", "CC325B", "CC325B", "45.00", "111.10", " ", " ", "THF 10/21/05, viv"},
                                 {"CC325B", "OC305A", "CC325B", "95.00", "111.00", " ", " ", "THF 10/21/05, viv"},
                                 {"HCA25A", "CC325B", "OC305A", "70.00", "107.30", " ", " ", "THF 10/21/05, viv"},
                                 {"HCA3A", "CC33A", "CC325B", "34.600", "110.10", "22.53", "2.179", "TF2M viv"},
                                 {"CC325B", "CC325B", "CC33A", "58.000", "115.00", "8.00", "2.561", "TF2M viv"},
                                 {"HCA25A", "CC325B", "CC33A", "34.600", "110.10", "22.53", "2.179", "TF2M viv"},
                                 {"OC305A", "CC325B", "CC33A", "45.00", "111.50", " ", " ", "TF2M, viv"},
                                 {"CC32A", "OC30A", "CC32A", "95.00", "109.70", " ", " ", "DEET, diethylether, alex"},
                                 {"CC33A", "OC30A", "CC32A", "95.00", "109.70", " ", " ", "DEET, diethylether, alex"},
                                 {"CC33A", "OC30A", "CC33A", "95.00", "109.70", " ", " ", "DEET, diethylether, alex"},
                                 {"OC30A", "CC32A", "CC32A", "45.00", "111.50", " ", " ", "DEET, diethylether, alex"},
                                 {"OC30A", "CC32A", "CC33A", "45.00", "111.50", " ", " ", "DEET, diethylether, alex"},
                                 {"HCA3A", "CC33A", "OC30A", "60.00", "109.50", " ", " ", "phosphate, alex"},
                                 {"HCA2A", "CC32A", "OC30A", "60.00", "109.50", " ", " ", "phosphate, alex"},
                                 {"HCA2A", "CC326A", "CC326A", "34.500", "110.10", "22.53", "2.179", "THP, sng cyclohexane 12/05"},
                                 {"HCA2A", "CC326A", "HCA2A", "35.50", "109.00", "5.40", "1.80200!", "viv"},
                                 {"CC326A", "CC326A", "CC326A", "58.350", "112.00", "11.16", "2.561", "THP, sng cyclohexane 12/05"},
                                 {"OC30A", "CC326A", "CC326A", "45.00", "111.50", " ", " ", "THP, viv"},
                                 {"CC326A", "OC30A", "CC326A", "95.00", "109.70", " ", " ", "THP, viv"},
                                 {"HCA2A", "CC326A", "OC30A", "45.00", "109.50", " ", " ", "THP, sng 02/06"}};

/*
 Dihedrals
  0-3= Keys, 4= Kchi (kcal mol^-1), 5= n (multi), 6= delta (deg), 7= charmm35_ethers info

  V(chi) = Kchi x (1 + cos (n x (chi) - delta))
*/
char * charmm35_ethers_dihedrals[106][8]= {{"CC31A", "CC30A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC32A", "CC30A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC33A", "CC30A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC31A", "CC30A", "CC33A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC32A", "CC30A", "CC32A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC33A", "CC30A", "CC32A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC31A", "HCA1A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC31A", "CC31A", "CC31A", "HCA1A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC32A", "CC31A", "CC31A", "HCA1A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC33A", "CC31A", "CC31A", "HCA1A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC32A", "CC31A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC32A", "CC32A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC32A", "CC33A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC31A", "CC31A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC32A", "CC31A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC33A", "CC31A", "CC32A", "HCA2A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA1A", "CC31A", "CC33A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC31A", "CC31A", "CC33A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC32A", "CC31A", "CC33A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"CC33A", "CC31A", "CC33A", "HCA3A", "0.20000", "3", "0.00", "alkane, 3/92"},
                                    {"HCA2A", "CC32A", "CC32A", "HCA2A", "0.19000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC30A", "CC32A", "CC32A", "HCA2A", "0.19000", "3", "0.00", "alkane, 4/98, yin and mackerell"},
                                    {"CC31A", "CC32A", "CC32A", "HCA2A", "0.19000", "3", "0.00", "alkane, 4/98, yin and mackerell"},
                                    {"CC32A", "CC32A", "CC32A", "HCA2A", "0.19000", "3", "0.00", "alkane, 4/98, yin and mackerell"},
                                    {"CC33A", "CC32A", "CC32A", "HCA2A", "0.19000", "3", "0.00", "alkane, 4/98, yin and mackerell"},
                                    {"HCA2A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC30A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC31A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC32A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC33A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"CC33A", "CC32A", "CC32A", "CC33A", "0.03179", "6", "180.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC32A", "CC32A", "CC33A", "0.03819", "2", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC32A", "CC32A", "CC32A", "0.20391", "5", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC32A", "CC32A", "CC32A", "0.10824", "4", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC32A", "CC32A", "CC32A", "0.08133", "3", "180.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC32A", "CC32A", "CC32A", "0.15051", "2", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC32A", "CC32A", "CC32A", "CC32A", "0.11251", "5", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC32A", "CC32A", "CC32A", "CC32A", "0.09458", "4", "0.0", "alkane, c27r klauda et al.2004"},
                                    {"CC32A", "CC32A", "CC32A", "CC32A", "0.14975", "3", "180.0", "alkane, c27r klauda et al 2004"},
                                    {"CC32A", "CC32A", "CC32A", "CC32A", "0.06450", "2", "0.0", "alkane, c27r klauda et al 2004"},
                                    {"CC33A", "CC325A", "CC325A", "CC33A", "0.16000", "3", "0.0", "4/98, yin and mackerell, cpen, viv"},
                                    {"CC33A", "CC325A", "CC325A", "CC325A", "0.16000", "3", "0.0", "4/98, yin and mackerell, cpen, viv"},
                                    {"CC33A", "CC325A", "CC325A", "HCA25A", "0.16000", "3", "0.0", "4/98, yin and mackerell, cpen, viv"},
                                    {"HCA25A", "CC325A", "CC325A", "HCA25A", "0.16000", "3", "0.0", "4/98, yin and mackerell, cpen, viv"},
                                    {"CC325A", "CC325A", "CC325A", "HCA25A", "0.16000", "3", "0.0", "4/98, yin and mackerell, cpen, viv"},
                                    {"CC325A", "CC325A", "CC325A", "CC325A", "0.41000", "3", "180.0", "cpen, cyclopentane, viv 10/4/05"},
                                    {"CC33A", "CC325B", "CC325B", "CC33A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thf, viv"},
                                    {"CC33A", "CC325B", "CC325B", "CC325B", "0.19000", "3", "0.0", "4/98, yin and mackerell, thf, viv"},
                                    {"CC33A", "CC325B", "CC325B", "HCA25A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thf, viv"},
                                    {"HCA25A", "CC325B", "CC325B", "HCA25A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thf, viv"},
                                    {"CC325B", "CC325B", "CC325B", "HCA25A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thf, viv"},
                                    {"OC305A", "CC325B", "CC325B", "HCA25A", "0.19000", "3", "0.0", "alkane, 4/98, yin and mackerell, thf viv"},
                                    {"HCA25A", "CC325B", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, tf2m viv"},
                                    {"CC325B", "CC325B", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, tf2m viv"},
                                    {"OC305A", "CC325B", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, tf2m viv"},
                                    {"CC325B", "CC325B", "CC325B", "CC325B", "0.41000", "3", "180.0", "CPEN viv 10/4/05"},
                                    {"HCA25A", "CC325B", "OC305A", "CC325B", "0.3000", "3", "0.0", "THF, 05/30/06, viv"},
                                    {"OC305A", "CC325B", "CC325B", "CC325B", "0.0000", "3", "0.0", "THF, 05/30/06, viv"},
                                    {"CC325B", "CC325B", "OC305A", "CC325B", "0.5000", "3", "0.0", "THF, 05/30/06, viv"},
                                    {"CC33A", "CC325B", "OC305A", "CC325B", "0.3000", "3", "0.0", "THF, 05/30/06, viv"},
                                    {"CC33A", "CC326A", "CC326A", "CC33A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp, viv"},
                                    {"CC33A", "CC326A", "CC326A", "CC326A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp, viv"},
                                    {"CC33A", "CC326A", "CC326A", "HCA2A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp, viv"},
                                    {"HCA2A", "CC326A", "CC326A", "HCA2A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp, viv"},
                                    {"HCA2A", "CC326A", "CC326A", "CC326A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp, viv"},
                                    {"OC30A", "CC326A", "CC326A", "HCA2A", "0.19000", "3", "0.0", "4/98, yin and mackerell, thp viv"},
                                    {"HCA2A", "CC326A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, me-thp viv"},
                                    {"CC326A", "CC326A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, me-thp viv"},
                                    {"OC30A", "CC326A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell, me-thp viv"},
                                    {"CC326A", "CC326A", "CC326A", "CC326A", "0.49829", "2", "0.0", "THP, viv"},
                                    {"CC326A", "CC326A", "CC326A", "CC326A", "-0.59844", "3", "0.0", "THP, viv"},
                                    {"CC326A", "CC326A", "CC326A", "CC326A", "0.41746", "4", "0.0", "THP, viv"},
                                    {"CC326A", "CC326A", "CC326A", "CC326A", "-0.24829", "5", "0.0", "THP, viv"},
                                    {"OC30A", "CC326A", "CC326A", "CC326A", "-0.19225", "1", "0.0", "THP, sng 1/06"},
                                    {"OC30A", "CC326A", "CC326A", "CC326A", "-1.00000", "2", "0.0", "THP, sng 1/06"},
                                    {"OC30A", "CC326A", "CC326A", "CC326A", "0.59457", "3", "0.0", "THP, sng 1/06"},
                                    {"OC30A", "CC326A", "CC326A", "CC326A", "-0.07862", "4", "0.0", "THP, sng 1/06"},
                                    {"HCA3A", "CC33A", "CC33A", "HCA3A", "0.15250", "3", "0.00", "ETHA, ethane, 4/98, yin and mackerell"},
                                    {"CC326A", "OC30A", "CC326A", "CC326A", "-0.52702", "1", "0.0", "THP, sng 1/06"},
                                    {"CC326A", "OC30A", "CC326A", "CC326A", "0.68297", "2", "0.0", "THP, sng 1/06"},
                                    {"CC326A", "OC30A", "CC326A", "CC326A", "-0.20977", "3", "0.0", "THP, sng 1/06"},
                                    {"CC326A", "OC30A", "CC326A", "CC326A", "0.15037", "4", "0.0", "THP, sng 1/06"},
                                    {"CC326A", "OC30A", "CC326A", "HCA2A", "0.28400", "3", "0.0", "DMET, viv"},
                                    {"HCA2A", "CC32A", "CC32A", "OC30A", "0.19000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"OC30A", "CC32A", "CC33A", "HCA3A", "0.16000", "3", "0.0", "alkane, 4/98, yin and mackerell"},
                                    {"HCA2A", "CC32A", "OC30A", "CC32A", "0.28400", "3", "0.0", "DEET, diethylether, alex"},
                                    {"HCA3A", "CC33A", "OC30A", "CC32A", "0.28400", "3", "0.0", "DEET, diethylether, alex"},
                                    {"HCA2A", "CC32A", "OC30A", "CC33A", "0.28400", "3", "0.0", "DEET, diethylether, alex"},
                                    {"HCA3A", "CC33A", "OC30A", "CC33A", "0.28400", "3", "0.0", "DME, viv"},
                                    {"CC33A", "CC32A", "OC30A", "CC32A", "0.40", "1", "0.0", "diethylether, 2/12/05, ATM"},
                                    {"CC33A", "CC32A", "OC30A", "CC32A", "0.49", "3", "0.0", "diethylether"},
                                    {"CC33A", "CC32A", "OC30A", "CC33A", "0.40", "1", "0.0", "diethylether, 2/12/05, ATM, MEE viv"},
                                    {"CC33A", "CC32A", "OC30A", "CC33A", "0.49", "3", "0.0", "diethylether, MEE viv"},
                                    {"CC32A", "CC32A", "OC30A", "CC33A", "0.57", "1", "0.0", "1,2 dimethoxyethane (DME), 2/12/05, ATM"},
                                    {"CC32A", "CC32A", "OC30A", "CC33A", "0.29", "2", "0.0", "1,2 dimethoxyethane (DME)"},
                                    {"CC32A", "CC32A", "OC30A", "CC33A", "0.43", "3", "0.0", "1,2 dimethoxyethane (DME)"},
                                    {"CC32A", "CC32A", "OC30A", "CC32A", "0.57", "1", "0.0", "1,2 dimethoxyethane, 2/12/05, ATM"},
                                    {"CC32A", "CC32A", "OC30A", "CC32A", "0.29", "2", "0.0", "1,2 dimethoxyethane"},
                                    {"CC32A", "CC32A", "OC30A", "CC32A", "0.43", "3", "0.0", "1,2 dimethoxyethane"},
                                    {"OC30A", "CC32A", "CC32A", "OC30A", "0.59", "1", "180.0", "1,2 dimethoxyethane, Aug 2007, HK Lee"},
                                    {"OC30A", "CC32A", "CC32A", "OC30A", "1.16", "2", "0.0", "1,2 dimethoxyethane"},
                                    {"OC30A", "CC32A", "CC32A", "CC33A", "0.16", "1", "180.0", "methylpropylether, 2/12/05, ATM"},
                                    {"OC30A", "CC32A", "CC32A", "CC33A", "0.39", "2", "0.0", "methylpropylether"},
                                    {"OC30A", "CC32A", "CC32A", "CC32A", "0.16", "1", "180.0", "methylpropylether, 2/12/05, ATM"},
                                    {"OC30A", "CC32A", "CC32A", "CC32A", "0.39", "2", "0.0", "methylpropylether"}};

/*
 Non-bonded
  0= Key, 1= epsilon (kcal mol^-1), 2= Rmin/2 (A), 3= epsilon[1-4], 4= Rmin[1-4]/2, 5= charmm35_ethers info

  V(rij) =  Eps(ij) x [(Rmin(ij)/rij)^12 - 2 x (Rmin(ij)/rij)^6]
  With:
    Esp(ij) = sqrt(epsilon([i) x epsilon[j])
    Rmin(ij)= (Rmin[i] + Rmin[j])/2
*/
char * charmm35_ethers_vdw[13][6]= {{"HCA1A", "-0.0450", "1.3400", " ", " ", "1/5/05 viv"},
                             {"HCA2A", "-0.0350", "1.3400", " ", " ", "11/16/04 viv"},
                             {"HCA3A", "-0.0240", "1.3400", " ", " ", "yin and mackerell, 4/98"},
                             {"CC30A", "-0.0320", "2.0000", "-0.01", "1.9", "from CC31A"},
                             {"CC31A", "-0.0320", "2.0000", "-0.01", "1.9", "alkane,isobutane 1/5/05 viv"},
                             {"CC32A", "-0.0560", "2.0100", "-0.01", "1.9", "alkane, 4/98, yin, adm jr."},
                             {"CC33A", "-0.0780", "2.0400", "-0.01", "1.9", "alkane, 4/98, yin, adm jr."},
                             {"CC326A", "-0.0560", "2.0100", "-0.01", "1.9", "THP, tetrahyropyran, viv"},
                             {"HCA25A", "-0.0350", "1.3000", " ", " ", "cyclopentane, 8/06 viv"},
                             {"CC325A", "-0.0600", "2.0200", "-0.01", "1.9", "CPEN, cyclopentane, 8/06 viv"},
                             {"CC325B", "-0.0600", "2.0200", "-0.01", "1.9", "CPEN, cyclopentane, 8/06 viv"},
                             {"OC305A", "-0.1000", "1.6500", " ", " ", "tetrahydropyran sng 1/06"},
                             {"OC30A", "-0.1000", "1.6500", " ", " ", "tetrahydropyran sng 1/06"}};
